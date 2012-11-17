//  $Id: Tofcharge_ihep.h,v 1.5 2012/11/17 22:22:30 qyan Exp $

//Author Qi Yan 2012/Oct/01 15:56 qyan@cern.ch  /*IHEP TOF Charge Likelihood version(BetaH)*/
#ifndef __TOFCHARGE_IHEP__
#define __TOFCHARGE_IHEP__
//#include "root.h"
#include "TObject.h"
#include "TF1.h"
#include "TrElem.h"
#include "Tofdbc.h"

class BetaHR;

/////////////////////////////////////////////////////////////////////////
/// TofChargePar structure
/*!
 *  Charge-Measurment for Each Layer
 * \author qyan@cern.ch
*/
//////////////////////////////////////////////////////////////////////////
class TofChargePar: public TObject{

   public:
  /// Layer
  int    Layer;
  /// Bar
  int    Bar;
  /// Anode Q2
  float  QLA;
  /// Dynode Q2
  float  QLD;
  /// Anode RawQ2 
  float  QLARaw;//Before BetaCor
  /// Dynode RawQ2
  float  QLDRaw;
  /// Beta 
  float  Beta;
  /// PathLength
  bool  IsGoodPath;
  /// Default Q //Best Between Anode +Dynode 
  float GetQ();
  /// All Z and ProbZ in PDF
  vector <pair<int, float> > ProbZ;
  /// Get Prob to Be Z 
  float GetProbZ(float Z); 
  /// Push_back ProbZ-vector prob value from Z=ZL to Z=ZH 
  void  FillProbZ(int ZL,int ZH);

  public:
    TofChargePar(){QLA=QLD=QLARaw=QLDRaw=Beta=Layer=Bar=-1; IsGoodPath=0; ProbZ.clear();}
    TofChargePar(int _Layer,int _Bar,float _QLA,float _QLD,float _QLARaw,float _QLDRaw,float _Beta,bool _IsGoodPath):
                   Layer(_Layer), Bar(_Bar), QLA(_QLA), QLD(_QLD), QLARaw(_QLARaw), QLDRaw(_QLDRaw),  Beta(_Beta), IsGoodPath(_IsGoodPath){
                      ProbZ.clear();
                   }
   virtual ~TofChargePar(){};

   ClassDef(TofChargePar,1);
};

/////////////////////////////////////////////////////////////////////////
/// TofLikelihoodPar structure
/*!
 * Likelihood for Each Charge
 * \author qyan@cern.ch
*/
//////////////////////////////////////////////////////////////////////////
class TofLikelihoodPar: public TObject{

  public: 
  /// Use TOF Layer Pattern =>1001 110 or Other
  int  LPattern;
  /// Charge Z (<0 Means No Measurement)
  int  Z;
  /// Log Likelihood
  float Likelihood;
  /// Prob
  float Prob;
  /// Number of Layer for Likelihood Measurement
  int  GetnLayer();
  ///return true  If iLayer(0-3) is used for  Likelihood Measurement
  bool IsUsedL  (int iLayer);

 public:
  TofLikelihoodPar(){Likelihood=Prob=LPattern=Z=-1;}
  TofLikelihoodPar(int  _LPattern,int _Z, float _Likelihood):
                  LPattern(_LPattern),Z(_Z),  Likelihood(_Likelihood){Prob=0;}
  virtual ~TofLikelihoodPar(){};

  bool operator <(const TofLikelihoodPar &right) const {//put likehood lagest to the first
    return (-Likelihood<-right.Likelihood);
  }
  
  ClassDef(TofLikelihoodPar,1);
};

