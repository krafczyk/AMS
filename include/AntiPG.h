#ifndef __ANTIPG__
#define __ANTIPG__

#include <vector>
#include <root.h>
#include <TString.h>
#include <point.h>

class AMSAntiCluster;

#define __USEANTICLUSTERPG__ //if we remove/comment this define the original class from Evgeni is used
#ifdef __USEANTICLUSTERPG__

//! Anti Coincidence Detector Clusters 
/*! Offline Anti Coincidence Counter software \n
  --basic instructions:  \n
 event->RebuildAntiClusters() rebuild with PG code \n
https://indico.cern.ch/conferenceDisplay.py?confId=251079 \n
2014 improvement: https://indico.cern.ch/event/321667/
*/
class AntiClusterR {

  /*! Offline Anti Coincidence Counter software
    use info from AntiRawSideR
    \author                                                                     
    francesco.nozzoli@pg.infn.it \n
    matteo.duranti@pg.infn.it \n
  */

  static char _Info[255];

 public:

  AntiClusterR();
  AntiClusterR(AMSAntiCluster *ptr);
  virtual ~AntiClusterR(){};

  /*!
    Goodness of the reconstructed Cluster:
    0: good cluster
    bit  0    ("1"): Bad ChiSq>30
    bit  1    ("2"): One ADC is 0
    bit  2    ("4"): Both ADC are 0
    bit  7  ("128"): No FT-coinc. on 2 sides
    bit  8  ("256"): 1 side Sector
    bit 10 ("1024"): miss.side
    
  */
  unsigned int Status;
  //! number of pairs (if <0 number of time history of the found side)
  int Npairs;
  //! 1-8 identify the sector
  int Sector;
  //! adc measure of both sides
  int adc[2];
  //! best time respect to the T&Z guess (<0.4ns resolution 1ns RMS)
  float time;
  //! best Zcoo respect to the T&Z guess (6cm resolution 10cm RMS)
  float zeta;
  //! minimum chisquare respect to the T&Z guess
  float chi;
  //! unfolded zeta in the [-40,40] range less than (6cm resolution 10cm RMS)
  float unfzeta;
  //! next time respect to the T&Z guess (time uncertainty is larger if this is neat to best time)
  float next_time;
  //! next zeta respect to the T&Z guess (zeta uncertainty is larger if next_time is near to best time)
  float next_zeta;
  //! next chisquare respect to the T&Z guess (zeta/time uncertainty larger if this is neat to minimum chisquare)
  float next_chi;
  //! unfolding of next_zeta possibility
  float next_unfzeta;
  //! Zcoo from ADC (resolution 35cm)
  float zeta_adc;
  //! Zcoo from ADC (unfolded in the [-40-40] range)
  float unfzeta_adc;
  //! charge without costheta and Beta corrections (adc saturation for Q>3)
  float rawq;
  //! center of sector position [0-360]
  float phi;
  //! AntiCoo[0] and AntiCoo[1] at center of sector, AntiCoo[3]=unfzeta
  AMSPoint AntiCoo;
  //! raw evaluation of energy deposition pairing ADC sides (for internal corrected for Temperature and Zcoo effects) 
  float rawdep;
//! raw evaluation of energy deposition for single ADC sides (for internal corrected for Temperature and Zcoo effects)
  float rawDepSide[2];
  
  //! evaluation of energy deposition (MeV) from rawq
  float Edep;

  //! evaluate Edep in a simple but very raw way
  inline float GetEdep() {return 0.8*0.001*171.2*9.67*rawq*rawq;}

  //! raw evaluation of energy deposition pairing ADC sides corrected for Temperature and Zcoo effects 
  float PairDep();

  /*! evaluate an estimation of which is the fired scintillator pad in the sector \n
      it use the fact that there are 1.16ns i data and 1.4ns in mc of time lag beetween adiacent pads \n
      then compare the betawrong with the beta compatible with energy release \n
      knowing costh and qtrue \n
      return best estimation of Acc phi. Correct pad detected in 60% of cases*/
  float FindPhiGuess(float betawrong, float costh, int qtrue);

  /*! evaluate AntiCluster charge (adc saturation for Q>3) \n
    if iside = -1 evaluate paired charge \n
    if Zcoo != 0 use this Zcoo for corrections \n
  timecorr = 0 no time dependence correction \n
  timecorr = 1 apply the timecorrection to charge to data \n
  timecorr = 2 force the time correction also if is MC */
  float Charge(float costh = 0.45, float beta = 1., int iside = -1, float Zcoo=0., int timecorr=1);

