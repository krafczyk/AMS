//  $Id$
#ifndef __SpFold__
#define __SpFold__

#include "TObject.h"

class TF1;
class TH1;
class TGraph;

/** Cubic spline interpolation and fitting.
 * Example   {#example1}
 * ========
 *~~~~~~~~~~~~~{.C}
 * TGraph *gr = new TGraph(np, xp, yp);  // Create TGraph
 *
 * Double_t x[5] = { 1, 2, 3, 4, 5 };    // Node x positions
 * SplFit::Fit(gr, 5, x);                // Fit with free boundary
 *
 * SplFit::Fit(gr, 5, x, 0, 0, 6);       // Fit boundary free from X=0 to X=6
 *
 * Double_t b[2] = { 0, 0 };             // Upper/Lower boundary dY/dX
 * SplFit::Fit(gr, 5, x, b, "bfu bfl");  // Fit with fixed boundary dY/dX
 *
 * SplFit::fXfree = (1<<1)|(1<<2);       // Set x[1],x[2] as free parameters
 * SplFit::Fit(gr, 5, x, b, "bfu bfl");
 *
 * SplFit::fBlxU = 1;                    // x>x[4] is linear extrapolated
 * SplFit::Fit(gr, 5, x, b, "bfu bfl");
 *~~~~~~~~~~~~~
 */
class SplFit {
public:
  enum { Nmax = 1000         ///< Max number of nodes
       };

  /// Spline Modes
  enum EModes { NORMAL = 1,  ///< Normal mode
	        DERIV1 = 2,  ///< 1st. derivative at x
		DERIV2 = 3,  ///< 2nd. derivative at x
		INTEG  = 4   ///< Integral from xn[0] to x
              };

  static Int_t  fN;          ///< Number of nodes; should be fN < Nmax
  static Int_t  fMode;       ///< Spline model see @ref EModes
  static Int_t  fXfree;      ///< See example @ref example1
  static Bool_t fLogX;       ///< LogX spline or not
  static Bool_t fLogY;       ///< LogY spline or not
  static Bool_t fBlxL;       ///< Linear extrapolation below lower boundary
  static Bool_t fBlxU;       ///< Linear extrapolation above upper boundary

  /** Fit with spline function
   * \param[in]  gr    TGraph to be fitted
   * \param[in]  n     Number of nodes
   * \param[in]  x     Node position in X
   * \param[in]  b     Upper/lower boundary dY/dX
   * \param[in]  opt   Fit option "bfu"/"bfl" fix upper/lower boundary
   * \param[in]  xmin  Fitting lower limit (xmin=xmax=0 : No limit)
   * \param[in]  xmax  Fitting upper limit (xmin=xmax=0 : No limit)
   * \return           TF1 of fitted function */
  static TF1 *Fit(TGraph *gr, Int_t n, Double_t *x, Double_t *b = 0,
		  Option_t *opt = "", Double_t xmin = 0, Double_t xmax = 0);

  /** Fit with spline function
   * \param[in]  hist  TH1 to be fitted
   * \param[in]  n     Number of nodes
   * \param[in]  x     Node position in X
   * \param[in]  b     Upper/lower boundary dY/dX
   * \param[in]  opt   Fit option "bfu"/"bfl" fix upper/lower boundary
   * \param[in]  xmin  Fitting lower limit (xmin=xmax=0 : No limit)
   * \param[in]  xmax  Fitting upper limit (xmin=xmax=0 : No limit)
   * \return           TF1 of fitted function */
  static TF1 *Fit(TH1 *hist, Int_t n, Double_t *x, Double_t *b = 0,
		  Option_t *opt = "", Double_t xmin = 0, Double_t xmax = 0);

  /** Create a TH1 with confidence interfals
   * \param[in]  func   TF1 of the fitted function
   * \param[in]  hname  Name of output TH1
   * \param[in]  cl     Confidence Level
   * \param[in]  norm   Normalize with chi^2/NDF or not
   * \param[in]  n      Number of bins
   * \param[in]  xmin   Lower limit
   * \param[in]  xmax   Upper limit
   */
  static TH1 *GetCi(TF1 *func, const char *hname = "hcfi",
		    Double_t cl = 0.68, Bool_t norm = kFALSE,
		    Int_t n = 100, Double_t xmin = 0.6, Double_t xmax = 6000);

