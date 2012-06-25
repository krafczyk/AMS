//**************************************************************//
// TRDKLikelihood methods
//
// For detailed information:
// https://twiki.cern.ch/twiki/bin/view/AMS/TRDKLikelihood
//
//
// 2012-06-18  v1.0,  First commit on cvs
//
//**************************************************************//


#ifndef TrdKCLUSTER_H
#define TrdKCLUSTER_H

#include <TrdKHit.h>
#include "TrdKCalib.h"
#include <TrdRawHit.h>
#include <TrdKSimpleHit.h>
#include <TrTrack.h>
#include <point.h>
#include <TrdHCalib.h>
#include <TMinuit.h>
#include <TVirtualFitter.h>
#include <TF1.h>
#include <TMath.h>
#include <TrdKPDF.h>
#include <root.h>



class TRDOnline{

public:
    TRDOnline():time(0),xe(0),co2(0){}
    TRDOnline(int _time, float _xe, float _co2):time(_time),xe(_xe),co2(_co2){}
    ~TRDOnline(){}

    int time;
    float xe;
    float co2;

};

class TRD_ImpactParameter_Likelihood{

public:
    TRD_ImpactParameter_Likelihood(){
        InitLikelihood();
    };
    ~TRD_ImpactParameter_Likelihood(){
        delete d0_likelihood_func;
    }

    double GetLikelihood(float Amp, float d0);
    void InitLikelihood();
    TF1* d0_likelihood_func;
    float d0_likelihood_func_integral;

    TF1* d0_likelihood_func_missed;
    float d0_likelihood_func_missed_integral;

};




class TrdKCluster : public TObject
{
public:
    TrdKCluster();

    // Constructor for ToyMC Study
    TrdKCluster(vector<TrdKHit> _collection,AMSPoint *P0, AMSPoint *Dir,AMSPoint *TRDTrack_P0, AMSPoint *TRDTrack_Dir,AMSPoint *MaxSpan_P0, AMSPoint *MaxSpan_Dir);

    // Constructor for Analysis on AMS Root files
    TrdKCluster(AMSEventR *evt, TrTrackR *track, int fitcode);
    ~TrdKCluster();




    // Cluster status
    int  NHits(){return TRDHitCollection.size();}
    bool IsCalibrated();
    bool IsAligned();


    // Set TrTrack, fitcode
    void SetTrTrack(TrTrackR* track, int fitcode);


    // Set TrTrack, for ToyMC
    void SetTrTrack(AMSPoint *P0, AMSDir *Dir, float Rigidity=0);


    // Set TRDTrack, for ISS Analysis and ToyMC
    void SetTRDTrack(AMSPoint *P0, AMSDir *Dir, float Rigidity=0);

    // Wrapper method to do TRD Refit
    void FitTRDTrack(int method=1, int hypothesis=1);

    // GetLikelihoodRatio and Event Properties
    int GetLikelihoodRatio(float threshold, double* LLR, int &nhits, AMSPoint *P0, AMSDir *Dir);
    int GetLikelihoodRatio_TrTrack(float threshold, double* LLR, int &nhits);
    int GetLikelihoodRatio_TRDRefit(float threshold, double* LLR, int &nhits, int fitmethod=1, int particle_hypothesis=1);

    // Number of Hits and Total Amplitude of Off-Track Tubes
    void GetOffTrackHit(int& nhits, float & amp,  AMSPoint* P0, AMSDir* Dir);
    void GetOffTrackHit_TrTrack(int& nhits, float & amp);
    void GetOffTrackHit_TRDRefit(int& nhits, float & amp);

    // Get Refitted TRD Track, accroding to the latest refit
    int GetTRDRefittedTrack(AMSPoint &P0, AMSDir &Dir);

    // Get TrTrack Extrapolation of current TrTrack
    int GetTrTrackExtrapolation(AMSPoint &P0, AMSDir &Dir);


    // Hit manipulation/operation
    TrdKHit* GetHit(int i){if(i>NHits())return 0;return &(TRDHitCollection.at(i));}
    //    TrdRawHitR* pTrdRawHit(int i){if(i>NHits())return 0;return GetHit(i)->_rawhit;}

    // Select Only Hit within distance/cm around the current track , outside hits are dropped from the cluster
    void DoHitPreselection(float distance);


    // Advanced operation , to select differenet collecionion of hits
    void SetCollection(vector<TrdKHit> _collection){TRDHitCollection=_collection;}

    // Advanced operation, Merge with existing cluster,  User is reponsible to make sure the "common properties" are the same
    void MergeWith(TrdKCluster* secondcluster);

    // Advanced operation, remove duplicate hit.
    void RemoveDuplicate(TrdKCluster* fired);


    // Helper function to get Xe partial pressure
    int GetXePressure();

    // Globlal Validity flag
    int GetValidity(){return IsValid;}


    // Default TrTrack fit parameters
    static int Default_TrPar[3];



    // For Charge Estimation

