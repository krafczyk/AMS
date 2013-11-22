#ifndef __ANTIPG__
#define __ANTIPG__

#include <vector>
#include <root.h>
#include <point.h>

class AMSAntiCluster;

#define __USEANTICLUSTERPG__ //if we remove/comment this define the original class from Evgeni is used
#ifdef __USEANTICLUSTERPG__

//! Anti Coincidence Detector Clusters 
/*! Offline Anti Coincidence Counter software \n
  --basic instructions:  \n
 event->RebuildAntiClusters() rebuild with PG code \n
https://indico.cern.ch/conferenceDisplay.py?confId=251079
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
  //! best time respect to the T&Z guess (1ns resolution 2ns RMS)
  float time;
  //! best Zcoo respect to the T&Z guess (8cm resolution 20cm RMS)
  float zeta;
  //! minimum chisquare respect to the T&Z guess
  float chi;
  //! unfolded zeta in the [-40,40] range (6cm resolution 15cm RMS)
  float unfzeta;
  //! next time respect to the T&Z guess (time uncertainty is larger if this is neat to best time)
  float next_time;
  //! next zeta respect to the T&Z guess (zeta uncertainty is larger if next_time is near to best time)
  float next_zeta;
  //! next chisquare respect to the T&Z guess (zeta/time uncertainty larger if this is neat to minimum chisquare)
  float next_chi;
  //! unfolding of next_zeta possibility
  float next_unfzeta;
  //! Zcoo from ADC (resolution 40-50cm)
  float zeta_adc;
  //! Zcoo from ADC (unfolded in the [-40-40] range)
  float unfzeta_adc;
  //! charge without any correction (adc saturation for Q>3)
  float rawq;
  //! center of sector position [0-360]
  float phi;
  //! AntiCoo[0] and AntiCoo[1] at center of sector, AntiCoo[3]=unfzeta
  AMSPoint AntiCoo;
  //! raw evaluation of energy deposition pairing ADC sides (for internal use large nonlinearity)
  float rawdep;
  //! evaluation of energy deposition (MeV) from rawq
  float Edep;

  //! evaluate Edep in a simple but very raw way
  inline float GetEdep() {return 0.8*0.001*171.2*9.67*rawq*rawq;}
  /*! evaluate AntiCluster charge (adc saturation for Q>3) \n
      (warning! adc saturation for Q>2 in MC; to be corrected in digitization) \n
      if Error on Beta is large (e.g. use beta from DoBeta AntiCluster Standalone)\n
      i suggest to not correct for beta. \n
      correction with Zcoo signal extinction to be improved \n}
    */
  float Charge(float costh = 0.45, float beta = 1., float ztrue = 0.);
  //! fill AntiCoo point and evaluate phi
  float FillCoo(AMSPoint* AntiCoo);
  /*! return distance, beta and cosine theta inclination respect to Acc pad \n
    assuming External point hypothesis and straight line trajectory \n
    connecting to the Anti Counter point ForcedCoo \n
    beta > 0 if Acc time is smaller than ExtTime \n
    if !ForcedCoo (default) the AntiCluster standalone point is used \n
    typ: ErrBeta = beta*beta*30.*ErrTime/dist, typ: ErrTime = 1.5ns is dominant \n
    maybe useful for evaluation of charge correction */
  float DoBeta(AMSPoint* ExtCoo, float ExtTime, float& beta, float& costh, AMSPoint* ForcedCoo = 0);

  /*! rebuild a single AntiCluster \n
    Sector should be already existing \n
    return 0 if no raw sides found \n
    return npairs>0 (both sides) or nsingle<0 (single side)                      
  */
  int ReBuildMe(float zzguess = 999, float err_zguess = 20, float ttguess=999 , float err_tguess = 30.);

  //! return human readable info about AntiClusterR
  char * Info(int number=-1){
    float errz = 6.;
    if (Npairs<=0) errz=45.;
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
access with: \n
  AntiRecoPG* Acci = new AntiRecoPG::gethead(); \n
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
  //! Default contructor
  AntiRecoPG();

  //! The head of the singleton
  static AntiRecoPG* head;
#pragma omp threadprivate (head)
 public:
 static AMSEventR*&AMSEventRHead();
  //! Default deconstructor
virtual  ~AntiRecoPG(){};
  
  /*!
    Accessor to the public instance of the singleton. \n
    If no instance has been previously created it will be done at this moment.
  */
  static AntiRecoPG* gethead();
  
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
  //! return time at choice pairing position. If choice=0 is the best pairing time respect to T&Z guess  
  inline float GetTime(int sect, int choice) {return timelist[choice][sect-1];}
  //! return Zcoo at choice pairing position. If choice=0 is the best pairing zcoo respect to T&Z guess  
  inline float GetZeta(int sect, int choice) {return zetalist[choice][sect-1];}
  //! return chisquare at choice pairing position. If choice=0 is the minimum chisquare respsct to T&Z guess  
  inline float GetChisquare(int sect, int choice) {return chilist[choice][sect-1];}
  //! inspect side/sector TDC timetable
  inline int GetTimeTable(int itdc, int iside, int sect) {return timetable[itdc][iside][sect-1];}
  //! get the pointer to an AntiClusterR
  inline AntiClusterR* pAntiClusterPG(int icluster) {return &ppAntiClusterPG[icluster];}

  /*! found the sector(1-8) you think (-hope) is crossed \n
    assuming is on the same straight line connecting Point1 and Point2\n                                     
    return = 0 no crossing at all -10 error (same points)\n       
    return <0 crossing at |Z|coo > 40 cm \n
    CrossPoint filled with the Hypo corssing point \n
    phicross filled with the hypo crossing phi \n
    if 2 AntiCounters are crossed in opposite directions the nearest to Point1 is choosen*/
  int GetCrossing(AMSPoint* Point1, AMSPoint* Point2, AMSPoint* CrossPoint, float& phicross);
  /*! file filename contains a correction to existing calibrations \n                                             
    still preliminary to be develop if necessary                                                                
  */
  int CalOnTopOfCal(char* filename);
  //! reload all info from AntiRawSides
  int ReLoadAcc();
  //! evaluate a guess of event time averaging tof times (typ. -130ns)
  float GetTofTimeGuess();
  //! evaluate a guess to Zcoo from Anti ADC sides (50 cm RMS)
  float GetAdcZGuess(int sect);
  //! scan the time history searching the best pairing respect to T&Z guess 
  int ScanThePairs(int sect);
  //! evaluate a pair (or a single time) with chisquare minimization respect to T&Z guess
  int DoThePair(double ttime, double zzeta, int sect);
  /*! unfolding of Zcoo knowing that the real Zcoo are in the [-40,40] range \n
    gaussian distribution assumption \n
    example for unfolding Zcoo from ADC (RMS 50cm): BayesEstimation(zadc,45,-40.,40.) \n
    example for unfolding Zcoo from TDC (RMS 20cm): BayesEstimation(ztdc,8,-40.,40.) \n
  */
static float BayesEstimation(float value, float error, float low, float upp);
/*! return = 1 DATA \n
 return = 2 pre-launch BT or Muons (pre 1305331200) \n
 return = 99  MC 
 */
 static int SelectRun();
  //! raw evaluation of energy deposition pairing ADC sides (for internal use large nonlinearity)
  float DoRawEdep(int sect);
  /*! re-build a cluster pairing list based on T&Z guess \n
    reccommended err_zguess>=20 cm (TDCZ RMS) for precise Zguess (from trk) \n
    reccommended err_zguess=50cm for minimal (default) zguess from ADC \n
    return number of pairs. if <0 then only single side time history \n
  */
  int BuildCluster(AntiClusterR* cluster, int sect, float zzguess = 999, float err_zguess = 20, float ttguess=999 , float err_tguess = 30.);
  /*! rebuild all cluster with default Z&T guess (from Tof and ADC) \n
    clear all old clusters \n
    return the number of anticlusters \n
    For a single defined sector sect a particular zguess can be set   
  */
  int BuildAllClusters(int sect = 0, float sect_zguess = 999, float err_sect_zguess = 999);

 private:
  //! set default parameters + calibrations initialization
  void InitPar();
  //! set calibration (run_type = 99 for MC)
  void InitCal(int run_type = 1);

  //! 0.0244 tdc bin width from antidbc02.C 
  float _htdcbw;
  //! 14.7 cm/ns speed of light in sect from job.C 
  float _lspeed;
  //! avg time guess default = -130 ns
  float _tguess;
  //! uncertainty time guess default = 30 sn 
  float _etime;
  //! Z coo guess default = 0 or from ADC signal division 
  float _zguess[8];
  //! Z coo guess error default = 43cm (from ADC signal division)
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
  //! TDC Zcoo linear cal. zgood = zraw*zcal[1][sect-1]+zcal[0][sect-1]
  float zcal[2][8];
  //! ADC Zcoo linear cal. zgood = zraw*zgcal[1][sect-1]+zgcal[0][sect-1]
  float zgcal[2][8];
  //! Time linear cal. tgood = traw*tcal[1][sect-1]+tcal[0][sect-1]
  float tcal[2][8];
  //! ADC non-linearity paramter for Zguess from ADC
  float zadcpar[8][2][4]; // parameter for Z adc guess

  //! ADC non-linearity paramter for edep from ADC
  float ADCAL[8][2][2]; // parameter for E adc calib
  //! Energy*|costheta| to Q calibrations
  float Etoqcc[8][3]; // parameter for E|costheta| to q
  //! bethe bloch approx correction parameter
  float bcorr;
  //! distance to border correction parameters
  float zcorr[4];
  //! residual costheta correction parameters
  float ttcorr[4];
  //! additional time smearing for MC
  float tsme;
  //! additional Zcoo smearing for MC
  float zsme;
  //! additional Z from ADC smearing for MC
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
