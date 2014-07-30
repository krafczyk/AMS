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


class LikelihoodObject{
public:
    LikelihoodObject(){}
    LikelihoodObject(int _type, float _amp, float _l, double _likelihood):type(_type),amp(_amp),l(_l),likelihood(_likelihood){}
    ~LikelihoodObject(){};


    int type;
    float amp;
    float l;
    double likelihood;



    //   static bool comp_l(const vector<LikelihoodObject>::iterator i,const vector<LikelihoodObject>::iterator j){ return (*i).l < (*j).l;}
    //   static bool comp_amp(const vector<LikelihoodObject>::iterator i,const vector<LikelihoodObject>::iterator j){ return (*i).amp < (*j).amp;}
    //   static bool comp_likelihood(const vector<LikelihoodObject>::iterator i,const vector<LikelihoodObject>::iterator j){ return (*i).likelihood < (*j).likelihood;}

    //    bool operator > ( const LikelihoodObject rhs) { return (likelihood > rhs.likelihood); }
    //    bool operator < ( const LikelihoodObject rhs) { return (likelihood < rhs.likelihood); }
    static bool  comp_likelihood (LikelihoodObject i ,LikelihoodObject j){ return (i.likelihood < j.likelihood); }

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

    TrdKCluster(AMSEventR *evt,AMSPoint *P0, AMSDir *Dir);

    TrdKCluster(AMSEventR *evt,TrdTrackR *trdtrack,float Rigidity=0);
    TrdKCluster(AMSEventR *evt,TrdHTrackR *trdhtrack,float Rigidity=0);
    TrdKCluster(AMSEventR *evt,EcalShowerR *shower);
    TrdKCluster(AMSEventR *evt, BetaHR *betah,float Rigidity=0);



    void Build(MCEventgR *mcpart){
        AMSPoint TrTrack_P0 = AMSPoint(mcpart->Coo);
        AMSDir TrTrack_Dir = AMSDir(mcpart->Dir);
        SetTrTrack(&TrTrack_P0,&TrTrack_Dir, mcpart->Momentum);
        Init(GetAMSEventRHead());
    }


    void Build(TrTrackR *track, int fitcode){
        SetTrTrack(track, fitcode);
        Init(GetAMSEventRHead());
    }

    void  Build(AMSPoint *P0, AMSDir *Dir,float Rigidity=0){
        if(!Rigidity)Rigidity=DefaultRigidity;
        SetTrTrack(P0, Dir, Rigidity);
        Init(GetAMSEventRHead());
    }

    void Build(TrdTrackR *trdtrack,float Rigidity=0){
        AMSPoint *P0= new AMSPoint(trdtrack->Coo);
        AMSDir *Dir = new AMSDir(trdtrack->Theta,trdtrack->Phi);
        if(!Rigidity)Rigidity=DefaultRigidity;
        SetTrTrack(P0, Dir, Rigidity);
        Init(GetAMSEventRHead());
        delete P0;
        delete Dir;
    }

    void  Build(TrdHTrackR *trdhtrack,float Rigidity=0){
        AMSPoint *P0= new AMSPoint(trdhtrack->Coo);
        AMSDir *Dir = new AMSDir(trdhtrack->Dir);
        if(!Rigidity)Rigidity=DefaultRigidity;
        SetTrTrack(P0, Dir, Rigidity);
        Init(GetAMSEventRHead());
        delete P0;
        delete Dir;
    }

    void Build(EcalShowerR *shower, float energy=0){
        if(!energy) energy=shower->EnergyC;
        AMSPoint *P0= new AMSPoint(shower->CofG);
        AMSDir *Dir = new AMSDir(shower->Dir);
        SetTrTrack(P0, Dir,energy);
        Init(GetAMSEventRHead());
        delete P0;
        delete Dir;
    }

    void Build(BetaHR *betah,float Rigidity=0){
        AMSPoint *P0=new AMSPoint();
        AMSDir *Dir=new AMSDir();
        double dummy_time;
        betah->TInterpolate(TRDCenter,*P0,*Dir,dummy_time,false);
        if(!Rigidity)Rigidity=DefaultRigidity;
        SetTrTrack(P0, Dir, Rigidity);
        Init(GetAMSEventRHead());
        delete P0;
        delete Dir;
    }


    int Build();
    int FindBestMatch_FromParticle();
    int FindBestMatch_FromECAL();
    int FindBestMatch_FromTrTrack();






