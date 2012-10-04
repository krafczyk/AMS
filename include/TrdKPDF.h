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

#ifndef TrdKPDF_H
#define TrdKPDF_H

#include "TROOT.h"
#include "Rtypes.h"
#include "math.h"
#include "stdio.h"
#include "stdlib.h"
#include "iostream"
#include "TF1.h"
#include "TMath.h"
#include "TString.h"
#include "TH1F.h"
#include "TRandom3.h"
#include "TSpline.h"
using namespace std;

class TrdKPDF:public TObject{

public:

    TrdKPDF(TString s){
        s_particle=s;
        if(s=="Proton"){
            particle=1;
            Init_Proton();
        }else if(s=="Electron"){
            particle=0;
            Init_Electron();
        }
        else if(s=="Helium"){
            particle=2;
            Init_Helium();
        }else if(s=="Nuclei"){  //Added by Wei Sun
            particle=3;
            Init_Nuclei();
        }
        else {
            std::cout<<"Particle: "<<s<<" Not Supported "<<std::endl;
        }


        SetNormalization(0,0);

//        f_func = new TF1("func_for_random",this,0,4000,0,"TrdKPDF");
//        f_func->SetNpx(4000);


    }
    ~TrdKPDF(){};


    void Init_Proton();
    void Init_Electron();
    void Init_Helium();
    void Init_Nuclei();      //Added by Wei Sun



    void GetPar(float Pmom, float Length, int Layer, float Press){
        //        cout<<"KPDF, GetPar: "<<Pre_Layer<<endl;
        Pre_Momentum=Pmom;
        Pre_PathLength=Length;
        Pre_Xe=Press*1000;
        Pre_Layer=Layer;

        if(particle==0)GetPar_Electron(Pmom,Length,Layer,Press);
        else if(particle==1)GetPar_Proton(Pmom,Length,Layer,Press);
        else if(particle==2)GetPar_Helium(Pmom,Length,Layer,Press);

    }

    void GetPar_Proton(Double_t Pmom, Double_t Length, Int_t Layer, Double_t Press);
    void GetPar_Electron(Double_t Pmom, Double_t Length, Int_t Layer, Double_t Press);
    void GetPar_Helium(Double_t Pmom, Double_t Length, Int_t Layer, Double_t Press);
    void GetPar_Nuclei(Double_t Charge, Double_t Pmom, Double_t Length, Int_t Layer, Double_t Press, Double_t Beta); 



    void SetPar_1TeV(){
        if(particle==0){
            Double_t _par[11]={0.434965, 114.903360, 80.460000, 0.500000, 0.565035, 442.466750, 413.437120, 0.207120, 600.000000, 700.000000, 0.500000};
            memcpy(Par,_par,sizeof(Double_t)*11);
            return;
        }

        if(particle==1){
            Double_t _par[11]={0.700000, 114.903360, 80.460000, 0.500000, 0.300000, 400.000000, 300.000000, 0.207120, 600.000000, 700.000000, 0.500000};
            memcpy(Par,_par,sizeof(Double_t)*11);
            return;
        }
    }

    void SetPar_2TeV(){
        if(particle==0){
            Double_t _par[11]={0.434965, 114.903360, 80.460000, 0.500000, 0.565035, 442.466750, 413.437120, 0.207120, 600.000000, 700.000000, 0.500000};
            memcpy(Par,_par,sizeof(Double_t)*11);
            return;
        }
        if(particle==1){
            Double_t _par[11]={0.500000, 114.903360, 80.460000, 0.500000, 0.500000, 442.466750, 320.000000, 0.250000, 600.000000, 700.000000, 0.500000};
            memcpy(Par,_par,sizeof(Double_t)*11);
            return;
        }
    }



    Double_t func_li(Double_t x, Double_t a, Double_t b, Double_t c);
    Double_t func_l(Double_t x, Double_t a, Double_t b, Double_t c);

    Double_t GetNormalization(double start);
    Double_t GetLikelihood(double Amp, float Corr=1.0);
    Double_t GetLikelihood(double Amp,float Pmom,float Length,int Layer,float Press){
        GetPar(Pmom,Length,Layer,Press);
        return GetLikelihood(Amp);
    }



    double operator() (double *x, double *p) {
        return GetLikelihood(x[0]);
    }


    vector<float> Generate(int nevents){
        vector<float> hits;
        TF1 *f = new TF1("temp_f",this,0,4000,0,"TrdKPDF");
        f->SetNpx(4000);
        for(int i=0;i<nevents;i++){
            hits.push_back(f->GetRandom());
        }
        delete f;
        return hits;
    }

    float GenerateHit(int start=-999){

        float Corr=1.0;

        float Norm=1.0;
        if(start>0)Norm=GetNormalization(start);
        //        if(Normalization_Method==0) Norm=1.0;

        double Prob = gRandom.Uniform(0,1);

        Float_t Amp, Amp1, Amp2;
        Double_t x, v;

        x = 15.*Corr;
        v = 1.-GetNormalization(x)/Norm;
        if ( Prob < v ) return 0;
        Amp1 = (Int_t)x;

        x = 3400.*Corr;
        v = 1.-GetNormalization(x)/Norm;
        if ( Prob >= v ) return x;
        Amp2 = (Int_t)x;

        Amp = (Amp1+Amp2)/2;

        float Amp_previous=0;

        while ( fabs(Amp-Amp_previous)>0.1 ) {
            Amp_previous=Amp;
            x = Corr*Amp;
            v = 1.-GetNormalization(x)/Norm;
            if ( Prob >= v ) Amp1 = Amp;
            else             Amp2 = Amp;
            Amp = (Amp1+Amp2)/2;
            x = Corr*Amp;
        }

        return x;
    }



