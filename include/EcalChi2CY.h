//  $Id: EcalChi2CY.h,v 1.14 2013/02/18 17:39:09 kaiwu Exp $
#ifndef __ECALCHI2CY_H__
#define __ECALCHI2CY_H__
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <vector>
#include "root.h"
#include "math.h"
#include "TH1F.h"
#include "TFile.h"
#include "TMath.h"
#include "TObject.h"
#include "TDirectory.h"
#include "TRandom3.h"
#include "TVirtualFitter.h"
using namespace std;

class EcalPDF{
public:
    ///Default Constructor
    static int  Version  ;
    string warn_messages ;
    static int  has_init ;
    vector<string> pdf_names;
    EcalPDF()            ;

    ///Constructor
    /*!
    \param[in] database  File name of the ecal pdf. e.g string("standard_prof_sim_inter.root")
    */
    EcalPDF(const char* fdatabase)    ;
    ~EcalPDF()  ;
    ///Get the normalized mean value of a hit
    /*!
    \param[in] flayer   Layer number. e.g 0~17
    \param[in] coo      Coordinate w.r.t the shower axis
    \param[in] Erg      Ecal Shower Deposition (GeV)
    \param[in] type     Currently only provide type 0
    \param[out] mean    if successful, mean>=0; if database has not been loaded, mean=-1
    */
    float get_mean(int flayer, float coo, float Erg, int type=0)    ;
    ///Get the normalized rms value of a hit
    /*!
    \param[in] flayer   Layer number. e.g 0~17
    \param[in] coo      Coordinate w.r.t the shower axis
    \param[in] Erg      Ecal Shower Deposition (GeV)
    \param[in] type     Currently only provide type 0
    \param[out] rms    if successful, rms>=0; if database has not been loaded, rms=-1
    */
    float get_rms (int flayer, float coo, float Erg, int type=0)    ;
    ///Get hit occupance
    /*!
    \param[in] flayer   Layer number. e.g 0~17
    \param[in] coo      Coordinate w.r.t the shower axis
    \param[in] Erg      Ecal Shower Deposition (GeV)
    \param[in] type     type= 0: return probability of given coo has hit, type= 1: return -log(1-p)
    \param[out] probability     if successful, probability>=0; if database has not been loaded, probability=-1
    */
    float get_prob(int flayer, float coo, float Erg, int type=0)    ;

    ///Normalize Chi2 and remove Energy Dependence. If EnergyE< 15.GeV, will use paramters of 15 GeV. If EnergyE>200 GeV, will use parameters of 200 GeV.
    /*!
    \param[in]  Chi2  	Ecal Chi2
    \param[in]  EnergyE     Reconstructed EcalShower Energy(GeV)
    \param[in]  algorithm   Algorithm to get Ecal Chi2
    return nChi2	Normalized Chi2
    */
    float normalize_chi2(float _chi2, float _EnergyE,int algorithm=2)   ;
    ///Normalize fisrt 2 layers' deposited energy and remove Energy Dependence. If EnergyE< 15.GeV, will use paramters of 15 GeV. If EnergyE>200 GeV, will use parameters of 200 GeV.
    /*!
    \param[in]  _f2edep     First 2 layers' deposited energy (MeV)
    \param[in] _EnergyE	EcalShower's EnergyE
    return nf2edep	Normalized f2edep
    */
    float normalize_f2edep(float _f2edep, float _EnergyE)   ;
    ///EcalChi2_plus: Combination of Lateral Fit's EcalChi2 and First 2 layers' energy
    /*!
    \param[in] 	_chi2		Lateral Fit's Ecal Chi2
    \param[in] 	_f2edep		First 2 Layer deposited energy
    \param[in]	_has_normalized Has the variables already been normalized? in case they have not been normalized, you need to provide erg information
    \param[in]      _EnergyE		Ecal Shower EnergyE
    Return	chi2_plus EcalChi2 Plus
    */
    float get_chi2plus(float _chi2, float _f2edep, bool _has_normalized=true, float _EnergyE=0.)	;
    ///EcalChi2_plus_BDT: Combination of EcalChi2_Plus and BDT
    /*!
    \param[in]      _chi2plus       Lateral Fit's Ecal Chi2 Plus
    \param[in]      _bdt         	EcalShower BDT
    Return elik  Ecal electron likelihood
    */
    float get_elik(float _chi2plus, float _bdt)	;

public:
    int   init(const char* fdatabase)   ;
    //Profile for chi2 calculation
    static TH1F*  param_mean_lf[18][6]  ;
    static TH1F*  param_rms_lf[18][6]   ;
    static TH1F*  param_prob_lf[18][2]  ;
    //Version 1 normalization
    static TH1F*  hprofele_Chi2_Erg_rms ;
    static TH1F*  hprofele_F2Edep_Erg_rms;
    //Version 2 normalization
    static TF1*       f_F2Edep_Erg_RMS  ;