    // Cluster status
    int  NHits(){return TRDHitCollection.size();}
    int  n_corridor_hits() {return corridor_hits.size();}
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
    int GetLikelihoodRatio(float threshold, double* LLR, double* L, int &nhits, float &total_pathlength, float &total_amp, AMSPoint *P0, AMSDir *Dir,float ECAL_Energy_Hypothesis=0);
    int GetLikelihoodRatio_DEBUG(float threshold, double* LLR, double* L, int &nhits, float &total_pathlength, float &total_amp, AMSPoint *P0, AMSDir *Dir, int start_index=0, float ECAL_Energy_Hypothesis=0);

    int GetLikelihoodRatio_TrTrack(float threshold, double* LLR, int &nhits);
    int GetLikelihoodRatio_TrTrack(float threshold, double* LLR, int &nhits, float ECAL_Energy_Hypothesis, double *LL);

    int GetLikelihoodRatio_TRDRefit(float threshold, double* LLR, int &nhits);
    int GetLikelihoodRatio_TRDRefit(float threshold, double* LLR, int &nhits, float ECAL_Energy_Hypothesis, double *LL, int fitmethod=1, int particle_hypothesis=1);


    // GetLikelihoodRatio and Event Properties,  Debug version
    int GetLikelihoodRatio_TrTrack(float threshold, double* LLR, double* L, int &nhits, float &total_pathlength, float &total_amp, int flag_debug=-1, float ECAL_Energy_Hypothesis=0);
    int GetLikelihoodRatio_TRDRefit(float threshold, double* LLR, double* L, int &nhits, float &total_pathlength, float &total_amp , int fitmethod=1, int particle_hypothesis=1, int flag_debug=-1,  float ECAL_Energy_Hypothesis=0);



    // Number of Hits and Total Amplitude of On-Track Tubes
    void GetOnTrackHit(int& nhits, float & amp,  AMSPoint* P0, AMSDir* Dir);
    void GetOnTrackHit_TrTrack(int& nhits, float & amp);
    void GetOnTrackHit_TRDRefit(int& nhits, float & amp);

    // Number of Hits and Total Amplitude of On-Track Tubes
    void GetNearTrackHit(int& nhits, float & amp,  AMSPoint* P0, AMSDir* Dir);
    void GetNearTrackHit_TrTrack(int& nhits, float & amp);
    void GetNearTrackHit_TRDRefit(int& nhits, float & amp);

    // Number of Hits and Total Amplitude of Off-Track Tubes
    void GetOffTrackHit(int& nhits, float & amp,  AMSPoint* P0, AMSDir* Dir);
    void GetOffTrackHit_TrTrack(int& nhits, float & amp);
    void GetOffTrackHit_TRDRefit(int& nhits, float & amp);

    // Get Refitted TRD Track, accroding to the latest refit
    int GetTRDRefittedTrack(AMSPoint &P0, AMSDir &Dir);

    // Get TrTrack Extrapolation of current TrTrack
    int GetTrTrackExtrapolation(AMSPoint &P0, AMSDir &Dir);
    float GetTrTrackRigidity(){return Track_Rigidity;};

    AMSPoint GetPropogated_TrTrack_P0();
    AMSDir GetPropogated_TrTrack_Dir();

    // Hit manipulation/operation
    TrdKHit* GetHit(int i){if(i>=NHits())return 0;return &(TRDHitCollection.at(i));}
    TrdKHit* GetCorridorHit(int i) {if(i>=corridor_hits.size()) return 0;return &(TRDHitCollection.at(corridor_hits[i]));}
    //    TrdRawHitR* pTrdRawHit(int i){if(i>NHits())return 0;return GetHit(i)->_rawhit;}

    // Select Only Hit within distance/cm around the current track , outside hits are dropped from the cluster
    void DoHitPreselection(float distance);


    // Advanced operation , to select differenet collecionion of hits
    void SetCollection(vector<TrdKHit> _collection){TRDHitCollection=_collection;}

    // Advanced operation, Merge with existing cluster,  User is reponsible to make sure the "common properties" are the same
    void MergeWith(TrdKCluster* secondcluster);

    // Helper function to get Xe partial pressure
    int GetXePressure();
    int GetDefaultMCXePressure(){return DefaultMCXePressure;}
    static void SetDefaultMCXePressure(float xe){ DefaultMCXePressure=xe;return;}



    // Globlal Validity flag
    int GetValidity(){return IsValid;}


