#ifndef __TRKCC_H__
#define __TRKCC_H__
#ifdef _PGTRACK_

/*! Charge Confusion Classificators Software \n
  --basic instructions:  \n
  event->RebuildAntiClusters()    UPDATE UPDATE! TO DO! TODO! \n
  https://indico. ...  UPDATE  \n
*/

//#define _NOGBATCH_

#include <TROOT.h>
#include <TGraph.h>
#include <TFile.h>
#include <TTree.h>
#include <TObject.h>

#ifndef _NOGBATCH_
#include "TrTrack.h"
#include "root.h"
#include "root_setup.h"
//#include "TofTrack.h"
#include "TrTrackSelection.h"
#include "Tofrec02_ihep.h"
#include <EcalChi2CY.h>
#include <TrdKCluster.h>
#include <MagField.h>
#include <bcorr.h>
#include <root.h>
#endif //_NOGBATCH_

#include "TMVA/Tools.h"
#include "TMVA/Reader.h"

//! number of variables in nTrkCCVar
#define nTrkCCVar 73
//! Number of patterns
#define nTrkCCpatts 6
//! Number of versions (so far only one version implemented)
#define nVer 1

//! Number of vars for TrkCCPizzolotto in version 1
#define nVarsTrkCCNozzoliVersion1 47
//! Number of vars for TrkCCNozzoli in version 1
#define nVarsTrkCCPizzolottoVersion1 47

//-----------------------------------------------------

class TrkCCVar;
class TrkCCNozzoli;
class TrkCCPizzolotto;
class TrkCCAlgorithm;

/*! <b> Charge Confusion Classificator </b>
  \author                                                                     
  francesco.nozzoli@pg.infn.it \n
  cecilia.pizzolotto@pg.infn.it
  !*/
#ifndef _NOGBATCH_
class TrkCC: public TObject {
#else
class TrkCC {
#endif //_NOGBATCH_
    
 public:
  /*!
    \brief Default contructor
    \param algo    particular algorithm to be used
    \li Nozzoli;
    \li Pizzolotto;
    \param particle    each classifier (algorithm) is tuned on a particular type of particle
    \li "electrons";
    \li "protons";
    \param version    version of the classifier, useful to try old versions
    \li 0=default, (last) one;
    \li n=version n;
    \param gbatchPRODversion    the version of gbatch of the production. The differences should be small
    \li 0=default, the one used for the last pass (620, pass4);
    \li 620,800,900,...;
    \param gbatchCODEversion    the version of gbatch used to analyze data. The differences should be small
    \li 0=vdev (default);
    \li 620,800,900,...;
    \param runtype    type of the data (ISS, MC, BT, KSC, ...). Not useful when processing AMSRoot files
    \li  0=autodetect from GBatch (default);
    \li  1=ISS data normalized;
    \li  2=BT;
    \li 99=MC;
    \param istolerant    to control the behaviour in case of calib files not found
    \li 0=crash if calib files not found (default)
    \li 1=use the default if calib file not found
    \param verbose    to increase the verbosity
    \li 0=minimum verbosity (default);
    \li 1=boh;
    \li 2=boh;
    \li etc...;
    !*/
  TrkCC(TString algo, TString particle, int version, int gbatchPRODversion=0, int gbatchCODEversion=0, int runtype=0, bool istolerant=0, int verbose=0);
  
 private:
#ifndef _NOGBATCH_
  static AMSEventR * _HeadE;
#pragma omp threadprivate(_HeadE)
#endif //_NOGBATCH_

  //! Algorithm to be used. For example: "NozzoliRegressionA", "PizzolottoBDTB", ...
  TString _algo;
  //! For which type of particle the algo shoud be considered: this, for example, determines the normalization varialve (_normRIG)
  TString _particle;
  //! TrkCC version number. 0 is the latest.
  int _version;
  //! if _verbose>0 print debugging info. (verbose=0 no printout, 1=if errors&special cases, 2=at init funtions, 9=print at each event )
  int _verbose; 
  //! Number of gbatch version of the DATA/MC PROD FILE. Example: 620. Needed to load the correct normalisation files
  int _gbatchPRODversion;
  //! Number of gbatch version of the used CODE. Example: 620. Needed to load the correct normalisation files
  int _gbatchCODEversion;
  //! 0 from file type. 1 is DATA ; 2 is Beam Test ; 99 is MC.
  int _runtype;
  //! 0 normalize as function of E; 1 normalize as function of R (is setted by 'particle' && 'algo')
  int _normRIG;
  //! istolerant=0 crash if calib files not found \n          
  //! istolerant=1 use the default files if required files not found \n      
  int _istolerant;
 
