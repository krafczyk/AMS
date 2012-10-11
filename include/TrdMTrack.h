//**************************************************************//
// TrdMTrack methods
//

//
//**************************************************************//


#ifndef _TrdMTrack_
#define _TrdMTrack_

#include <TrdRawHit.h>
#include <TrTrack.h>
#include <TrRecHit.h>
#include <TrFit.h>
#include <MagField.h>
#include <TkDBc.h>
#include <TrdKCalib.h>
#include <TrdKCluster.h>
#include <TrdKPDF.h>
#include <TGraph.h>
#include <TVector3.h>
#include <TMath.h>
#include <TTree.h>
#include <TBranch.h>
#include <root.h>


class TrdMTrack : public TObject
{
 public:
  
  
  // Constructor for Analysis on AMS Root files
  TrdMTrack();
  TrdMTrack(int gaintype, int aligntype, int pdftype);
  ~TrdMTrack();
  
  // processing event
  float GetLogLikelihood(AMSEventR* _evt, ParticleR* par);
  
  // Cluster status
  int  NHits();
  int  nMissHit_usedX();
  int  nMissHit_usedY();
  
  //  TRD Refit
  void SetupMHTrack(TrTrackR *track, TrdTrackR *trd);
  
  //Setup Hit Selection
  void MakeHitSelection();
  
  // GetLikelihoodRatio
  float CalcLogLikelihood();
  
  // Get Refitted TRD Track
  TrFit* GetMHFit(){ if(isprocessed) return _fit; else return 0;}
  
  // Hit accessor
  TrdRawHitR* pTrdMHit(int i){if(i>NHits() || i<0)return 0;return TrdMHits.at(i);}
  
  // for processing
  float GetGainCorrection(int module, int tube=0);
  float GetXePressure();
 
  // Globlal Validity flag
  int GetValidity(){return IsValid;}
  
  // trd methods (0: TrdSCalib, 1: TrdKCalib)
  int TrdGainType, TrdPdfType, TrdAlignType ; 
		   
  
 private:
  
  // Initilization
  void Init_Base();
  void generate_modul_matrix();
  
  // for processing
  void SetAlignment();
  int which_tube(int lay, float loc, int mod);
  float GetPathLength3D(AMSPoint trdpnt, AMSDir trddir, AMSPoint tpnt, AMSDir tdir );
  void interpolate_trd_track(TrdTrackR* trdtrack, AMSPoint &pnt, AMSDir &dir, int z);
  float get_trd_rz(int layer, int ladder, int tube, int mod);
  double GetProProb(int xeindex, int sindex, float amp);
  double GetProProb(float amp, float rig, float pl, float lay, float xe );
  double GetElProb(int xeindex, int sindex, float amp);
  double GetElProb(float amp, float rig, float pl, float lay, float xe );

  // additional Initalisation
  void Init_Alignment();
  void Init_GainCorrection();
  void Init_PDFs();
  void Init_Xe();
  
  long unsigned int FirstRun;
  long unsigned int LastRun;
  
  static TrdKCalib *trdk_db;
  static TrdKPDF *Epdf;
  static TrdKPDF *Ppdf;

  int _id;
  float _rig;
  TrFit* _fit;
  AMSEventR *evt;
  
  time_t _Time;
  bool IsValid;
  bool isprocessed;
  bool misshit;
  int n_mhX;
  int n_mhY;
  float xycorr[328][6];
  
  vector<float> mod_corr[328][4];
  vector<float> mod_time[328];
  float z_corr[328][2];
  
  vector<float> gaintime[328];
  vector<float> gain[328];
  
  //TGraph *xenon;
  vector<float> xetime;
  vector<float> xe;
  
  TGraph *ElPDF[6][20];
  TGraph *ProPDF[6][30];
  
  float mStraw[18][20];
  
  vector<TrdRawHitR*> TrdMHits;
  vector<float> phitlen;
  
  ClassDef(TrdMTrack,1);
    
};

#endif // TrdMTRACK_H