    // Default TrTrack fit parameters
    static int Default_TrPar[3];

    // Set Minimum Distance between Track and Tube that will be taken into the cluster
    void SetCorridorRadius(float rad) {corridor_radius = rad; SetCorridorHits();}
    float GetCorridorRadius() const {return corridor_radius;}
    void SetCorridor(const AMSPoint& P, const AMSDir& D) { corridor_p = P; corridor_d = D; SetCorridorHits();}
    void SetCorridor(float rad, const AMSPoint& P, const AMSDir& D) {corridor_radius = rad; corridor_p = P; corridor_d = D; SetCorridorHits();}
    const AMSPoint& GetCorridorP() const {return corridor_p;}
    const AMSDir& GetCorridorD() const {return corridor_d;}
    void SetCorridorHits();

    // For Charge Estimation

    //Get QTRDHitCollection
    vector<TrdKHit> GetQTRDHitCollection() {return QTRDHitCollection;}
    vector<TrdKHit> GetQTRDHitCollectionRefit() {return QTRDHitCollectionRefit;}
    vector<TrdKHit> GetQTRDHitCollectionNuclei() {return QTRDHitCollectionNuclei;}
    vector<TrdKHit> GetQTRDHitCollectionDeltaRay() {return QTRDHitCollectionDeltaRay;}

    //Get likelihood from TrdKPDF
    double GetTRDChargeLikelihood(double Z,int Option);
    double GetTRDChargeLikelihoodUpper(double Z);
    double GetTRDChargeLikelihoodLower(double Z);

    //Calculate TRD charge value and error
    int CalculateTRDCharge(int Option=0, double Velocity=1);

    //Get TRD charge value and error
    double GetTRDCharge() {return TRDChargeValue;}
    double GetTRDChargeError() {return TRDChargeError;}
    double GetTRDChargeUpper();
    double GetTRDChargeLower();

    int GetQNHit() {return QTRDHitCollectionNuclei.size();}
    int GetQNHitUpper() {return QTRDHitCollectionNucleiUpper.size();}
    int GetQNHitLower() {return QTRDHitCollectionNucleiLower.size();}

    double GetCleanliness() {return ((double)(QTRDHitCollection.size()))/((double)(_event->NTrdRawHit()));}

    //Get track refit (charge) IPChi2
    double GetIPChi2();

    //Get TRD asymmetry for dE/dX and delta ray measurements
    double GetAsyNormE(); //Normalized dE/dX
    double GetAsyD(); //Delta ray

    // TRD Partial Pressure
    float Pressure_Xe;


    // Flags for Calibration / Alignment status
    static int IsReadPlaneAlignment;
    static int IsReadGlobalAlignment;
    static int IsReadAlignmentOK;
    static int IsReadCalibOK;

    static int ForceReadAlignment;
    static int ForceReadCalibration;
    static int ForceReadXePressure;
    static float DefaultMCXePressure;

    static Double_t LastProcessedRun_Calibration;
    static Double_t LastProcessedRun_Alignment;

    static bool DebugOn;
    static float threshold_ECAL_Energy_Hypothesis;
    void Set_ECALEnergyHypothesisThreshold(float val){threshold_ECAL_Energy_Hypothesis=val;}


    static TrdKCalib *_DB_instance;
    TrdKCalib* GetTrdKDB(){return _DB_instance;}


    int Track_type;
    int Event_type;

    int IsMC;
    int IsTB;
    int IsISS;

    static int dir_to_k(double& kx,double& ky,bool& up,const AMSDir& dir) {
        kx = dir.x()/dir.z();
        ky = dir.y()/dir.z();
        if(dir.z()>=0) {
            up = true;
        } else {
            up = false;
        }
        return 0;
    }
    static int k_to_dir(AMSDir& dir,const double& kx,const double& ky,const bool& up) {
        double b = TMath::Sqrt(1+kx*kx+ky*ky);
        double dx;
        double dy;
        double dz;
        if(up) {
            dx = kx/b;
            dy = ky/b;
            dz = TMath::Sqrt(1-dx*dx-dy*dy);
        } else {
            dx = -kx/b;
            dy = -ky/b;
            dz = -TMath::Sqrt(1-dx*dx-dy*dy);
        }
        dir = AMSDir(dx,dy,dz);
        return 0;
    }
    Double32_t GetTime() {
        return Time;
    }

