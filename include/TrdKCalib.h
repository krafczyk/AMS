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


#ifndef TrdKCalib_H
#define TrdKCalib_H

#include <stdio.h>
#include <stdlib.h>

#include <time.h>
#include <id.h>
#include "TTree.h"
#include "signal.h"
#include <iostream>
#include <fstream>
#include <math.h>
#include <TMatrixD.h>
#include <TFile.h>
#include <TGraph.h>
#include <amsdbc.h>
#include <TChain.h>




using namespace std;


//#include <TRDDBClass_ROOT.h>
//class TRDAlignmentPar_ROOT;
//class TRDAlignmentDB_Plane_ROOT;
//class TRDAlignmentDB_Global_ROOT;



class TRDAlignmentPar{
public:
    TRDAlignmentPar(int _id, float _dX, float _dY, float _dZ, float _alpha, float _beta, float _gamma, float CX, float CY, float CZ):
        id(_id),dX(_dX),dY(_dY),dZ(_dZ),alpha(_alpha),beta(_beta),gamma(_gamma),RotationCenter_X(CX),RotationCenter_Y(CY),RotationCenter_Z(CZ){}
    TRDAlignmentPar():id(-1),dX(0),dY(0),dZ(0),alpha(0),beta(0),gamma(0),RotationCenter_X(0),RotationCenter_Y(0),RotationCenter_Z(0){}

    ~TRDAlignmentPar(){}

    int id;
    float dX;
    float dY;
    float dZ;
    float alpha;
    float beta;
    float gamma;
    float RotationCenter_X;
    float RotationCenter_Y;
    float RotationCenter_Z;

};

class TRDAlignmentDB_Global{
public:
    TRDAlignmentDB_Global():size(0),IsValidate(0),MAX_SIZE(0),interval(0),time_start(0),time_end(0){}
    TRDAlignmentDB_Global(double t_start, int _interval):size(0),IsValidate(0),MAX_SIZE(500),interval(_interval),time_start(t_start){
        //        for(int i=0;i<MAX_SIZE;i++)status[i]=0;
        //        for(int i=0;i<MAX_SIZE;i++)globalpar[i]=TRDAlignmentPar();
        time_end=time_start+interval*size;
    }
    ~TRDAlignmentDB_Global(){}

    TRDAlignmentPar *GetPar(int time){
        int index=GetIndex(time);
        return &globalpar[index];
    }


    int GetIndex(int time){
        int index= int(time-time_start)/interval;
        if(index<0){
            printf("Asking Time before validity time, need to investigate: %i,%f , %i , %f\n",time,time_start,index,interval);
            index=0;
        }else if(index>=MAX_SIZE){
            cout<<"Asking Time exceed end of validity time, need to investigate"<<endl;
            index=MAX_SIZE-1;
        }
        //        printf("Get Index: %i\n",index);
        return index;
    }
    void SetPar(int time, TRDAlignmentPar *_globalpar){
        int index=GetIndex(time);
        if(index>MAX_SIZE){cout<<"******TRDDB ERROR****** array size exceeded******"<<endl;return;}
        globalpar[index]=*_globalpar;
        status[index]=1;
        if(index>size)size=index+1;
        time_end=time_start+interval*size;
    }

    void Validate(){
        for(int i=0;i<MAX_SIZE;i++){
            if(!status[i]){
                cout<<"ERROR Some Interval Missing"<<endl;
                IsValidate=0;
                return;
            }
        }
        IsValidate=1;
    }




    int size;
    bool IsValidate;
    int MAX_SIZE;
    int flag;
    float interval;

    double time_start;
    double time_end;

    int n_par;

    TRDAlignmentPar globalpar[500];

    bool status[500];

};

class TRDAlignmentDB_Plane{
public:
    TRDAlignmentDB_Plane():status(0),flag(0),interval(0),time_start(0),time_end(0){}
    TRDAlignmentDB_Plane(int _interval,TRDAlignmentPar _planepar[20]/*, TRDAlignmentPar _globalpar*/):
        interval(_interval)/*,globalpar(_globalpar)*/{
        for(int i=0;i<20;i++) planepar[i]=_planepar[i];
    };
    int status;
    int flag;
    float interval;

    TRDAlignmentPar planepar[20];
    //    TRDAlignmentPar globalpar;

    TRDAlignmentPar* getplaneparp(int index){return &planepar[index];};
    //    TRDAlignmentPar* getglobalparp(){return &globalpar;};
    void setplanepar(TRDAlignmentPar* _par){planepar[_par->id]=*_par;};
    //    void setglobalpar(TRDAlignmentPar* _par){globalpar=*_par;};


    int time_start;
    int time_end;

};


