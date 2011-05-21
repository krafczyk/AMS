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
  static TrdHChargeR* head;
 public:

  /// amplitude cut for cluster counting
  float ccampcut;

  /// container to store tube energy spline fit for charges
  //  map<int,TSpline5> pdfs;
  map<int,TrPdf*> pdfs;

  map<int,TH1F> spectra;

  /// container to store probability of charge for event
  map<double,int> charge_probabilities;

  /// factor to convert from adc to kev (default 33.33)
  float adc2kev;

  float charge_hist_array[30000];
  
  map<string,AMSTimeID*> tdvmap;

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
    pdfs.clear();
    charge_probabilities.clear();
    //    BuildPDFs();
    for(int i=0;i<10000;i++)charge_hist_array[i]=0.;
  };
  
    ~TrdHChargeR(){
      for(int i=0;i<pdfs.size();i++)
	delete pdfs[i];
      pdfs.clear();
      charge_probabilities.clear();
    };

    /// build PDFs from database
    int CreatePDFs(int debug=0);

    /// get most probable charge
    int GetCharge(TrdHTrackR *tr,float beta=0., int debug=0);

    //  double GetTrdChargeMH(TrdHTrackR *trd_track, float beta, int z);

    /// get electron likelihood (-log(elik/elik+elik) - 2 hypothesis e or p)
    float GetELikelihood(TrdHTrackR *tr,float beta=0., int opt=0,int debug=0);

    float GetELikelihoodNEW(int debug=0);

    /// get numbr of hits above CC amplitude cut
    int GetNCC(TrdHTrackR *tr,int debug=0);
  
    bool readTDV(unsigned int t, int debug=0);
    
    bool closeTDV();
    
    bool FillPDFsFromTDV();
    
    bool FillTDVFromPDFs();
    
    int AddEvent(TrdHTrackR *track,float beta, int charge, int debug=0);
    
    bool InitTDV(unsigned int bgtime, unsigned int edtime, int type,
#ifdef __ROOTSHAREDLIBRARY__
		 char *tempdirname="/Offline/AMSDataDirRW");
#else
    char *tempdirname="/f2users/mmilling");
#endif

int writeTDV(unsigned int begin, unsigned int end,int debug=0,
#ifdef __ROOTSHAREDLIBRARY__
	     char *tempdirname="/Offline/AMSDataDirRW");
#else
char *tempdirname="/f2users/mmilling");
#endif

ClassDef(TrdHChargeR,1)
  };
#endif

