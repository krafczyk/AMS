#ifndef __TRKLHVAR_H__
#define __TRKLHVAR_H__
#ifdef _PGTRACK_

#include <TObject.h>

//! used to access to TrkLH internal variables
class TrkLHVar: public TObject {
  /*!
    Object containing all the variables used by the Tracker Likelihood. \n
    This class is mainly thought to be used internally but is useful to access to used variables
    and is needed if the user wants to use the likelihood on custom ntuples: \n 
    the user should fill by itself the TrkLHVar object (take filltrklhvarfromgbatch() as example) \n
    then call the trklk:fillvar(TrkLHVar & ) and call trklk:dotrklk((*varv).energy, (*varv).pat). \n

    \author
    francesco.nozzoli@pg.infn.it \n
    matteo.duranti@pg.infn.it \n
    domenico.durso@pg.infn.it \n
  */

 public:

  TrkLHVar();
  ~TrkLHVar();
  void init();
  
//! tracker bitpattern
  int btp;   
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
 //! ecal energy
  double energy;  
 //! max span choutko (algo = 1) 
  double rigidity_ch; 
 //! max span alcaraz (algo = 2) 
  double rigidity_al; 
 //! max span choutko normalized chisquare x 
  double chix_ch;  
 //! max span choutko normalized chisquare y 
  double chiy_ch;  
//! max span choutko with same weight fitting (algo = 21) 
  double err_ch;  
//! inner only choutko   (pattern = 3) 
  double rigidity_in;  
//! inner + Ly9 choutko  (pattern = 6) 
  double rigidity_bt;  
//! inner + Ly1 choutko  (pattern = 5)  
  double rigidity_tp;  
//! inner bottom choutko(pattern = 2) 
  double rigidity_ibt; 
 //! inner top choutko   (pattern = 1) 
  double rigidity_itp;
//! inner tracker charge 
  double trkqin;  
 //! tracker charge RMS 
  double trkqrms;  
 //! minimum noise hit distance in all layers
  double minoise1; 
 //! minimum noise hit distance in all layers excluding the layer where minoise1 is 
  double minoise2; 
//! minimum noise hit distance excluding the layer where minoise1 and minoise2 are 
  double minoise3; 
 //! minimum noise hit distance excluding the layer where minoise1,minoise2,minoise3 are  
  double minoise4; 
//! absolute value of cosine of angle of trk respect to Y axis 
  double cosy; 
//! number of layers used for the track
  double nlayer; 
//! lower tof charge
  double tof_qd; 
//! upper tof charge 
  double tof_qu; 
//! squared sine of angle trk-ecal
  double sinq_trk_ecal; 
//! squared sine of angle trk-trd 
  double sinq_trk_trd; 
//! ecal cog x coordinate 
  double ecal_ecogx;  
//! ecal cog y coordinate 
  double ecal_ecogy;  
//! tracker extrapolation of ecal cog x coordinate
  double trk_ecogx; 
//! tracker extrapolation of ecal cog y coordinate
  double trk_ecogy; 
//! number of tracks 
  double ntrk;  
//! number of anti fired
  double nanti;

 private:

  ClassDef(TrkLHVar,0);//0 since no I/O feature is needed
};

#endif

//----------------------------------------------------------------

#ifndef __TRKLH_H__
#define __TRKLH_H__

#include <TH2D.h>
#include <TGraph.h>
#include <TObject.h>

#ifndef _NOGBATCH_
#include "root.h"
#endif

//! Tracker Likelihood
  /*!
  Evaluate tracker likelihood \n
  include TrkLH.h and in the event loop \n
  double lktrk = TrkLH::gethead()->GetLikelihoodRatioElectronWG(ipart); \n
  where 'ipart' is the number of the ParticleR you want to use... \n
  Existence of ECAL shower pointer for the particle is necessary \n
  To avoid annoying messages: \n
  Error in <TBufferFile::ReadObject>: trying to read an emulated class \n
  (TPaletteAxis) to store in a compiled pointer (TObject) \n
  #include <TApplication.h> \n
  and then in the main function insert somewhere: \n
  TApplication theApp("App", &argc, argv); \n

  */  
class TrkLH: public TObject {
  /*!
  Evaluate tracker likelihood \n
  include TrkLH.h and in the event loop \n
  double lktrk = TrkLH::gethead()->GetLikelihoodRatioElectronWG(ipart); \n
  where 'ipart' is the number of the ParticleR you want to use... \n
    \author
    francesco.nozzoli@pg.infn.it \n
    matteo.duranti@pg.infn.it \n
    domenico.durso@pg.infn.it \n
  */  

 private:

  //! Default contructor
  TrkLH();

