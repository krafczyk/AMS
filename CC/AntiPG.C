#include "AntiPG.h"

#ifndef __ROOTSHAREDLIBRARY__
#include "antirec02.h"
#endif

static int maxerr = 0;
static int errmax = 99;

using namespace std;

char  AntiClusterR::_Info[255];

#ifdef __USEANTICLUSTERPG__

  AntiRecoPG* AntiRecoPG::head=0;
  AMSEventR* AntiRecoPG::_HeadE=0;
  AMSEventR*& AntiRecoPG::AMSEventRHead(){return _HeadE;}

ClassImp(AntiRecoPG)
ClassImp(AntiClusterR)

AntiClusterR::AntiClusterR() {
  Status = 0xFFFFFFFF;
  Npairs = 0;
  Sector = 0;
  adc[0] = 0;
  adc[1] = 0;
  time=0;
  zeta=1000;
  chi=0;
  unfzeta=1000;
  next_time   = 0;
  next_zeta   = 0;
  next_chi   = 0;
  next_unfzeta= 0;
  zeta_adc   = 0;
  unfzeta_adc   = 0;
  rawq=0;
  phi=FillCoo(&AntiCoo);
  rawdep   = 0;
  Edep   = 0;
}

AntiClusterR::AntiClusterR(AMSAntiCluster *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  Status = ptr->_status;
  Npairs = ptr->_npairs;
  Sector = ptr->_sector;
  adc[0] = 0;
  adc[1] = 0;
  time=1.e9;
  for(int i=0;i<ptr->_ntimes;i++){
    if(fabs(ptr->_times[i])<fabs(time))time=ptr->_times[i];
  }
  zeta= ptr->_coo[2];
  chi=0;
  unfzeta= ptr->_coo[2];
  next_time   = 0;
  next_zeta   = 0;
  next_chi   = 0;
  next_unfzeta= 0;
  zeta_adc   = 0;
  unfzeta_adc   = 0;
  rawq=1;
  phi=FillCoo(&AntiCoo);
  rawdep   = ptr->_edep;
  Edep   = ptr->_edep;
#endif
}



/* return distance, beta and cosine theta inclination respect to Acc pad \n
  assuming External point hypothesis and straight line trajectory \n
  beta > 0 if Acc time is smaller than ExtTime \n
  when phiGuess >0 or (ForcedCoo is given) the 1.16ns correction of the time\n 
  depending of pad in the sector is applied\n
  when phiGuess = -2 the 1.16 ns correction is NOT APPLIED also if ForcedCoo is given */
float AntiClusterR::DoBeta(AMSPoint* ExtCoo, float ExtTime, float& beta, float& costh, AMSPoint* ForcedCoo, float phiGuess){
  float dx;
  float dy;
  float dz;
  float xcoo;
  float ycoo;
  double rr = 54.95;
  float pgs = phiGuess;
  if (!ForcedCoo){
    dz = ExtCoo->z()-AntiCoo[2];
    if (phiGuess>=0 && phiGuess<=360.){
      xcoo = rr*cos(phiGuess*3.141592654/180.);
      ycoo = rr*sin(phiGuess*3.141592654/180.);
      dx = ExtCoo->x()-xcoo;
      dy = ExtCoo->y()-ycoo;
    }
    else {
      xcoo = AntiCoo[0];
      ycoo = AntiCoo[1];
      dx = ExtCoo->x()-AntiCoo[0];
      dy = ExtCoo->y()-AntiCoo[1];
    }
  }
  else
    {
      dx = ExtCoo->x()-ForcedCoo->x();
      dy = ExtCoo->y()-ForcedCoo->y();
      dz = ExtCoo->z()-ForcedCoo->z();
      xcoo = ForcedCoo->x();
      ycoo = ForcedCoo->y();
      if (phiGuess != -2) pgs = (acos(xcoo/rr)*180./3.1415926-180.)*ycoo/fabs(ycoo)+180.;
    }

  float dist = sqrt(dx*dx+dy*dy+dz*dz);

  float phicor = 0;
    int nvr = AntiRecoPG::gethead()->NVER;
    if(pgs>=0. && pgs<=360. && phiGuess!=-2 && nvr>=0) { //only new versions
    float padspread;
    // average shift (1.16 nanoseconds) of a pad from the sector mean time 
    padspread = 1.16; 
    int rtp = AntiRecoPG::gethead()->rtp;
    if (rtp==99) padspread=padspread*1.2;  //MC correction
    phicor = -padspread;
    if (Sector == 3 || Sector == 4 || Sector == 7 || Sector == 8) phicor = padspread;
    float phicnt = 45.*(Sector-1)+22.5;
    if(pgs-phicnt>0) phicor = -phicor;
  }

  if (ExtTime != time+phicor) beta = dist/((29.9792458)*(ExtTime-time-phicor));
  float cos = xcoo*dx+ycoo*dy;
  if (dist > 0.) costh = cos/(rr*dist); 
  return dist;
}

// return the acc phi angle and fill AntiCoo AMSPoint with AntiCoo[2]=unfzeta 
float AntiClusterR::FillCoo(AMSPoint* AntiCoo){
  float phi = 45.*(Sector-1)+22.5;
  float zz = unfzeta;
  float xx = 54.95*cos(phi*3.141592654/180.);
  float yy = 54.95*sin(phi*3.141592654/180.);
  AntiCoo->setp(xx,yy,zz);
  return phi;
}

// raw evaluation of energy deposition pairing ADC sides corrected for Temperature and Zcoo effects
float AntiClusterR::PairDep(){
  float e0=0.;
  float e1=0.;
  e0=rawDepSide[0];
  e1=rawDepSide[1];
  float medep = 0;
  if (e0>0) medep = medep+e0;
  if (e1>0) medep = medep+e1;
  if(e0>0 && e1>0) medep = 0.5*medep; // dual sided
  if(e0<0 && e1<0) medep = 60; //large fixed and unknown both sides saturation
  return medep;
}


/* return the phi that most probably you should add to accphi
 */

float AntiClusterR::FindPhiGuess(float betawrong, float costh, int qtrue){
  float phiret = 0;
  float bcorr = AntiRecoPG::gethead()->bcorr;   
  float medep = rawdep;
  if (!(medep >= 60 || medep<=0)){
    double bbetaq = betawrong*betawrong;
    if (bbetaq>1 || bbetaq==0.) bbetaq = 1;
    double fbeta = (1.+bcorr/bbetaq);
    float fret = log10(2.*medep*fabs(costh)/(fbeta*qtrue*qtrue));
    float lmin = AntiRecoPG::gethead()->pth_min[Sector-1];
    float lmax = AntiRecoPG::gethead()->pth_max[Sector-1];
    float dl = (lmax-lmin);
    float ltop = lmax+2.*dl;
    float lbot = lmin-2.*dl; 
    if(fret>ltop) phiret = 22.5/2.;  
    if(fret<lbot) phiret = -22.5/2.;  
    if(fret>=lbot && fret<=ltop) phiret = -22.5/2.+22.5*(fret-lbot)/(ltop-lbot);  
    if(fret<lbot-dl || fret>ltop+dl) phiret = 0;  //anomalous too far
    if(Sector == 3 || Sector == 4 || Sector == 7 || Sector == 8) phiret = -phiret;
  }
  return phi+phiret;
}


/* return AntiCluster charge with corrections \n
  costh = cosine track inclination respect to Anti Pad \n
  iside = -1 default do the average charge from sectors \n
  if Zcoo!=0 force this zcoo in corrections
*/
float AntiClusterR::Charge(float costh, float beta, int iside, float Zcoo, int timecorr){
  float qret=0;
  float bcorr = AntiRecoPG::gethead()->bcorr;   // trick: reload calibrations par from Acc reconstruction      
  int nvr = AntiRecoPG::gethead()->NVER;
  if (nvr>=0){
    if (iside == -1) return PairedCharge(costh, beta, Zcoo, timecorr);
    float medep = rawdep; 
    if (Zcoo == 0 && iside==0) medep = rawDepSide[0]; //already with timecorr
    if (Zcoo == 0 && iside==1) medep = rawDepSide[1]; //already with timecorr
    if (Zcoo != 0) {  // redone with correction Zcoo
      medep = AntiRecoPG::gethead()->DoSideRawEdep(Sector,iside,Zcoo,timecorr);    
    }
    if (timecorr==0 && Zcoo==0 ) medep = AntiRecoPG::gethead()->DoSideRawEdep(Sector,iside,unfzeta,timecorr); //case of self zeta but no time corrections
    if (medep >= 60 || medep<0) return 6.;
    double bbetaq = beta*beta;
    if (bbetaq>1 || bbetaq==0.) bbetaq = 1;
    double fbeta = (1.+bcorr/bbetaq);
    qret = sqrt(medep*fabs(costh)/fbeta);
  }
  else{  // OLD (pre jan 2014) VERSION
    float Etoqcc[8][3];
    float zcorr[4];
    float ttcorr[4];
    for (int j=0;j<3;j++){Etoqcc[Sector-1][j] = AntiRecoPG::gethead()->Etoqcc[Sector-1][j];}
    for (int j=0;j<4;j++){
      zcorr[j] = AntiRecoPG::gethead()->zcorr[j];
      ttcorr[j] = AntiRecoPG::gethead()->ttcorr[j];}
    float eng = rawdep*fabs(costh);
    float dlt = Etoqcc[Sector-1][1]*Etoqcc[Sector-1][1]-4.*Etoqcc[Sector-1][2]*(Etoqcc[Sector-1][0]-eng);
    float qraw = -Etoqcc[Sector-1][1];
    if (dlt>0.) qraw = qraw + sqrt(dlt);  // not saturation of ADC  
    qret = qraw/(2.*Etoqcc[Sector-1][2]);
    if (fabs(beta)>1) beta=1.;
    if (fabs(beta)<0.4) beta=0.4; // slow cutoff  
    if (beta != 1. && beta != 0.) qret = qret - bcorr*fabs((1.-fabs(beta))/beta); //bethe block approx 
    if (Zcoo !=0. && fabs(Zcoo)<40.){
      float dtb = 40.-fabs(Zcoo);
      if (dtb>10.) dtb = 10.; // dtb > 10 still not investigated  
      dtb = log10(dtb);
      float qzcorr = zcorr[0]+zcorr[1]*dtb+zcorr[2]*dtb*dtb+zcorr[3]*dtb*dtb*dtb;
      qret = qret - qzcorr; // small distance from border correction            
    }
    if (costh != 0.45){
      float act = fabs(costh);
      float qttcorr = ttcorr[0]+ttcorr[1]*act+ttcorr[2]*act*act+ttcorr[3]*act*act*act;
      qret = qret - qttcorr; // fine costheta correction (outside [0.1 to 0.7] still not investigated)
    }
    if (qret <0.) qret = 0;
  } 
  return qret;
}

// evaluate charge from both sides
float AntiClusterR::PairedCharge(float costh, float beta, float Zcoo, int timecorr){
  double q0= Charge(costh,beta,0,Zcoo,timecorr);
  double q1= Charge(costh,beta,1,Zcoo,timecorr);
  double meq = 0;
  if (q0>0 && q0<6) meq = meq+q0;
  if (q1>0 && q1<6) meq = meq+q1;
  if(q0>0 && q1>0 && q0<6 && q1<6) meq = 0.5*meq;
  if(meq == 0 && (q0>=6 || q1>=6)) meq = 6.;
  return meq;
}