  /// Create a TGraph from TF1
  static TGraph *Plot(TF1 *func, Int_t n = 100, Double_t xmin = 0.6,
		                                Double_t xmax = 6000);

  /** Create and initialize TF1 with spline
   */
  static TF1 *GetF(TGraph *gr, Int_t n, Double_t *x, Double_t *b = 0,
		   Option_t *opt = "", Double_t xmin = 0, Double_t xmax = 0);

  /** Spline function
   * \param[in]  xp[0]          Interpolated position in X    
   * \param[in]  par[0:fN-1]    Node position in X
   * \param[in]  par[fN:fN*2-1] Node position in Y
   * \param[in]  par[fN*2]      Derivative of lower boundary
   * \param[in]  par[fN*2+1]    Derivative of upper boundary
   * \return                    Interpolated position in Y */
  static Double_t SpFunc(Double_t *xp, Double_t *par);

  /** Spline interpolation
   * \param[in]  n     Number ofnodes
   * \param[in]  xn    Node position in X
   * \param[in]  yn    Node position in Y
   * \param[in]  d     2nd derivatives 
   * \param[in]  x     Interpolated position in X
   * \param[in]  mode  Spline mode; see EModes
   * \return           Interpolated position in Y */
  static Double_t Splint(Int_t n, Double_t *xn, Double_t *yn, Double_t *d,
			 Double_t x, Int_t mode = fMode);

  /** Get spline 2nd derivatives
   * \param[in]  n   Number ofnodes
   * \param[in]  xn  Node position in X
   * \param[in]  yn  Node position in Y
   * \param[in]  bn  Upper/lower boundary dY/dX
   * \param[out] d   2nd derivatives */
  static void Spline(Int_t n, Double_t *xn,
		              Double_t *yn, Double_t *bn, Double_t *d);

  // Dummy destructor to avoid compile warnings
  virtual ~SplFit() {}
  ClassDef(SplFit, 1)
};


/** Rigidity resolution PDF.
 * Resolution is parameterized with single "stretched" Gaussian 
 *  with sigma as a function of x (= 1/Rrec-1/Rgen).
 * The integral is NOT normalized to 1 in order to 
 * take account of interaction loss (i.e. migration to lower energy).
 * It is designed as thread-safe. You can use it in parallel processing.
 * (e.g. with SpFold::fMT=1)
 *
 * Example   {#example2}
 * ========
 *~~~~~~~~~~~~~{.C}
 * // PDF with x=Rgen/Rrec
 * TF1 *frp1 = new TF1("frp1", SpReso::PDFvR, -10, 12, 1);
 * frp1->SetParameter(0, 1000);  // Rgen= 1000
 * frp1->Draw();                 // Draw PDF (Rgen/Rrec)
 *
 * // PDF with x=1/Rgen-1/Rrec
 * TF1 *frp2 = new TF1("frp2", SpReso::PDFdR, -0.01, 0.01, 1);
 * frp2->SetParameter(0, 1000);  // Rgen= 1000
 * frp2->Draw();                 // Draw PDF (1/Rgen-1/Rrec)
 *~~~~~~~~~~~~~
 */
class SpReso {
public:
  /** PDF of xp[0]= Rgen/Rrec with par[0]= Rgen
   * \param[in]  xp[0]  Rgen/Rrec
   * \param[in] par[0]  Rgen
   * \return    PDF */
  static Double_t PDFvR(Double_t *xp, Double_t *par);

  /** PDF of xp[0]= 1/Rgen-1/Rrec with par[0]= Rgen
   * \param[in]  xp[0]  1/Rgen-1/Rrec
   * \param[in] par[0]  Rgen
   * \return    PDF */
  static Double_t PDFdR(Double_t *xp, Double_t *par);