  //! Number of variables
  int _nvars;
  
  //! Number of integer variables
  int _nvari;
  //! Number of float variables
  int _nvarf;

  //! _gr graphs with normalisation parameters.
  //! third index identifies quantile: 0=lowesttail, 1=lowtail, 2=median, 3=uppertail, 4=mostuppertail
  TGraph**** _gr;
  //! input files;
  TFile* _infile[nTrkCCpatts];
  //! Contains info on variable description, transformation from TrkCCVar and normalization options 
  TTree* _list; 

  /// Initialize variables and calls initialization functions
  //void _Initialize();
  /// Load all files
  void _LoadFiles();
  /// Load files and retrieve normalisation parameters
  int  _LoadNormalisation(TString dbPath);
  /// Load List of variables and used normalizations  
  int  _LoadListaVars();
  /// Load weights for classificators
  int _LoadWeights(TString dbPath);

  /// Correct parameters if hierarchy is not maintained by extrapolation
  int _CureIfSlopeCrossing(Double_t loge, Double_t& xminus2,Double_t& xminus1, Double_t& median,Double_t& xplus1,Double_t& xplus2,TGraph* glow2, TGraph* glow1,TGraph* gmedian,TGraph* gupp1,TGraph* gupp2);
  //! Manly transformation http://www.jstor.org/stable/2988129 DOI: 10.2307/2988129
  Double_t _Manly(Double_t lambda, Double_t x);
  Int_t _NozPar(Double_t& eta, Double_t qval2, Double_t qval1, Double_t pval2, Double_t pval1);

  //! an anti-symmetric monotonic tranformation from F.Nozzoli
  Double_t _Nozly(Double_t eta, Double_t x);
  /// return LAMBDA of a Manly transformation that symmetrize xminus and xplus
  /// respect to the median
  Int_t _ManlyPar(Double_t& lambda, Double_t xminus, Double_t xplus, Double_t mediana=0);
  Double_t _Findc(Double_t a,Double_t b,Double_t& ex);
  Double_t _ApplyTailLinearisation(Double_t delta, Double_t eta, Double_t lambda, Double_t mediana, Double_t x, Double_t xtreme, Double_t epsilon);
  /// NormBozNoz : returns the normalized variable
  /*
    \param[in] x  input variable
    \param[in] loge  log10(energy)
    \param[in] TGraphs  5 values with threasholds as function of loge
    \param[in] ilin     if 0 use linear transformation, if 1 use boznoz
    \param[in] cureTails  if true use tail linearisation when ( x<xminus2 || x>xplus2). To avoid "inf" values when transformation goes flat
  */
  Double_t _NormBozNoz(Double_t x, Double_t loge, TGraph* glow1, TGraph* gupp1, TGraph *gmedian, TGraph* glow2, TGraph* gupp2, Int_t ilin, Bool_t cureTails=true );


  /// Fills normalized array _varnorm
  int _NormalizeList();
  

  //! load the files and autofix runtype if not already done
  void _FirstLoad();

  /*! check, cure and copies the structure TrkCCVar to array _varlist
    also fill _evenergy (when energy=0 the |Rigidity| is used) \n
    also fille _evpatt with trk event pattern\n
    maskingnumber = 2*n = ecal missing     \n                                                       
    maskingnumber = 3*n = tof missing   \n                                                    
    maskingnumber = 5*n = trd missing   \n                                                                     
    maskingnumber = 7*n = rich missing  \n 
    return the recommended bitwise mask based on maskingnumber*/
  ULong64_t _FillVarList(int maskingnumber=1);
  
 public:
#ifndef _NOGBATCH_
  static inline AMSEventR*& AMSEventRHead(){return _HeadE;}
#endif //_NOGBATCH_

  //! Default deconstructor
  ~TrkCC();
  //! Clean a previous instance of the class.                  
  void Clear(Option_t* option="");
  