/* rebuild a single AntiCluster \n
Sector should be already existing \n
return 0 if no raw sides found \n
return npairs>0 (both sides) and nsingle<0 (single side)
*/
int AntiClusterR::ReBuildMe(float zzguess, float err_zguess, float ttguess, float err_tguess){ 
  AntiRecoPG* Acci = AntiRecoPG::gethead();
  int nbc = Acci->BuildCluster(this, Sector, zzguess,err_zguess,ttguess,err_tguess);
  Info();
  return nbc;
}

//-------------------------------------------------------------------------------------------------

AntiRecoPG::AntiRecoPG(int version, int Istolerant){
  //  cout << " da fare initpar" << endl;
  InitPar(version,Istolerant);
  //  cout << " fatto initpar" << endl;
}

AntiRecoPG* AntiRecoPG::gethead(int version, int Istolerant) {
  if (!head) {
    printf("AntiRecoPG::gethead()-M-CreatingObjectAntiRecoPGAsSingleton\n");
    head = new AntiRecoPG(version, Istolerant);//this also initialize at the default version
    if(!AMSEventRHead())AMSEventRHead()=AMSEventR::Head();
  }
  else {
    if (version!=0) cerr << "AntiRecoPG::gethead()-E- version change not permitted! " <<endl;
  }
  return head;
}


AntiRecoPG::~AntiRecoPG(){
  clear();
  head = 0;
};

void AntiRecoPG::clear(){

  for (int iz=0; iz<2; iz++) {
    for (int i8=0; i8<8; i8++) {
      if( Qcal[iz][i8]) delete[] Qcal[iz][i8];
    }}
  for (int iz=0; iz<2; iz++) {
    if (Qcal[iz]) delete[] Qcal[iz];
  }
  if(Timecal) delete[] Timecal;
  if(ZTimecal) delete[] ZTimecal;
  if(Qcal) delete[] Qcal;
  infile->Close();
  return;
}

/* found the sector(1-8) you think (-hope) is crossed assuming straight line \n
    = 0 no crossing at all ;  -10 error (same points)\n
    <0 sector but crossing at |Z| > 40 cm \n
    if AntiCounters are crossed in both directions the nearest to Point1 is chosen*/ 
int AntiRecoPG::GetCrossing(AMSPoint* Point1, AMSPoint* Point2, AMSPoint* CrossPoint, float& phicross){
  phicross = 0;
  double rr = 54.95;
  double dx = Point2->x(); 
  double dy = Point2->y(); 
  double dz = Point2->z(); 
  double px = Point1->x(); 
  double py = Point1->y(); 
  double pz = Point1->z(); 
  dx = dx - px;
  dy = dy - py;
  dz = dz - pz;
  double dmod = sqrt(dx*dx+dy*dy+dz*dz);
  if (dmod<=0.) return -10;
  dx = dx/dmod;
  dy = dy/dmod;
  dz = dz/dmod;
  double aa = dx*dx+dy*dy;
  if (aa == 0.) return -10;
  double bb = 2.*(px*dx+py*dy);
  double cc = px*px+py*py-(rr*rr);
  double delta = bb*bb-4.*aa*cc;
  if (delta < 0.) return 0;
  double s1 = (-bb-sqrt(delta))/(2.*aa);
  double z1 = dz*s1+pz;
  double s2 = (-bb+sqrt(delta))/(2.*aa);
  double z2 = dz*s2+pz;
  double sc = s1;
  if (fabs(z1)>40.) sc = s2;
  if ((fabs(z1)<40.) && (fabs(z2)<40.)) {
    sc = s2;
    if (fabs(s1)<fabs(s2)) sc = s1;
  }
 double zc =  dz*sc+pz;
 double xc =  dx*sc+px;
 double yc =  dy*sc+py;
 CrossPoint->setp(xc,yc,zc);
 phicross = (acos(xc/rr)*180./3.1415926-180.)*yc/fabs(yc)+180.;
 int rsec = 0;
 for (int is = 1; is<9;is++){
   double phis = 45.*(is-1)+22.5;
 if(fabs(phis-phicross)<22.5) rsec = is;
 }
 if (fabs(zc)>=40) rsec = -rsec;
 return rsec;
}


int AntiRecoPG::SelectRun(){
  // return = 1 DATA                                             
  // return = 2 pre-launch BT or Muons (pre 1305331200)                   
  // return = 99  MC                                              
  int run_type;
  //  cout << " in selectrun" << endl;
  AMSEventR* evt = AMSEventRHead()?AMSEventRHead():AMSEventR::Head();
    MCEventgR *pmc = evt->pMCEventg(0);
    if (!pmc) { run_type = 1;  // is DATA                                      
      //  cout << "Run autoselected as DATA" << endl;
      if (evt->Run()<1305331200) {// is pre-launch
       run_type = 2;
	//  cout << "Run autoselected as pre-ISS DATA (BT or Muons)" << endl;
      }
      //  else cout << "Run autoselected as ISS DATA" << endl;
    }
    else { // is MC                          
      run_type = 99;
      //  cout << "Run autoselected as MC" << endl;
    }
  return run_type;
}


void AntiRecoPG::InitCal(int run_type, int VERNUM){ 
    // memory of run type
    rtp = run_type;
    //reload ACC temperature
    temptime = 0;
    tempavg = GetAvgTempAcc();

    if (run_type==0) DefaultCal(run_type);
  LoadHistoCaliFile(VERNUM);
  LoadCaliFile(VERNUM);
  //  cout << " loaded" << endl;
  return;
}


