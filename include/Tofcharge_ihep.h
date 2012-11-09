//  $Id: Tofcharge_ihep.h,v 1.2 2012/11/09 00:36:06 qyan Exp $

//Author Qi Yan 2012/Oct/01 15:56 qyan@cern.ch  /*IHEP TOF Charge Likelihood version(BetaH)*/
#ifndef __TOFCHARGE_IHEP__
#define __TOFCHARGE_IHEP__
//#include "root.h"
#include "TObject.h"
#include "TF1.h"
#include "TrElem.h"
#include "Tofdbc.h"
//////////////////////////////////////////////////////////////////////////
class BetaHR;
//////////////////////////////////////////////////////////////////////////

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
 ///-Default Q //Best Between Anode +Dynode 
     float GetQ();
 /// All Z and ProbZ in PDF
     vector <pair<int, float> > ProbZ;
 /// Get Prob to Be Z 
    float GetProbZ(float Z); 
 /// Push_back ProbZ-vector prob value from Z=ZL to Z=ZH 
    void  FillProbZ(int ZL,int ZH);

   public:
     TofChargePar(){}
     TofChargePar(int _Layer,int _Bar,float _QLA,float _QLD,float _QLARaw,float _QLDRaw,float _Beta,bool _IsGoodPath):
                   Layer(_Layer), Bar(_Bar), QLA(_QLA), QLD(_QLD), QLARaw(_QLARaw), QLDRaw(_QLDRaw),  Beta(_Beta), IsGoodPath(_IsGoodPath){
                      ProbZ.clear();
                   }

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
   TofLikelihoodPar(){}
   TofLikelihoodPar(int  _LPattern,int _Z, float _Likelihood):
                  LPattern(_LPattern),Z(_Z),  Likelihood(_Likelihood){Prob=0;}

  bool operator <(const TofLikelihoodPar &right) const {//put likehood lagest to the first
    return (-Likelihood<-right.Likelihood);
  }
  
  ClassDef(TofLikelihoodPar,1);
};

//////////////////////////////////////////////////////////////////////////
/// TofClusterHR structure
/*!
 *  TOF Charge Likelihood Manager
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
    int fTrTrack;

public:
/// Init Function
    void Init();
    TofChargeHR(){Init();}
    TofChargeHR(BetaHR *betah);
/// Update All Charge-Measurement Likelihood in Class Using Using Fix Pattern
   /*!
    * @param[in] pattern -1: Remove Big-dQ(From PDF)+BadPath-Length Layer; -10: Remove BadPath-Length Layer; -11: Remove Max-dQ(Q deviation) Layer; 1111: Using all 4Layers(if exist);1011: Using Lay0,2,3 exclude Layer; 1100: Using Up-TOF; 11 Using Down-TOF...
    */
    void UpdateZ(int pattern=-10);
/// TOF Charge_Size /// Number of Max-Prob Charge in Likelihood
    int GetNZ(int pattern=-10);
/// TOF Charge Z 
   /*!
    * @param[out] nlay Number of TOF Layers Used For Charge Z-Measument
    * @param[out] Z Prob Var
    * @param[in]  IZ  =0 Max-Prob Z, =1 Second-Max-Prob Z...
    * @param[in]  pattern -1: Remove Big-dQ(From PDF)+BadPath-Length Layer; -10: Remove BadPath-Length Layer; -11: Remove Max-dQ(Q deviation) Layer; 1111: Using all 4Layers(if exist);1011: Using Lay0,2,3 exclude Layer; 1100: Using Up-TOF; 11 Using Down-TOF...
    * @return Charge Z (<0 Faild)
    */
    int GetZ(int &nlay,float &Prob,int IZ=0,int pattern=-10);
 /// TOF Charge LikehoodQ
   /*!
    * @param[out] nlay Number of TOF Layers Used For Q-Measument
    * @param[in]  pattern -1: Remove Big-dQ(From PDF)+BadPath-Length Layer; -10: Remove BadPath-Length Layer; -11: Remove Max-dQ(Q deviation) Layer; 1111: Using all 4Layers(if exist);1011: Using Lay0,2,3 exclude Layer; 1100: Using Up-TOF; 11 Using Down-TOF...
    */
    float GetLikeQ(int &nlay,int pattern=-10);  

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


  ClassDef(TofChargeHR,1);
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
///---ReBuild For BetaH-Charge
  static int ReBuild(BetaHR *betah,TofChargeHR &tofch);