  /*! evaluate AntiCluster charge (adc saturation for Q>3) \n
    if Zcoo != 0 use this Zcoo for corrections \n
  timecorr = 0 no time dependence correction \n
  timecorr = 1 apply the timecorrection to charge to data \n
  timecorr = 2 force the time correction also if is MC */
  float PairedCharge(float costh = 0.45, float beta = 1., float Zcoo=0, int timecorr=1);

  //! fill AntiCoo point and return phi
  float FillCoo(AMSPoint* AntiCoo);

  /*! return distance, beta and cosine theta inclination respect to Acc pad \n
    assuming External point hypothesis and straight line trajectory \n
    connecting to the Anti Counter point ForcedCoo \n
    beta > 0 if Acc time is smaller than ExtTime \n
    if !ForcedCoo (default) the AntiCluster standalone point is used \n
    typ: ErrBeta = beta*beta*30.*ErrTime/dist, typ: ErrTime = 1ns is dominant \n
    when phiGuess >0 or (ForcedCoo is given) the 1.16ns correction of the time depending of pad in the sector is applied\n                                                             
    when phiGuess = -2 the 1.16 ns correction is NOT APPLIED also if ForcedCoo is given */
  float DoBeta(AMSPoint* ExtCoo, float ExtTime, float& beta, float& costh, AMSPoint* ForcedCoo = 0, float phiGuess = -1);

  /*! rebuild a single AntiCluster \n
    Sector should be already existing \n
    return 0 if no raw sides found \n
    return npairs>0 (both sides) or nsingle<0 (single side)                      
  */
  int ReBuildMe(float zzguess = 999, float err_zguess = 10, float ttguess=999 , float err_tguess = 30.);

  //! return human readable info about AntiClusterR
  char * Info(int number=-1){
    float errz = 8.;
    if (Npairs<=0) errz=40.;
    sprintf(_Info,"Anticluster No %d Sector=%d R=%5.2f#pm%5.2f #Phi=%5.2f#pm%5.2f Z=%5.2f#pm%5.2f E_{Dep}(MeV)=%7.3g CTime(nsec)=%7.2f",number,Sector,54.95,0.4,phi,22.5,unfzeta,errz,Edep,time);
    return _Info;
  }
  
  ClassDef(AntiClusterR,6);
#pragma omp threadprivate(fgIsA)
};

//---------------------------------------------------------------------------------------------

//! reconstruction functions for AntiCluster
/*! Offline Anti Coincidence Counter software \n
https://indico.cern.ch/conferenceDisplay.py?confId=251079 \n
2014 improvement: https://indico.cern.ch/event/321667/ \n
access with: \n
  AntiRecoPG* Acci = new AntiRecoPG::gethead(version,istolerant); \n
  version 0 is always the last one (default) and version -1 is PRE-June2014\n
*/
class AntiRecoPG {

  /*! Offline Anti Coincidence Counter software
    use info from AntiRawSideR
    \author                                                                     
    francesco.nozzoli@pg.infn.it \n
    matteo.duranti@pg.infn.it \n
  */
  friend class AntiClusterR;

 private:
 static AMSEventR * _HeadE;
#pragma omp threadprivate(_HeadE)
 /* Default contructor \n
    version 0 is the default (last) one \n
    version -1 is PRE-June 2014 \n
    Istolerant=0 crash if Calib file not found \n
    Istolerant=1 use the default id file not found \n
 */
 AntiRecoPG(int version=0, int Istolerant=0);

  //! The head of the singleton
  static AntiRecoPG* head;
#pragma omp threadprivate (head)
 public:
 static AMSEventR*&AMSEventRHead();

  //! Default deconstructor
 //virtual  ~AntiRecoPG(){}; // non c'era vitrual in TrkLH
 ~AntiRecoPG();
  
 //! Clean a previous instance of the class.                  
 void clear();

  /*!
    Accessor to the public instance of the singleton. \n
    If no instance has been previously created it will be done at this moment. \n
    version 0 is the default (last) one \n
    version -1 is the PRE JUNE 2014 one \n
    Istolerant=0 crash if Calib file not found \n
    Istolerant=1 use the default id file not found \n
    They are setted only at first call of gethead, after you cannot change version \n
  */
 static AntiRecoPG* gethead(int version=0, int Istolerant=0);
  
  //! number of reconsructed AntiClusterR
  int nAntiClusterPG;
  //! vector of reconsructed AntiClusterR
  std::vector<AntiClusterR> ppAntiClusterPG;