  /*!
    Accessor to the public instance of the singleton. \n
    If no instance has been previously created it will be done at this moment. \n
    version 0 is the default (last) one \n
    They are setted only at first call of gethead, after you cannot change version \n
  */
  
  // variables & functions ...  
  
  ///Finds the parameters delta,eta,lambda starting from 5 values on the x axis.
  /*!
    \param[out] delta
    \param[out] eta
    \param[out] labda
    \param[in] pval1   correspond to xminus2 and xminus1 (or xplus2 and xplus1)
    \param[in] pval2   correspond to xminus2 and xminus1 (or xplus2 and xplus1)
    \param[in] whichsymmetrize   decide if simmetrize on tails (if =2) or on the core (if =1) 
  */
  int FindBozNozPar(Double_t& delta, Double_t& eta, Double_t& lambda,
		    Double_t xminus2, Double_t xminus1, Double_t mediana, Double_t xplus1, Double_t xplus2,
		    Double_t pval1 = 1, Double_t pval2 = 2, Int_t whichsymmetrize12 = 2);
  //! return the BozNoz(x)
  Double_t ApplyBozNoz(Double_t delta, Double_t eta, Double_t lambda, Double_t mediana = 0, Double_t x=0);

  /*!  Process the event from external user-filled TrkCCVar version 
    return -2 = event problem \n                                                  
    return 1 = ok                 \n                        
    maskingnumber = 2*n = ecal missing     \n                                                       
    maskingnumber = 3*n = tof missing   \n                                                    
    maskingnumber = 5*n = trd missing   \n                                                                     
    maskingnumber = 7*n = rich missing  \n 
    fill also the _recommendmask bitwise mask based on maskingnumber*/                                         
  int ProcessEvent(TrkCCVar* extTrkCCVar, int maskingnumber=1, float energyGuess=0);
  
  /*!  Process the event from external user-prefilled full vector \n
    maskingnumber = 2*n = ecal missing     \n                                                       
    maskingnumber = 3*n = tof missing   \n                                                    
    maskingnumber = 5*n = trd missing   \n                                                                     
    maskingnumber = 7*n = rich missing  \n 
    fill also the _recommendmask bitwise mask based on maskingnumber*/                                         
  int ProcessEvent(float varlistfull[nTrkCCVar],int maskingnumber=1, float energyGuess=0);
  
  /*!  Process the event from external user-prefilled vector \n
    WARNING  TrkCCVar is not filled \n
    WARNING  the position/meaning of varlist element could change with version \n
    use recommendmask bitwise mask to disable the variables that are miscalculated \n 
    BE SURE You Know what you are doing
    if isnorm = 0 veclist is the NOT NORMALIZED and both _varlist and _varnorm are filled
    if isnorm = 1 veclist is normalized and only _varnorm is filled 
  */
  int ProcessEvent(float veclist[], float energyGuess, int pattern, int isnorm = 1, ULong64_t recommendmask=0xFFFFFFFFFFFFFUL);
    
#ifndef _NOGBATCH_
  /*!  Process the event from GBatch version 
    \param[in] ipart number of the particle (to save CPU process only the first time)
    \param[in] energyGuess is external guess (if ecal is not hitted default energy is fabs(Rigidity) or you can put here your alterantive choice) used for the normalization
    \param[in] abs(rebuilfRefit)=iTrTrackPar 3rd parameter (0 = stored, 1 refit if necessary, 2 force trk refit, 3 fit and rebuild also Coo) | rebuilfRefit <0 force rebuild of betah and anticluster                                          
    \param[in] fullProcess = true (slow 2.5% slower) fill all the TrkCCvar structure \n
    return the maskingnumber coding the possible missing variables in the event: \n
    return -2 = event problem \n                                                  
    return -1 = particle problem \n                                                           
    return 0 = trk problem        \n                                                             
    return 1 = ok                 \n                                                                 
    return 2*n = ecal problem     \n                                                                   
    return 3*n = tof problem   \n                                                                      
    return 5*n = trd problem   \n                                                                     
    return 7*n = rich problem  \n  
  */
  int ProcessEvent(int ipart, int rebuilfRefit=0, float energyGuess=0, bool fullProcess=true);