void AntiRecoPG::DefaultCal(int run_type){ 

  /*
  for (int ii=0;ii<20;ii++){
    cout << ii << " default calib" << endl;
  } 
  */ 
  if (run_type == 99) {
    slr[0] = 1.145;
    slr[1] = -1.395e-3;
    slr[2] = 4.625;
    slr[3] = -2.285e-4;
    slr[4] = 3.2;

    for(int side=0;side<=1;side++){
      for(int bar=1;bar<=8;bar++){
	ADCAL[bar-1][side][0]= 0.;
	ADCAL[bar-1][side][1]= 428.;
	ADCAL[bar-1][side][2]= 0.03;
      }
    }

    for(int bar=1;bar<=8;bar++){
      Zextinction[bar-1][0] = -0.004;
      Zextinction[bar-1][1] = 0.0044;
    }
    // calibration of border effect for light collection     
    Zborders = -1.09e-7; // cm^-4

    //thresholds for phiguess
    for  (int ibb = 0; ibb<8; ibb++) {
	pth_min[ibb]=0.1845;
	pth_max[ibb]=0.884;
	}


    zsme = 1.8; // cm artificial extra smearing for MC 5.3cm->5.6cm
    for (int bar=1;bar<9;bar++){
      zcal[1][bar-1]=1.;
      zcal[0][bar-1]=4.;
    }

    // calibration constants for Z from adc
    zgsme = 25.; // artificial extra smearing for MC 20cm->32cm 
    for (int bar=1;bar<9;bar++){
    zgcal[0][bar-1]=-0.0269749;
    zgcal[1][bar-1]=0.00411284;
    }

    tsme = 0.0; // artificial extra smearing for MC    

    // calibration constants for timing
    tcal[1][0]=1.; //par1
    tcal[1][1]=1.;
    tcal[1][2]=1.;
    tcal[1][3]=1.;
    tcal[1][4]=1.;
    tcal[1][5]=1.;
    tcal[1][6]=1.;
    tcal[1][7]=1.; 

    tcal[0][0]=-4.751888;
    tcal[0][1]=1.18947;
    tcal[0][2]=1.21969;
    tcal[0][3]=-4.7425;
    tcal[0][4]=-4.693583;
    tcal[0][5]=1.246988;
    tcal[0][6]=1.235582;
    tcal[0][7]=-4.723954;

  }
  else {
    // (time vs ADC) slew rate correction parameters 
    slr[0] = 1.57144e+00;
    slr[1] = -1.77592e-03;
    slr[2] = 6.17880e+00;
    slr[3] = -5.79882e-04;
    slr[4] = 3.95;
    
    // calibration constants for E from adc
    // E[sect-1][Side]=ADCAL[sect-1][side][0]+ADCAL[sect-1][side][1]*adc71
    //like a pedestal
    ADCAL[0][0][0]= 350.382;
    ADCAL[0][1][0]= 290.573;
    ADCAL[1][0][0]= 210.919;
    ADCAL[1][1][0]= 291.517;
    ADCAL[2][0][0]= 355.634;
    ADCAL[2][1][0]= 269.412;
    ADCAL[3][0][0]= 173.215;
    ADCAL[3][1][0]= 341.238;
    ADCAL[4][0][0]= 247.926;
    ADCAL[4][1][0]= 357.681;
    ADCAL[5][0][0]= 184.428;
    ADCAL[5][1][0]= 291.802;
    ADCAL[6][0][0]= 250.863;
    ADCAL[6][1][0]= 280.386;
    ADCAL[7][0][0]= 277.383;
    ADCAL[7][1][0]= 265.17;
    // like a linear term
    ADCAL[0][0][1]= 319.696;
    ADCAL[0][1][1]= 214.992;
    ADCAL[1][0][1]= 377.902;
    ADCAL[1][1][1]= 238.784;
    ADCAL[2][0][1]= 325.162;
    ADCAL[2][1][1]= 263.712;
    ADCAL[3][0][1]= 336.163;
    ADCAL[3][1][1]= 177.71;
    ADCAL[4][0][1]= 316.484;
    ADCAL[4][1][1]= 225.828;
    ADCAL[5][0][1]= 356.575;
    ADCAL[5][1][1]= 262.797;
    ADCAL[6][0][1]= 286.45;
    ADCAL[6][1][1]= 294.669;
    ADCAL[7][0][1]= 273.871;
    ADCAL[7][1][1]= 203.409;
    // a saturation term
    ADCAL[0][0][2]= 0.112189;
    ADCAL[0][1][2]= 0.057368;
    ADCAL[1][0][2]= 0.126427;
    ADCAL[1][1][2]= 0.0635941;
    ADCAL[2][0][2]= 0.114318;
    ADCAL[2][1][2]= 0.0717296;
    ADCAL[3][0][2]= 0.110431;
    ADCAL[3][1][2]= 0.0395994;
    ADCAL[4][0][2]= 0.101397;
    ADCAL[4][1][2]= 0.058701;
    ADCAL[5][0][2]= 0.10748;
    ADCAL[5][1][2]= 0.0758706;
    ADCAL[6][0][2]= 0.0841825;
    ADCAL[6][1][2]= 0.0924872;
    ADCAL[7][0][2]= 0.0819102;
    ADCAL[7][1][2]= 0.0617998;

    // calibrations Z light extinction along the bar (1/cm)
    Zextinction[0][0] = -0.00171627;
    Zextinction[0][1] = 0.00364679;
    Zextinction[1][0] = -0.000401146;
    Zextinction[1][1] = 0.00399522;
    Zextinction[2][0] = -0.00120933;
    Zextinction[2][1] = 0.00470612;
    Zextinction[3][0] = -0.00201883;
    Zextinction[3][1] = 0.00369474;
    Zextinction[4][0] = -0.00316947;
    Zextinction[4][1] = 0.00190449;
    Zextinction[5][0] = -0.00267031;
    Zextinction[5][1] = 0.00350772;
    Zextinction[6][0] = -0.00182413;
    Zextinction[6][1] = 0.00354171;
    Zextinction[7][0] = -0.00329158;
    Zextinction[7][1] = 0.00317311;

    // calibration of border effect for light collection
    Zborders = -1.09e-7; // cm^-4

    // calibration of adc vs avg temperature
    TempCal[0][0] = -0.00485724;
    TempCal[0][1] = -0.00981563;
    TempCal[1][0] = -0.00416527;
    TempCal[1][1] = -0.00453614;
    TempCal[2][0] = -0.00330919;
    TempCal[2][1] = -0.00435237;
    TempCal[3][0] = -0.00482868;
    TempCal[3][1] = -0.00821987;
    TempCal[4][0] = -0.00405155;
    TempCal[4][1] = -0.00640026;
    TempCal[5][0] = -0.0033872;
    TempCal[5][1] = -0.00690268;
    TempCal[6][0] = -0.0056574;
    TempCal[6][1] = -0.00443635;
    TempCal[7][0] = -0.005805;
    TempCal[7][1] = -0.0115087;

    //thresholds for phiguess
    pth_min[0] = -0.1;
    pth_max[0] = -0.028;
    pth_min[1] = -0.078;
    pth_max[1] = 0.058;
    pth_min[2] = -0.0564;
    pth_max[2] = 0.064;
    pth_min[3] = -0.112;
    pth_max[3] = -0.038;
    pth_min[4] = -0.056;
    pth_max[4] = 0.019;
    pth_min[5] = -0.041;
    pth_max[5] = 0.066;
    pth_min[6] = -0.079;
    pth_max[6] = 0.050;
    pth_min[7] = -0.152;
    pth_max[7] = -0.046;

    // extrapolation time behaviour of timing corrections
    TTimecal[0][0] = -2.12338;
    TTimecal[1][0] = 1.61684e-09;
    TTimecal[0][1] = -2.08009;
    TTimecal[1][1] = 1.58649e-09;
    TTimecal[0][2] = -2.35851;
    TTimecal[1][2] = 1.79479e-09;
    TTimecal[0][3] = -1.91485;
    TTimecal[1][3] = 1.46353e-09;
    TTimecal[0][4] = 0.495176;
    TTimecal[1][4] = -3.7511e-10;
    TTimecal[0][5] = 0.271688;
    TTimecal[1][5] = -2.08653e-10;
    TTimecal[0][6] = -0.102912;
    TTimecal[1][6] = 7.27244e-11;
    TTimecal[0][7] = 0.3378;
    TTimecal[1][7] = -2.57767e-10;
    
    // extrapolation time behaviour of zeta corrections
    ZTTimecal[0][0] = 5.90601;
    ZTTimecal[1][0] = -4.27533e-09;
    ZTTimecal[0][1] = -0.612166;
    ZTTimecal[1][1] = 6.1755e-10;
    ZTTimecal[0][2] = 5.15546;
    ZTTimecal[1][2] = -3.11814e-09;
    ZTTimecal[0][3] = 6.50103;
    ZTTimecal[1][3] = -4.59458e-09;
    ZTTimecal[0][4] = 17.3756;
    ZTTimecal[1][4] = -8.52626e-09;
    ZTTimecal[0][5] = 9.13265;
    ZTTimecal[1][5] = -1.96745e-09;
    ZTTimecal[0][6] = 18.5582;
    ZTTimecal[1][6] = -8.89398e-09;
    ZTTimecal[0][7] = 18.4959;
    ZTTimecal[1][7] = -9.39017e-09;
    
    // estrapolation of time behaviour of P[0][sect-1][side] and He[1][sect-1][side] charges
    Qcal0[0][0][0] = 1.69765;
    Qcal1[0][0][0] = -5.79354e-10;
    Qcal0[0][1][0] = 1.86603;
    Qcal1[0][1][0] = -7.02299e-10;
    Qcal0[0][2][0] = 1.4888;
    Qcal1[0][2][0] = -4.2093e-10;
    Qcal0[0][3][0] = 1.88147;
    Qcal1[0][3][0] = -7.18447e-10;
    Qcal0[0][4][0] = 1.43503;
    Qcal1[0][4][0] = -3.26502e-10;
    Qcal0[0][5][0] = 1.42107;
    Qcal1[0][5][0] = -3.34984e-10;
    Qcal0[0][6][0] = 1.63079;
    Qcal1[0][6][0] = -5.10058e-10;
    Qcal0[0][7][0] = 1.75717;
    Qcal1[0][7][0] = -5.88929e-10;
    Qcal0[0][0][1] = 1.79592;
    Qcal1[0][0][1] = -6.62655e-10;
    Qcal0[0][1][1] = 1.50627;
    Qcal1[0][1][1] = -4.36379e-10;
    Qcal0[0][2][1] = 1.62029;
    Qcal1[0][2][1] = -4.9703e-10;
    Qcal0[0][3][1] = 1.95709;
    Qcal1[0][3][1] = -7.90456e-10;
    Qcal0[0][4][1] = 2.00896;
    Qcal1[0][4][1] = -8.04856e-10;
    Qcal0[0][5][1] = 1.75499;
    Qcal1[0][5][1] = -5.9247e-10;
    Qcal0[0][6][1] = 1.83863;
    Qcal1[0][6][1] = -6.71794e-10;
    Qcal0[0][7][1] = 2.31068;
    Qcal1[0][7][1] = -1.06138e-09;
    Qcal0[1][0][0] = 2.7892;
    Qcal1[1][0][0] = -6.92568e-10;
    Qcal0[1][1][0] = 3.00905;
    Qcal1[1][1][0] = -8.62712e-10;
    Qcal0[1][2][0] = 2.22023;
    Qcal1[1][2][0] = -2.63557e-10;
    Qcal0[1][3][0] = 3.27927;
    Qcal1[1][3][0] = -1.06974e-09;
    Qcal0[1][4][0] = 2.63633;
    Qcal1[1][4][0] = -4.80618e-10;
    Qcal0[1][5][0] = 2.6208;
    Qcal1[1][5][0] = -5.17706e-10;
    Qcal0[1][6][0] = 2.79315;
    Qcal1[1][6][0] = -6.41129e-10;
    Qcal0[1][7][0] = 3.17345;
    Qcal1[1][7][0] = -9.10207e-10;
    Qcal0[1][0][1] = 3.47911;
    Qcal1[1][0][1] = -1.21009e-09;
    Qcal0[1][1][1] = 2.86379;
    Qcal1[1][1][1] = -7.18182e-10;
    Qcal0[1][2][1] = 2.90821;
    Qcal1[1][2][1] = -7.14341e-10;
    Qcal0[1][3][1] = 3.84886;
    Qcal1[1][3][1] = -1.48352e-09;
    Qcal0[1][4][1] = 3.82573;
    Qcal1[1][4][1] = -1.43217e-09;
    Qcal0[1][5][1] = 3.06888;
    Qcal1[1][5][1] = -8.20999e-10;
    Qcal0[1][6][1] = 3.34831;
    Qcal1[1][6][1] = -1.06624e-09;
    Qcal0[1][7][1] = 4.44337;
    Qcal1[1][7][1] = -1.95628e-09;
    
    //scaling factor for gain correction                                      
    Scal[0][0] = 0.5;
    Scal[1][0] = 0.5;
    Scal[2][0] = 0.55;
    Scal[3][0] = 0.45;
    Scal[4][0] = 0.25;
    Scal[5][0] = 0.45;
    Scal[6][0] = 0.55;
    Scal[7][0] = 0.6;
    Scal[0][1] = 0.7;
    Scal[1][1] = 0.8;
    Scal[2][1] = 0.8;
    Scal[3][1] = 0.83;
    Scal[4][1] = 0.7;
    Scal[5][1] = 0.7;
    Scal[6][1] = 0.65;
    Scal[7][1] = 0.7;

  // calibration constants for Z from trd
    zsme = 0.; // no extra smearing for data
    zcal[0][7]=6.392457e+01;
    zcal[1][7]=1.130916e+00;
    zcal[0][6]=6.270665e+01;
    zcal[1][6]=1.171432e+00;
    zcal[0][5]=6.444458e+01;
    zcal[1][5]=1.154871e+00;
    zcal[0][4]=6.537712e+01;
    zcal[1][4]=1.162057e+00;
    zcal[0][3]=4.178925e+01;
    zcal[1][3]=1.130742e+00;
    zcal[0][2]=3.497706e+01;
    zcal[1][2]=1.163380e+00;
    zcal[0][1]=3.803472e+01;
    zcal[1][1]=1.172850e+00;
    zcal[0][0]=3.507813e+01;
    zcal[1][0]=1.144851e+00;


    // calibration constants for timing
    tcal[1][0]=1.; //par1
    tcal[1][1]=1.;
    tcal[1][2]=1.;
    tcal[1][3]=1.;
    tcal[1][4]=1.;
    tcal[1][5]=1.;
    tcal[1][6]=1.;
    tcal[1][7]=1.;

    tsme = 0.; // no extra smearing for data
    //already added timecal questi li aggiunge a tacc
    tcal[0][0]=9.242; //par0
    tcal[0][1]=13.6926;
    tcal[0][2]=13.73352;
    tcal[0][3]=8.362;
    tcal[0][4]=-13.714473;
    tcal[0][5]=-8.91772;
    tcal[0][6]=-8.870157;
    tcal[0][7]=-14.121876;

    // calibration constants for Z from adc
    zgsme = 0.; // no extra smearing for data
    zgcal[0][0] = 0.00428277;
    zgcal[1][0] = 0.00377735;
    zgcal[0][1] = -0.00611472;
    zgcal[1][1] = 0.00331785;
    zgcal[0][2] = -0.00248941;
    zgcal[1][2] = 0.00404178;
    zgcal[0][3] = 0.00591568;
    zgcal[1][3] = 0.00357377;
    zgcal[0][4] = -0.00821163;
    zgcal[1][4] = 0.00312464;
    zgcal[0][5] = -0.0293793;
    zgcal[1][5] = 0.00358966;
    zgcal[0][6] = -0.00492832;
    zgcal[1][6] = 0.00324728;
    zgcal[0][7] = 0.00821338;
    zgcal[1][7] = 0.00409475;
  }
  
  // approx bethe bloch correction par.
  bcorr = 0.365;

  return;
}