  //! Clean a previous instance of the class. Use internally
  void clear();

  //! The head of the singleton
  static TrkLH* head;

  //! Populate, from gbatch, the TrkLHVar used internally ipart is the particle number
  int PopulateTrkLHVar(int ipart);

 public:

  //! Default deconstructor
  ~TrkLH();
  
  /*!
    Accessor to the public instance of the singleton. \n
    If no instance has been previously created it will be done at this moment.
  */
  static TrkLH* gethead();

  //This method is given since if the user wants to use, in the same programs two version,
  // he could "save" the head (gave by gethead()), call renewhead() and TRLKINIT(NVER) once
  // and then use gethead() normally.
  //This is a trick and essentially means that is not a singleton even if, for the "standard user"
  // it behaves as...
  static void renewhead();

  //! Initialize the Likelihood and this will need to access the database directory (return 0 = ok)
  // (via $AMSDataDir/v5.00/TrkLH or $TrkLHDataDir) holding the root files for pdfs and normalizations
  int TRLKINIT(int _NVER=1);

  int VARN;
  int PATN;
  int NVER;

  //! return the trk likelihood for the particle ipart
  double GetLikelihoodRatioElectronWG(int ipart=0);

  //! to access to internally used variables for the particle ipart
  TrkLHVar GetTrkLHVar(int ipart);

  //! internally used variables for the particle ipart, (to access use GetTrkLHVar)
  TrkLHVar* varv;

  //! masking vector if varmask[3] = 0 => variable[3] not used in TrkLH
  int* varmask;
  //! vector of not normalized variables  
  double* varlist;
  //! vector of normalized (mean=0 sigma=1 when possible) variables  
  double* varnorm;
//! PDFs for WRONG SIGN
  TH2D*** hlist0; 
//! PDFs for GOOD SIGN
  TH2D*** hlist1; 
  //! 16% lower threshold for variables 
  TGraph*** low;
  //! 16% upper threshold for variables 
  TGraph*** upp;
  //! number of cases pdf wrong is empty and pdf good is below the pdf wrong error (uknown)   
  int ntail0;
  //! number of cases pdf good is empty and pdf wrong is below the pdf good error (uknown)   
  int ntail1;
  //! number of cases pdf wrong is empty and pdf good is empty (uknown)   
  int ntail2;
  //! number of cases pdf wrong is empty and pdf good is larger than the pdf wrong error (recovered as good-like)   
  int ntailc0;
  //! number of cases pdf good is empty and pdf wrong is larger than the pdf good error (recovered as wrong-like)   
  int ntailc1;
  
  /* fill the var1 data structure and normalize the variables\n  
     starting from the pre-filled human readable data structure TrkLHVar*/
  int fillvar(TrkLHVar& var1);
  //! internally evaluate tracker likelihood for energy and pattern
  double dotrklk(double energy, int pat);

  /*! set an user defined variable mask  (0 means variable not used)                      
  to avoid use of some variables in TrkLH e.g. \n
  int newmask[23]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0}; \n
  TrkLH::replacemask(newmask); \n
  use all variables but the last one (E/R) \n
  see fillvar() for the meaning of variables */
  int replacemask(int* newmask);
  
  /*!  from a TH2D interpolate value at (x,y) \n  
    ilin = 0 no interpolation     \n                                                
    ilin = 1 bilinear interpolation \n 
    TH2D values expected in the interval [0,1] \n
    normal return is expected in the range (0,1] \n
    if interpolation result is zero return a negative sign 
    value equal in size to the error in the TH2D bin (negative upper limit)*/ 
  static double bilinear(TH2D* h, double y , double x, int ilin);

  /*! low and upp TGraph are 1 sigma lower and upper thresholds of
  the variable at different abs(rigidity) \n                 
  return normalized variable thet is centered and the width is in number of sigma */
  static double normvar(TGraph* low, TGraph* upp, double var, double rigid);

  //! set the recommended mask 
  void SetDefaultMask(); //using default value of VARN=23
  
#ifndef _NOGBATCH_
  /*! fill the TrkLHVar&, variables from the ams rootuples, for the event and the ipart particle number \n
   ipart is the pre-selected particle number \n                                  
   return -2 = event problem \n
   return -1 = particle problem \n
   return 1 = ok                 \n                                             
   return 0 = trk problem        \n                                             
   return 2*n = ecal problem     \n                                             
   return 3*n = tof problem   \n 
   */
  int filltrklhvarfromgbatch(TrkLHVar&, AMSEventR* evt, int ipart);
#endif

 private:

  int TRLKINIT_v1();

  ClassDef(TrkLH,0);//0 since no I/O feature is needed
};

#endif
#endif