  /*!  Process the event from GBatch version 
    \param[in] ParticleR pointer
    \param[in] energyGuess is external guess (if ecal is not hitted default energy is fabs(Rigidity) or you can put here your alterantive choice) used for the normalization
    \param[in] abs(rebuilfRefit)=itrTrk 3rd parameter (0 = stored, 1 refit if necessary, 2 force trk refit, 3 fit and rebuild also Coo) | rebuilfRefit <0 force rebuild of betah and anticluster
    \param[in] forceProcess  true=(default) the particle is processed  false=processed if not already done \n
    \param[in] fullProcess = true (slow 2.5% slower) fill all the TrkCCvar structure \n
    return the maskingnumber coding the possible missing variables in the event: \n
    return -2 = event problem \n                                                  
    return -1 = particle problem \n                                                           
    return 0 = trk problem        \n                                                             
    return 1 = ok                 \n                                                                 
    return 2*n = ecal problem     \n                                                                   
    return 3*n = tof problem   \n                                                                      
    return 5*n = trd problem   \n                                                                     
    return 7*n = rich problem  \n  
  */
  int ProcessEvent(ParticleR* part, int rebuilfRefit=0, bool forceProcess = true, float energyGuess=0, bool fullProcess=true);
#endif // _NOGBATCH_
  
  //! return the value of normalized variable identified by varnumber
  float GetNormalizedVar(int varnumber, int isnormalized=1);
  //! return the number of normalized variables
  inline int GetNormalizedVarLength() { return _nvars; }
  
  /*! fill the TH1 with the varlist values, return the number of entries filled
    \param[in] regtype = 0  CC-spillover   optimized for Electrons E-normalized
    \param[in] regtype = 1  CC-interaction optimized for Electrons E-normalized
    \param[in] regtype = 2  CC-poor-but-independent from 0 and 1 CC optimized for Electrons E-normalized
    \param[in] regtype = 3  Electron/Proton separation for Electrons E-normalized
    \param[in] regtype = 4  CC for Protons (R normalized)
    \param[in] regtype = 5  Electron/Proton separation for Protons R-normalized
    \param[in] masktype = 0 the used mask is the recommended one
    \param[in] masktype = 1 the used mask is forced with all the variables used
    \param[in] masktype > 1 the used mask is the recomended one masked with the masktype value \n
    example: FillTH1(hh, 1,(ULong64_t) 0xFFFEFFFF7FEFD) will mask also vars 1,9,19,36
  */
  int FillTH1(TH1* histo, int regtype=0, ULong64_t masktype=0);
  
  /*! Retrieve the recommended bitwise mask from maskingnumber \n
    maskingnumber = 2*n = ecal missing     \n                               
    maskingnumber = 3*n = tof missing   \n                                                    
    maskingnumber = 5*n = trd missing   \n                                                                     
    maskingnumber = 7*n = rich missing  \n 
    return the recommended bitwise mask based on maskingnumber
  */
  ULong64_t GetRecoMask(int maskingnumber=1);
  
#ifndef _NOGBATCH_
  /*! Find the run type (ISS /BT-muons/ MC)  
    \return 1 if DATA                                             
    \return  2 if pre-launch BT or Muons (pre 1305331200) 
    \return  99 if MC                                         
  */
  static int FindRunType();  
#endif

  double GetEvaluator();

 private:
  TrkCCVar* varCC;
  TrkCCAlgorithm* algCC;
  
 public:
  inline TrkCCVar* GetTrkCCVar() { return varCC; }

 private:
#ifndef _NOGBATCH_
  ClassDef(TrkCC, 0)//0 since no I/O feature is needed
#endif //_NOGBATCH_
};
 
//----------------------------------------------------------------
 
#ifndef _NOGBATCH_
class TMVAxmlReader: public TObject {
#else
class TMVAxmlReader{
#endif
  /*! Gets list of variables names and spectator names from xml files of TMVA
    \author                                                                     
    cecilia.pizzolotto@pg.infn.it \n
  */
  
 public:
  /*!
  // Fills TString vector bdtvars with the variables got from the weight file "filename"
  // the last two position of vector are the number of Variables and the number of Spectators
  !*/
  void Get(std::vector <TString> &bdtvars,TString filename, Bool_t debug);

  TMVAxmlReader();
  ~TMVAxmlReader();