  //! set a guess of vertical coo (z) (e.g. from tracker or from AntiADC) for help time history pairing
  inline void SetZGuess(float Zguess, float Zerror, int sect) {_zguess[sect-1]=Zguess; _ezeta[sect-1]=Zerror; return;}
  //! set a guess for time (e.g. from tof pads) for help time history pairing
  inline void SetTGuess(float tguess, float terror) {_tguess=tguess; _etime=terror; return;}
  //! return number of pairs found (if <0 is the number of history in the single side existing 
  inline int GetNpairs(int sect) {return npairs[sect-1];}
  //! return chisquare at choice pairing position. If choice=0 is the minimum chisquare respsct to T&Z guess  
  inline float GetChisquare(int sect, int choice) {return chilist[choice][sect-1];}
  //! inspect side/sector TDC timetable
  inline int GetTimeTable(int itdc, int iside, int sect) {return timetable[itdc][iside][sect-1];}
  //! get the pointer to an AntiClusterR
  inline AntiClusterR* pAntiClusterPG(int icluster) {return &ppAntiClusterPG[icluster];}


  /*! return time at choice pairing position. If choice=0 is the best pairing time respect to T&Z guess \n
    slewrate = 0 no slewrate correction \n
    slewrate = 1 apply the slewrate delay correction related to adc amplitude only to data \n
slewrate = 2 apply the slewrate delay correction always also on MC \n
  timecorr = 0 no time dependence correcxtion \n
  timecorr = 1 apply the timecorrection to timing to data \n
  timecorr = 2 force the time correction also if is MC \n
  phiGuess >=0 and <=360 apply the 1.16ns correxction (normally not done here) */
  float GetTime(int sect, int choice , int slewrate = 1, int timecorr = 1, float phiGuess = -1); 

  /*! return Zcoo at choice pairing position. \n
     If choice=0 is the best pairing zcoo respect to T&Z guess \n
   timecorr = 0 no time dependence correcxtion \n
   timecorr = 1 apply the timecorrection to zeta to data \n
   timecorr = 2 force the time correction also if is MC */
  float GetZeta(int sect, int choice, int timecorr = 1);

  /*! found the sector(1-8) you think (-hope) is crossed \n
    assuming is on the same straight line connecting Point1 and Point2\n                                     
    return = 0 no crossing at all -10 error (same points)\n       
    return <0 crossing at |Z|coo > 40 cm \n
    CrossPoint filled with the Hypo corssing point \n
    phicross filled with the hypo crossing phi \n
    if 2 AntiCounters are crossed in opposite directions the nearest to Point1 is choosen*/
  int GetCrossing(AMSPoint* Point1, AMSPoint* Point2, AMSPoint* CrossPoint, float& phicross);

  //! reload all info from AntiRawSides
  int ReLoadAcc();

  /*! get the average ACC(+/-)Z(Wake/Ram) TEMPERATURE \n
    return 0 for MC (rtp=99) \n
    if last call is beyond 60sec set the value timenow\n
    if last call is within 60sec the value returned is tempavg\n
  */
  float GetAvgTempAcc();

  /*! get the timing correction as function of time \n
    for the sector sect. Time is from temptime. \n
    extrapolation is used for too large time
  */
  float GetTimingTcorr(int sect); 

  /*! get the extrapolation or interpolation for histogram h \n
    at time tt. the extrapolation is linear with parameter p0 and p1 \n
    return an interpolation if tt<timecallimit \n
    timecallimit is setted at the correct value the first time you call
  */
  float GetExtrapolation(TH1D* h,double tt,double p0,double p1); 

  //! evaluate a guess of event time averaging all existing tof cluster times (typ. -130ns)
  float GetTofTimeGuess();
  //! evaluate a guess to Zcoo from Anti ADC sides (35 cm sigma)
  float GetAdcZGuess(int sect);
  //! scan the time history searching the best pairing respect to T&Z guess 
  int ScanThePairs(int sect);
  //! evaluate a pair (or a single time) with chisquare minimization respect to T&Z guess
  int DoThePair(double ttime, double zzeta, int sect);
  /*! unfolding of Zcoo knowing that the real Zcoo are in the [-40,40] range \n
    gaussian distribution assumption \n
    example for unfolding Zcoo from ADC (RMS 40cm): BayesEstimation(zadc,35,-40.,40.) \n
    example for unfolding Zcoo from TDC (RMS 10cm): BayesEstimation(ztdc,6,-40.,40.) \n
  */
static float BayesEstimation(float value, float error, float low, float upp);
/*! return = 1 DATA \n
 return = 2 pre-launch BT or Muons (pre 1305331200) \n
 return = 99  MC 
 */
 static int SelectRun();
  //! raw evaluation of F(beta)*Q^2/costheta (propto energy deposition) pairing ADC sides corrected for nonlinearity, Tempearture and Zcoo effects
 float DoRawEdep(int sect, float zcoo=0);
 /*! raw evaluation of F(beta)*Q^2/costheta single ADC side (correcting nonlinearity and temperature and Zcoo effects) \n
  timecorr = 0 no time dependence correction \n
  timecorr = 1 apply the timecorrection to edep to data \n
  timecorr = 2 force the time correction also if is MC */
 float DoSideRawEdep(int sect ,int side,float Zcoo = 0,int timecorr=1);