int AntiRecoPG::LoadHistoCaliFile( int VERNUM, int iforce){ 

  if (VERNUM>=0){
    //  cout << " dentro load" << endl;
    if (iforce == 0 && ifild<0) return 0;
    //***** TRY LOAD FROM FILE *************
    //  cout << " forced load" << endl;
    Timecal = new TH1D*[8];
    ZTimecal = new TH1D*[8];
    Qcal = new TH1D***[2];
    for (int iz=0; iz<2; iz++) {
      Qcal[iz] = new TH1D**[8];
    }
    for (int iz=0; iz<2; iz++) {
      for (int i8=0; i8<8; i8++) {
	Qcal[iz][i8] = new TH1D*[2];
      }}

    TDirectory* curr = gDirectory;
    
    const char* dbPath;
    const char* dbdir;
    dbPath = getenv("AntiPGDataDir");
    if (dbPath==NULL) {
      dbdir = getenv("AMSDataDir");
      if (dbdir!=NULL) {
	dbPath = Form("%s/v5.00/AntiPG", dbdir);
      }
      else dbPath="/afs/cern.ch/exp/ams/Offline/AMSDataDir/v5.00/AntiPG";
    }
    
    cout << dbPath << endl;
    //    cout << "NOT USED NOW" << endl;
    //    dbPath="/storage/gpfs_ams/ams/users/fnozzoli/WD_3/CALACC/";  //TESTING         
    char nfile[100];
    sprintf(nfile, "%s/AntiPG_calib_%d.root", dbPath, VERNUM);
    infile = new TFile(nfile);
    if (!infile || infile->IsZombie()) {
      if (maxerr<errmax) {   cerr << "AntiRecoPG::LoadHistoCaliFile-E-" << dbPath << "/AntiPG_calib_" << VERNUM << ".root not found!" << endl;
	maxerr++; }
      if (itolerance==0) {  // program will crash
	cerr << "AntiRecoPG::LoadHistoCaliFile-E-" << "program is crashing NOW" << dbPath << "/AntiPG_calib_" << VERNUM << ".root not found!" << endl;
	TH1F* h=NULL;
	h->Fill(10);
      }
      return -1; 
    }
    if (maxerr<errmax) {  cout << "AntiRecoPG::LoadHistoCaliFile-I-Loaded " << nfile << endl;
      maxerr++; }
    
    for (int isecto = 0;isecto<8;isecto++){
      sprintf(nfile,"Timecal%d",isecto+1);
      Timecal[isecto]= (TH1D*) infile->Get(nfile); 
      sprintf(nfile,"ZTimecal%d",isecto+1);
      ZTimecal[isecto]= (TH1D*) infile->Get(nfile);
      for (int iz=0; iz<2; iz++) { 
	for (int isi=0; isi<2; isi++) { 
	  sprintf(nfile,"Qcal%d_%d_%d",iz+1,isecto+1,isi);
	  Qcal[iz][isecto][isi]= (TH1D*) infile->Get(nfile);
	}}
      //cout << Timecal[isecto]->GetEntries()<< " read entries" << endl;
    }                     
    if (maxerr<errmax) {  cout << "AntiRecoPG::LoadHistoCaliFile-I-LoadedInfosForAll " << endl;
      maxerr++; }
    if (curr) curr->cd();
    ifild = -1*abs(ifild);
  }
  return 1;
}
  