//////////////////////////////////////////////////////////////////////////
/// TofChargeHR structure
/*!
 *  TOF Charge Likelihood Manager which can be access From BetaH gTofCharge()
 *  \author qyan@cern.ch
*/
////////////////////////////////////////////////////////////////////////
class TofChargeHR: public TrElem {

public:
  /// Charge Measument Par Of All Layer
  vector<TofChargePar> cpar;
  /// Likelihood Value Of All Charge for Pattern
  map<int, vector<TofLikelihoodPar> > like;
  /// LikelihoodQ for Pattern
  map<int, float>likeQ;
  /// Mean Q
  map<int, float>Q;
  /// Q RMS
  map<int, float>RQ;
  ///--Tk Index
  int fTrTrack;

public:
  /// Init Function
  void Init();
  TofChargeHR(){Init();}
  TofChargeHR(BetaHR *betah);
  virtual ~TofChargeHR(){};
/// Update All Charge-Measurement Likelihood in Class Using Using Fix Pattern
   /*!
    * @param[in] pattern -1: Remove Big-dQ(From PDF)+BadPath-Length Layer; -10: Remove BadPath-Length Layer; -11: Remove Max-dQ(Q deviation) Layer; 1111: Using all 4Layers(if exist);1011: Using Lay0,2,3 exclude Layer; 1100: Using Up-TOF; 11 Using Down-TOF...
    * @param[in] opt%10=1 Update Integer Likelihood--- opt/10%10=1 Update LikelihoodQ 
    */
  void UpdateZ(int pattern=-10,int opt=1);

/** @name TOF Layer Charge Measurment
 * @{
 */
  /// Return True If TOF Charge iLayer(0-3) Measurment exists
  bool  TestExistHL (int ilay); 
  /// Return True if TOF-ilay Q PathLength is Good ///First Require Track-Match-TOF
  bool  IsGoodQPathL(int ilay);
  /// iLay TOF Q ///If Not Exist Return -1
  float GetQL(int ilay);
  /// Sum Measrument Number of Layer 
  int  GetNL();
  /// Access All TOF Charge Measurement Data-For iLayer ///Please First Check If ilay Exit TestExistHL
  TofChargePar  gTofChargePar(int ilay);
/**@}*/


/** @name TOF Integer Charge Estimation
 * @{
 */
  /// TOF Charge_Size /// Number of Charge in Likelihood-Measurment (Prob is neglectable will not keep)
  /*
   * @param[in]  pattern -1: Remove Big-dQ(From PDF)+BadPath-Length Layer;-2: Remove Max-Q+BadPath-Length Layer; -10: Remove BadPath-Length Layer; -11: Remove Max-dQ(Q deviation) Layer; -12: Remove Max-Q Layer;1111: Using all 4Layers(if exist);1011: Using Lay0,2,3 exclude Layer; 1100: Using Up-TOF; 11 Using Down-TOF...
   */     
  int  GetNZ(int pattern=-10);
  /// Find TOF Charge Z Index IZ /// Return -1 if Prob is neglectable 
  /*
   * @param[in]  Z: Charge-Z
   * @param[in]  pattern -1: Remove Big-dQ(From PDF)+BadPath-Length Layer; -2: Remove Max-Q+BadPath-Length Layer;-10: Remove BadPath-Length Layer; -11: Remove Max-dQ(Q deviation) Layer;-12: Remove Max-Q Layer;  1111: Using all 4Layers(if exist);1011: Using Lay0,2,3 exclude Layer; 1100: Using Up-TOF; 11 Using Down-TOF...
   */  
  int  GetZI(int Z,int pattern=-10);
  /// TOF Likelihood Integer Charge Z 
  /*!
   * @param[out] nlay Number of TOF Layers Used For Charge Z-Measument
   * @param[out] Prob Likelihood Prob
   * @param[in]  IZ  =0 Max-Prob Z, =1 Second-Max-Prob Z (0~GetNZ()-1)
   * @param[in]  pattern: pattern -1: Remove Big-dQ(From PDF)+BadPath-Length Layer;-2: Remove Max-Q+BadPath-Length Layer; -10: Remove BadPath-Length Layer; -11: Remove Max-dQ(Q deviation) Layer;-12: Remove Max-Q Layer; 1111: Using all 4Layers(if exist);1011: Using Lay0,2,3 exclude Layer; 1100: Using Up-TOF; 11 Using Down-TOF...
   * @return Charge Z (<0 Faild)
   */
  int  GetZ(int &nlay,float &Prob,int IZ=0,int pattern=-10);
  /// Access Data Likelihood for IZ ///IZ  =0 Max-Prob Z, =1 Second-Max-Prob Z (0~GetNZ()-1
  TofLikelihoodPar  gTofLikelihoodPar(int IZ=0, int pattern=-10);
  /// TOF Likelihood-Prob For Charge Z ///Return 0 if Prob is neglectable
  float GetProbZ(int Z,int pattern=-10);
  /// TOF Log(Likelihood) /// TOF Likelihood Inteface To Combine with Other Detector to Global-Charge determination
  float GetLkh  (int Z,int &nlay,int pattern=-10);
/**@}*/


/** @name TOF Float Charge Estimation
 * @{
 */
  /// TOF Q Mean
  /*!
   * @param[out] nlay Number of TOF Layers Used For Q-Measument
   * @param[out] qrms Q-RMS of TOF Layers Used
   * @param[in] pattern -1: Remove Big-dQ(From PDF)+BadPath-Length Layer; -2: Remove Max-Q+BadPath-Length Layer; -10: Remove BadPath-Length Layer; -11: Remove Max-dQ(Q deviation) Layer; -12: Remove Max-Q Layer;  1111: Using all 4Layers(if exist);1011: Using Lay0,2,3 exclude Layer; 1100: Using Up-TOF; 11 Using Down-TOF...
   * @return =0 No Good TOF Layer for measurement  >0 Q(or Q^2) value
   */
  float GetQ(int &nlay,float &qrms,int pattern=-2); 
  /// TOF Charge LikehoodQ //CPU Consuming
  /*!
   * @param[out] nlay Number of TOF Layers Used For Q-Measument
   * @param[in]  pattern -1: Remove Big-dQ(From PDF)+BadPath-Length Layer; -2: Remove Max-Q+BadPath-Length Layer; -10: Remove BadPath-Length Layer; -11: Remove Max-dQ(Q deviation) Layer; -12: Remove Max-Q Layer; 1111: Using all 4Layers(if exist);1011: Using Lay0,2,3 exclude Layer; 1100: Using Up-TOF; 11 Using Down-TOF...
   */
   float GetLikeQ(int &nlay,int pattern=-10);  
/**@}*/