///---Calculate Likelihood and Prob for different Charge
  static void LikelihoodCal(vector<TofChargePar> cpars,vector<TofLikelihoodPar> &like);
///---Calculate Likelihood for index IZ
  static number GetLikelihood(int IZ,vector<TofChargePar> cpars);
///---LikelihoodQ Estimator
  static number GetLikelihoodQ(vector<TofChargePar> cpars);
///---LikelihoodQ Minuit Fun
  static void   GetLikelihoodF(Int_t & /*nPar*/, Double_t * /*grad*/ , Double_t &fval, Double_t *par, Int_t /*iflag */ );
//---Select Measurment TOF Charge Measurement
  static int SelectM(int pattern,vector<TofChargePar> cpar,int fTrTrack,vector<TofChargePar> &cpars);
///---ProbZ from PDF
  static number GetProbZ(int ilay,int ibar,number Z,number QLA,number QLD,number QLARaw,number QLDRaw,number betah);
///---ProbZI from PDF
  static number GetProbZI(int ilay,int ibar,int ZI,number QL,number betah,int isanode);
///---Choose Anode or Dynode From PDF
  static int ChooseDA(int ilay,int ibar,number Z,number QLA,number QLD, number QLARaw,number QLDRaw,number &QL);
///---PDF Function For Proton+Helium
  static Double_t PDFPrHe(Double_t *x, Double_t *par);
///---PDF Fuction For BZ
  static Double_t PDFBZ(Double_t *x, Double_t *par);
///---PDF Function for HBZ(>=16)
  static Double_t PDFHBZ(Double_t *x, Double_t *par);

  ClassDef(TofPDFH,1)
};


/////////////////////////////////////////////////////////////////////////
/// TofPDFPar structure
/*!
 *  TOF-Charge-PDF TDV Manager
 *  \author qyan@cern.ch
*/
//////////////////////////////////////////////////////////////////////////
class  TofPDFPar:public TofTDVTool<float>{
  public:
      static const int   ZType=3;
      static const int   ZPDFgate[ZType];
      static const int   nPDFCh=19;
      static const int   PDFCh[nPDFCh];
      static const int   nPDFVel=6; 
      static const int   nPDFPar=7;
      static const float ProbLimit=1e-4;//Not Possible Case
      static const int   ZHLim=200;//Limit of High Z
      static const float DAgate[TOFCSN::SCLRS][TOFCSN::SCMXBR];//Dynode+Anode Switch Gate
//---
      static const float pdfvel[nPDFCh][nPDFVel];
//----PDF Par
      double pdfpara[nPDFCh][nPDFVel][TOFCSN::SCLRS][TOFCSN::SCMXBR][nPDFPar];
      double pdfpard[nPDFCh][nPDFVel][TOFCSN::SCLRS][TOFCSN::SCMXBR][nPDFPar];
  public:
      TofPDFPar();
      TofPDFPar(float *arr,int brun,int erun);//load 
      static TofPDFPar *Head;
   #pragma omp threadprivate (Head)   
      static TofPDFPar *GetHead();
      static void HeadLoadTDVPar(){GetHead()->LoadTDVPar();}
      void LoadOptPar(int opt=0);//copy TDV to class 0 From TDV 1 Read From Default
      void LoadTDVPar();//copy TDV to class
      int  LoadFromFile(const char *fpdf,int ida,int ichl,int ichh,int nv,int np);//read data from file->Block data
      void PrintTDV();
};

//////////////////////////////////////////////////////////////////////////
#endif