int AntiRecoPG::LoadCaliFile( int VERNUM, int iforce){ 

    if (rtp==0) {
      if (ifild>0) ifild = 1;
      if (ifild<0) ifild = -1;
      return 0;  // force a new call to this function later
    }

    if (iforce == 0 && ifild%2 == 0) return 0;

  if (VERNUM>=0){
    TString stringa;
    TString stringa2;
    TString test;
    int ifind = 0;
    int itot = 0;
    
    
    const char* dbPath;
    const char* dbdir;
    dbPath = getenv("AntiPGDataDir");
    if (dbPath==NULL) {
      dbdir = getenv("AMSDataDir");
      if (dbdir!=NULL) {
	dbPath = Form("%s/v5.00/AntiPG", dbdir);
      }
      else dbPath="/afs/cern.ch/exp/ams/Offline/AMSDataDir/v5.00/AntiPG";
    }
    
    cout << dbPath << endl;
    //cout << "NOT USED NOW" << endl;
    //dbPath="/storage/gpfs_ams/ams/users/fnozzoli/WD_3/CALACC/";  //TESTING             
    char nfile[100];
    
    if (rtp==99) {
      sprintf(nfile, "%s/AntiPG_static_MC_calib_%d.dat", dbPath, VERNUM);}
    else {
      sprintf(nfile, "%s/AntiPG_static_ISS_calib_%d.dat", dbPath, VERNUM);}
    ifstream fin(nfile);
    if (!fin || !(fin.good())) {
      if (maxerr<errmax) {   cerr << "AntiRecoPG::LoadCaliFile-E-" << nfile  << " not found!" << endl;
	maxerr++; }
      if (itolerance==0) {  // program will crash
	cerr << "AntiRecoPG::LoadCaliFile-E-" << "program is crashing NOW" << nfile << ".root not found!" << endl;
	TH1F* h=NULL;
	h->Fill(10);
      }
      return -1; 
    }
    if (maxerr<errmax) {  cout << "AntiRecoPG::LoadCaliFile-I-Loaded " << nfile << endl;
      maxerr++; }
    
    while (!fin.eof()) {
      test = fin.peek();
      if (test == "E") break;
      if( test == "#" ) {
	fin.ignore(100, '\n');}
      else {
	fin >> stringa;
	fin >> stringa2;
	//   cout << stringa << " = " << stringa2 << endl;
	itot = itot+ifind;
	ifind = 0;
	if(stringa == "version" || stringa == "submitted") {
	  cout << stringa << " " << stringa2 << endl; ifind++;}
	if(stringa == "isMC") {
	  ifind++;
	  int isMC = (int) atoi(stringa2.Data());
	  if ((isMC == 1 && rtp != 99) || (isMC==0 && rtp==99)) {
	    if (maxerr<errmax) {
	      cerr << "AntiRecoPG::LoadCaliFile-E-" << isMC << " not maching run type " << rtp << endl;
	      maxerr++; }
	  }
	}
	for (int ipar=0;ipar<5;ipar++){
	  if (ifind >0) break;
        if(stringa == Form("slr[%d]",ipar)){
          slr[ipar]=(float) atof(stringa2.Data()); ifind++;}
	}
	for (int ibar=0;ibar<8;ibar++){
	  if (ifind >0) break;
	  for (int isi=0;isi<2;isi++){
	    if (ifind >0) break;
	    for (int ipar=0;ipar<3;ipar++){
	      if (ifind >0) break;
	      if(stringa == Form("ADCAL[%d][%d][%d]",ibar,isi,ipar)){
		ADCAL[ibar][isi][ipar]=(float) atof(stringa2.Data());ifind++;}
	    }}}
	if(stringa == "Zborders") {Zborders = (float) atof(stringa2.Data());ifind++;}
	if(stringa == "zsme") {zsme = (float) atof(stringa2.Data());ifind++;}
	if(stringa == "tsme") {tsme = (float) atof(stringa2.Data());ifind++;}
	if(stringa == "zgsme") {zgsme = (float) atof(stringa2.Data());ifind++;}
	if(stringa == "bcorr") {bcorr = (float) atof(stringa2.Data());ifind++;}
	for (int ibar=0;ibar<8;ibar++){
	  if (ifind >0) break;
	  if(stringa == Form("pth_min[%d]",ibar)){
	    pth_min[ibar]=(float) atof(stringa2.Data());ifind++;}
	  if(stringa == Form("pth_max[%d]",ibar)){
	    pth_max[ibar]=(float) atof(stringa2.Data());ifind++;}
	  for (int ipar=0;ipar<2;ipar++){
	    if (ifind >0) break;
	    if(stringa == Form("Zextinction[%d][%d]",ibar,ipar)){
	      Zextinction[ibar][ipar]=(float) atof(stringa2.Data());ifind++;}
	    if(stringa == Form("zcal[%d][%d]",ipar,ibar)){
	      zcal[ipar][ibar]=(float) atof(stringa2.Data());ifind++;}
	    if(stringa == Form("tcal[%d][%d]",ipar,ibar)){
	      tcal[ipar][ibar]=(float) atof(stringa2.Data());ifind++;}
	    if(stringa == Form("zgcal[%d][%d]",ipar,ibar)){
	      zgcal[ipar][ibar]=(float) atof(stringa2.Data());ifind++;}
	    if(stringa == Form("TempCal[%d][%d]",ibar,ipar)){
	      TempCal[ibar][ipar]=(float) atof(stringa2.Data());ifind++;}
	    if(stringa == Form("TTimecal[%d][%d]",ipar,ibar)){
	      TTimecal[ipar][ibar]=(float) atof(stringa2.Data());ifind++;}
	    if(stringa == Form("ZTTimecal[%d][%d]",ipar,ibar)){
	      ZTTimecal[ipar][ibar]=(float) atof(stringa2.Data());ifind++;}
	    if(stringa == Form("Scal[%d][%d]",ibar,ipar)){
	      Scal[ibar][ipar]=(float) atof(stringa2.Data());ifind++;}
	    for (int isi=0;isi<2;isi++){
	      if (ifind >0) break;
	      if(stringa == Form("Qcal0[%d][%d][%d]",ipar,ibar,isi)){
		Qcal0[ipar][ibar][isi]=(float) atof(stringa2.Data());ifind++;}
	      if(stringa == Form("Qcal1[%d][%d][%d]",ipar,ibar,isi)){
		Qcal1[ipar][ibar][isi]=(float) atof(stringa2.Data());ifind++;}
	    }}}
	test = fin.peek();
	if( test == " " || test == '\n') fin.ignore(100, '\n');
	
	if(ifind==0 && maxerr<errmax) {
	  cerr << "AntiRecoPG::LoadCaliFile-E-parameter" << stringa << " wrong or not found" << endl;
	  maxerr++; }
      }
    }               
    if (maxerr<errmax) {
      cout << "AntiRecoPG::LoadCaliFile-I-LoadedInfosForAll " << itot+1 << "entries" << endl;
      maxerr++; }
    //expected 269 for data and 141 for MC  
    fin.close();
  }
  else{  // OLD PRE JUNE 2014 CALIBRATIONS
    // calibration constants for Z from trd                                                                                                                               
    cout << "AntiRecoPG::LoadCaliFile-I-USING OLD PRE-JUNE2014 CALIBRATIONS" << endl;
    if (rtp != 99) {
      zsme = 0.; // no extra smearing for data                                                                                                                               
      zcal[0][7]=(5.992457e+01)+4.; //par0                                            
      zcal[1][7]=(1.130916e+00); //par1                                                     
      zcal[0][6]=(5.870665e+01)+4.;
      zcal[1][6]=(1.171432e+00);
      zcal[0][5]=(6.044458e+01)+4.;
      zcal[1][5]=(1.154871e+00);
      zcal[0][4]=(6.137712e+01)+4.;
      zcal[1][4]=(1.162057e+00);
      zcal[0][3]=(3.778925e+01)+4.;
      zcal[1][3]=(1.130742e+00);
      zcal[0][2]=(3.097706e+01)+4.;
      zcal[1][2]=(1.163380e+00);
      zcal[0][1]=(3.403472e+01)+4.;
      zcal[1][1]=(1.172850e+00);
      zcal[0][0]=(3.107813e+01)+4.;
      zcal[1][0]=(1.144851e+00);
    }
    else{ // montecarlo                                                                                                                                                      
      zsme = 7.5; // cm artificial extra smearing for MC                                                                                                                   
      for (int sect=1;sect<9;sect++){
	zcal[1][sect-1]=1.;
	zcal[0][sect-1]=0.;
      }}
    // calibration constants for timing                                                                                                                                      
    tcal[1][0]=1.; //par1                                                                                                                                                    
    tcal[1][1]=1.;
    tcal[1][2]=1.;
    tcal[1][3]=1.;
    tcal[1][4]=1.;
    tcal[1][5]=1.;
    tcal[1][6]=1.;
    tcal[1][7]=1.;
    if (rtp != 99) {
      tsme = 0.; // no extra smearing for data                                                                                                                               
      tcal[0][0]=-10.5-(-1.984259e+01)+0.705622+0.1-0.36-0.61; //par0                                                                                                          
      tcal[0][1]=-4.5-(-1.616684e+01)+2.83776+0.2-1.6+0.64;
      tcal[0][2]=-4.5-(-1.629861e+01)+1.19891-0.2+0.37+0.71;
      tcal[0][3]=-10.5-(-1.885605e+01)-1.05101-0.3+1.58-0.3;
      tcal[0][4]=-10.5-(3.077705e+00)+0.581232+0.2-0.59-0.48;
      tcal[0][5]=-4.5-(6.362599e+00)+2.68788+0.3-1.71+0.41;
      tcal[0][6]=-4.5-(6.418757e+00)+1.2676-0.2+0.43+0.36;
      tcal[0][7]=-10.5-(3.750677e+00)-0.818199-0.3+1.32-0.74;
    }
    else{ // montecarlo                                                                                                                                                      
      tsme = 1.; // artificial extra smearing for MC                                                                                                                         
      tcal[0][0]=-4.18751+0.705622+0.1-1.37+0.25;
      tcal[0][1]=-0.92829+2.83776+0.2-1.06-0.06;
      tcal[0][2]=-0.87922+1.19891-0.2+1.16-0.16;
      tcal[0][3]=-4.19148-1.05101-0.3+0.88+0.07;
      tcal[0][4]=-4.274815+0.581232+0.2-1.37+0.29;
      tcal[0][5]=-1.190892+2.68788+0.3-0.52-0.03;
      tcal[0][6]=-1.152018+1.2676-0.2+1.52-0.3;
      tcal[0][7]=-4.325756-0.818199-0.3+0.77-0.05;
    }
    // calibration constants for Z from adc                                                                                                                                  
    zgsme = 0.; // no extra smearing for data                                                                                                                              
    zgcal[0][0]=(-5.114639e+00)+3.; //par0                                                                                                                                   
    zgcal[1][0]=(8.831339e-01); //par1                                                                                                                                       
    zgcal[0][1]=(2.450412e+00)+3.;
    zgcal[1][1]=(1.093610e+00);
    zgcal[0][2]=(3.722304e-01)+3.;
    zgcal[1][2]=(9.557022e-01);
    zgcal[0][3]=(4.104709e-01)+3.;
    zgcal[1][3]=(9.771197e-01);
    zgcal[0][4]=(-1.160993e+01)+3.;
    zgcal[1][4]=(8.794939e-01);
    zgcal[0][5]=(-4.695314e+00)+3.;
    zgcal[1][5]=(9.931499e-01);
    zgcal[0][6]=(-4.741135e+00)+3.;
    zgcal[1][6]=(8.653529e-01);
    zgcal[0][7]=(-9.100708e+00)+3.;
    zgcal[1][7]=(9.217961e-01);
    if (rtp == 99) { // is MC                                                                                                                                           
      zgsme = 40.; // artificial extra smearing for MC  
      float zgcal2[2][8];
      zgcal2[0][0]=(-2.587772e+01); zgcal2[1][0]=(4.250970e-01);
      zgcal2[0][1]=(-1.744702e+01); zgcal2[1][1]=(2.692123e-01);
      zgcal2[0][2]=(1.964827e+01); zgcal2[1][2]=(7.494298e-01);
      zgcal2[0][3]=(-1.465733e+01); zgcal2[1][3]=(3.389210e-01);
      zgcal2[0][4]=(-2.570675e+00); zgcal2[1][4]=(4.447068e-01);
      zgcal2[0][5]=(7.700909e+00); zgcal2[1][5]=(6.448281e-01);
      zgcal2[0][6]=(-1.664268e+00); zgcal2[1][6]=(3.102823e-01);
      zgcal2[0][7]=(-1.635550e+01); zgcal2[1][7]=(3.386805e-01);
      for(int sect=1;sect<=8;sect++){
	zgcal[1][sect-1]=zgcal[1][sect-1]*zgcal2[1][sect-1];
	zgcal[0][sect-1]=zgcal2[1][sect-1]*zgcal[0][sect-1]+zgcal2[0][sect-1];
      }}
    // calibration constants for E from adc                                                                                                                                  
    // E[sect-1][Side]=ADCAL[sect-1][side][0]+ADCAL[sect-1][side][1]*adc71                                                                                                   
    if (rtp != 99) { // not MC                                                                                                                                        
      ADCAL[7][1][0] = (-2.885599e+01);
      ADCAL[7][1][1] = (1.677857e-02);
      ADCAL[7][0][0] = (-1.368362e+01);
      ADCAL[7][0][1] = (1.660347e-02);
      ADCAL[6][1][0] = (-2.450851e+01);
      ADCAL[6][1][1] = (1.682045e-02);
      ADCAL[6][0][0] = (-3.224218e+01);
      ADCAL[6][0][1] = (2.258989e-02);
      ADCAL[5][1][0] = (-2.991301e+01);
      ADCAL[5][1][1] = (1.737330e-02);
      ADCAL[5][0][0] = (-2.344694e+01);
      ADCAL[5][0][1] = (1.817467e-02);
      ADCAL[4][1][0] = (-4.068939e+01);
      ADCAL[4][1][1] = (2.110805e-02);
      ADCAL[4][0][0] = (-2.252669e+01);
      ADCAL[4][0][1] = (1.686996e-02);
      ADCAL[3][1][0] = (-4.052459e+01);
      ADCAL[3][1][1] = (2.244979e-02);
      ADCAL[3][0][0] = (-1.527537e+01);
      ADCAL[3][0][1] = (1.664669e-02);
      ADCAL[2][1][0] = (-4.032059e+01);
      ADCAL[2][1][1] = (2.279973e-02);
      ADCAL[2][0][0] = (-2.518801e+01);
      ADCAL[2][0][1] = (1.982831e-02);
      ADCAL[1][1][0] = (-3.582119e+01);
      ADCAL[1][1][1] = (2.166176e-02);
      ADCAL[1][0][0] = (-2.089689e+01);
      ADCAL[1][0][1] = (1.901233e-02);
      ADCAL[0][1][0] = (-3.601734e+01);
      ADCAL[0][1][1] = (2.003254e-02);
      ADCAL[0][0][0] = (-1.467593e+01);
      ADCAL[0][0][1] = (1.499330e-02);
    }
    else{ // MC                                                                                                                                                            
      for(int sect=1;sect<=8;sect++){
	ADCAL[sect-1][0][0] = (-1.121383e+00);
	ADCAL[sect-1][1][0] = (-1.121383e+00);
	ADCAL[sect-1][0][1] = (3.731829e-03);
	ADCAL[sect-1][1][1] = (3.731829e-03);
      }
    }
    // calibration constants for Q from adc                                                                                                                                  
    if (rtp != 99) { // not MC                                                                                                                                      
      Etoqcc[0][0] = (-1.302855e+01);
      Etoqcc[1][0] = (-1.737939e+01);
      Etoqcc[2][0] = (-1.938383e+01);
      Etoqcc[3][0] = (-1.955287e+01);
      Etoqcc[4][0] = (-2.216669e+01);
      Etoqcc[5][0] = (-1.736887e+01);
      Etoqcc[6][0] = (-1.637681e+01);
      Etoqcc[7][0] = (-1.596862e+01);
      Etoqcc[0][1] = (9.982128e+00);
      Etoqcc[1][1] = (1.331861e+01);
      Etoqcc[2][1] = (1.428105e+01);
      Etoqcc[3][1] = (1.431181e+01);
      Etoqcc[4][1] = (1.757587e+01);
      Etoqcc[5][1] = (1.360795e+01);
      Etoqcc[6][1] = (1.229189e+01);
      Etoqcc[7][1] = (1.198994e+01);
      Etoqcc[0][2] = (-7.823282e-01);
      Etoqcc[1][2] = (-1.331476e+00);
      Etoqcc[2][2] = (-1.399119e+00);
      Etoqcc[3][2] = (-1.357421e+00);
      Etoqcc[4][2] = (-1.955539e+00);
      Etoqcc[5][2] = (-1.516216e+00);
      Etoqcc[6][2] = (-1.116547e+00);
      Etoqcc[7][2] = (-1.087966e+00);
    }
    else{ // MC                                                                                                                                                          
      for(int sect=1;sect<=8;sect++){
	Etoqcc[sect-1][0] = (-5.717292e+00);
	Etoqcc[sect-1][1] = (8.228021e+00);
	Etoqcc[sect-1][2] = (-1.248155e+00);
      }
    }
    //calibration constants for Z from adc for adc non linearity                                                                                                             
    // 3rd degree polinomial for describing par0 and par1                                                                                                                    
    zadcpar[7][0][0]=5.296932e+01; //par0 of par0                                                                                                                            
    zadcpar[7][0][1]=5.757388e-02; //par1 of par0                                                                                                                            
    zadcpar[7][0][2]=4.251760e-05; //par2 of par0                                                                                                                            
    zadcpar[7][0][3]=-7.357387e-09; //par3 of par0                                                                                                                           
    zadcpar[7][1][0]=-1.088640e+00;
    zadcpar[7][1][1]=-1.190032e-03;
    zadcpar[7][1][2]=-5.375866e-07;
    zadcpar[7][1][3]=1.123374e-10;
    zadcpar[6][0][0]=-1.730706e+00;
    zadcpar[6][0][1]=-6.911670e-02;
    zadcpar[6][0][2]=1.842045e-05;
    zadcpar[6][0][3]=-1.048745e-09;
    zadcpar[6][1][0]=1.678117e-01;
    zadcpar[6][1][1]=-3.246332e-03;
    zadcpar[6][1][2]=4.767933e-07;
    zadcpar[6][1][3]=-1.009630e-11;
    zadcpar[5][0][0]=2.494518e+01;
    zadcpar[5][0][1]=2.753340e-02;
    zadcpar[5][0][2]=6.565379e-06;
    zadcpar[5][0][3]=-1.256878e-09;
    zadcpar[5][1][0]=1.276284e+00;
    zadcpar[5][1][1]=-4.357025e-03;
    zadcpar[5][1][2]=5.184154e-07;
    zadcpar[5][1][3]=4.904257e-12;
    zadcpar[4][0][0]=-5.637283e+01;
    zadcpar[4][0][1]=1.457245e-01;
    zadcpar[4][0][2]=-2.227284e-05;
    zadcpar[4][0][3]=3.129885e-10;
    zadcpar[4][1][0]=7.717951e-01;
    zadcpar[4][1][1]=-3.350824e-03;
    zadcpar[4][1][2]=3.483621e-07;
    zadcpar[4][1][3]=1.075340e-11;
    zadcpar[3][0][0]=-1.048862e+02;
    zadcpar[3][0][1]=2.762670e-01;
    zadcpar[3][0][2]=-4.837396e-05;
    zadcpar[3][0][3]=1.432339e-09;
    zadcpar[3][1][0]=-9.504642e-01;
    zadcpar[3][1][1]=-1.693068e-03;
    zadcpar[3][1][2]=-1.085776e-07;
    zadcpar[3][1][3]=4.627746e-11;
    zadcpar[2][0][0]=-1.681855e+02;
    zadcpar[2][0][1]=2.906046e-01;
    zadcpar[2][0][2]=-8.793957e-05;
    zadcpar[2][0][3]=6.861332e-09;
    zadcpar[2][1][0]=-4.885449e-02;
    zadcpar[2][1][1]=-4.128466e-03;
    zadcpar[2][1][2]=6.334161e-07;
    zadcpar[2][1][3]=-1.229904e-11;
    zadcpar[1][0][0]=-4.689831e+01;
    zadcpar[1][0][1]=1.184588e-01;
    zadcpar[1][0][2]=-1.258733e-05;
    zadcpar[1][0][3]=-8.395488e-10;
    zadcpar[1][1][0]=-1.411851e-01;
    zadcpar[1][1][1]=-3.732621e-03;
    zadcpar[1][1][2]=7.340715e-07;
    zadcpar[1][1][3]=-3.706938e-11;
    zadcpar[0][0][0]=-1.263022e+02;
    zadcpar[0][0][1]=3.584499e-01;
    zadcpar[0][0][2]=-7.913037e-05;
    zadcpar[0][0][3]=4.284198e-09;
    zadcpar[0][1][0]=-8.438613e-01;
    zadcpar[0][1][1]=-1.853986e-03;
    zadcpar[0][1][2]=-3.229991e-08;
    zadcpar[0][1][3]=3.922388e-11;


    // approx bethe bloch correction par.                                                                                                                                    
    // dq = bcorr*fabs((1.-fabs(bbetahs))/bbetahs)                                                                                                                           
    bcorr = (5.792472e-01);

    // distance from border correction par (flat for dbo>1 (10cm))                                                                                                           
    // dbo=log10(40.-z_true)                                                                                                                                                 
    // dq = zcorr[0]+zcorr[1]*dbo+zcorr[2]*dbo*dbo+zcorr[3]*dbo*dbo*dbo                                                                                                      
    zcorr[0]=(-5.165454e-01);
    zcorr[1]=(2.461939e+00);
    zcorr[2]=(-3.811314e+00);
    zcorr[3]=(1.955971e+00);
    //Cos(theta) fine correction (direct term in energy)                                                                                                                     
    // dq = ttcorr[0]+|ttcorr[1]|*cost+ttcorr[2]*cost*cost+ttcorr[3]*|cost*cost*cost|                                                                                        
    ttcorr[0]=(-8.520307e-01);
    ttcorr[1]=(7.003510e+00);
    ttcorr[2]=(-1.598534e+01);
    ttcorr[3]=(1.046251e+01);

  }

    ifild = 2*ifild;  
  return 1;
}
  