  /** PDF of xp[0]= 1/Rrec-1/Rgen with par[]= Norm,Mean,Sig_i
   * \param[in] xp [0]     1/Rgen-1/Rrec
   * \param[in] par[0]     Norm
   * \param[in] par[1]     Mean
   * \param[in] par[2-9]   Sigma^2 spline position in Y
   * \param[in] par[10,11] Sigma^2 spline dY/dX at lower/upper boundary
   * \param[in] par[12]    x scaling
   * \return    PDF or sigma^2 (if par[0]=0)
   */
  static Double_t PDFpar(Double_t *xp, Double_t *par);

  enum { Nx = 8,           ///< Number of spline nodes in d1/R
	 Nr = 7,           ///< Number of spline nodes in Rgen
	 Np = Nx+4,        ///< Number of parameters   in d1/R (p0,p1,y0,y1...)
	 Nt = Np*(Nr+2)    ///< Number of spline parameters in total
       };
  static Double_t fXn[Nx]; ///< Spline nodes in d1/R
  static Double_t fRn[Nr]; ///< Spline nodes in Rgen
  static Double_t fSp[Nt]; ///< Spline parameters

  /// Parameters of resolution scaling : sqrt(p0^2+p1^2/R/R)
  static Double_t fPref[2];

  /// Overall normalization
  static Double_t fNorm;

  /// Parameters of PDFpar with xp[0]= Rgen and par[0]= index [0:Np-1]
  static Double_t PARrg(Double_t *xp, Double_t *par);

  /// Saturation fuction
  static Double_t SATUR(Double_t *xp, Double_t *par) {
    return Satur(xp[0], par[0], par[1]);
  }

  /// PDF normalization factor with type= 1:PDFvR 2:PDFdR
  static Double_t GetNorm(Double_t rgen, Int_t type);

  /// PDF of x= 1/Rrec-1/Rgen for a given Rgen
  static Double_t GetPDF(Double_t rgen, Double_t x);

  /// Stretched Gaussian sigma at x= 1/Rrec-1/Rgen for a given Rgen
  static Double_t GetSig(Double_t rgen, Double_t x);

  /// Saturation fuction
  static Double_t Satur(Double_t x, Double_t s0, Double_t s1);

  /// Get stretched Gaussian parameters (0:Norm,1:Mean,2:Sig0,3:Sig1,4:Sig2)
  static Double_t GetPar(Double_t rgen, Int_t i);

  /** Set spline resolution parameters with different condition
   * \param[in] part   Particle 1:p 2:He
   * \param[in] cfw    MC cutoff 0:Not applied  1:Applied
   */
  static void SetPar(Int_t part, Int_t cfw);

  /** Fit a histogram with PDFpar
   * \param[in] hist   Histogram to fit: (1/Rrec-1/Rgen)/sigma0
   * \param[in] option Fitting option, recommended to add "L", no fit if =0
   * \param[in] range  Fitting range (xmin= -range, xmax= range)
   * \return    TF1 of fitted function, 0 in case of error
   */
  static TF1 *Fit(TH1 *hist, Option_t *option = "Lq0", Double_t range = 20);

  /// Reentrant test
  static void REtest(Double_t x = 0);

  // Dummy destructor to avoid compile warnings
  virtual ~SpReso() {}
  ClassDef(SpReso, 1)
};


/** Spectrum folding.
 * Example   {#example3}
 * ========
 *~~~~~~~~~~~~~{.C}
 * // Get acceptance (cm^2 s); it works with TGraph itself only but
 * // it is recommended to provide fPacc/fNacc after spline fitting on TGraph
 * TGraph *gac = (TGraph *)f.Get("gAcc"); 
 * 
 * // Get rate histogram (dN/dT VS R) 
 * // it is recommended to assign systematic errors to have stable results
 * TH1 *hrt = (TH1 *)f.Get("hRate");
 *
 * SpFold::fRtype = SpFold::RG_RR;         // par[0]= Rgen, x= Rgen/Rrec
 * SpFold::fXtype = SpFold::RREC;          // x= Rrec
 *
 * // Folding fit to rate with acceptance and resolution function (PDFvR)
 * TF1 *ff = SpFold::Fit(hrt, gac, SpReso::PDFvR, "I", 1, 6000);
 *~~~~~~~~~~~~~
 *
 * Tips for speeding up.
 * - Provide fPacc and fNacc instead of fGacc for the acceptance, where
 *  + fPacc[0      :fNacc  -1] Node X position
 *  + fPacc[fNacc  :fNacc*2-1] Node Y position
 *  + fPacc[fNacc*2:fNacc*3-1] D2 from SplFit::Spline
 *  + fPacc[fNacc*3]           Lower boundary
 *  + fPacc[fNacc*3+1]         Upper boundary
 *
 * - Activate multi-threading with fMT= 1 with 
 *  thread-safe resolution function.
 */