    //Get QTRDHitCollection
    vector<TrdKHit> GetQTRDHitCollection() {return QTRDHitCollection;}
    vector<TrdKHit> GetQTRDHitCollectionRefit() {return QTRDHitCollectionRefit;}
    vector<TrdKHit> GetQTRDHitCollectionNuclei() {return QTRDHitCollectionNuclei;}
    vector<TrdKHit> GetQTRDHitCollectionDeltaRay() {return QTRDHitCollectionDeltaRay;}

    //Get likelihood from TrdKPDF
    double GetTRDChargeLikelihood(double Z,int Option);

    //Calculate TRD charge value and error
    void CalculateTRDCharge(int Option=0);

    //Get TRD charge value and error
    double GetTRDCharge() {return TRDChargeValue;}
    double GetTRDChargeError() {return TRDChargeError;}


    // TRD Partial Pressure
    float Pressure_Xe;


    // Flags for Calibration / Alignment status
    static int IsReadPlaneAlignment;
    static int IsReadGlobalAlignment;
    static int IsReadAlignmentOK;
    static int IsReadCalibOK;

    static int ForceReadAlignment;
    static int ForceReadCalibration;

    static Double_t LastProcessedRun_Calibration;
    static Double_t LastProcessedRun_Alignment;


private:

    // Additinal Initilizationa
    void Init_Base();
    void Constrcut_TRDTube();

    void SelectClosest();
    void AddEmptyTubes(float);

    // Gain Calibration and Alignment
    void SetDB(TrdKCalib *rhs){_DB_instance=rhs;}
    int DoGainCalibration();
    int DoAlignment(int &readplane, int &readglobal);


    // TrTrack for propogation/extrapolation
    void Propogate_TrTrack(double z);
    AMSPoint GetPropogated_TrTrack_P0();
    AMSDir GetPropogated_TrTrack_Dir();

    // TRDTrack Refit
    static void fcn_TRDTrack(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag);
    static void fcn_TRDTrack_PathLength(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag);

    double TRDTrack_ImpactChi2(Double_t *par);
    double TRDTrack_PathLengthLikelihood(Double_t *par);
    void SetImpLikelihood(TRD_ImpactParameter_Likelihood *Likelihood){ TRDImpactlikelihood=Likelihood;}

    void FitTRDTrack_IPLikelihood(int IsCharge=0); //IsCharge==1 is used for charge
    void FitTRDTrack_PathLength(int particle_hypothesis);
    void FitTRDTrack_Analytical();
    void AnalyticalFit_2D(int direction, double x, double z , double dx, double dz, double &TRDTrack_x, double &TRDTrack_dx);


    // Online Parameters
    void InitXePressure();



    // For TRD Charge

    //Select TRD hits used for different purpose
    void HitSelectionBeforeRefit(double DistanceCut); //For general and refit
    void HitSelectionAfterRefit(); //For nuclei and delta ray


    //TMinuit preparation
    static void fcn_TRDTrack_Charge(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag);
    double TRDTrack_ImpactChi2_Charge(Double_t *par);



    AMSEventR* _event;
    Double32_t Time;
    bool IsValid;
    float Zshift;
    float TRDCenter;
    float Minimum_dR;
    vector <TrdKHit> TRDHitCollection;


    float Track_Rigidity;
    float Track_Rigidity_Inner;
    float Track_Rigidity_Full;
    float TRDTrack_Rigidity;

    AMSDir track_extrapolated_Dir;
    AMSPoint	 track_extrapolated_P0;
    TrProp TrTrack_Pro;

    AMSDir TRDtrack_extrapolated_Dir;
    AMSPoint	 TRDtrack_extrapolated_P0;
    bool HasTRDTrack;


    TMatrixD *M_MyDD;
    TMatrixD *M_MyD;
    TMatrixD *M_A;

    int Refit_hypothesis;

    // Static object

    static bool DebugOn;

    static TRD_ImpactParameter_Likelihood *TRDImpactlikelihood;
    static vector <TrdKHit> TRDTubeCollection;


    static TrdKCalib *_DB_instance;

    static TrdKPDF *kpdf_e;
    static TrdKPDF *kpdf_p;
    static TrdKPDF *kpdf_h;
    static TrdKPDF *kpdf_q;

    static TVirtualFitter *gMinuit_TRDTrack;

    static map<int, TRDOnline> map_TRDOnline;

    static float MinimumDistance;

    TRDAlignmentPar par_alignment;


    // For TRD Charge Estimation

    //TRD hits collection
    vector<TrdKHit> QTRDHitCollection;
    vector<TrdKHit> QTRDHitCollectionRefit;
    vector<TrdKHit> QTRDHitCollectionNuclei;
    vector<TrdKHit> QTRDHitCollectionDeltaRay;

    //Total amplitude of delta ray tubes
    double DAmp;

    //TRD charge value and error
    double TRDChargeValue;
    double TRDChargeError;




    ClassDef(TrdKCluster,1)

};

#endif // TrdKCLUSTER_H