 private:
  Bool_t _debug;
  std::string _GetField(std::string line, std::string searchkey);
  Int_t  _GetVariables(TString varhead,  TString key, FILE *file, std::vector <TString> &bdtvars);

 private:
#ifndef _NOGBATCH_
  ClassDef(TMVAxmlReader, 0)//0 since no I/O feature is needed
#endif
};

//! used to access to TrkCC internal variables
#ifndef _NOGBATCH_
class TrkCCVar: public TObject {
#else
class TrkCCVar {
#endif
  /*!
    Object containing all the variables used by the Charge Confusion Classificators \n
    This class is mainly thought to be used internally but is useful to access to used variables
    and is needed if the user wants to use the TrkCC on custom ntuples: \n 
    UPDATE TODO! TO DO!
    the user should fill by itself the TrkCCVar object (take FillTrkCCVarFromGbatch() as example) \n
    then call the TrkCC:FillVar(TrkCCVar & ) and call TrkCC:DoTrkCC((*varv).energy, (*varv).pat). \n
    
    \author
    francesco.nozzoli@pg.infn.it \n
    matteo.duranti@pg.infn.it \n
    domenico.durso@pg.infn.it \n
  */

 private:
  //! Default contructor
  TrkCCVar();
  
  //! The head of the singleton
  static TrkCCVar* _head;
#pragma omp threadprivate(_head)
  
 public:
  /*!
    Accessor to the public instance of the singleton. \n
    If no instance has been previously created it will be done at this moment.
  */
  static TrkCCVar* gethead();
  
  ~TrkCCVar();

  //! initialize all (default) or the interested one
  void Init(int varnumber=-1);
  //! get the number of elements in TrkCCvar
  static inline int Get_nTrkCCvar() { return nTrkCCVar; }
  //! return 0 = ok   return -1 = problem
  int CheckVar(int varnumber);  

  //! fill the TrkCCVar&, variables from external full vector
  void FillFromVector(float varlistfull[nTrkCCVar]);

  //! retrieve the ordered element of TrkCCVar
  float GetFullVectorElement(int varnumber);

#ifndef _NOGBATCH_
  /*! fill the TrkCCVar&, variables from the ams rootuples, for the event and the ipart particle number \n
    abs(rebuilfRefit)=itrTrk 3rd parameter (0 = stored, 1 refit if necessary, 2 force trk refit, 3 fit and rebuild also Coo) \n
    rebuilfRefit <0 force rebuild of betah and anticluster \n                                             
    _gbatchPRODversion >0 and <900 force the B field correction \n
    fullProcess=false (only partial filling, fast)  fullprocess=true (2.5% slower) \n   
    part is the pre-selected ParticleR pointer \n                        
    return -2 = event problem \n
    return -1 = particle problem \n
    return 0 = trk problem        \n                               
    return 1 = ok                 \n                                        
    return 2*n = ecal problem     \n                                 
    return 3*n = tof problem   \n
    return 5*n = trd problem   \n                                           
    return 7*n = rich problem  \n
  */
  int FillFromGbatch(ParticleR* part, int rebuilfRefit = 0, int gbatchPRODversion=0, bool fullProcess=true);

  // Authors: M.Duranti - INFN di Perugia
  void PathOnB(TrTrackR* tr, int kDef, MagField* magfield, Double_t& B, Double_t& SenThetaX, Double_t& SenThetaY,Double_t& L, Double_t& pMDR, Double_t& pMDR_known, Double_t step=0.1, bool modulus=false);
#endif

  //-------------------------------------
  
  //useful since TrkVar is a singleton
  unsigned int run;
  //useful since TrkVar is a singleton
  unsigned int evtno;
  //useful since TrkVar is a singleton
  long ppno;
  
  // TOPOLOGIC VARIABLES
  //! event maskingnumber
  int nmsk;   
  //! presence of ly1  (derived from btp) 
  int ly1;    
  //! presence of ly2  (derived from btp)  
  int ly2;    
  //! presence of ly9  (derived from btp) 
  int ly9;    
  //! human readable pattern (derived from btp)
  /*! human readable pattern (derived from btp) \n
    3 bit pattern code: L1L9L2 e.g. (001) = (noLy1;noLy9;Ly2 fired) \n
    0 = 000; 1 = 001; 2 = 010; 3 = 011; 4 = 10x; 5 = 11x (full span) \n */
  int pat;    
  