  void _PrepareOutput(int opt=0){
    sout.clear();
    sout.append("TofChargeHR Info");
  };

  void Print(int opt=0){
   _PrepareOutput();
   cout<<sout<<endl;
  }

  char * Info(int number=-1){
   sprintf(_Info,"TofChareHR");
   return _Info;
  }

  std::ostream& putout(std::ostream &ostr = std::cout){
    _PrepareOutput(1);
    return ostr << sout  << std::endl;
  };

//----
  ClassDef(TofChargeHR,2);
#pragma omp threadprivate(fgIsA)
};

//////////////////////////////////////////////////////////////////////////
/// TofPDFH structure
/*!
 *  TOF Charge-Likelihood ReBuild Engine
 *  \author qyan@cern.ch
*/
//////////////////////////////////////////////////////////////////////////
class TofPDFH{

protected:
  static vector<TofChargePar> chargepar;
   #pragma omp threadprivate (chargepar)

public:
  TofPDFH(){};
  /// ReBuild For BetaH-Charge
  static int ReBuild(BetaHR *betah,TofChargeHR &tofch);
  /// Calculate Likelihood and Prob for different Charge
  static void LikelihoodCal(vector<TofChargePar> cpars,vector<TofLikelihoodPar> &like);
  /// Calculate Likelihood for index IZ
  static number GetLikelihood(int IZ,vector<TofChargePar> cpars);
  /// LikelihoodQ Estimator opt=0 QMean-QRMS opt=1 LikelihoodQ
  static number GetLikelihoodQ(vector<TofChargePar> cpars,number &MeanQ,number &RMSQ,int opt=0);
  /// LikelihoodQ Minuit Fun
  static void   GetLikelihoodF(Int_t & /*nPar*/, Double_t * /*grad*/ , Double_t &fval, Double_t *par, Int_t /*iflag */ );
  /// Select Measurment TOF Charge Measurement
  static int SelectM(int pattern,vector<TofChargePar> cpar,int fTrTrack,vector<TofChargePar> &cpars);
  /// ProbZ from PDF
  static number GetProbZ(int ilay,int ibar,number Z,number QLA,number QLD,number QLARaw,number QLDRaw,number betah);
  /// ProbZI from PDF
  static number GetProbZI(int ilay,int ibar,int ZI,number QL,number betah,int isanode);
  /// Choose Anode or Dynode From PDF
  static int ChooseDA(int ilay,int ibar,number Z,number QLA,number QLD, number QLARaw,number QLDRaw,number &QL);
  /// PDF Function For Proton+Helium
  static Double_t PDFPrHe(Double_t *x, Double_t *par);
  /// PDF Fuction For BZ
  static Double_t PDFBZ(Double_t *x, Double_t *par);
  /// PDF Function for HBZ(>=16)
  static Double_t PDFHBZ(Double_t *x, Double_t *par);

  ClassDef(TofPDFH,1)
};

/////////////////////////////////////////////////////////////////////////
#endif