  /*! re-build a cluster pairing list based on T&Z guess \n
    reccommended err_zguess>=10 cm (TDCZ RMS) for precise Zguess (from trk) \n
    reccommended err_zguess=35cm for minimal (default) zguess from ADC \n
    return number of pairs. if <0 then only single side time history \n
  */
  int BuildCluster(AntiClusterR* cluster, int sect, float zzguess = 999, float err_zguess = 10, float ttguess=999 , float err_tguess = 30.);
  /*! rebuild all cluster with default Z&T guess (from Tof and ADC) \n
    clear all old clusters \n
    return the number of anticlusters \n
    For a single defined sector sect a particular zguess can be set   
  */
  int BuildAllClusters(int sect = 0, float sect_zguess = 999, float err_sect_zguess = 999);

 private:
  /* set default parameters + calibrations initialization \n
    version 0 points to the default (last) one, 1 is the first (older) one \n               
    Istolerant=0 crash if Calib file not found \n          
    Istolerant=1 use the default id file not found \n      
  */
  void InitPar(int version=0, int Istolerant=0);
  //! set calibration from file (run_type = 99 for MC)
  void InitCal(int run_type = 1, int VERNUM = 0);

  //! set defaultcalibration (run_type = 99 for MC)
  void DefaultCal(int run_type = 1);

  /*! load the time-histo calibration from a file\n
  iforce != 0 force the file to be reloaded \n
  if VERNUM<0 put the pre-JUNE2014 values 
  */
  int LoadCaliFile(int VERNUM = 0, int iforce = 0);
  /*! load the static calibrations from a file\n
  iforce != 0 force the file to be reloaded
  */
  int LoadHistoCaliFile(int VERNUM = 0, int iforce = 0);

  //! version NUMBER default is 0 that point to the last one, 1 is the first(older) one ,-1 is pre-JUNE 2014
  int NVER;
  //! file loaded? ifild = 1 none loaded; ifild <0 histoloaded; ifild%2=0 static-cali file loaded
  int ifild;
  /*! tolerance to the inputfile errors\n
    =0 not tolerant and program will crash if static-califilenotfound*/
  int itolerance;
  //! 0.0244 tdc bin width from antidbc02.C 
  float _htdcbw;
  //! 14.7 cm/ns speed of light in sect from job.C 
  float _lspeed;
  //! avg time guess default = -130 ns
  float _tguess;
  //! uncertainty time guess default = 30 ns 
  float _etime;
  //! Z coo guess default = 0 or from ADC signal division 
  float _zguess[8];
  //! Z coo guess error default = 40cm (from ADC signal division)
  float _ezeta[8];
  //! >0 # pairs found (<0 is # times from the single side)
  int npairs[8]; // >0 pairs || <0 singlets
  //! table of time history from AntiRawSide 
  int timetable[16][2][8];
  //! [0][side][sect-1] = # fast trigger (last used) [1][side][sect-1] = # tdc history
  int timeindx[2][2][8];
  //! list of chisquare respect to T and Zcoo guess (ordered)
  float chilist[256][8];
  //! list of time (ordered: 1st = min chisquare)
  float timelist[256][8];
  //! list of Z coo (ordered: 1st = min chisquare)
  float zetalist[256][8];
  //! table of ADC from AntiRawSide
  float adctable[2][8];
  //! (time vs adc) slewrate correction function parameters
  float slr[5];
  //! average ACC(+/-)Z(Wake/Ram) TEMPERATURE
  float tempavg;
  //! Time of TEMPERATURE to avoid too fast TEMP sampling
  double temptime;
  //! run type 1=data 2=pre-launch BT or Muons (pre 1305331200) 99=MC
  int rtp;
  //! Z light extinction along the bar (1/cm)
  float Zextinction[8][2];
  //! light collection deficit at the bar borders (1/cm^4)
  float Zborders;
  //! Temperature dependence parameter of adc sides
  float TempCal[8][2];
  //! ADC non-linearity paramter for edep from ADC
  float ADCAL[8][2][3]; // parameter for E adc calib
  //! thresholds for phiguess determinations
  float pth_min[8];
  //! thresholds for phiguess determinations
  float pth_max[8];