    static TF1*       f_Chi2_Erg    ;
    static TF1*       f_F2Edep_Erg  ;
    //Profile for chi2, f2edep, bdt combination
    static TH2D* hpdfele_Chi2_F2Edep    ;
    static TH2D* hpdfpro_Chi2_F2Edep    ;
    static TH2D* hpdfele_Chi2plus_BDT   ;
    static TH2D* hpdfpro_Chi2plus_BDT   ;

private:
    double myfunc_lf(float x,float* par,int type=0) ;
    double nns(TH2D* hsig,TH2D* hbkg,double range,double x,double y,double order);

};

class EcalChi2{
public:
    static int Version  ;
    ///Default constructor, database will be the default location
    EcalChi2(int ftype=0)   ;
    ///Constructor
    /*!
    \param[in] fdatabase   Ecal pdf file location
    \param[in] ftype       data type: 0 ISS Data, 1 Test Beam Data, 2 Simulation Data(disalignment:<20120813)
    */
    EcalChi2(const char* fdatabase,int ftype=0)   ;
    ~EcalChi2() ;
    /// Prcess an AMSEvent, the shower axis is provided by trtrack
    /*!
    \param[in] ev            AMSEventR
    \param[in] trtrack       Tracker Track
    \param[in] iTrTrackPar   Tracker Track iTrTrackPar
    ret chi2      if successful, return the normalized total chi2; else if iTrTrackPar<0, return -1
    */
    float process(AMSEventR* ev, TrTrackR* trtrack, int iTrTrackPar);
    /// Prcess an ecalshower, the shower axis is provided by trtrack
    /*!
    \param[in] trtrack    Tracker Track
    \param[in] esh        Ecal Shower
    \param[in] iTrTrackPar   Tracker Track iTrTrackPar
    ret chi2      if successful, return the normalized total chi2; else if iTrTrackPar<0, return -1
    */
    float process(TrTrackR*  trtrack, EcalShowerR* esh, int iTrTrackPar)	;
    ///Interface to EcalAxis class
    /*!
    \param[in] coo   An array float[18], store Ecal Axis coordinate, for xz projection, it is coordinate x; for yz projection, it is coordinate y
    \param[in] sign  particle sign     , defult -1(electron). +1 (positron)
    ret chi2 if successful, return the normalized total chi2
    */
    float process(float* coo,float sign=-1.);
    //		float process(TrTrack*  trtrack, EcalShower* esh,vector<int>& ndofs,vector<float>& chi2_layer, vector<float>& chi2_layer2,vector<float>& chi2_layer3);
    ///Get Normalized xz total chi2
    /*!
    ret chi2x  The normalized total chi2. sum_{hit i(belong to xz plane,ie layer 2,3 ...)}(chi2_i)/Nhits
    */
    float   get_chi2x() {return _chi2x;}
    ///Get Normalized yz total chi2
    /*!
    ret chi2y  The normalized total chi2. sum_{hit i(belong to yz plane,ie layer 0,1 ...)}(chi2_i)/Nhits
    */
    float   get_chi2y(){return _chi2y;}
    int   get_variance(float& variance){variance=_variance;return  tot_ndof; }
    int   get_skewness(float& skewness){skewness=_skewness;return  tot_ndof; }
    int   get_kurtosis(float& kurtosis){kurtosis=_kurtosis;return  tot_ndof; }
    ///Get first 2 layers' deposited energy(MeV)
    /*!
    ret  f2dep first 2 layers' deposited energy(MeV)
    */
    float get_f2dep(){return _f2dep;}
    ///Interface for EcalAxis Class, Set ecal deposited energy array
    /*!
    \param[in] edeps  ecal deposited energy array(MeV)
    \param[in] erg    ecal deposted energy (GeV)
    \param[in] erge   ecal shower energy (GeV)
    ret 0
    */
    int   set_edep(float* edeps, float erg,float erge) ;
    ///Get Chi2 of each layer
    /*!
     \param[in] ilayer, default -1 ,ret chi2   average of all hits' chi2. sum_{hit i}(chi2_i)/Nhits
                           ilayer!=-1,ret chi2 of layer ilayer
    */
    float   get_chi2 (int ilayer=-1)    ;
    ///EcalPDF pointer
    EcalPDF* ecalpdf    ;
    ///Z coordinates of each layer
    float	  	   ecalz[18]    ;

private:
    void init(const char* fdatabase,int type) ;
    int         _ndofs[18]    ;
    float   _chi2_layer[18]   ;
    float   _chi22_layer[18]  ;
    float   _chi23_layer[18]  ;
    float   _chi24_layer[18]  ;
    float   _f2dep    ;
    float   cal_f2dep()   ;

