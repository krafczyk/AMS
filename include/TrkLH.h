#ifndef __TRKLHVAR_H__
#define __TRKLHVAR_H__

#include <TObject.h>

class TrkLHVar: public TObject {
  /*!
    Object containing all the variables used by the Tracker Likelihood. \n
    This class is mainly thought to be used internally but is needed if the user wants to use 
    the likelihood on custom ntuples: the user should fill by itself the TrkLHVar object and call the 
    trklk:trklkvarfromgbatch(TrkLHVar&, AMSEventR*, int).\n

    \author
    francesco.nozzoli@pg.infn.it
    matteo.duranti@pg.infn.it
    domenico.durso@pg.infn.it
  */
 public:

  TrkLHVar();
  ~TrkLHVar();
  void init();
  
  int btp;    //! tracker bitpattern
  int ly1;     //! presence of ly1  (derived from btp) 
  int ly2;     //! presence of ly2  (derived from btp) 
  int ly9;     //! presence of ly9  (derived from btp) 
  int pat;     //! human readable pattern   (derived from btp) 
  double energy;       //! ecal energy
  double rigidity_ch;  //! max span choutko (algo = 1) 
  double rigidity_al;  //! max span alcaraz (algo = 2) 
  double chix_ch;   //! max span choutko normalized chisquare x
  double chiy_ch;   //! max span choutko normalized chisquare y
  double err_ch;   //! max span choutko with same weight fitting (algo = 21) 
  double rigidity_in;  //! inner only choutko   (pattern = 3) 
  double rigidity_bt;  //! inner + Ly9 choutko  (pattern = 6) 
  double rigidity_tp;  //! inner + Ly1 choutko  (pattern = 5) 
  double rigidity_ibt;  //! inner bottom choutko(pattern = 2) 
  double rigidity_itp;  //! inner top choutko   (pattern = 1) 
  double trkqin;  //! inner tracker charge
  double trkqrms;  //! tracker charge RMS 
  double minoise1;  //! minimum noise hit distance in all layers
  double minoise2;  //! minimum noise hit distance in all layers excluding the layer where minoise1 is 
  double minoise3;  //! minimum noise hit distance excluding the layer where minoise1 and minoise2 are 
  double minoise4;  //! minimum noise hit distance excluding the layer where minoise1,minoise2,minoise3 are 
  double cosy; //! absolute value of cosine of angle of trk respect to Y axis 
  double nlayer; //! number of layers used for the track
  double tof_qd; //! lower tof charge
  double tof_qu; //! upper tof charge 
  double sinq_trk_ecal; //! squared sine of angle trk-ecal
  double sinq_trk_trd; //! squared sine of angle trk-trd
  double ecal_ecogx; //! ecal cog x coordinate
  double ecal_ecogy; //! ecal cog y coordinate
  double trk_ecogx; //! tracker extrapolation of ecal cog x coordinate
  double trk_ecogy; //! tracker extrapolation of ecal cog y coordinate
  double ntrk; //! number of tracks 
  double nanti; //! number of anti fired

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
#include <root_RVSP.h>
#endif

class TrkLH: public TObject {
  /*!
    \author
    francesco.nozzoli@pg.infn.it
    matteo.duranti@pg.infn.it
    domenico.durso@pg.infn.it
  */  

 private:

  //! Default contructor
  TrkLH();

  //! Clean a previous instance of the class. Use internallu
  void clear();

  //! The head of the singleton
  static TrkLH* head;

  //! Populate, from gbatch, the TrkLHVar used internally
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

  //! Initialize the Likelihood and this will need to access the database directory
  // (via $AMSDataDir/v5.00/TrkLH or $TrkLHDataDir) holding the root files for pdfs and normalizations
  int TRLKINIT(int _NVER=1);

  int VARN;
  int PATN;
  int NVER;

  double GetLikelihoodRatioElectronWG(int ipart=0);

  TrkLHVar GetTrkLHVar(int ipart);

  TrkLHVar* varv;

  int* varmask;
  double* varlist;
  double* varnorm;
  TH2D*** hlist0; //! PDFs for WRONG SIGN
  TH2D*** hlist1; //! PDFs for GOOD SIGN
  TGraph*** low;
  TGraph*** upp;  
  int ntail0;
  int ntail1;
  int ntail2;
  int ntailc0;
  int ntailc1;
  
  int fillvar(TrkLHVar&);
  double dotrklk(double, int);
  int replacemask(int*);
  
  static double bilinear(TH2D*, double, double, int);
  static double normvar(TGraph*, TGraph*, double, double);

  void SetDefaultMask(); //using default value of VARN=23
  
#ifndef _NOGBATCH_
  int filltrklhvarfromgbatch(TrkLHVar&, AMSEventR*, int);
#endif

 private:
  void curazeri(TH2D*);

  int TRLKINIT_v1();

  ClassDef(TrkLH,0);//0 since no I/O feature is needed
};

#endif