  // R dependent variables
  //! max span choutko (algo = 1) 
  double rigidity_ch; 
  //! max span alcaraz (algo = 2) 
  double rigidity_al; 
  //! max span chikaninan (algo = 3) 
  double rigidity_ck; 
  //! max span choutko normalized chisquare x 
  double chix_ch;  
  //! max span choutko normalized chisquare y 
  double chiy_ch;  
  //! inner only choutko   (pattern = 3) 
  double rigidity_in;  
  //! inner + Ly9 choutko  (pattern = 6) 
  double rigidity_bt;  
  //! inner + Ly1 choutko  (pattern = 5)  
  double rigidity_tp;  
  //! inner bottom choutko (pattern = 2) 
  double rigidity_ibt; 
  //! inner top choutko   (pattern = 1) 
  double rigidity_itp;
  //! max span choutko with same weight fitting (algo = 21) 
  double err_ch;  
  
  // E dependent Variables
  
  //! ecalaxis chisquare 
  double axischi2;  
  //! tracker max charge on all Ly9 hits also offtrk
  double trk_ql9;  
  //! lower tof charge
  double tof_qd; 
  //! max tof charge 
  double tof_qmax; 
  //! nunused rich photons 
  int rich_nunused; 
  //! rich ring width (M)
  double rich_w; 
  //! rich photons/expected 
  double rich_ratio; 
  //! rich unused hit vs trk residual (M)
  double rich_res; 
  //! trk - ecal cofgx 
  double trk_cofgx; 
  //! trk - ecalaxis diry (M) 
  double trk_dirx; 
  //! tof chi2c (m) 
  double tof_chi2c; 
  //! tof chi2t (m) 
  double tof_chi2t; 
  //! tof-acc Nbackspash (P,E) 
  int nbks; 
  //! tof-acc N late backspash much delayed (P,E) 
  int nlbks; 
  //! number of trk cluster on ly9 over threshold (M,E) 
  int ntrkcl9;  
  //! number of tracker trks (M,E) 
  int ntrk;  
  //! number of tof trks (M) 
  int nbeta;  
  //! number of rich rings (M) 
  int nrings;  
  
  // Traking variables
  //! trk - ecalaxis cofgy 
  double trk_cofgy; 
  //! trk - ecalaxis diry 
  double trk_diry; 
  //! TRD-TRK y inclination 
  double trd_diry;  
  
  // ALMOST INDEPENDENT on E
  //! upper tof charge 
  double tof_qu; 
  //! rms tof charge 
  double tof_qrms; 
  //! used tof hits (M)
  int tof_nused; 
  //! tof betahs (P)
  double tof_beta;
  
  //! tracker charge 
  double trk_q;  
  //! tracker average rms of cluster y position alla layers 
  double trk_yrms;  
  //! tracker max charge on all (also off track) Ly1-8 hits 
  double trk_ql18;  
  
  // INDEPENDENT ON E
  //! TRD ntrack (M)
  int trd_ntrk;  
  //! TRD nhitted 
  int trd_nhit;  
  //! TRD sum(signal/pathlenght)/nhitted ontraks (M) 
  double trd_ratio;  
  //! TRD sumvoff(signal)/noff offtraks  
  double trd_ratioff;  
  //! TRD sumv(signal) total  
  double trd_sumv;  
  //! TRD nofftrack (M) 
  int trd_noff;  
  //! TRD pathlenght 
  double trd_path;  
  //! TRD-TRK minimum distance 
  double trd_mindist;  
  //! TRD charge (?) 
  double trd_q;  
  //! TRD chi2 (NULL) 
  double trd_chi2;  
   
  //! tracker charge RMS 
  double trk_qrms;  
  //! tracker max charge on track 
  double trk_qmax; 
  //! number of trk y cluster within 5cm layer 1-8 over threshold (M) 
  int ntrky5cl18;  
  //! tof-acc interaction hits (P) 
  int nint;  
  //! trk direction in y (M) 
  double dtrkavgy; 

  //! rich beta (P)
  double rich_beta;  
  //! rich unused hit rms 
  double rich_rms; 
   