class SpFold {
public:
  /// Flux models
  enum EModels { PWR   = 1,  ///< Single power law
		 PWRBR = 2,  ///< Power law with a break
		 PWGRE = 3,  ///< Power law with smooth break (Grenoble model)
		 PWSUM = 4,  ///< Sum of two power laws
		 SPGRE = 5,  ///< Grenoble model with spline Solar mod.
		 SPLOG = 11, ///< Spline in LogXY
		 SPINT = 12  ///< Integral of spline in LogXY
              };

  /// Resolution PDF types
  enum ERtypes { RG_RR = 1,  ///< par[0]= Rgen, x= Rgen/Rrec
		 DRINV = 2   ///< par[0]= Rgen, x= 1/Rrec-1/Rgen
               };

  /// Rrec types
  enum ERrecs  { RREC  = 1,  ///< x= Rrec
		 RINV  = 2   ///< x= 1/Rrec
               };

  static Int_t     fModel;   ///< Flux model; see @ref EModels
  static Int_t     fNnode;   ///< Number of nodes for the spline function
  static Double_t *fXnode;   ///< Node positions in X for the spline function

   /// Resolution PDF; see @ref ERtypes
  static Double_t (*fRPDF)(Double_t*, Double_t*);

  static Int_t     fRtype;   ///< Resolution PDF type; see @ref ERtypes
  static Int_t     fXtype;   ///< Rrec type; see @ref ERrecs

  static TGraph   *fGacc;    ///< Acceptance (cm^2 sr) in TGraph
  static Double_t *fPacc;    ///< Acceptance (cm^2 sr) in spline
  static Int_t     fNacc;    ///< Number of nodes for fPacc

  static Int_t     fNdiv;    ///< Number of divisions of flux integral
  static Double_t  fRmin;    ///< Min. true rigidity  of flux integral
  static Double_t  fRmax;    ///< Max. true rigidity  of flux integral

  static Int_t     fVcont;   ///< Verbose counter (set <0 to be quiet)
  static Int_t     fCrChk;   ///< Control parallel processing
  static Bool_t    fMT;      ///< Enable  parallel processing

  /// Solar modulation [0]:potential(GV) [1]:Mass(GeV) [2]:Charge
  static Double_t fSolMod[3];

  /** Fit on event rate.
   * \param[in] hrt    Event rate (1/sec/GV) either in R or 1/R
   * \param[in] gac    Acceptance in TGraph (cm^2 sr) or provide fPacc
   * \param[in] rpdf   Resolution function; @ref fRtype should be specified
   * \param[in] opt    Fitting option. Use "I" to have enough precision
   * \param[in] xmin   Lower fiting range (xmin=xmax=0 : full range)
   * \param[in] xmax   Upper fiting range (xmin=xmax=0 : full range)
   * \param[in] model  Flux model; see @ref EModels
   * \param[in] nnode  Number of spline nodes (Default: 5)
   * \param[in] xnode  Spline node position   (Default: 1, 20, 50, 300, 1000)
   */
  static TF1 *Fit(TH1 *hrt, TGraph *gac, 
		  Double_t (*rpdf)(Double_t*, Double_t*),
		  Option_t *opt = "I", Double_t xmin = 0, Double_t xmax = 0,
		  Int_t model = SPLOG, Int_t nnode = 0, Double_t *xnode = 0);

  /** Get bin center according to Lafferty and Wyatt, NIM 355 (1995) 541.
   * The value is obtained by solving an equation 
   *  F(xc)(xu-xl) = int_xl^xu F(x)dx
   * \param[in] fflx  TF1 of flux VS R(GV)
   * \param[in] bl    Lower bin boundary in GV
   * \param[in] bu    Upper bin boundary in GV
   * \return          Bin center in GV
   */
  static Double_t GetX(TF1 *fflx, Double_t bl, Double_t bu);