    float   shiftxy[18]  ;
    float   shiftz[18]   ;
    float   ecalz0[18]  ;
    int     cal_chi2(int start_cell,int end_cell,int layer,double coo,float& chi2,float& chi22,float& chi23, float& chi24, float sign=-1.);
    float   Edep_raw[1296]  ;
    int fdead_cell[18][72]  ;
    int Max_layer_cell[18]  ;
    float   pos[18] ;

    float   _chi2   ;
    float   _chi2x  ;
    float   _chi2y  ;
    float   _variance   ;
    float   _skewness   ;
    float   _kurtosis   ;
    int tot_ndof    ;
    int tot_ndofx   ;
    int tot_ndofy   ;

    float   _erg    ;
    float   _erge   ;
};

class EcalCR:public TObject{
public:
	int runtype	;
	EcalCR(int _runtype=0,const char* fdatabase=NULL)	;
	~EcalCR()	;
public:
	double est_int[2][18][301];
	double est_slope[2][18][301];
	double est_curve[2][18][301];
	static int proj[18];
	static double zCooEcal[18];
	static int est_nrange;
	static double est_erange_low[2]	;
	static double est_erange_high[2];
	Double_t scfit_x0[3],scfit_v0[3];
	double	Theta_x;
	double  Theta_y;
	
	float   erg;

	double sc_local[18] ;
	double sc_global[18];
	int* sc_local_quality;
	float* sc_local_CoG;
	TRandom3 *rdm;
	static double MCsmear;
	
	void init_est(const char* fdatabase);
	double local_shower_center(int layer,double el, double ec, double er, double energy);
	int fit_shower_center_line(double *x0, double *v0, double *shower_center, int *shower_center_quality);	
	double ecalcoo_corrected(double tran_coo, double long_coo, double thetax, double thetay, int lay);
	int get_esh_axis(float* Edep_raw,int* Max_layer_cell,float* init_raw_cr,int* Layer_quality,float _erg);
	double shower_center_layer_orthogonal(int layer_number);
	
	ClassDef(EcalCR,1)
};

class EcalAxis: public TObject{
public:
    static int Version  ;
    ///Default Constructor
    EcalAxis    (int ftype=0);
    ///Constructor
    /*!
    \param[in] fdatabase   Ecal pdf file location
    \param[in] ftype       data type: 0 ISS Data, 1 Test Beam Data, 2 Simulation Data(disalignment:<20120813)
    */
    EcalAxis    (char* fdatabase, int ftype=0);
    ~EcalAxis   ();
    ///Process Ecal Shower
    /*!
    \param[in] esh         EcalShowerR*
    \param[in] algorithm   3bits, 0bzyx x: Cell Ratio method, y: Lateral Fit method, z: Simple CoG method
    \param[in] sign        Particle sign, default -1 (electron)
    */
    int   process      (EcalShowerR* esh,int algorithm=2,float sign=-1);
    ///Process AMSEventR. Use all hits of Ecal to calculate the chi2
    /*!
    \param[in] ev           AMSEventr*, the event needed to be processed
    \param[in] algorithm    3bits, 0bzyx x: Cell Ratio method, y: Lateral Fit method, z: Simple CoG method
    \param[in] trtrack      Tracker track which will be associated to the Ecalshower to determine the sign of the particle. If it is null, program will try to find the best tracker track.
    */
    int   process     (AMSEventR* ev, int algorithm=2, TrTrackR* trtrack=NULL);
    ///Interface for testing
    int   process      (float* fedep,int* fcell,int* fplane, int nEcalhits,float EnergyD, float _EnergyE,int algorithm,float sign);
    ///Interpolate function, interpolate shower axis to plane z
    /*!
    \param[in] zpl         Z coordinate
    \param[out] p0         position of shower axis at plane zpl
    \param[out] dir        direction of shower axis at plane zpl
    \param[in]  algorithm  1: Cell Ratio method, 2: Lateral Fit method, 4: Simple CoG method
    if corresponding lagorithm has been performed at process function, will return 0; else will return -1; if algorithm doesn't exist, will return -2
    */
    int interpolate    (float zpl       ,AMSPoint& p0   ,AMSDir  &dir, int algorithm=2);