  //! minimum noise hit distance in all layers
  double minoise1; 
  //! minimum noise hit distance in all layers excluding the layer where minoise1 is 
  double minoise2; 
  //! minimum noise hit distance excluding the layer where minoise1 and minoise2 are 
  double minoise3; 
  //! minimum noise hit distance excluding the layer where minoise1,minoise2,minoise3 are  
  double minoise4;

  //! number of layers used for the track
  int nlayer; 

  //! ecal energy
  double energy;  

  //! path_on_B_matteo_MDR 
  double pMDR; 

  //! max residual x point included 
  double maxresx0; 
  //! max residual y point included 
  double maxresy0; 
  //! next to max residual x point included 
  double maxresx1; 
  //! next to max residual y point included 
  double maxresy1; 
  //! min residual x point included 
  double minresx; 
  //! min residual y point included 
  double minresy; 
  //! Radiation Length crossed from AMS top to Z=0
  double x0rad;
  
 private:
#ifndef _NOGBATCH_
  ClassDef(TrkCCVar,0)//0 since no I/O feature is needed
#endif
};

#ifndef _NOGBATCH_
class TrkCCAlgorithm: public TObject {
#else
class TrkCCAlgorithm {
#endif

 public:
  //! Default contructor
  TrkCCAlgorithm();
  
  //! Get the sub-identifier
  inline virtual TString GetSubId() { return _subid; }
  
  //! Number of variables used internally
  inline virtual int GetNVars() { return _nvars; }
  
  //! Number of variables used internally
  inline virtual int GetVersion() { return _version; }

  virtual double GetEvaluator() = 0;
  
  virtual int LoadWeights(TString dbPath) = 0;

  // used bitwise mask in evaluation
  ULong64_t _usedmask;
  // recommendend bitwise mask in evaluation
  ULong64_t _recommendmask;// normally filled by TrkCC::FillVarList

  //! Not normalized variables
  float* _varlist;
  //! Variables after normalization
  float* _varnorm;
  //! matrix with the variables specifications
  float** _listamatr;

  //! energy used to select proper energy range in classifier
  float _evenergy;  //filled by FillVarList
  //! trk pattern used to select proper classifier 0=inner no ly2, 5=fullspan
  int _evpatt; //filled by FillVarList
  
 protected:
  //! Level of verbosity
  int _verbose;
  
  //! Evaluator sub-identifier ("A", "B", "C", ...)
  TString _subid;
  
  //! Identifier number of variables
  int _nvars;

  //! Identifier Version
  int _version;
  
#ifndef _NOGBATCH_
  ClassDef(TrkCCAlgorithm,0)//0 since no I/O feature is needed
#endif
};
 
//! max number of different regressions supported
#define nReg 6
//! max number of regression energy intervals
#define rnEne 10
 
class TrkCCNozzoli: public TrkCCAlgorithm {
  /*!
    Write something
    \author
    francesco.nozzoli@pg.infn.it \n
  */  
  
 private:  
  //! Default contructor
  TrkCCNozzoli(TString subid="A", int version=0, int verbose=0);
  
  //! The head of the singleton
  static TrkCCNozzoli* _head[nReg][nVer];
#pragma omp threadprivate(_head)

  //! Default deconstructor
  ~TrkCCNozzoli();

  /*! evaluate a regression that is the weight average of single var regressions \n
    reg[i]=(varnor[i]-b[i])/a[i]  and vreg=sum_i(reg[i]*ps[i])  nvr is maximum of i
    return 0 is OK;
    return <0 problem (e.g. if return is -2 the problem is at second variable that is varnor[1])
  */
  int _EvaluateRegression(float varnor[], float a[], float b[], float ps[], float& vreg);
  
  /*! retrieve the regression classifier
    \param[in] regtype = 0  CC-spillover   optimized for Electrons E-normalized
    \param[in] regtype = 1  CC-interaction optimized for Electrons E-normalized
    \param[in] regtype = 2  CC-poor-but-independent from 0 and 1 CC optimized for Electrons E-normalized
    \param[in] regtype = 3  Electron/Proton separation for Electrons E-normalized
    \param[in] regtype = 4  CC for Protons (R normalized)
    \param[in] regtype = 5  Electron/Proton separation for Protons R-normalized
    \param[in] masktype = 0 the used mask is the recommended one
    \param[in] masktype = 1 the used mask is forced with all the variables used
    \param[in] masktype > 1 the used mask is the recomended one masked with the masktype value \n
    example: GetRegression(1,(ULong64_t) 0xFFFEFFFF7FEFD) will mask also vars 1,9,19,36 \n
    fixbin>=0 is used for forcing the use of a fixed training energy bin (calibration)
  */
  float GetRegression(int regtype=0, ULong64_t masktype=0, int fixbin=-1);


