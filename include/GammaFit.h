//  $Id: GammaFit.h,v 1.1 2014/03/03 15:08:32 shaino Exp $
#ifndef __GammaFit__
#define __GammaFit__

//////////////////////////////////////////////////////////////////////////
///
///\class GammaFit
///\brief Lorentz gamma fitting on TRD signal (and TrTrack rigidity)
///
///\date  2014/03/02 SH  Introduction of the class
///
///$Date: 2014/03/03 15:08:32 $
///
///$Revision: 1.1 $
///
//////////////////////////////////////////////////////////////////////////

#include "TObjArray.h"

class AMSEventR;
class TrTrackR;
class TrdTrackR;
class TrdKCluster;

class GammaFit : public TObject {

protected:
  enum { Npart = 3, Nmax = 25 };

  Int_t    _nhit;            ///< Number of TRDK hits used for the fitting
  Double_t _len[Nmax];       ///< Array of path length
  Double_t _amp[Nmax];       ///< Array of amplitude
  Double_t _rgt;             ///< TrTrack Rigidity
  Double_t _drv;             ///< TrTrack Rinv error
  Double_t _pxe;             ///< Xe pressure

  Double_t _gamma[Npart*2];  ///< Fitted gamma for each particle hypothesis
  Double_t _likel[Npart*2];  ///< Likelihood   for each particle hypothesis

  static Double_t _zme;      ///< Z/M for electron
  static Double_t _zmp;      ///< Z/M for proton
  static Double_t _zmh;      ///< Z/M for Helium

public:
  enum EParticle { kElectron = 1, kProton  = 2, kHelium = 3 };
  enum EMethods  { kTRDOnly  = 0, kTrTrack = 1 };

  /////////////////////////////////////////////////////////////////////////
  //// Constructors
  /////////////////////////////////////////////////////////////////////////

  /// Default constructor
  GammaFit() { Init(); }

  /// Constructor with TrTrack and fitID (0: default fit)
  GammaFit(TrTrackR *track, int fitid = 0);

  /// Constructor with TrdTrack (independent of any TrTrack)
  GammaFit(TrdTrackR *track);

  /// Destructor
 ~GammaFit() {}

  /// Initialize parameters
  void Init();


  /////////////////////////////////////////////////////////////////////////
  //// Accessors
  /////////////////////////////////////////////////////////////////////////

  /// Get number of hits used for the gamma fitting
  Int_t GetNhit(void) const { return _nhit; }

  /// Get path length
  Double_t GetLen(Int_t i) const { return (0 <= i && i < _nhit)? _len[i] : 0; }

  /// Get amplitude
  Double_t GetAmp(Int_t i) const { return (0 <= i && i < _nhit)? _amp[i] : 0; }

  /// Get TrTrack rigidity
  Double_t GetRigidity() const { return _rgt; }

  /// Get TrTrack Rinv error
  Double_t GetDrinv() const { return _drv; }

  /// Get Xe pressure
  Double_t GetXePressure() const { return _pxe; }

  /// Get fitted gamma factor for a given particle hypothesis and method
  /*
    \param[in]  particle hypothesis (EParticle) 1:e 2:p 3:He
    \param[in]  method   fitting method 0:TRD-only 1:TRD-TrTrack combined
    \return     Gamma factor
   */
  Double_t GetGamma(int particle = kProton,
		    int method   = kTRDOnly) const {
    return (1 <= particle && particle < Npart &&
	    0 <= method   && method   < 2) ? _gamma[method*Npart+particle] : 0;
  }

  /// Get likelihood for a given particle hypothesis and method
  /*
    \param[in]  particle hypothesis (EParticle) 1:e 2:p 3:He
    \param[in]  method   fitting method 0:TRD-only 1:TRD-TrTrack combined
    \return     Gamma factor
   */
  Double_t GetLikelihood(int particle = kProton,
			 int method   = kTRDOnly) const {
    return (1 <= particle && particle < Npart &&
	    0 <= method   && method   < 2) ? _likel[method*Npart+particle] : 0;
  }


  /////////////////////////////////////////////////////////////////////////
  //// Main methods
  /////////////////////////////////////////////////////////////////////////

