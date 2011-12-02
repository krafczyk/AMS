#ifndef _TrdHCharge_
#define _TrdHCharge_
#include <map>
#include "point.h"
#include "TrPdf.h"

class AMSTimeID;
class AMSEventR;
class TrdHTrackR;


using namespace std;

/// class to perform and store TRD H charge reconstruction
class TrdHChargeR{
 private:
  /// singleton instance
  static TrdHChargeR* head;
  
  /// internal map of TDV pointers
  map<string,AMSTimeID*> tdvmap;

  /// internal variable to control TDV
  unsigned int lastrun;

  string tag;
 public:

  /// container to store tube energy deposition spline fit for charges
  map<int,TrPdf*> pdfs;

  /// container to store tube energy deposition histograms for charges
  map<int,TH1F*> spectra;

  /// array for TDV interaction (charged particles)
  static float charge_hist_array[10][1000];
  
  /// array for TDV interaction (electrons)
  static float electron_hist_array[10000];

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
  TrdHChargeR(){
    lastrun=0;
    tag="";
    pdfs.clear();
    spectra.clear();
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
    
  };
  
  /// build PDFs
  int CreatePDFs(int debug=0);
  
  /// calculate charge probabilities 
  int GetCharge(TrdHTrackR *tr,float rig=0.,int debug=0);

  //  double GetTrdChargeMH(TrdHTrackR *trd_track, float beta, int z);
  
  /// get electron likelihood (-log(elik/elik+elik) - 2 hypothesis e or p)
  /// opt%10 == 0/1/2 -> ratio / electron / proton likelihood
  /// opt <0: old method, 0<opt<10: standalone TRD, 10<opt<20: rigidity corrected
  float GetELikelihood(TrdHTrackR *tr, int opt=-10,int debug=0);

  /// Convert information from TDV into internal median structure
  bool FillPDFsFromTDV(int debug=0);
  
  /// Convert information from internal median to TDV structure
  bool FillTDVFromPDFs(int debug=0);
  
  /// Add TrdHTrack information to charge histograms
  int AddEvent(TrdHTrackR *track,int charge=0, int debug=0);
  
  /// Create logarithmic binning - 10 bins per decade - parameters: number of [decades], starting at 10^([base])
  int CreateBins(int decades=3, int base=1, int binsperdecade=10 );
  
  /// Initialization for event analysis 
  int Initialize(AMSEventR* ev, char *databasedir="");

  /// Initialize interfaces to TDV for ALL Containers
  int initAllTDV(unsigned int bgtime, unsigned int edtime, int type,char *databasedir="");

  /// Write ALL available TDV interfaces (loaded by Init*) to TDV at 'databasedir'
  int writeAllTDV(unsigned int begin, unsigned int end,int debug=0,char *databasedir="");
  
  /// Write single TDV interface 'which' (loaded by Init*) to TDV at 'databasedir'
  int writeSpecificTDV(string which,unsigned int begin, unsigned int end,int debug=0,char *databasedir="");
  
  /// Read ALL loaded TDV interfaces for time 't'
  int readAllTDV(unsigned int t, int debug=0);

  /// Read single loaded TDV interface for time 't'
  int readSpecificTDV(string which,unsigned int t, int debug=0);
  
  /// Close ALL currently open TDV interfaces
  bool closeTDV();
  
  string GetStringForTDVEntry(int n);
  int GetTDVEntryForMapKey(int c);

  ClassDef(TrdHChargeR,3)
};
#endif

