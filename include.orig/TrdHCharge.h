#ifndef _TrdHCharge_
#define _TrdHCharge_
#include <map>
#include "point.h"
#include "TrdHTrack.h"
#include "TrPdf.h"
class AMSTimeID;

using namespace std;

/// class to perform and store TRD H charge reconstruction
class TrdHChargeR{
 private:
  /// singleton instance
  static TrdHChargeR* head;
  
  /// internal map of TDV pointers
  map<string,AMSTimeID*> tdvmap;

  string tag;
 public:

  /// amplitude cut for cluster counting
  float ccampcut;

  /// container to store tube energy deposition spline fit for charges
  map<int,TrPdf*> pdfs;

  /// container to store tube energy deposition histograms for charges
  map<int,TH1F*> spectra;

  /// container to store probability of charge for event
  map<double,int> charge_probabilities;

  /// factor to convert from adc to kev (default 33.33)
  float adc2kev;

  /// array for TDV interaction (charged particles)
  float charge_hist_array[10][1000];
  
  /// array for TDV interaction (electrons)
  float electron_hist_array[20000];

  /// number of bins in logarithmic binning (optional)
  int nlogbins;

  /// logarithmic binning (optional)
  double logbins[100];
  
  /// flag to control usage of electron PDFS 1:per layer 0:all layers
  bool use_single_layer_pdfs;

  /// Get the pointer to the DB
  static TrdHChargeR* gethead(){
    if(!head)head=new TrdHChargeR();
    return head;
  }
  
  /// Get the pointer to the DB
  static void  KillPointer(){
    delete head;
    head=0;  //VC
  }

  /// default ctor
  TrdHChargeR():ccampcut(6.),adc2kev(100./3.){
    tag="";
    pdfs.clear();
    spectra.clear();
    charge_probabilities.clear();
    nlogbins=0;
    use_single_layer_pdfs=true;
    for(int i=0;i<100;i++)logbins[i]=0.;
    for(int i=0;i<10;i++)
      for(int j=0;j<1000;j++)charge_hist_array[i][j]=0.;
    for(int i=0;i<10000;i++)
      electron_hist_array[i]=0.;
  };
    
  TrdHChargeR(string s ){
    TrdHChargeR();
    tag=s;
  };

  /// dtor
  ~TrdHChargeR(){
    for(int i=0;i<pdfs.size();i++)
      delete pdfs[i];
    pdfs.clear();
    
    for(int i=0;i<spectra.size();i++)
      delete spectra[i];
    spectra.clear();
    
    charge_probabilities.clear();
  };
  
  /// build PDFs
  int CreatePDFs(int debug=0);
  
  /// get most probable charge
  int GetCharge(TrdHTrackR *tr,int opt=0,float beta=0., int debug=0);
  
  //  double GetTrdChargeMH(TrdHTrackR *trd_track, float beta, int z);
  
  /// get electron likelihood (-log(elik/elik+elik) - 2 hypothesis e or p)
  float GetELikelihood(TrdHTrackR *tr,float beta=0., int opt=0,int debug=0);

  /// IN DEVELOPMENT - new electron likelihood method - multihypothesis - bitwise flagged by opt bit 0:all 1:use proton 2:use helium etc. 
  float GetELikelihoodNEW(int opt=0,int debug=0);
  
  /// get numbr of hits above CC amplitude cut
  int GetNCC(TrdHTrackR *tr,int debug=0);
  
  /// Convert information from TDV into internal median structure
  bool FillPDFsFromTDV(int debug=0);
  
  /// Convert information from internal median to TDV structure
  bool FillTDVFromPDFs(int debug=0);
  
  /// Add TrdHTrack information to charge histograms - 'opt' is bitwise disable of gain/path/beta correction
  int AddEvent(TrdHTrackR *track,int opt=0, float corr=1.,float beta=0., int charge=0, int debug=0);
  
  /// Create logarithmic binning - 10 bins per decade - parameters: number of [decades], starting at 10^([base])
  int CreateBins(int decades=3, int base=1);
  
  /// Initialize interfaces to TDV for ALL Containers
  int initAllTDV(unsigned int bgtime, unsigned int edtime, int type,char *tempdirname="");

  /// Write ALL available TDV interfaces (loaded by Init*) to TDV at 'dirname'
  int writeAllTDV(unsigned int begin, unsigned int end,int debug=0,char *tempdirname="");
  
  /// Write single TDV interface 'which' (loaded by Init*) to TDV at 'dirname'
  int writeSpecificTDV(string which,unsigned int begin, unsigned int end,int debug=0,char *tempdirname="");
  
  /// Read ALL loaded TDV interfaces for time 't'
  int readAllTDV(unsigned int t, int debug=0);

  /// Read single loaded TDV interface for time 't'
  int readSpecificTDV(string which,unsigned int t, int debug=0);
  
  /// Close ALL currently open TDV interfaces
  bool closeTDV();

  /// Get Name string for TDV entry iterator
  string GetStringForTDVEntry(int n);
  
  ClassDef(TrdHChargeR,2)
};
#endif