  int _LoadRegressionWeights(TString weightsPath);
  
 public:
  /*!
    Accessor to the public instance of the singleton. \n
    If no instance has been previously created it will be done at this moment.
  */
  static TrkCCNozzoli* gethead(TString subid="A", int version=0, int verbose=0);
    
  //! number of different regressions really adopted
  int _lastreg;

  // max number of variables used in each regression
  int _rmaxvar[nReg];

  //! adopted number of regression energy intervals
  int _nene[nReg];
  //! thresholds of regression energy intervals
  int _reneth[nReg][rnEne];

  //! weights of regressions
  float* _rweight[nReg][nTrkCCpatts][3][rnEne];  // [][][0][][] = pendence  [][][1][][]= termine noto [][][2][][]=peso
    //! average of the regressions
    float _mddd[nReg][rnEne][nTrkCCpatts];
  //! RMS of the regressions
  float _rmss[nReg][rnEne][nTrkCCpatts];

  /*!
    default: normalize the vector ps                 \n            
    when 0<=imatr<_nvars and 0<nval<1                \n
    then ps is normalized keeping fix ps[nvars]=nval \n
    return  0=ok, 1= error */                  
  static int NormWeights(float ps[], int nvars, int imatr=-1, float nval=0);

  /*! same as _EvaluateRegression but return also the single regression values through reg[]
    return 0 is OK;
    return <0 problem (e.g. if return is -2 the problem is at second variable that is varnor[1])
  */
  static int EvaluateRegression(float varnor[], float a[], float b[], float ps[], float reg[], int nvr, float& vreg, int debug = 0, ULong64_t mask=0xFFFFFFFFFFFFFUL);
  
  static inline int Get_nVarsTrkCCNozzoliVersion(int iver=1) {
    if (iver==1) return nVarsTrkCCNozzoliVersion1;
    else {
      printf("TrkCC-W-Version %d not valid...\n", iver);
      return -99;
    }
  }
  double GetEvaluator();

  int LoadWeights(TString dbPath) { return _LoadRegressionWeights(dbPath); };
  
#ifndef _NOGBATCH_
  ClassDef(TrkCCNozzoli,0)//0 since no I/O feature is needed
#endif
};

// number of trained bdts
#define nBDT 3
 
class TrkCCPizzolotto: public TrkCCAlgorithm {
  /*!
    Write something
    \author
    cecilia.pizzolotto@pg.infn.it \n
  */  
  
 private:
  //! Default contructor
  TrkCCPizzolotto(TString subid="A", int version=0, int verbose=0);
  
  //! The head of the singleton
  static TrkCCPizzolotto* _head[nBDT][nVer];
#pragma omp threadprivate(_head)

  //! Default deconstructor
  ~TrkCCPizzolotto();

  bool _rarity;

  float GetBDT(int ib);
  float GetRarity(int ib);

  /// Initialize BDT reader
  TMVA::Reader* _InitBDTReader(TString weightsPath, TString bdtName);

  int _LoadBDTWeights(TString weightsPath);
  
 public:
  /*!
    Accessor to the public instance of the singleton. \n
    If no instance has been previously created it will be done at this moment.
  */
  static TrkCCPizzolotto* gethead(TString subid="A", int version=0, int verbose=0);
  
  void SetRarity(bool on=true);
  
  // TMVA reader for all bdts
  TMVA::Reader *_reader[nBDT];
  
  double GetEvaluator();
  
  int LoadWeights(TString dbPath) { return _LoadBDTWeights(dbPath); };
  
#ifndef _NOGBATCH_
  ClassDef(TrkCCPizzolotto,0) //0 since no I/O feature is needed
#endif
};
 
static int MakeSubIdInt(TString subid);
static void PrintError(TString err, bool exempt=false);

#endif // _PGTRACK_
#endif // __TRKCC_H__