  //! TFile containing histo of corrections
  TFile* infile;

  //! extrapolation time behaviour of timing corrections                         
  float TTimecal[2][8];
  //! limit of time where calibrations are done
  float timecallimit;
  //! TH1D with the timing time corrections
  TH1D** Timecal;


  //! extrapolation time behaviour of zeta corrections                         
  float ZTTimecal[2][8];
  //! TH1D with the zeta time corrections
  TH1D** ZTimecal;

  //! p0 extrapolation of time behaviour of P and He charges
  float Qcal0[2][8][2];
  //! p1 extrapolation of time behaviour of P and He charges
  float Qcal1[2][8][2];
  //! TH1D with the charge time corrections
  TH1D**** Qcal;
  //! scaling factor of the correction
  float Scal[8][2];

  //! TDC Zcoo linear cal. zgood = zraw*zcal[1][sect-1]+zcal[0][sect-1]
  float zcal[2][8];
  //! ADC Zcoo linear cal. zgood = zraw*zgcal[1][sect-1]+zgcal[0][sect-1]
  float zgcal[2][8];
  //! Time linear cal. tgood = traw*tcal[1][sect-1]+tcal[0][sect-1]
  float tcal[2][8];

  //! ADC non-linearity paramter for Zguess from ADC; USED ONLY BY PRE JUNE 2014 NVER=-1     
  float zadcpar[8][2][4]; // parameter for Z adc guess  
  //! Energy*|costheta| to Q calibrations USED ONLY BY PRE JUNE 2014 NVER=-1               
  float Etoqcc[8][3]; // parameter for E|costheta| to q   
  //! distance to border correction parameters; USED ONLY BY PRE JUNE 2014 NVER=-1         
  float zcorr[4];
  //! residual costheta correction parameters ; USED ONLY BY PRE JUNE 2014 NVER=-1            
  float ttcorr[4];

  //! bethe bloch approx correction parameter
  float bcorr;
  //! additional time smearing for MC  (expected = 0 if NVER>=0)
  float tsme;
  //! additional Zcoo smearing for MC  (expected 1.8cm if NVER>=0)
  float zsme;
  //! additional Z from ADC smearing for MC (expected 25cm if NVER>=0)
  float zgsme;
  //! event index
  int evto;
  //! run index
  int runo;
  //! saved number of Antiraw sides 
  int nrsd;

  ClassDef(AntiRecoPG,0);//0 since no I/O feature is needed
};

#else //#ifdef __USEANTICLUSTERPG__

/// AntiClusterR structure

/*!
  \author e.choumilov@cern.ch
*/
class AntiClusterR {
  static char _Info[255];
 public:
  unsigned int   Status;   ///< Bit"128"->No FT-coinc. on 2 sides;"256"->1sideSector;"1024"->miss.side#
  int   Sector;   ///< //Sector number(1-8)
  int   Ntimes;  ///<Number of time-hits(1st come paired ones)
  int   Npairs;   ///<Numb.of time-hits, made of 2 side-times(paired)
  float Times[16];  ///<Time-hits(ns, wrt FT-time, "+" means younger hit)
  float Timese[16]; ///<Time-hits errors(ns)
  
  float Edep;    ///<Edep(mev)
  float Coo[3];   ///<R(cm),Phi(degr),Z(cm)-coordinates
  float ErrorCoo[3]; ///<Their errors
  
  AntiClusterR(){};
  AntiClusterR(AMSAntiCluster *ptr);
  /// \param number index in container
  /// \return human readable info about AnticlusterR
  char * Info(int number=-1){
    float xm=1.e9;
    //    for(int i=0;i<Time.size();i++){
    //      if(fabs(Time[i])<fabs(xm))xm=Time[i];
    //    }
    for(int i=0;i<Ntimes;i++){
      if(fabs(Times[i])<fabs(xm))xm=Times[i];
    }
    sprintf(_Info,"Anticluster No %d Sector=%d R=%5.2f#pm%5.2f #Phi=%5.2f#pm%5.2f Z=%5.2f#pm%5.2f E_{Dep}(MeV)=%7.3g CTime(nsec)=%7.2f",number,Sector,Coo[0],ErrorCoo[0],Coo[1],ErrorCoo[1],Coo[2],ErrorCoo[2],Edep,xm);
    return _Info;
  }
  virtual ~AntiClusterR(){};
  ClassDef(AntiClusterR,2);       //AntiClusterR
#pragma omp threadprivate(fgIsA)
};

#endif //#ifdef __USEANTICLUSTERPG__

#endif //#define __ANTIPG__