    ///Get EcalChi2_plus, Validated for energy range 15.~200GeV, below 15 GeV samples will use the pdf of 15 GeV, above 200 GeV will use the pdf of 200 GeV.
    /*!
    \param[in]  ev		AMSEventR pointer
    \param[out] _nchi2      Normalized Chi2
    \param[out] _nf2edep    Normalized First 2 layer deposted energy
    \param[out] _chi2plus	EcalChi2 Plus
    return chi2 plus
    */
    float get_chi2plus(AMSEventR* ev,float& _nchi2,float& _nf2edep,float& _chi2plus);
    ///Get Ecal Electron Likelihood by combining BDT and EcalChi2_plus. BDT is version 4 BDT. For multi-shower event, BDT of most energetic shower will be used.
    /*!
    \param[in]  ev          AMSEventR pointer
    \param[out] _nchi2	Normalized Chi2
    \param[out] _nf2edep	Normalized First 2 layer deposted energy
    \param[out] _chi2plus	Chi2 plus by combining Chi2 and F2Edep
    \param[out] _elik	Ecal Electron likelihood
    return electron likelihood
    */
    float get_elik(AMSEventR* ev,float& _nchi2,float& _nf2edep,float& _chi2plus, float& _elik)	;

    ///Ecal Chi2 Object, can be used to access chi2, f2dep
    EcalChi2* ecalchi2		;
    EcalCR*   ecalcr		;

    ///Get Normalized Ecal Chi2
    /*!
    \param[in]  ev          AMSEventR pointer
    \param[out] nchi2    	Normalized EcalChi2
    */
    float get_nchi2(AMSEventR* ev)	;
    ///default 0. if set to 1, use the EMDir & Ecal CoG as shower axis	 
    int simple				;
    int     use_ext            ;
private:
    void init(const char* fdatabase, int ftype=0);
    AMSPoint p0_lf             ;
    AMSDir   dir_lf            ;
    int      ndof_lfx          ;
    int      ndof_lfy          ;
    float    chi2x_lf          ;
    float    chi2y_lf          ;
    float	GetChi2(double* par);
    bool     init_lf()         ;
    float    _sign		   ;
    float   ext_d0[3]          ;
    float   ext_p0[3]          ;


    AMSPoint p0_cr             ;
    AMSDir   dir_cr            ;
    int      ndof_crx          ;
    int      ndof_cry          ;
    float    chi2x_cr          ;
    float    chi2y_cr          ;
    bool     init_cr()         ;
    float get_slope_cr  (int flayer) ;
    float get_ratio_cr  (int flayer) ;
    float get_deltaxy_cr(int flayer) ;
    float    EnergyE	   ;
    float	init_raw_cr[18]    ;

    //cell ratio 2
    AMSPoint p0_cr2		;
    AMSDir   dir_cr2            ;
    bool     init_cr2()		;

    AMSPoint p0_cg             ;
    AMSDir   dir_cg            ;
    int      ndof_cgx          ;
    int      ndof_cgy          ;
    float    chi2x_cg          ;
    float    chi2y_cg          ;
    bool     init_cg()         ;
    float    init_raw[18]      ;


    float    Edep_raw[1296]  ;
    int      fdead_cell[18][72];
    int      Layer_quality[18] ;
    float	 layer_Edep[18]    ;
    float	 Max_layer_edep[18];
    int      Max_layer_cell[18];
    float	 _erg		   ;
    float	 ecalz0[18]	   ;
    float    ecalz[18]         ;

    float    shiftxy[18]       ;
    float    shiftz [18]       ;

    int      _type		   ;
    void	 get_z()	   ;
    bool     straight_line_fit(float *x ,float*y, int npoints,float &a, float&b,float &fchi2,float* weight=NULL);
    //_algorithm=0bx1x2x3: x1--> CG, x2--> LF, x3--> CR
    //        xi=0  disable
    //        xi=1  enable
    int	 _algorithm        ;
    int      _status	   ;
    double   SIZE		   ;
    static TVirtualFitter *gMinuit_EcalAxis;
    static void fcn_EcalAxis_Chi2(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag);

    ClassDef(EcalAxis,1)
};
#endif