void AntiRecoPG::InitPar(int version, int Istolerant){ 
  _htdcbw=0.0244; // from antidbc02.C
  _lspeed=14.7; // from job.C
  _tguess = -130.;
  _etime = 30.; //ns

  for(int isect=1;isect<=8;isect++){
    _zguess[isect-1] = 0.;
    _ezeta[isect-1] = 40.; //(40 cm RMS 35cm sigma of zguess from ADC || 10cm RMS 6cm sigma Z from timepairs)
    for(int itt=0;itt<256;itt++){
      chilist[itt][isect-1]=-1.; // global chisquare list
      timelist[itt][isect-1]=0.;
      zetalist[itt][isect-1]=0.;
    }
  }
    // maximum time for calibrations (beyond is extrapolation)
    timecallimit = 0;
  tempavg = 0;
  int run_type = 0;

  NVER = version;
  itolerance = Istolerant;
  ifild = 1;  // file to be loaded
  //  cout << "pre initcal" << endl;
  InitCal(run_type,NVER);
  //  cout << "post initcal" << endl;

  evto = 0;
  runo = 0;
  nrsd = 0; 
  return;
}

/* unfolding of Zcoo knowing that the real Zcoo are in the [-40,40] range \n
  gaussian distribution assumption \n
  example for unfolding Zcoo from ADC (RMS 40cm): BayesEstimation(zadc,45,-40.,40.) \n
  example for unfolding Zcoo from TDC (RMS 10cm): BayesEstimation(ztdc,8,-40.,40.)
*/
float AntiRecoPG::BayesEstimation(float value, float error, float low, float upp){
  if (error==0.) return 0.;
  if (low>=upp) return low;
  // variable normalization
  double xm=value/error;
  double xl=low/error-xm;  // lower integration edge
  double xu=upp/error-xm;  // upper integration edge
  double numerat = exp(-xl*xl/2.)-exp(-xu*xu/2.);
  double normaliz = sqrt(3.141592654/2.)*(erf(xu/sqrt(2.))-erf(xl/sqrt(2.)));
  double vret = value;
  if (normaliz != 0.) vret = error*(numerat/normaliz)+vret;
  if (vret>upp) vret = upp;
  if (vret<low) vret = low;
  return vret;
}

// evaluate a guess of event time averaging tof times
 float AntiRecoPG::GetTofTimeGuess(){
  AMSEventR* evt = AMSEventRHead()?AMSEventRHead():AMSEventR::Head();
  
  float tguess = 0.;
  int nguess = 0;
  for(unsigned int i=0; i<evt->NTofClusterH(); i++){
    TofClusterHR* ptcl=evt->pTofClusterH(i);
    if( !ptcl) continue;
    tguess+=(ptcl->Time);
    nguess++;
  }
  if (nguess>0) {tguess=tguess/(1.* nguess);}
  else {tguess = -130.;}
  return tguess;
}

// evaluate timing correction 
float AntiRecoPG::GetTimingTcorr(int sect){
  if (NVER<0) return 0;
  AMSEventR* evt = AMSEventRHead()?AMSEventRHead():AMSEventR::Head();
  double timenow = evt->fHeader.Time[0];
  // cout << "letto fheder" << endl;
  int bin = Timecal[sect-1]->FindBin(timenow);
  // cout << "usato gidto" << endl;
  double tshift = Timecal[sect-1]->GetBinContent(bin);
  double terr = Timecal[sect-1]->GetBinError(bin);
  if (tshift == 0 || terr==0 || terr>2 || timenow>timecallimit) tshift=GetExtrapolation(Timecal[sect-1],timenow,TTimecal[0][sect-1],TTimecal[1][sect-1]);
  return tshift;
}

float AntiRecoPG::GetExtrapolation(TH1D* h, double xx,double p0, double p1){
  if (timecallimit == 0) {
    for (int ibin=h->GetNbinsX(); ibin>0;ibin--) {
      if (h->GetBinContent(ibin)!=0) {
	timecallimit=h->GetBinCenter(ibin);
	break;
      }
    }
  }
  
  if (xx>=timecallimit) {
    if (maxerr<errmax) {  cout << "AntiRecoPG::GetExtrapolation-E-extrapolating at time " << xx << " beyond calibration limit " << timecallimit << endl;
      maxerr++; }
    return xx*p1+p0;
  } else // interpolation
    {
      int bin = h->FindBin(xx);
      if (bin<1) return h->GetBinContent(1);
      int bleft = bin;
      int bright = bin;
      for (int ibin=bin; ibin>0;ibin--) {
	if (h->GetBinContent(ibin)!=0 && h->GetBinError(ibin)!=0) {
	  bleft=ibin;
	  break;
	}
      }
      for (int ibin=bin; ibin<h->GetNbinsX();ibin++) {
	if (h->GetBinContent(ibin)!=0 && h->GetBinError(ibin)!=0) {
	  bright=ibin;
	  break;
	}
      }
      if (bright==bleft) return h->GetBinContent(bright); 
      double xright = h->GetBinCenter(bright);
      if (xright>=timecallimit) {
	if (maxerr<errmax) {  cout << "AntiRecoPG::GetExtrapolation-E-extrapolating at time " << xx << " at calibration limit " << timecallimit << endl;
	  maxerr++; }
	return xx*p1+p0;
      }
      double yright = h->GetBinContent(bright);
      double xleft = h->GetBinCenter(bleft);
      double yleft = h->GetBinContent(bleft);
      return yleft+(yright-yleft)/(xright+xleft);
    }
  }

// evaluate average ACC temperature
float AntiRecoPG::GetAvgTempAcc(){

  if (rtp == 0 || rtp == 99) return 0; //montecarlo or still not initiated

  AMSEventR* evt = AMSEventRHead()?AMSEventRHead():AMSEventR::Head();

  double timenow = evt->fHeader.Time[0];
  if ((timenow-temptime) < 60. && tempavg != 0) return tempavg;
  // > 1 minute sampling

  double tret = 0;
  double nsum = 0;
  for (int indt=0;indt<4;indt++){
    float sensor=0;
    int igret = 0;
    vector<float> values;
    if (indt == 0)  igret = evt->GetSlowControlData ("M-3X:14:ACC+ZWake", values, 1);
    if (indt == 1)  igret = evt->GetSlowControlData ("M-3X:20:ACC-ZWake", values, 1);
    if (indt == 2)  igret = evt->GetSlowControlData ("M-3X:27:ACC-ZRam", values, 1);
    if (indt == 3)  igret = evt->GetSlowControlData ("M-3X:5:ACC+ZRam", values, 1);
    if (igret != -2 && values.size()>0){
      if(values[0]!=0) {sensor = values[0];}
      else {if(values[1]!=0) {sensor = values[1];}}
      if (sensor != 0) {
	tret = tret + sensor;
	nsum = nsum+1;}
    }}
    if (nsum == 4) {
      tret = tret/nsum;
      temptime = timenow;
    }
    else {tret = tempavg;}
  return tret;
  }