    bool IsHitGood(TrdKHit* hit) {
        if(preselected) {
            return true;
	} else {
	    double dist = fabs(hit->Tube_Track_Distance_3D(&corridor_p, &corridor_d));
	    if(dist > corridor_radius) {
                return false;
	    }
        }
	return true;
    }

    static AMSEventR* GetAMSEventRHead(){
        if(!_HeadE)_HeadE=AMSEventR::Head();
        return _HeadE;
    }


    static TrdKCalib* GetTRDKCalibHead(){
        if(!_HeadCalib)_HeadCalib=TrdKCalib::gethead();
        return _HeadCalib;
    }



    static TrdKCluster* gethead(){
        if (!_Head) {
            printf("TrdKCluster::gethead()-M-CreatingObject TrdKCluster\n");
            _Head = new TrdKCluster();
        }
        GetAMSEventRHead();
        GetTRDKCalibHead();
        return _Head;
    }


    static void ResetHead(){
        if(_Head)delete _Head;
        _Head=0;
        _HeadE=0;
        _HeadCalib=0;
        return;
    }


private:

    static TrdKCluster* _Head;
    static AMSEventR * _HeadE;
    static TrdKCalib* _HeadCalib;

    AMSEventR* pev;
    TrTrackR    *ptrk;
    TrdTrackR   *ptrd;
    TrdHTrackR  *ptrdh;
    EcalShowerR *pecal;
    BetaHR      *ptof;
    MCEventgR   *mcpart;

    float corridor_radius;
    AMSPoint corridor_p;
    AMSDir corridor_d;
    vector<int> corridor_hits;
    bool preselected;

    // Additinal Initilizationa
    void Init_Base();
    void Init(AMSEventR* evt);
    void Construct_TRDTube();

    void SelectClosest();
    void FillHitCollection(AMSEventR* evt);

    // Gain Calibration and Alignment
    void SetDB(TrdKCalib *rhs){_DB_instance=rhs;}
    int DoGainCalibration();
    int DoAlignment(int &readplane, int &readglobal);


    // TrTrack for propogation/extrapolation
    void Propogate_TrTrack(double z);


    // TRDTrack Refit
    static void fcn_TRDTrack(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag);
    static void fcn_TRDTrack_PathLength(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag);
    
    double TRDTrack_ImpactChi2(Double_t *par);

    double TRDTrack_PathLengthLikelihood(Double_t *par);
    
    static Double_t trd_parabolic_fit(Int_t N, Double_t *X, Double_t *Y, Double_t *V);
    
    void KounineRefit(AMSPoint& P_fit, AMSDir& D_fit,
                      const AMSPoint& P_init, const AMSDir& D_init);
    
    void SetImpLikelihood(TRD_ImpactParameter_Likelihood *Likelihood){ TRDImpactlikelihood=Likelihood;}
    void FitTRDTrack_IPLikelihood(int IsCharge=0); //IsCharge==1 is used for charge
    void FitTRDTrack_PathLength(int particle_hypothesis);

    void FitTRDTrack_PathLength_KFit(int particle_hypothesis);
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



    static TRD_ImpactParameter_Likelihood *TRDImpactlikelihood;
    static vector <TrdKHit> TRDTubeCollection;




    static TrdKPDF *kpdf_e;
    static TrdKPDF *kpdf_p;
    static TrdKPDF *kpdf_h;
    static TrdKPDF *kpdf_q;

    static TMinuit *gMinuit_TRDTrack;

    static map<int, TRDOnline> map_TRDOnline;

    static float DefaultRigidity;

    TRDAlignmentPar par_alignment;


    // For TRD Charge Estimation

    //TRD hits collection
    vector<TrdKHit> QTRDHitCollection;
    vector<TrdKHit> QTRDHitCollectionRefit;
    vector<TrdKHit> QTRDHitCollectionNuclei;
    vector<TrdKHit> QTRDHitCollectionDeltaRay;
    vector<TrdKHit> QTRDHitCollectionNucleiUpper;
    vector<TrdKHit> QTRDHitCollectionNucleiLower;

    //Total amplitude of delta ray tubes
    double DAmp;
    double DAmpUpper;
    double DAmpLower;

    //TRD charge value and error
    double TRDChargeValue;
    double TRDChargeError;

    //Number of dE/dX tubes not saturated
    int NBelowThreshold;
    int NBelowThresholdUpper;
    int NBelowThresholdLower;

    //For beta correction
    double Beta;


    ClassDef(TrdKCluster,1)

};







#endif // TrdKCLUSTER_H