class TRDCalibPar{
public:
    TRDCalibPar():Time_S(0),Time_E(0),Time_A(0),Theta(0),Phi(0){
        memset(Par,0,sizeof(float)*5248);
        memset(Err,0,sizeof(float)*5248);
        memset(Gain,0,sizeof(float)*328);
        memset(Rate,0,sizeof(float)*328);
    };
    TRDCalibPar(int _time_start, int _time_end, double _time_average, double _theta, double _phi, float _par[5248], float _err[5248], float _gain[328], float _rate[328]):
        Time_S(_time_start),Time_E(_time_end),Time_A(_time_average),Theta(_theta),Phi(_phi){
        memcpy (Par,_par,sizeof(float)*5248);
        memcpy (Err,_err,sizeof(float)*5248);
        memcpy (Gain,_gain,sizeof(float)*328);
        memcpy (Rate,_rate,sizeof(float)*328);
    };

    int Time_S;
    int Time_E;
    Double_t Time_A;
    Double_t Theta;
    Double_t Phi;
    float Par[5248];
    float Err[5248];
    float Gain[328];
    float Rate[328];




    double GetGainCorrectionFactorTube(int tubeid, double asktime){
        if(tubeid>=5248)printf("******Error******TubeID exceed 5247******\n");
        if(!Gain[int(tubeid/16)])return 1;
        return Par[tubeid]+(asktime-Time_A)*Rate[int(tubeid/16)]*Par[tubeid]/Gain[int(tubeid/16)];
    };
    double GetGainCorrectionFactorModule(int Moduleid, double asktime){
        if(Moduleid>=328)printf("******Error******ModuleId exceed 327******\n");
        if(!Gain[Moduleid])return 1;
        return Gain[Moduleid]+(asktime-Time_A)*Rate[Moduleid];
    };

};

class TrdKCalib{

public:

    TrdKCalib(){
        alignment_last_valide_time=0;
        //        tid_Calib=0;
        //        tid_Align_Plane=0;
        //        tid_Align_Global=0;
    };

    TrdKCalib(double _time){
        alignment_last_valide_time=_time;
        //        tid_Calib=0;
        //        tid_Align_Plane=0;
        //        tid_Align_Global=0;
    };

    virtual ~TrdKCalib(){}



    static TrdKCalib* gethead(){
        if (!_Head) {
            printf("TrdKCalib::gethead()-M-CreatingObject TrdKCalib \n");
            _Head = new TrdKCalib();
        }
        return _Head;
    }

    TRDCalibPar _trddb;
    TRDAlignmentDB_Global _trdaligndb_Global;
    TRDAlignmentDB_Plane _trdaligndb_Plane;

    double alignment_last_valide_time;


    TString GetEnv( const string & var );
    template <class T>
    void fillDB(TString s_type,  T *_db, Double_t time_start, Double_t time_end);
    template <class T>
    int readDB(TString s_type,  T *db, Double_t asktime);
    void fillDB_Calibration(TRDCalibPar *_db, Double_t time_start, Double_t time_end);
    void fillDB_Alignment_Plane(TRDAlignmentDB_Plane *_db, Double_t time_start, Double_t time_end);
    void fillDB_Alignment_Global(TRDAlignmentDB_Global *_db, Double_t time_start, Double_t time_end);
    int readDB_Calibration(Double_t asktime);
    int readDB_Alignment_Plane(Double_t asktime);
    int readDB_Alignment_Global(Double_t asktime);
    int readDB_Alignment(Double_t asktime,int readplane=1, int readglobal=1);
    double GetGainCorrectionFactorTube(int tubeid, double asktime);
    double GetGainCorrectionFactorModule(int Moduleid, double asktime);
    TRDAlignmentPar *GetAlignmentPar_Global(int t);
    TRDAlignmentPar *GetAlignmentPar_Plane(int plane);
    TRDAlignmentPar GetAlignmentPar(int plane,int t);
    AMSRotMat GetRotationMatrix(float angle_alpha, float angle_beta, float angle_gamma);
    //    void TestDB_Alignment(Double_t  t_start, Double_t t_end, Double_t t_interval, int layer, int mode);
    void WriteDBFromRoot_Calib(vector<TString> f_input);
    void WriteDBFromRoot_Calib_TB(TString f_input);
    //    void TestDB_Calibration(Double_t  t_start, Double_t t_end, Double_t t_interval, int tube, int mode);
    void WriteDBFromRoot_Alignment(TTree *fChain);

    TRDAlignmentPar TRDAlignmentPar_Global_average;


private:

    static TrdKCalib* _Head;
#ifdef __ROOTSHAREDLIBRARY__
#pragma omp threadprivate (_Head)
#endif

    //    AMSTimeID* tid_Calib;
    //    AMSTimeID* tid_Align_Plane;
    //    AMSTimeID* tid_Align_Global;


    ClassDef(TrdKCalib,2)


};




#endif // TrdKCalib_H