// reload all info from AntiRawSides
//fill timetable and timeindex
int AntiRecoPG::ReLoadAcc(){  
  int iret = 0; // no raw sides
  AMSEventR* evt = AMSEventRHead()?AMSEventRHead():AMSEventR::Head();
  if (int(evt->Event()) == evto && int(evt->Run()) == runo) return nrsd;
  if (int(evt->Run()) != runo || ifild>0 || ifild%2 != 0){ // needs to reload calibrations
  int run_type = SelectRun();
  InitCal(run_type,NVER);}
  
  if (evt->NAntiRawSide()!=0){
  for(int isect=1; isect<=8; isect++){ //clear timetable 
    for(int k=0; k<2; k++){ //side index
      timeindx[0][k][isect-1]=0; // index for FT times
      timeindx[1][k][isect-1]=0;  // index for TDC times
      adctable[k][isect-1]=0.; // adc values
      for(int i=0; i<16; i++){
	timetable[i][k][isect-1]=0; }}}
  for(unsigned int ir=0; ir<evt->NAntiRawSide(); ir++){
    AntiRawSideR* rsd = evt->pAntiRawSide(ir);
    if( !rsd) continue;
    int swid = rsd->swid;
    int side = swid%10-1;
    int sect = swid/10;
    if ((sect <1 || sect >8) && (maxerr<errmax)) {
      cerr << "AntiRecoPG::ReLoadAcc-E open ERRROR sect index" << endl; 
      maxerr++;}
    adctable[side][sect-1]=rsd->adca;
    if ((timeindx[1][side][sect-1]!=0 || timeindx[1][side][sect-1]!=0) && maxerr<errmax)  {
      cerr << "AntiRecoPG::ReLoadAcc-E open Error double hit" << endl;
      maxerr++;}
    timeindx[1][side][sect-1]=rsd->ntdct;
    if (timeindx[1][side][sect-1]>16) timeindx[1][side][sect-1]=16;
    int iftt=rsd->nftdc-1;
    if (iftt>7) iftt=7;
    timeindx[0][side][sect-1]=iftt+1;
    if(iftt<0) {
      if (maxerr<errmax)  {
	cerr << "AntiRecoPG::ReLoadAcc-E open Error null or negative number of #FT= " << iftt+1 << "| Run= " << evt->Run() << " event= " << evt->Event() << endl;
	maxerr++;}
    }
    else{
    for (int itdc = 0; itdc<rsd->ntdct; itdc++){
      if (itdc>15) continue;
      timetable[itdc][side][sect-1]=(rsd->tdct[itdc])-(rsd->ftdc[iftt]); 
      iret++;}
    }
  }}
  evto = evt->Event();
  runo = evt->Run();
  nrsd = iret;
  if (iret!=0){
  float tgg=GetTofTimeGuess();
  SetTGuess(tgg,30);  
  }
  tempavg = GetAvgTempAcc();
  return iret; // side loaded
}

// scan the time history searching the best pairing respect to T&Z guess
int  AntiRecoPG::ScanThePairs(int sect){
  npairs[sect-1]=0;
  for (int itt=0;itt<256;itt++){
    if (chilist[itt][sect-1]==-1 && zetalist[itt][sect-1]==0 &&  timelist[itt][sect-1]==0) break; //fast already clean
    chilist[itt][sect-1]=-1;
    zetalist[itt][sect-1]=0;
    timelist[itt][sect-1]=0;
  }  // clean the lists

  if (!(timeindx[0][0][sect-1]<=0 || timeindx[1][0][sect-1] <= 0 || timeindx[0][1][sect-1]<=0 ||timeindx[1][1][sect-1] <= 0)){
    for (int itdc0 = 0; itdc0<timeindx[1][0][sect-1]; itdc0++){ // loop on possible tdc side 0  
      double tside0 = 1.*(timetable[itdc0][0][sect-1])*_htdcbw;
      for (int itdc1 = 0; itdc1<timeindx[1][1][sect-1]; itdc1++){ // loop on possible tdc side 1  
	double tside1 = 1.*(timetable[itdc1][1][sect-1])*_htdcbw;
	double ttime = 0.5*(tside0+tside1)*tcal[1][sect-1]+tcal[0][sect-1];
	double zzeta = 0.5*(tside0-tside1)*_lspeed*zcal[1][sect-1]+zcal[0][sect-1];

	DoThePair(ttime,zzeta,sect);
	npairs[sect-1]=npairs[sect-1]+1;
      }}}
  else {// impossible pairing single side
    if ((timeindx[1][1][sect-1]<=0 || timeindx[0][1][sect-1]<=0) && (timeindx[1][0][sect-1]<=0 || timeindx[0][0][sect-1]<=0)) {npairs[sect-1]=0;}
    else{  // SINGLE SIDE PAIRING and use Z from Guess
      int sideon = 1;
      if (timeindx[1][1][sect-1]<=0 || timeindx[0][1][sect-1]<=0) sideon = 0;
      for (int itdc = 0; itdc<timeindx[1][sideon][sect-1]; itdc++){ // loop on possible tdc side on  
	double tside = 1.*(timetable[itdc][sideon][sect-1])*_htdcbw;
	double delta = 2.*(_zguess[sect-1]-zcal[0][sect-1])/(_lspeed*zcal[1][sect-1]); // reconstructing time diff from zguess
	if (sideon==1) delta = -1.*delta;
	double ttime = 0.5*(2.*tside-delta)*tcal[1][sect-1]+tcal[0][sect-1];
	double zzeta = _zguess[sect-1];
	DoThePair(ttime,zzeta,sect);
	npairs[sect-1]=npairs[sect-1]-1;
      }}}
  return npairs[sect-1];
}

// evaluate a pair (or a single time) with chisquare minimization respect to T&Z guess
int  AntiRecoPG::DoThePair(double ttime, double zzeta, int sect){
  ttime = ttime+tsme*gRandom->Gaus(); // artificial spread for MC
  zzeta = zzeta+zsme*gRandom->Gaus(); // artificial spread for MC
  int iret = 1;
  if ((_etime == 0. ||  _ezeta[sect-1] == 0.) && (maxerr<errmax))  {
    cerr << "AntiRecoPG::DoThePair-E guess error are 0 reset to std" << endl;
    maxerr++;}
  if (_etime == 0. ||  _ezeta[sect-1] == 0.) iret = 0; 
  if (_etime == 0.) _etime=30; // savatage reset
  if ( _ezeta[sect-1] == 0.) _ezeta[sect-1]=35.; // savatage reset
  double chi = ((ttime-_tguess)/_etime)*((ttime-_tguess)/_etime);
  if (fabs(zzeta)>40) {chi = chi + ((fabs(zzeta)-40.)/10.)*((fabs(zzeta)-40)/10.); // over the border 10 cm sigma
  }
  else
    {
  chi = chi + ((zzeta-_zguess[sect-1])/_ezeta[sect-1])*((zzeta-_zguess[sect-1])/_ezeta[sect-1]);
    }
  int tobeinsert=1;
  for (int itt=0;itt<256;itt++){
    if (chilist[itt][sect-1]<0 || chilist[itt][sect-1]>chi){
      if (chilist[itt][sect-1]>0){ // need space for insertion
	int ittmax=abs(npairs[sect-1])+1;  if (ittmax>254) ittmax=254;
	for (int itt2=ittmax;itt2>=itt;itt2--){
	  if (chilist[itt2][sect-1]>0){ // make space for insertion
	    chilist[itt2+1][sect-1]=chilist[itt2][sect-1];
	    timelist[itt2+1][sect-1]=timelist[itt2][sect-1];
	    zetalist[itt2+1][sect-1]=zetalist[itt2][sect-1];}
	}}
      chilist[itt][sect-1]=chi;  // insert the new value
      timelist[itt][sect-1]=ttime;
      zetalist[itt][sect-1]=zzeta;
      tobeinsert=0;
      iret=1;
    }
    if (tobeinsert==0) break;
  }
  return iret;// return 0 if errors set to 0
}  


// evaluate a guess to Zcoo from Anti ADC sides (35 cm RMS)
float AntiRecoPG::GetAdcZGuess(int sect){
  float zguess = 0.;
  if (NVER>=0){
    float fufi_0 = DoSideRawEdep(sect,0,0,1);
    float fufi_1 = DoSideRawEdep(sect,1,0,1);
    if ((fufi_0 <= 0. && fufi_1<= 0)) return 0;  
    if (fufi_0 > 35.) fufi_0 = 35.;  
    if (fufi_1 > 35.) fufi_1 = 35.;
    
    float asym = (fufi_1-fufi_0)/(fufi_1+fufi_0);
    zguess=(asym-zgcal[0][sect-1])/zgcal[1][sect-1];
    zguess = zguess+zsme*gRandom->Gaus();
    if (((adctable[0][sect-1])>3000 || (adctable[1][sect-1])>3000) && fabs(zguess)>40) return 0.;
  }
  else{ // OLD PRE JUNE 2014 version
    float adcs=adctable[0][sect-1]+adctable[1][sect-1];
    float adcd=adctable[0][sect-1]-adctable[1][sect-1]; // check the sign    
    float par0=zadcpar[sect-1][0][0]+zadcpar[sect-1][0][1]*adcs+zadcpar[sect-1][0][2]*adcs*adcs+zadcpar[sect-1][0][3]*adcs*adcs*adcs;
    float par1=zadcpar[sect-1][1][0]+zadcpar[sect-1][1][1]*adcs+zadcpar[sect-1][1][2]*adcs*adcs+zadcpar[sect-1][1][3]*adcs*adcs*adcs;
    if(par1!=0.) zguess=zgcal[0][sect-1]+zgcal[1][sect-1]*(adcd-par0)/par1;
    zguess = zguess+zsme*gRandom->Gaus();
  }
  return zguess;
}

// raw evaluation of energy deposition pairing ADC sides corrected for Temperature and Zcoo effects (not used)
float AntiRecoPG::DoRawEdep(int sect, float zcoo){
  float e0=0.;
  float e1=0.;
  float medep = 0;
  if (NVER>=0){
    e0=DoSideRawEdep(sect,0,zcoo,1);
    e1=DoSideRawEdep(sect,1,zcoo,1);
    if (e0>0) medep = medep+e0;
    if (e1>0) medep = medep+e1;
    if(e0>0 && e1>0) medep = 0.5*medep; // dual sided
    if(e0<0 && e1<0) medep = 60; //large fixed and unknown both sides saturation
  }
  else{ // OLD PRE JUNE 2014 version
    if (adctable[0][sect-1]>0) e0=(ADCAL[sect-1][0][0]+adctable[0][sect-1]*ADCAL[sect-1][0][1]);
    if (adctable[1][sect-1]>0) e1=(ADCAL[sect-1][1][0]+adctable[1][sect-1]*ADCAL[sect-1][1][1]);
    medep = (e0+e1);
    if(adctable[0][sect-1]>0 && adctable[1][sect-1]>0) medep = 0.5*medep; // dual sided   
  }
  return medep;
}

