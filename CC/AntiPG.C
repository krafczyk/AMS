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
  beta > 0 if Acc time is smaller than ExtTime */
float AntiClusterR::DoBeta(AMSPoint* ExtCoo, float ExtTime, float& beta, float& costh, AMSPoint* ForcedCoo){
  float dx;
  float dy;
  float dz;
  float xcoo;
  float ycoo;
  if (!ForcedCoo){
  dx = ExtCoo->x()-AntiCoo[0];
  dy = ExtCoo->y()-AntiCoo[1];
  dz = ExtCoo->z()-AntiCoo[2];
  xcoo = AntiCoo[0];
  ycoo = AntiCoo[1];
  }
  else
    {
    dx = ExtCoo->x()-ForcedCoo->x();
    dy = ExtCoo->y()-ForcedCoo->y();
    dz = ExtCoo->z()-ForcedCoo->z();
    xcoo = ForcedCoo->x();
    ycoo = ForcedCoo->y();
    }

  float dist = sqrt(dx*dx+dy*dy+dz*dz);
  if (ExtTime != time) beta = dist/((29.9792458)*(ExtTime-time));
  float cos = xcoo*dx+ycoo*dy;
  if (dist > 0.) costh = cos/(54.95*dist); 
  return dist;
}

// return the acc phi angle and fill AntiCoo AMSPoint with AntiCoo[2]=unfzeta 
float AntiClusterR::FillCoo(AMSPoint* AntiCoo){
  float phi = 22.5+45.*(Sector-1);
  float zz = unfzeta;
  float xx = 54.95*cos(phi*3.141592654/180.);
  float yy = 54.95*sin(phi*3.141592654/180.);
  AntiCoo->setp(xx,yy,zz);
  return phi;
}