  /// Folded spectrum PDF; xp[0]= rrec (fXtype=RREC) or 1/rrec (fXtype=RINV)
  static Double_t PDF(Double_t *xp, Double_t *par);

  /// Folded/True ratio; xp[0]= rrec (fXtype=RREC) or 1/rrec (fXtype=RINV)
  static Double_t Ratio(Double_t *xp, Double_t *par);

  /// Product of flux and resolution with xp[0]= rgen
  static Double_t Prod(Double_t rgen, Double_t rrec, Double_t *par);

  /// Get acceptance at rgen
  static Double_t GetA(Double_t rgen);

  /// Create and initialize TF1 with model and gr
  static TF1 *GetF(Int_t model, TGraph *gr, Int_t nn = 0, Double_t *xn = 0);

  /// Create a TGraph from TF1 with x^pwr scaling
  static TGraph *Plot(TF1 *func, Double_t pwr = 0, Int_t n = 100,
		      Double_t xmin = 0.6, Double_t xmax = 6000);

  /// Create a TH1 with confidence interfals with x^pwr scaling
  static TH1 *GetCi(TF1 *func, const char *hname = "hcfi",
		    Double_t cl = 0.68, Double_t pwr = 0, Bool_t norm = 1, 
		    Int_t n = 100, Double_t xmin = 0.6, Double_t xmax = 6000);

  /// Convert TH1 into TGraph with (norm=1) normalization with BinWith or not
  static TGraph *HtoG(TH1 *hist, Bool_t norm = 0, Bool_t invx = 0);

  /// Normalize (or reverse) TH1 with bin width 
  static void Norm(TH1 *hist, Bool_t reverse = 0); 

  /// Scale TH1 with X^pwr from @ref GetX based on fflx
  static void Scale(TH1 *hist, Double_t pwr, TF1 *fflx);

  /// Flux specified with fModel
  static Double_t Flux(Double_t *xp, Double_t *par);

  /** Single power law flux
   * \param[in] xp[0]  Rigidity (GV)
   * \param[in] par[]  0: Norm 1: Gamma */
  static Double_t FluxPwr(Double_t *xp, Double_t *par);

  /** Power law flux with break
   * \param[in] xp[0]  Rigidity (GV)
   * \param[in] par[]  0: Norm 1: Gamma 2: Break 3: delta Gamma */
  static Double_t FluxPbr(Double_t *xp, Double_t *par);

  /** Grenoble flux model
   * \param[in] xp[0]  Rigidity (GV)
   * \param[in] par[]  0: Norm 1: Gamma 2: Break 3: delta Gamma 4: smooth */
  static Double_t FluxGre(Double_t *xp, Double_t *par);

  /** Sum of two power law flux
   * \param[in] xp[0]  Rigidity (GV)
   * \param[in] par[]  0: Norm 1: Gamma 2: Norm2 3: Gamma2 */
  static Double_t FluxSum(Double_t *xp, Double_t *par);

  /** Spline in LogXY
   * \param[in] xp[0]  Rigidity (GV)
   * \param[in] par[]  Node positions in Y and upper/lower dY/dX */
  static Double_t FluxSpl(Double_t *xp, Double_t *par);

  /** Integral of Spline in LogXY
   * \param[in] xp[0]  Rigidity (GV)
   * \param[in] par[]  Node positions in Y and upper/lower dY/dX */
  static Double_t FluxSpi(Double_t *xp, Double_t *par);

  /** Grenoble model with spline Solar modulation
   * \param[in] xp[0]  Rigidity (GV)
   * \param[in] par[]  Node positions in Y and upper/lower dY/dX */
  static Double_t FluxSgr(Double_t *xp, Double_t *par);

  /** Force field solar modulation
   * \param[in]  xp[0]  Rigidity (GV)
   * \param[in]  par[]  0: Phi 1: Mass 2: Charge
   * \param[out] xp[0]  Modulated rigidity */
  static Double_t SolMod(Double_t *xp, Double_t *par);

  // Dummy destructor to avoid compile warnings
  virtual ~SpFold() {}
  ClassDef(SpFold, 1)
};

#endif