  /// Fit gamma factor for a given particle hypothesis and method
  /*
    \param[out]  Likelihood value
    \param[in]   particle hypothesis (EParticle) 1:e 2:p 3:He
    \param[in]   method   fitting method  0:TRD-only 1:TRD-TrTrack combined
    \return      Gamma factor
   */
  Double_t Fit(Double_t &likelihood, int particle = kProton,
	                             int method   = kTRDOnly);

  /// Fit all the methods, returns <0 in case of error
  Int_t FitAll(void);

  /// Constructor for advanced users
  GammaFit(Int_t nhit, Double_t *len, Double_t *amp,
	   Int_t time, Double_t  rgt, Double_t  drv);

protected:
  Int_t SetXePressure();
  Int_t FillTrdHits(TrdKCluster &trdk);

public:
  ClassDef(GammaFit, 1)
};


//////////////////////////////////////////////////////////////////////////
///
///\class GammaPDF
///\brief Manage TRD signal PDF
///
///\date  2014/03/02 SH  Introduction of the class
///
///$Date: 2014/03/03 15:08:32 $
///
///$Revision: 1.1 $
///
//////////////////////////////////////////////////////////////////////////

class TF1;
class TH1;

class GammaPDF : public TObject {

public:
  TObjArray fAr;

  Int_t fPart;
  TF1  *fFPDF;
  TF1  *fFPDFL;

  TF1  *fFtrf;
  TF1  *fFmn1;
  TF1  *fFsig1;
  TF1  *fFtail;
  TF1  *fFfrac;
  TF1  *fFmn2;
  TF1  *fFsig2;
  TF1  *fFsig3;

  Double_t fLH[2];

  static Int_t fDebug;

protected:
  GammaPDF(int part);

public:
 ~GammaPDF() {}

  static GammaPDF *fPtr[3];
  static GammaPDF *GetPtr(int part);

  Int_t Load(const char *fname);

  TF1 *GetFdE(void);
  TF1 *GetFTR(void);

  Int_t SetPar(Double_t prxe, Double_t plen, Double_t bgm,
	       Int_t fix = 11111111);

  TF1 *operator() () { return fFPDF; }
  TF1 *operator() (Double_t bgm, Int_t fix = 0);

  Double_t Eval(TString fid, Double_t prxe, Double_t plen);
  Double_t Norm(void);

  static Int_t    GenNh(void);
  static Double_t GenPx(void);

  Int_t Gen(Double_t  prxe, Double_t  bgm, Int_t nhit,
	    Double_t *plen, Double_t *amp);

  Double_t GammaL  (Double_t prxe, Double_t *plen, Double_t *amp, Int_t nhit, 
		    Double_t  bgm);
  Double_t GammaFit(Double_t prxe, Double_t *plen, Double_t *amp, Int_t nhit,
		    Double_t  rgt, Double_t  drv,  Double_t  zpm,
		    Double_t wtk = 1);

  void EEscale(Int_t fid, Double_t scl);

  void Init(int particle);
  void Dump(void) const;

  static Double_t Peak (Double_t *x,  Double_t *y);
  static Double_t LPDF (Double_t *xx, Double_t *pp);
  static Double_t GEGGS(Double_t *xx, Double_t *pp);
  static Double_t GEGG (Double_t *xx, Double_t *pp);
  static Double_t EEEE (Double_t *xx, Double_t *pp);
  static Double_t EE   (Double_t *xx, Double_t *pp);
  static Double_t BB   (Double_t *xx, Double_t *pp);
  static Double_t G    (Double_t *xx, Double_t *pp);
  static Double_t GE   (Double_t *xx, Double_t *pp);
  static Double_t GG   (Double_t *xx, Double_t *pp);
  static Double_t GES  (Double_t *xx, Double_t *pp);
  static Double_t GGS  (Double_t *xx, Double_t *pp);
  static Double_t LEE  (Double_t *xx, Double_t *pp);
  static Double_t LGE  (Double_t *xx, Double_t *pp);
  static Double_t LGG  (Double_t *xx, Double_t *pp);
  static Double_t P11  (Double_t *xx, Double_t *pp);
  static Double_t P22  (Double_t *xx, Double_t *pp);
  static Double_t P31  (Double_t *xx, Double_t *pp);

  ClassDef(GammaPDF, 1)
};

#endif