/* return AntiCluster charge with corrections \n
  costh = cosine track inclination respect to Anti Pad \n
  ztrue = true Zcoo (e.g. if known from tracker)
*/
float AntiClusterR::Charge(float costh, float beta, float ztrue){
  float bcorr = AntiRecoPG::gethead()->bcorr;   // trick: reload calibrations par from Acc reconstruction
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
  float qret = qraw/(2.*Etoqcc[Sector-1][2]);
  if (fabs(beta)>1) beta=1.;
  if (fabs(beta)<0.4) beta=0.4; // slow cutoff
  if (beta != 1. && beta != 0.) qret = qret - bcorr*fabs((1.-fabs(beta))/beta); //bethe block approx
  if (ztrue !=0. && fabs(ztrue)<40.){
    float dtb = 40.-fabs(ztrue);
    if (dtb>10.) dtb = 10.; // dtb > 10 still not investigated
    dtb = log10(dtb);
    float qzcorr = zcorr[0]+zcorr[1]*dtb+zcorr[2]*dtb*dtb+zcorr[3]*dtb*dtb*dtb; 
    qret = qret - qzcorr; // small distance from border correction
  } 
  if (costh != 0.45){  
    float act = fabs(costh);
    //    if (act<0.1) act = 0.1;
    //    if (act>0.7) act = 0.7;
    float qttcorr = ttcorr[0]+ttcorr[1]*act+ttcorr[2]*act*act+ttcorr[3]*act*act*act; 
    qret = qret - qttcorr; // fine costheta correction (outside [0.1 to 0.7] still not investigated)
  }
  if (qret <0.) qret = 0;
  
  return qret;
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

AntiRecoPG::AntiRecoPG(){
  InitPar();
}

AntiRecoPG* AntiRecoPG::gethead() {
  if (!head) {
    printf("AntiRecoPG::gethead()-M-CreatingObjectAntiRecoPGAsSingleton\n");
    head = new AntiRecoPG();//this also initialize at the default version
    if(!AMSEventRHead())AMSEventRHead()=AMSEventR::Head();
  }
  return head;
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


/* file filename contains a correction to existing calibrations \n
  still preliminary to be develop if necessary
*/
int AntiRecoPG::CalOnTopOfCal(char* filename)
{ int iret = 0;   
  float zgcal2[2][8];
  float zcal2[2][8];
  float tcal2[2][8];

  ifstream myfile (filename);
  if (!myfile)
    {   iret=-1;  
      if (maxerr<errmax) {
	cerr << "AntiRecoPG::CalOnTopOfCal-E open file" << filename << "error" << endl;
	maxerr++;}
    }
  else{
    for(int sect=1;sect<=8;sect++){
      for(int par=0;par<2;par++){
	if (! myfile.eof() ) {myfile >> zgcal2[par][sect-1];}
	else{ iret = -1;
	  if (maxerr<errmax) {
	    cerr << "AntiRecoPG::CalOnTopOfCal-E file" << filename << "too short" << endl;
	    maxerr++;}
	}}}

    for(int sect=1;sect<=8;sect++){
      for(int par=0;par<2;par++){
	if (! myfile.eof() ) {myfile >> zcal2[par][sect-1];}
	else{ iret = -1;
	  if (maxerr<errmax) {
	    cerr << "AntiRecoPG::CalOnTopOfCal-E file" << filename << "too short" << endl;
	    maxerr++;}
	}}}

    for(int sect=1;sect<=8;sect++){
      for(int par=0;par<2;par++){
	if (! myfile.eof() ) {myfile >> tcal2[par][sect-1];}
	else{ iret = -1;
	  if (maxerr<errmax) {
	    cerr << "AntiRecoPG::CalOnTopOfCal-E file" << filename << "too short" << endl;
	    maxerr++;}
	}}}
  } myfile.close();

  if (iret==0){
    for(int sect=1;sect<=8;sect++){
      // cal on top of old cal
      zgcal[1][sect-1]=zgcal[1][sect-1]*zgcal2[1][sect-1];
      zcal[1][sect-1]=zcal[1][sect-1]*zcal2[1][sect-1];
      tcal[1][sect-1]=tcal[1][sect-1]*tcal2[1][sect-1];
      zgcal[0][sect-1]=zgcal2[1][sect-1]*zgcal[0][sect-1]+zgcal2[0][sect-1];
      zcal[0][sect-1]=zcal2[1][sect-1]*zcal[0][sect-1]+zcal2[0][sect-1];
      tcal[0][sect-1]=tcal2[1][sect-1]*tcal[0][sect-1]+tcal2[0][sect-1];
      /*
	for(int par=0;par<2;par++){
	cout << par << sect << " " << zgcal2[par][sect-1]<< " " << zcal2[par][sect-1]<< " " << tcal2[par][sect-1]<<endl;
	}*/
    }
  }
  return iret;
}

void AntiRecoPG::InitCal(int run_type){ 

  // calibration constants for Z from trd
  if (run_type != 99) {
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
  if (run_type != 99) {
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
  if (run_type == 99) { // is MC
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
    if (run_type != 99) { // not MC
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
      if (run_type != 99) { // not MC
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
 
  return;
}

void AntiRecoPG::InitPar(){ 

  _htdcbw=0.0244; // from antidbc02.C
  _lspeed=14.7; // from job.C
  _tguess = -130.;
  _etime = 30.; //ns

  for(int isect=1;isect<=8;isect++){
    _zguess[isect-1] = 0.;
    _ezeta[isect-1] = 43.; //(43 cm RMS of zguess from ADC || 20cm RMS Z from timepairs)
    for(int itt=0;itt<256;itt++){
      chilist[itt][isect-1]=-1.; // global chisquare list
      timelist[itt][isect-1]=0.;
      zetalist[itt][isect-1]=0.;
    }
  }
  InitCal();
  evto = 0;
  runo = 0;
  nrsd = 0; 
  return;
}

/* unfolding of Zcoo knowing that the real Zcoo are in the [-40,40] range \n
  gaussian distribution assumption \n
  example for unfolding Zcoo from ADC (RMS 50cm): BayesEstimation(zadc,45,-40.,40.) \n
  example for unfolding Zcoo from TDC (RMS 20cm): BayesEstimation(ztdc,8,-40.,40.)
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
  for(int i=0; i<evt->NTofClusterH(); i++){
    TofClusterHR* ptcl=evt->pTofClusterH(i);
    if( !ptcl) continue;
    tguess+=(ptcl->Time);
    nguess++;
  }
  if (nguess>0) {tguess=tguess/(1.* nguess);}
  else {tguess = -130.;}
  return tguess;
}

// reload all info from AntiRawSides
//fill timetable and timeindex
int AntiRecoPG::ReLoadAcc(){  
  int iret = 0; // no raw sides
  AMSEventR* evt = AMSEventRHead()?AMSEventRHead():AMSEventR::Head();
  if (evt->Event() == evto && evt->Run() == runo) return nrsd;
  if (evt->Run() != runo){ // new run reload calibrations
  int run_type = SelectRun();
  InitCal(run_type);}
  
  if (evt->NAntiRawSide()!=0){
  for(int isect=1; isect<=8; isect++){ //clear timetable 
    for(int k=0; k<2; k++){ //side index
      timeindx[0][k][isect-1]=0; // index for FT times
      timeindx[1][k][isect-1]=0;  // index for TDC times
      adctable[k][isect-1]=0.; // adc values
      for(int i=0; i<16; i++){
	timetable[i][k][isect-1]=0; }}}
  for(int ir=0; ir<evt->NAntiRawSide(); ir++){
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
    if(iftt<0) {iftt=iftt; 
      if (maxerr<errmax)  {
	cerr << "AntiRecoPG::ReLoadAcc-E open Error null or negative number of #FT= " << iftt+1 << "| Run= " << evt->Run() << " event= " << evt->Event() << endl;
	maxerr++;}}
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
  if ( _ezeta[sect-1] == 0.) _ezeta[sect-1]=43.; // savatage reset
  double chi = ((ttime-_tguess)/_etime)*((ttime-_tguess)/_etime);
  chi = chi + ((zzeta-_zguess[sect-1])/_ezeta[sect-1])*((zzeta-_zguess[sect-1])/_ezeta[sect-1]);
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

// evaluate a guess to Zcoo from Anti ADC sides (50 cm RMS)
float AntiRecoPG::GetAdcZGuess(int sect){
  float zguess = 0.;
  //  if (adctable[0][sect-1]<=3400. && adctable[1][sect-1]<=3400. && adctable[0][sect-1]>0. && adctable[1][sect-1]>0.){
    float adcs=adctable[0][sect-1]+adctable[1][sect-1];
    float adcd=adctable[0][sect-1]-adctable[1][sect-1]; // check the sign
    float par0=zadcpar[sect-1][0][0]+zadcpar[sect-1][0][1]*adcs+zadcpar[sect-1][0][2]*adcs*adcs+zadcpar[sect-1][0][3]*adcs*adcs*adcs;
    float par1=zadcpar[sect-1][1][0]+zadcpar[sect-1][1][1]*adcs+zadcpar[sect-1][1][2]*adcs*adcs+zadcpar[sect-1][1][3]*adcs*adcs*adcs;
    if(par1!=0.) zguess=zgcal[0][sect-1]+zgcal[1][sect-1]*(adcd-par0)/par1;
    //}

  zguess = zguess+zsme*gRandom->Gaus();
  //  if (adctable[0][sect-1]>3400. && adctable[1][sect-1]<3400.) zguess = -1000;
  //  if (adctable[0][sect-1]<3400. && adctable[1][sect-1]>3400.) zguess = +1000;

  return zguess;
}

// raw evaluation of energy deposition pairing ADC sides (for internal use large nonlinearity)
float AntiRecoPG::DoRawEdep(int sect){
  float e0=0.;
  float e1=0.;
  if (adctable[0][sect-1]>0) e0=(ADCAL[sect-1][0][0]+adctable[0][sect-1]*ADCAL[sect-1][0][1]);
  if (adctable[1][sect-1]>0) e1=(ADCAL[sect-1][1][0]+adctable[1][sect-1]*ADCAL[sect-1][1][1]);
  float edep = (e0+e1);
  if(adctable[0][sect-1]>0 && adctable[1][sect-1]>0) edep = 0.5*edep; // dual sided
  return edep;
}

/* re-build a cluster pairing list based on T&Z guess \n
  ttguess = 999 and zzguess = 999 then use the default values (from ToF and ADC) \n 
  reccommended err_zguess>=20 cm (TDCZ RMS) for precise Zguess (from trk) \n
  reccommended err_zguess=50cm for minimal (default) zguess from ADC \n
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
    ezg = 50.;}
  SetZGuess(zg,ezg,sect);
  if (ttguess != 999) SetTGuess(ttguess,err_tguess);
  int npa = ScanThePairs(sect);
  if (npa != 0) {
    if (!cluster) cluster = new AntiClusterR;//FIX ME: This function is never used "internally" with cluster=0, but if a user was calling it in this way the value of "cluster" would never be "returned".
    cluster->Npairs = npa;
    cluster->Sector = sect;
    cluster->time = GetTime(sect,0);
    cluster->zeta = ((npa)>0 ? GetZeta(sect,0) : zadc);
    cluster->chi = GetChisquare(sect,0);
    cluster->unfzeta = ((npa)>0 ? BayesEstimation(cluster->zeta,8.,-40.,40.) : zadc_unf);
    cluster->next_time = ((abs(npa)>1) ? GetTime(sect,1) : 0);
    cluster->next_zeta = (((npa)>1) ? GetZeta(sect,1) : 0);
    cluster->next_chi = ((abs(npa)>1) ? GetChisquare(sect,1) : 1.e6);
    cluster->next_unfzeta = (((npa)>1) ? BayesEstimation(cluster->next_zeta,8.,-40.,40.) : 0);
    cluster->zeta_adc = zadc;
    cluster->unfzeta_adc = zadc_unf;
    cluster->rawdep = DoRawEdep(sect);
    cluster->rawq = cluster->Charge();
    cluster->Edep = cluster->GetEdep();
    cluster->phi = cluster->FillCoo(& cluster->AntiCoo);
    cluster->adc[0] = (int)(adctable[0][sect-1]);
    cluster->adc[1] = (int)(adctable[1][sect-1]);
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