// raw evaluation of energy deposition single ADC sides (for internal correcting nonlinearity temperature and Zcoo effects)
float AntiRecoPG::DoSideRawEdep(int sect, int side,float zcoo, int timecorr){
  if (NVER<0) return 0;
  if (rtp==0 || ifild>0 || ifild%2 != 0){ // needs to reload calibrations
    int run_type = SelectRun();
    InitCal(run_type,NVER);}

   double epsilon = 1;
   if ((timecorr == 1 && rtp != 99) || (timecorr==2)) {
     AMSEventR* evt = AMSEventRHead()?AMSEventRHead():AMSEventR::Head();
     double timenow = evt->fHeader.Time[0];
     int bin = Qcal[0][sect-1][side]->FindBin(timenow);
     double qm1 = Qcal[0][sect-1][side]->GetBinContent(bin);
     double qm2 = Qcal[1][sect-1][side]->GetBinContent(bin);
     double qerr = Qcal[0][sect-1][side]->GetBinError(bin);
     if (qm1 == 0 || qerr==0 || qerr>2 || timenow>timecallimit) {
       qm1=GetExtrapolation(Qcal[0][sect-1][side],timenow,Qcal0[0][sect-1][side],Qcal1[0][sect-1][side]);
       qm2=GetExtrapolation(Qcal[1][sect-1][side],timenow,Qcal0[1][sect-1][side],Qcal1[1][sect-1][side]);
     }
     epsilon = Scal[sect-1][side]*((qm2*qm2-qm1*qm1)/3.-1.)+1.;  //corrective parameter
   }
   if (epsilon==0) { 
     if (maxerr<errmax) {
       cerr << "AntiRecoPG::DoSideRawEdep-E error: null epsilon" << endl;
       maxerr++;}
     epsilon = 1.;
   }

   double linpar = ADCAL[sect-1][side][1]*epsilon;
   double floga = (1.-ADCAL[sect-1][side][2]*((adctable[side][sect-1]-ADCAL[sect-1][side][0])/linpar));
   /*
   cout << "*******************"<< endl;
   cout << side << " =side | sect= "<<sect<< " adctable = " << adctable[side][sect-1]<<endl;
   cout << ADCAL[sect-1][side][0] << " " << ADCAL[sect-1][side][1] << " " << ADCAL[sect-1][side][2] << endl;
   cout << floga << " =floga" << endl;
   */
   if (floga<=0) return -1; // overflow the saturation
   double medep = -log(floga)/ADCAL[sect-1][side][2];
   if (medep<0) {medep = 0;}
   else {
     if (zcoo>40) zcoo = 40;
     if (zcoo<-40) zcoo = -40;
     float zetacor = 1+Zextinction[sect-1][side]*zcoo+Zborders*zcoo*zcoo*zcoo*zcoo;
     tempavg = GetAvgTempAcc();
     float tempcor = 1+tempavg*TempCal[sect-1][side];
     /*
   cout << medep << " =edep" << endl;
   cout << zetacor << " =zetacor" << endl;
   cout << tempcor << " =tempcor" << endl;
   cout << "*******************"<< endl;
     */
     if (zetacor >0 ) medep = medep/zetacor;
     if (tempcor >0 ) medep = medep/tempcor;
   }
  return medep;
}

/* re-build a cluster pairing list based on T&Z guess \n
  ttguess = 999 and zzguess = 999 then use the default values (from ToF and ADC) \n 
  reccommended err_zguess>=10 cm (TDCZ RMS) for precise Zguess (from trk) \n
  reccommended err_zguess=35cm for minimal (default) zguess from ADC \n
  return number of pairs. if <0 then only single side time history
*/  
int AntiRecoPG::BuildCluster(AntiClusterR* cluster, int sect, float zzguess, float err_zguess, float ttguess, float err_tguess){ 
  //  printf("BuildCluster: %d\n", sect);//only for debug
  int nld = ReLoadAcc();
  if (nld==0) return 0;
  float zg = zzguess;
  float ezg = err_zguess;
  float zadc = GetAdcZGuess(sect); 
  float zadc_unf = (zadc==0. ? 0. : BayesEstimation(zadc,45,-40.,40.));
  if (fabs(zg)>40.) {  // default guess from ADC
    zg = zadc_unf;
    ezg = 45.;}
  SetZGuess(zg,ezg,sect);
  if (ttguess != 999) SetTGuess(ttguess,err_tguess);
  int npa = ScanThePairs(sect);
  if (npa != 0) {
    if (!cluster) cluster = new AntiClusterR;//FIX ME: This function is never used "internally" with cluster=0, but if a user was calling it in this way the value of "cluster" would never be "returned".
    cluster->Npairs = npa;
    cluster->Sector = sect;
    cluster->zeta = ((npa)>0 ? GetZeta(sect,0) : zadc);
    cluster->unfzeta = ((npa)>0 ? BayesEstimation(cluster->zeta,8.,-40.,40.) : zadc_unf);
    cluster->adc[0] = (int)(adctable[0][sect-1]);
    cluster->adc[1] = (int)(adctable[1][sect-1]);
    cluster->rawDepSide[0] = DoSideRawEdep(sect,0,cluster->unfzeta);
    cluster->rawDepSide[1] = DoSideRawEdep(sect,1,cluster->unfzeta);
    if (NVER>=0) {
      cluster->rawdep = cluster->PairDep(); }
    else { // OLD PRE JUNE 2014 version
      cluster->rawdep = DoRawEdep(sect);}

    cluster->time = GetTime(sect,0); 
    cluster->chi = GetChisquare(sect,0);
    cluster->next_time = ((abs(npa)>1) ? GetTime(sect,1) : 0);
    cluster->next_zeta = (((npa)>1) ? GetZeta(sect,1) : 0);
    cluster->next_chi = ((abs(npa)>1) ? GetChisquare(sect,1) : 1.e6);
    cluster->next_unfzeta = (((npa)>1) ? BayesEstimation(cluster->next_zeta,8.,-40.,40.) : 0);
    cluster->zeta_adc = zadc;
    cluster->unfzeta_adc = zadc_unf;
    cluster->phi = cluster->FillCoo(& cluster->AntiCoo);
    if (NVER>=0) {
      cluster->rawq = cluster->PairedCharge();}
    else { // OLD PRE JUNE 2014 version
      cluster->rawq = cluster->Charge();
    }
    cluster->Edep = cluster->GetEdep();
    cluster->Status=0;
    if (cluster->chi>30.0) cluster->Status|=1;
    if (cluster->chi>30.0) cluster->Status|=128;
    if ((cluster->adc[0]==0) || (cluster->adc[1]==0)) cluster->Status|=2;
    if ((cluster->adc[0]==0) && (cluster->adc[1]==0)) cluster->Status|=4;
    if (cluster->Npairs<0) cluster->Status|=1024;
    //    printf("Sector %d) Npairs=%d, Chisq=%f, Status=%x\n", cluster->Sector, cluster->Npairs, cluster->chi, cluster->Status);//only for debug
  }
  return npa;
}

/* rebuild all cluster with default Z&T guess (from Tof and ADC) \n
  clear all old clusters \n
  return the number of anticlusters \n
  For a single defined sect a partucular zguess can be set
*/  
int AntiRecoPG::BuildAllClusters(int sect, float sect_zguess, float err_sect_zguess){
  //  printf("Anti: sect = %d\n");//only for debug
  ppAntiClusterPG.clear();
  int nacc=0;
  for (int isect=1;isect<=8;isect++){
    AntiClusterR* cluster = new AntiClusterR;
    int nbc;
    //    printf("Anti: trying isect = %d\n", isect);//only for debug
    if (sect!=0 && isect==sect) {
      //      printf("Anti (1)\n");//only for debug
      nbc = BuildCluster(cluster, isect, sect_zguess, err_sect_zguess);
    }
    else {
      //      printf("Anti (2)\n");//only for debug
      nbc = BuildCluster(cluster, isect);
    }
    if (nbc != 0) {
      cluster->Info(nacc);
      nacc++;
      ppAntiClusterPG.push_back(*cluster);
      //      printf("Anti (3)\n");//only for debug
    }
    delete cluster;
  }
  //  printf("Anti (after loop)\n");//only for debug
  nAntiClusterPG = nacc;
  return nacc;  // total number of anticounter

}


/* return Zcoo at choice pairing position. \n                                          
     If choice=0 is the best pairing zcoo respect to T&Z guess \n                        
   timecorr = 0 no time dependence correcxtion \n                                        
   timecorr = 1 apply the timecorrection to zeta to data \n                              
   timecorr = 2 force the time correction also if is MC */
float AntiRecoPG::GetZeta(int sect, int choice, int timecorr) {
  double ztimecor = 0;
  if ((NVER>=0 && timecorr == 1 && rtp != 99) || (NVER>=0 && timecorr==2)) {
    AMSEventR* evt = AMSEventRHead()?AMSEventRHead():AMSEventR::Head();
    double timenow = evt->fHeader.Time[0];
    int bin = ZTimecal[sect-1]->FindBin(timenow);
    double zshift = ZTimecal[sect-1]->GetBinContent(bin);
    double zerr = ZTimecal[sect-1]->GetBinError(bin);
    if (zshift == 0 || zerr==0 || zerr>2 || timenow>timecallimit) zshift=GetExtrapolation(ZTimecal[sect-1],timenow,ZTTimecal[0][sect-1],ZTTimecal[1][sect-1]);
    ztimecor = zshift;
  }
  return zetalist[choice][sect-1]-ztimecor;
}


/* return time at choice pairing position. If choice=0 is the best pairing time respect to T&Z guess  \n
   the deafult is slewrate = 1 i.e. apply the slew rate correction   \n                                       
   when phiGuess >0 the 1.16ns correction of the time depending of pad in the sector is applied\n
   (normally it is not done, and correction is applied in dobeta if necessary) \n    
*/

float AntiRecoPG::GetTime(int sect, int choice, int slewrate, int timecorr, float pgs) {

  double trisecor = 0;
  if (slewrate == 1 && NVER>=0) { //default slewrate=1
    if (rtp==99 && zgsme==0.) InitCal(99,NVER);
  double adc0 = (adctable[0][sect-1]);
  double adc1 = (adctable[1][sect-1]);
  double adcm = (adc0+adc1);
  if (adcm>0) {
    if (fabs((adc0-adc1)/adcm)<0.5) adcm = adcm/2.;} // if both side adc reasonably on
  trisecor = exp(slr[0]+adcm*slr[1])+slr[2]+adcm*slr[3]-slr[4];
  }
  
  float phicor = 0;
  if(pgs>=0. && pgs<=360. && NVER>=0) {
    float padspread;
    // average shift (1.16 nanoseconds) of a pad from the sector mean time 
    padspread = 1.16; 
    if (rtp==99) padspread=padspread*1.2;  //MC correction 1.4ns
    phicor = -padspread;
    if (sect == 3 || sect == 4 || sect == 7 || sect == 8) phicor = padspread;
    float phicnt = 45.*(sect-1)+22.5;
    if(pgs-phicnt>0) phicor = -phicor;
  }

  if ((timecorr == 1 && rtp != 99 && NVER>=0) || (NVER>=0 && timecorr==2)) trisecor = trisecor + GetTimingTcorr(sect); // default timecorr=1
   return timelist[choice][sect-1]-trisecor+phicor;
}


#else // #ifdef __USEANTICLUSTERPG__

AntiClusterR::AntiClusterR(AMSAntiCluster *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  Status = ptr->_status;
  Sector = ptr->_sector;
  Ntimes = ptr->_ntimes;
  Npairs = ptr->_npairs;
  //Time.clear();
  //TimeE.clear();
  //for(int i=0;i<ptr->_ntimes;i++){
  //  Time.push_back(ptr->_times[i]);
  //  TimeE.push_back(ptr->_etimes[i]);
  //  }
  for(int i=0;i<sizeof(Times)/sizeof(Times[0]);i++)Times[i] = i<Ntimes?ptr->_times[i]:0;
  for(int i=0;i<sizeof(Timese)/sizeof(Timese[0]);i++)Timese[i] = i<Ntimes?ptr->_etimes[i]:0;
  
  Edep   = ptr->_edep;
  for (int i=0; i<3; i++) Coo[i] = ptr->_coo[i];
  for (int i=0; i<3; i++) ErrorCoo[i] = ptr->_ecoo[i];
#endif
}

#endif //#ifdef __USEANTICLUSTERPG__