    TH1F* DrawFunc(){
        TString s_name=Form("h_TrdKPDF_M%.0f_P%.2f_Xe%.0f_L%i_"+s_particle,Pre_Momentum,Pre_PathLength,Pre_Xe,Pre_Layer);
        cout<<s_name<<endl;

        s_name.ReplaceAll(".","_");
        TF1 *f = new TF1(s_name,this,-100,4000,0,"TrdKPDF");
        f->SetNpx(4100);
        TH1F* h=(TH1F*)f->GetHistogram();
        h->SetTitle(s_name);
        h->SetName(s_name);

        return h;
    }


    void SetSeed(int seed){gRandom.SetSeed(seed);}



    TRandom3 gRandom;

//    TF1* f_func;

    TString s_particle;
    int particle;


    Double_t mom[50];
    Double_t p01_m[50];
    Double_t p02_m[50];
    Double_t p03_m[50];
    Double_t p04_m[50];
    Double_t p05_m[50];
    Double_t p06_m[50];
    Double_t p07_m[50];
    Double_t p08_m[50];
    Double_t p09_m[50];
    Double_t p10_m[50];
    Double_t p11_m[50];
    Double_t p12_m[50];
    Double_t p13_m[50];
    Double_t p14_m[50];
    Double_t p15_m[50];

    Double_t len[18];
    Double_t p01_l[18];
    Double_t p02_l[18];
    Double_t p03_l[18];
    Double_t p04_l[18];
    Double_t p05_l[18];
    Double_t p06_l[18];
    Double_t p07_l[18];
    Double_t p08_l[18];
    Double_t p09_l[18];
    Double_t p10_l[18];
    Double_t p11_l[18];
    Double_t p12_l[18];
    Double_t p13_l[18];
    Double_t p14_l[18];
    Double_t p15_l[18];


    Double_t p01_n[20];
    Double_t p02_n[20];
    Double_t p03_n[20];
    Double_t p04_n[20];
    Double_t p05_n[20];
    Double_t p06_n[20];
    Double_t p07_n[20];
    Double_t p08_n[20];
    Double_t p09_n[20];
    Double_t p10_n[20];
    Double_t p11_n[20];
    Double_t p12_n[20];
    Double_t p13_n[20];
    Double_t p14_n[20];
    Double_t p15_n[20];

    Double_t c01_x[18];
    Double_t c02_x[18];
    Double_t c03_x[18];
    Double_t c04_x[18];
    Double_t c05_x[18];
    Double_t c06_x[18];
    Double_t c07_x[18];
    Double_t c08_x[18];
    Double_t c09_x[18];
    Double_t c10_x[18];
    Double_t c11_x[18];
    Double_t c12_x[18];
    Double_t c13_x[18];
    Double_t c14_x[18];
    Double_t c15_x[18];



    Double_t Par[15];


    float Pre_Momentum;
    float Pre_PathLength;
    float Pre_Xe;
    int Pre_Layer;







    int Normalization_Method;
    double Normalization_Start;
    double Norm;

    void SetNormalization(int _Method, double start=-9999){
        Normalization_Method=_Method;
        Normalization_Start=start;
//        Norm=GetNormalization(Normalization_Start);
    }




    //=========For TRD Charge Estimation==========

    Double_t chrg[80];
    Double_t p02_c[80];
    Double_t p03_c[80];
    Double_t p04_c[80];
    Double_t p09_c[80];
    Double_t p10_c[80];

    Double_t ichrg[8];
    Double_t ip02_c[8];
    Double_t ip03_c[8];
    Double_t ip04_c[8];
    Double_t ip09_c[8];
    Double_t ip10_c[8];

    TSpline3 *gs02;
    TSpline3 *gs03;
    TSpline3 *gs04;
    TSpline3 *gs09;
    TSpline3 *gs10;

    Double_t p01_b[31];

    Double_t GetLikelihoodQ(double Amp);
    Double_t GetLikelihoodHA(Double_t Corr);
    Double_t GetLikelihoodLA(Double_t Corr);
    Double_t GetLikelihood(Double_t Amp,Double_t Charge,Double_t Corr,Double_t Pmom,Double_t Length,Int_t Layer,Double_t Press,Double_t Beta){
        GetPar_Nuclei(Charge,Pmom,Length,Layer,Press,Beta);
        if(Amp>3400*Corr) return GetLikelihoodHA(Corr);
        else if(Amp<15*Corr) return GetLikelihoodLA(Corr);
        else return GetLikelihoodQ(Amp);
    }

    Double_t GetLikelihoodDR(Double_t DAmpL,Double_t Charge,Double_t Rigidity);




        ClassDef(TrdKPDF,1)

};






#endif // TrdKPDF_H
