#include "EcalChi2CY.h"
//  $Id: EcalChi2CY.C,v 1.25 2013/02/18 17:39:38 kaiwu Exp $
#define SIZE  0.9

ClassImp(EcalAxis);
ClassImp(EcalCR);
int EcalPDF::Version =2 ;
int EcalPDF::has_init=-1;
EcalPDF::EcalPDF(const char* fdatabase){
    if(has_init==-1)
        init(fdatabase);
}
EcalPDF::EcalPDF(){
    char* amsdatadir=getenv("AMSDataDir");
    string tempname ;
    if(!amsdatadir)
        amsdatadir="/afs/cern.ch/ams/Offline/AMSDataDir/";
    tempname=amsdatadir;
    if(Version==1)
        tempname+="/v5.00/EcalChi2CY_prof.20121106.1.root";
    else if(Version==2)
        tempname+="/v5.00/EcalChi2CY_prof.20130218.1.root";
    else{
        has_init=-1;
        warn_messages="EcalPDF-Error: Version ";
        warn_messages+=Version;
        warn_messages+=" hasn't been supported yet.";
    }
    if(Version==1||Version==2)
        init(tempname.c_str());
}

TH1F*  EcalPDF::param_mean_lf[18][6];
TH1F*  EcalPDF::param_rms_lf[18][6] ;
TH1F*  EcalPDF::param_prob_lf[18][2]    ;
TH1F*  EcalPDF::hprofele_Chi2_Erg_rms   ;
TH1F*  EcalPDF::hprofele_F2Edep_Erg_rms ;
TF1*   EcalPDF::f_F2Edep_Erg_RMS    ;
TF1*   EcalPDF::f_Chi2_Erg  ;
TF1*   EcalPDF::f_F2Edep_Erg;
TH2D*  EcalPDF::hpdfele_Chi2_F2Edep ;
TH2D*  EcalPDF::hpdfpro_Chi2_F2Edep ;
TH2D*  EcalPDF::hpdfele_Chi2plus_BDT    ;
TH2D*  EcalPDF::hpdfpro_Chi2plus_BDT    ;


int EcalPDF::init(const char* fdatabase){
    has_init=1	      ;
    char tempname[300];

    for(int layer=0;layer<18;layer++){
        for(int i1=0;i1<6;i1++){
            sprintf(tempname,"hrms_lay%.2d_%d_2",layer,i1);
            param_rms_lf[layer][i1] =new TH1F(tempname,tempname,710, -31.905, 31.995) ;
            pdf_names.push_back(string(tempname));
            sprintf(tempname,"hmean_lay%.2d_%d_2",layer,i1);
            param_mean_lf[layer][i1]=new TH1F(tempname,tempname,710, -31.905, 31.995);
            pdf_names.push_back(string(tempname));
        }
        for(int i1=0;i1<2;i1++){
            sprintf(tempname,"hprob_lay%.2d_%d_2",layer,i1);
            param_prob_lf[layer][i1]=new TH1F(tempname,tempname,710, -31.905, 31.995);
            pdf_names.push_back(string(tempname));
        }
    }
    if(Version==1){
        hpdfele_Chi2_F2Edep=new TH2D("hpdfele_Chi2_F2Edep1","hpdfele_Chi2_F2Edep1",512,-6.,6.,512,-6.,6.);
        pdf_names.push_back(string("hpdfele_Chi2_F2Edep1"));
        hpdfpro_Chi2_F2Edep=new TH2D("hpdfpro_Chi2_F2Edep1","hpdfpro_Chi2_F2Edep1",512,-6.,6.,512,-6.,6.);
        pdf_names.push_back(string("hpdfpro_Chi2_F2Edep1"));
        hpdfele_Chi2plus_BDT=new TH2D("hpdfele_Chi2plus_BDT1","hpdfele_Chi2plus_BDT1",256,-0.001,1.001,256,-1.001,1.001);
        pdf_names.push_back(string("hpdfele_Chi2plus_BDT1"));
        hpdfpro_Chi2plus_BDT=new TH2D("hpdfpro_Chi2plus_BDT1","hpdfpro_Chi2plus_BDT1",256,-0.001,1.001,256,-1.001,1.001);
        pdf_names.push_back(string("hpdfpro_Chi2plus_BDT1"));
        f_Chi2_Erg=new TF1("f_Chi2_Erg1","pol1")	;
        pdf_names.push_back(string("f_Chi2_Erg1"));
        f_F2Edep_Erg=new TF1("f_F2Edep_Erg","pol1")	;
        pdf_names.push_back(string("f_F2Edep_Erg"));
        hprofele_Chi2_Erg_rms=new TH1F("hprofele_Chi2_Erg_rms","hprofele_Chi2_Erg_rms",40,log10(15),log10(200))		    ;
        pdf_names.push_back(string("hprofele_Chi2_Erg_rms"));
        hprofele_F2Edep_Erg_rms=new TH1F("hprofele_F2Edep_Erg_rms","hprofele_F2Edep_Erg_rms",40,log10(15),log10(200))	;
        pdf_names.push_back(string("hprofele_F2Edep_Erg_rms"));
    }
    if(Version==2){
        hpdfele_Chi2_F2Edep=new TH2D("hpdfele_Chi2_F2Edep1","hpdfele_Chi2_F2Edep",256,-6.,6.,256,-6.,6.);
        pdf_names.push_back(string("hpdfele_Chi2_F2Edep1"));
        hpdfpro_Chi2_F2Edep=new TH2D("hpdfpro_Chi2_F2Edep1","hpdfpro_Chi2_F2Edep",256,-6.,6.,256,-6.,6.);
        pdf_names.push_back(string("hpdfpro_Chi2_F2Edep1"));
        hpdfele_Chi2plus_BDT=new TH2D("hpdfele_Chi2plus_BDT1","hpdfele_Chi2plus_BDT1",256,-0.01,1.01,256,-1.01,1.01);
        pdf_names.push_back(string("hpdfele_Chi2plus_BDT1"));
        hpdfpro_Chi2plus_BDT=new TH2D("hpdfpro_Chi2plus_BDT1","hpdfpro_Chi2plus_BDT1",256,-0.01,1.01,256,-1.01,1.01);
        pdf_names.push_back(string("hpdfpro_Chi2plus_BDT1"));
        f_Chi2_Erg=new TF1("f_Chi2_Erg1","pol3")	;
        pdf_names.push_back(string("f_Chi2_Erg1"));
        f_F2Edep_Erg=new TF1("f_F2Edep_Erg1","pol3");
        pdf_names.push_back(string("f_F2Edep_Erg1"));
        f_F2Edep_Erg_RMS=new TF1("f_F2Edep_Erg_RMS1","pol3")	;
        pdf_names.push_back(string("f_F2Edep_Erg_RMS1"));
    }
    TH1F* hdummy ;
    TFile* _fdatabase=TFile::Open(fdatabase);
    if(fdatabase==NULL){
        warn_messages ="EcalPDF-Error: ECALChi2CY database file : ";
        warn_messages+=fdatabase;
        warn_messages+=" is NULL";
        has_init=-1;
        return -1;
    }
    warn_messages="EcalPDF-Error: There is no ";
    for(int layer=0;layer<18;layer++){
        for(int i1=0;i1<6;i1++){
            sprintf(tempname,"param_rms_%d_lay%.2d",i1,layer);
            hdummy=(TH1F*)_fdatabase->Get(tempname);
            if(hdummy==NULL){
                warn_messages +=tempname;
                has_init=-1;
            }
            else{
                for(int i2=1;i2<=hdummy->GetXaxis()->GetNbins();i2++)
                    param_rms_lf[layer][i1]->SetBinContent(i2,hdummy->GetBinContent(i2));
            }
            hdummy=NULL;
            sprintf(tempname,"param_mean_%d_lay%.2d",i1,layer);
            hdummy=(TH1F*)_fdatabase->Get(tempname);
            if(hdummy==NULL){
                warn_messages+=tempname;
                has_init=-1;
            }
            else{
                for(int i2=1;i2<=hdummy->GetXaxis()->GetNbins();i2++)
                    param_mean_lf[layer][i1]->SetBinContent(i2,hdummy->GetBinContent(i2));
            }
            hdummy=NULL;
        }
    }
    for(int layer=0;layer<18;layer++){
        for(int i1=0;i1<2;i1++){
            sprintf(tempname,"param_prob_%d_lay%.2d",i1,layer);
            hdummy=(TH1F*)_fdatabase->Get(tempname);
            if(hdummy==NULL){
                warn_messages+=tempname;
                has_init=-1;
            }
            else{
                for(int i2=1;i2<=hdummy->GetXaxis()->GetNbins();i2++)
                    param_prob_lf[layer][i1]->SetBinContent(i2,hdummy->GetBinContent(i2));
            }
            hdummy=NULL;
        }
    }
    if(Version==1){
        TH2D* hdummy2;
        TProfile* hdummyp;
        TF1* f	;
        f=(TF1*)_fdatabase->Get("f_Chi2_Erg");
        if(f==NULL){
            warn_messages+=" f_Chi2_Erg";
            has_init=-1;
        }
        else{
            f_Chi2_Erg->SetParameter(0,f->GetParameter(0));
            f_Chi2_Erg->SetParameter(1,f->GetParameter(1));
        }
        f=(TF1*)_fdatabase->Get("f_F2Edep_Erg");
        if(f==NULL){
            warn_messages+=" f_F2Edep_Erg";
            has_init=-1;
        }
        else{
            f_F2Edep_Erg->SetParameter(0,f->GetParameter(0));
            f_F2Edep_Erg->SetParameter(1,f->GetParameter(1));
        }
        hdummy2=(TH2D*)_fdatabase->Get("hpdfele_Chi2_F2Edep");
        if(hdummy2==NULL){
            warn_messages+=" hpdfele_Chi2_F2Edep";
            has_init=-1;
        }
        else{
            for(int i1=1;i1<=hdummy2->GetNbinsX();i1++){
                for(int i2=1;i2<=hdummy2->GetNbinsY();i2++){
                    hpdfele_Chi2_F2Edep->SetBinContent(i1,i2,hdummy2->GetBinContent(i1,i2));
                }
            }
        }
        hdummy2=(TH2D*)_fdatabase->Get("hpdfpro_Chi2_F2Edep");
        if(hdummy2==NULL){
            warn_messages+=" hpdfpro_Chi2_F2Edep";
            has_init=-1;
        }
        else{
            for(int i1=1;i1<=hdummy2->GetNbinsX();i1++){
                for(int i2=1;i2<=hdummy2->GetNbinsY();i2++){
                    hpdfpro_Chi2_F2Edep->SetBinContent(i1,i2,hdummy2->GetBinContent(i1,i2));
                }
            }
        }
        hdummy2=(TH2D*)_fdatabase->Get("hpdfele_Chi2_plus_BDT");
        if(hdummy2==NULL){
            warn_messages+=" hpdfele_Chi2_plus_BDT";
            has_init=-1;
        }
        else{
            for(int i1=1;i1<=hdummy2->GetNbinsX();i1++){
                for(int i2=1;i2<=hdummy2->GetNbinsY();i2++){
                    hpdfele_Chi2plus_BDT->SetBinContent(i1,i2,hdummy2->GetBinContent(i1,i2));
                }
            }
        }
        hdummy2=(TH2D*)_fdatabase->Get("hpdfpro_Chi2_plus_BDT");
        if(hdummy2==NULL){
            warn_messages+=" hpdfpro_Chi2_plus_BDT";
            has_init=-1;
        }
        else{
            for(int i1=1;i1<=hdummy2->GetNbinsX();i1++){
                for(int i2=1;i2<=hdummy2->GetNbinsY();i2++){
                    hpdfpro_Chi2plus_BDT->SetBinContent(i1,i2,hdummy2->GetBinContent(i1,i2));
                }
            }
        }
        hdummyp=(TProfile*)_fdatabase->Get("profele_Chi2_Erg");
        if(hdummyp==NULL){
            warn_messages+=" profele_Chi2_Erg";
            has_init=-1;
        }
        else{
            for(int i1=1;i1<=hdummyp->GetNbinsX();i1++){
                hprofele_Chi2_Erg_rms->SetBinContent(i1,hdummyp->GetBinError(i1));
            }
        }
        hdummyp=(TProfile*)_fdatabase->Get("profele_F2Edep_Erg");
        if(hdummyp==NULL){
            warn_messages+=" profele_F2Edep_Erg";
            has_init=-1;
        }
        else{
            for(int i1=1;i1<=hdummyp->GetNbinsX();i1++){
                hprofele_F2Edep_Erg_rms->SetBinContent(i1,hdummyp->GetBinError(i1));
            }
        }
    }
    if(Version==2){
        TH2D* hdummy2;
        TF1* f	;
        f=(TF1*)_fdatabase->Get("f_Chi2_Erg");
        if(f==NULL){
            warn_messages+=" f_Chi2_Erg";
            has_init=-1;
        }
        else{
            for(int i1=0;i1<4;i1++)
                f_Chi2_Erg->SetParameter(i1,f->GetParameter(i1));
        }
        f=(TF1*)_fdatabase->Get("f_F2Edep_Erg");
        if(f==NULL){
            warn_messages+=" f_F2Edep_Erg";
            has_init=-1;
        }
        else{
            for(int i1=0;i1<4;i1++)
                f_F2Edep_Erg->SetParameter(i1,f->GetParameter(i1));
        }
        f=(TF1*)_fdatabase->Get("f_F2Edep_Erg_Err");
        if(f==NULL){
            warn_messages+=" f_F2Edep_Erg_Err";
            has_init=-1;
        }
        else{
            for(int i1=0;i1<4;i1++)
                f_F2Edep_Erg_RMS->SetParameter(i1,f->GetParameter(i1));
        }
        hdummy2=(TH2D*)_fdatabase->Get("hpdfele_Chi2_F2Edep");
        if(hdummy2==NULL){
            warn_messages+=" hpdfele_Chi2_F2Edep";
            has_init=-1;
        }
        else{
            for(int i1=1;i1<=hdummy2->GetNbinsX();i1++){
                for(int i2=1;i2<=hdummy2->GetNbinsY();i2++){
                    hpdfele_Chi2_F2Edep->SetBinContent(i1,i2,hdummy2->GetBinContent(i1,i2));
                }
            }
        }
        hdummy2=(TH2D*)_fdatabase->Get("hpdfpro_Chi2_F2Edep");
        if(hdummy2==NULL){
            warn_messages+=" hpdfpro_Chi2_F2Edep";
            has_init=-1;
        }
        else{
            for(int i1=1;i1<=hdummy2->GetNbinsX();i1++){
                for(int i2=1;i2<=hdummy2->GetNbinsY();i2++){
                    hpdfpro_Chi2_F2Edep->SetBinContent(i1,i2,hdummy2->GetBinContent(i1,i2));
                }
            }
        }
        hdummy2=(TH2D*)_fdatabase->Get("hpdfele_Chi2_plus_BDT");
        if(hdummy2==NULL){
            warn_messages+=" hpdfele_Chi2_plus_BDT";
            has_init=-1;
        }
        else{
            for(int i1=1;i1<=hdummy2->GetNbinsX();i1++){
                for(int i2=1;i2<=hdummy2->GetNbinsY();i2++){
                    hpdfele_Chi2plus_BDT->SetBinContent(i1,i2,hdummy2->GetBinContent(i1,i2));
                }
            }
        }
        hdummy2=(TH2D*)_fdatabase->Get("hpdfpro_Chi2_plus_BDT");
        if(hdummy2==NULL){
            warn_messages+=" hpdfpro_Chi2_plus_BDT";
            has_init=-1;
        }
        else{
            for(int i1=1;i1<=hdummy2->GetNbinsX();i1++){
                for(int i2=1;i2<=hdummy2->GetNbinsY();i2++)
                    hpdfpro_Chi2plus_BDT->SetBinContent(i1,i2,hdummy2->GetBinContent(i1,i2));
            }
        }
    }
    if(has_init==1)
        cout<<"EcalPDF::init-EcalPDF init OK! DataBase file is "<<fdatabase<<", Version is "<<Version<<" has_init= "<<has_init<<endl;
    else
        cout<<"EcalPDF::init-EcalPDF init BAD! Warn message "<<warn_messages<<" in "<<fdatabase<<", Version is "<<Version<<endl;
    _fdatabase->Close();
    //cout<<hpdfele_Chi2_F2Edep<<", "<<hpdfele_Chi2_F2Edep->GetXaxis()->GetNbins()<<", "<<hpdfpro_Chi2_F2Edep<<", "<<hpdfpro_Chi2_F2Edep->GetXaxis()->GetNbins()<<endl;
    return has_init;
}
EcalPDF::~EcalPDF(){
   //for(int i1=0;i1<pdf_names.size();i1++)
   //    gDirectory->Delete(pdf_names[i1].c_str());
   //cout<<"Call EcalPDF::~EcalPDF()"<<endl;
   //exit(0);
}
float EcalPDF::get_mean(int flayer,float coo,float Erg,int type){
    if(has_init==-1){
        cerr<<"EcalPDF::get_mean-E-EcalPDF has not been loaded! "<<has_init<<" Version= "<<Version<<endl;
        return -1;
    }
    if(flayer<0||flayer>17)
        return -1;
    float ret,ret0,ret1 ;
    int binx0,binx1     ;
    float x,x0,x1       ;
    float param_lf[6]   ;
    switch(type){
    case 0:
        x=log(Erg);
        binx0=param_mean_lf[flayer][0]->FindBin(coo);
        x0=param_mean_lf[flayer][0]->GetBinCenter(binx0);
        if(coo<x0){
            x1=param_mean_lf[flayer][0]->GetBinCenter(binx0-1);
            binx1=binx0-1;
            for(int i2=0;i2<6;i2++){
                param_lf[i2]=param_mean_lf[flayer][i2]->GetBinContent(binx0);
            }
            ret0=myfunc_lf(x,param_lf,0);
            for(int i2=0;i2<6;i2++){
                param_lf[i2]=param_mean_lf[flayer][i2]->GetBinContent(binx1);
            }
            ret1=myfunc_lf(x,param_lf,0);
            ret=ret0*(coo-x1)/(x0-x1)+ret1*(x0-coo)/(x0-x1);
        }
        else{
            x1=param_mean_lf[flayer][0]->GetBinCenter(binx0+1);
            binx1=binx0+1;
            for(int i2=0;i2<6;i2++){
                param_lf[i2]=param_mean_lf[flayer][i2]->GetBinContent(binx0);
            }
            ret0=myfunc_lf(x,param_lf,0);
            for(int i2=0;i2<6;i2++){
                param_lf[i2]=param_mean_lf[flayer][i2]->GetBinContent(binx1);
            }
            ret1=myfunc_lf(x,param_lf,0);
            ret=ret0*(x1-coo)/(x1-x0)+ret1*(coo-x0)/(x1-x0);
        }
        break;
    }
    return ret;
}

float EcalPDF::get_rms (int flayer,float coo,float Erg,int type){
    if(has_init==-1){
        cout<<"EcalPDF::get_rms-E-EcalPDF has not been loaded!"<<endl;
        return -1;
    }
    float ret,ret0,ret1 ;
    int binx0,binx1     ;
    float x,x0,x1       ;
    float param_lf[6]   ;
    if(flayer<0||flayer>17)
        return -1;
    switch(type){
    case 0:
        x=log(Erg);
        binx0=param_rms_lf[flayer][0]->FindBin(coo);
        x0=param_rms_lf[flayer][0]->GetBinCenter(binx0);
        if(coo<x0){
            x1=param_rms_lf[flayer][0]->GetBinCenter(binx0-1);
            binx1=binx0-1;
            for(int i2=0;i2<6;i2++){
                param_lf[i2]=param_rms_lf[flayer][i2]->GetBinContent(binx0);
            }
            ret0=myfunc_lf(x,param_lf,0);
            for(int i2=0;i2<6;i2++){
                param_lf[i2]=param_rms_lf[flayer][i2]->GetBinContent(binx1);
            }
            ret1=myfunc_lf(x,param_lf,0);
            ret=ret0*(coo-x1)/(x0-x1)+ret1*(x0-coo)/(x0-x1);
        }
        else{
            x1=param_rms_lf[flayer][0]->GetBinCenter(binx0+1);
            binx1=binx0+1;
            for(int i2=0;i2<6;i2++){
                param_lf[i2]=param_rms_lf[flayer][i2]->GetBinContent(binx0);
            }
            ret0=myfunc_lf(x,param_lf,0);
            for(int i2=0;i2<6;i2++){
                param_lf[i2]=param_rms_lf[flayer][i2]->GetBinContent(binx1);
            }
            ret1=myfunc_lf(x,param_lf,0);
            ret=ret0*(x1-coo)/(x1-x0)+ret1*(coo-x0)/(x1-x0);
        }
        break;
    }
    return ret;
}

float EcalPDF::get_prob(int flayer,float coo,float Erg,int type){
    if(has_init==-1){
        cout<<"EcalPDF::get_prob-E-EcalPDF has not been loaded!"<<endl;
        return -1;
    }
    if(flayer<0||flayer>17)
        return -1;
    float ret,ret0,ret1 ;
    int binx0,binx1     ;
    float x,x0,x1       ;
    float param_lf[2]   ;
    switch(type){
    case 0:
        x=log(Erg);
        binx0=param_prob_lf[flayer][0]->FindBin(coo);
        x0=param_prob_lf[flayer][0]->GetBinCenter(binx0);
        if(coo<x0){
            x1=param_prob_lf[flayer][0]->GetBinCenter(binx0-1);
            binx1=binx0-1;
            for(int i2=0;i2<2;i2++){
                param_lf[i2]=param_prob_lf[flayer][i2]->GetBinContent(binx0);
            }
            ret0=myfunc_lf(x,param_lf,1);
            for(int i2=0;i2<2;i2++){
                param_lf[i2]=param_prob_lf[flayer][i2]->GetBinContent(binx1);
            }
            ret1=myfunc_lf(x,param_lf,1);
            ret=ret0*(coo-x1)/(x0-x1)+ret1*(x0-coo)/(x0-x1);
        }
        else{
            x1=param_prob_lf[flayer][0]->GetBinCenter(binx0+1);
            binx1=binx0+1;
            for(int i2=0;i2<2;i2++){
                param_lf[i2]=param_prob_lf[flayer][i2]->GetBinContent(binx0);
            }
            ret0=myfunc_lf(x,param_lf,1);
            for(int i2=0;i2<2;i2++){
                param_lf[i2]=param_prob_lf[flayer][i2]->GetBinContent(binx1);
            }
            ret1=myfunc_lf(x,param_lf,1);
            ret=ret0*(x1-coo)/(x1-x0)+ret1*(coo-x0)/(x1-x0);
        }
        break;
    case 1:
        x=log(Erg);
        binx0=param_prob_lf[flayer][0]->FindBin(coo);
        x0=param_prob_lf[flayer][0]->GetBinCenter(binx0);
        if(coo<x0){
            x1=param_prob_lf[flayer][0]->GetBinCenter(binx0-1);
            binx1=binx0-1;
            for(int i2=0;i2<2;i2++){
                param_lf[i2]=param_prob_lf[flayer][i2]->GetBinContent(binx0);
            }
            ret0=myfunc_lf(x,param_lf,2);
            for(int i2=0;i2<2;i2++){
                param_lf[i2]=param_prob_lf[flayer][i2]->GetBinContent(binx1);
            }
            ret1=myfunc_lf(x,param_lf,2);
            ret=ret0*(coo-x1)/(x0-x1)+ret1*(x0-coo)/(x0-x1);
        }
        else{
            x1=param_prob_lf[flayer][0]->GetBinCenter(binx0+1);
            binx1=binx0+1;
            for(int i2=0;i2<2;i2++){
                param_lf[i2]=param_prob_lf[flayer][i2]->GetBinContent(binx0);
            }
            ret0=myfunc_lf(x,param_lf,2);
            for(int i2=0;i2<2;i2++){
                param_lf[i2]=param_prob_lf[flayer][i2]->GetBinContent(binx1);
            }
            ret1=myfunc_lf(x,param_lf,2);
            ret=ret0*(x1-coo)/(x1-x0)+ret1*(coo-x0)/(x1-x0);
        }
        break;
    }
    return ret;
}
double EcalPDF::myfunc_lf(float x,float* par,int type){
    double ret;
    if(x<=0)
        return -1;
    if(x<log(5))
        x=log(5);
    if(x>log(570))
        x=log(570);
    if(type==0){
        double a0,a1,a2,a3,a4,a5;
        a0=par[0];a1=par[1];a2=par[2];
        a3=par[3];a4=par[4];a5=par[5];
        ret=a0+a1*x+a2*x*x+a3*x*x*x+a4*x*x*x*x+a5*x*x*x*x*x;
        if(ret>1)
            ret=1.0;
        else if(ret<0.0)
            ret=0.0;
    }
    else if(type==1){
        double a0,a1,x0;
        a0=par[0];
        a1=par[1];
        if(a1==0)
            x0=1.5;
        else
            x0=(log(11.)-a0)/a1;
        ret=11.0;
        if(x<x0){
            if(a0+a1*x>10)
                ret=11.0;
            else if(a0+a1*x<-10)
                ret=0.0;
            else
                ret=exp(a0+a1*x);
        }
        if(ret>=11.0)
            ret=1.0;
        else
            ret=1.0-exp(-ret);

    }
    else if (type==2){
        double a0,a1,x0;
        a0=par[0];
        a1=par[1];
        if(a1==0)
            x0=1.5;
        else
            x0=(log(11.)-a0)/a1;
        ret=11.0;
        if(x<x0){
            if(a0+a1*x>10)
                ret=11.0;
            else if(a0+a1*x<-10)
                ret=0.0;
            else
                ret=exp(a0+a1*x);
        }
        if(ret>=11.0)
            ret=11.0;
    }
    return ret;
}
float EcalPDF::normalize_chi2(float _chi2, float _EnergyE,int algorithm){
    float ret;
    if(algorithm!=2){
        cout<<"EcalPDF-Warn:Currently only normalization of chi2 from lateral fit is provided!"<<endl;
        return _chi2;
    }
    if(_EnergyE<=15)
        _EnergyE=15.01	;
    if(Version==1&&_EnergyE>=200.)
        _EnergyE=199.99	;
    if((Version==2)&&_EnergyE>=400.)
        _EnergyE=399.99	;
    if(has_init==-1){
        cout<<warn_messages<<endl;
        return _chi2;
    }
    if(Version==1)
        ret=(_chi2-f_Chi2_Erg->Eval(log10(_EnergyE)))/hprofele_Chi2_Erg_rms->GetBinContent(hprofele_Chi2_Erg_rms->FindBin(log10(_EnergyE)));
    if(Version==2)
        ret=_chi2-f_Chi2_Erg->Eval(log10(_EnergyE));
    return ret;
}

float EcalPDF::normalize_f2edep(float _f2edep, float _EnergyE){
    float ret;
    if(_EnergyE<=15)
        _EnergyE=15.01	;
    if(Version==1&&_EnergyE>=200.)
        _EnergyE=199.99	;
    if((Version==2)&&_EnergyE>=400.)
        _EnergyE=399.99	;
    if(has_init==-1){
        cout<<warn_messages<<endl;
        return _f2edep;
    }
    if(Version==1)
        ret=(_f2edep-f_F2Edep_Erg->Eval(log10(_EnergyE)))/hprofele_F2Edep_Erg_rms->GetBinContent(hprofele_F2Edep_Erg_rms->FindBin(log10(_EnergyE)));
    if(Version==2)
        ret=(_f2edep-f_F2Edep_Erg->Eval(log10(_EnergyE)))/f_F2Edep_Erg_RMS->Eval(log10(_EnergyE));
    return ret;
}
float EcalPDF::get_chi2plus(float _chi2, float _f2edep, bool _has_normalized, float _EnergyE){
    float ret;
    double psig,pbkg;
    double range=2.,factor=2.;
    //cout<<"EcalPDF::get_chi2plus _chi2= "<<_chi2<<", _f2edep= "<<_f2edep<<", _has_normalized= "<<_has_normalized<<", _EnergyE= "<<_EnergyE<<endl;
    if(!_has_normalized){
        if(_EnergyE==0){
            cout<<"EcalPDF-Warn: Warn you need give the EnergyE to get chi2plus!"<<endl;
            return _chi2;
        }
        _chi2	=normalize_chi2(_chi2,_EnergyE)		;
        _f2edep	=normalize_f2edep(_f2edep,_EnergyE)	;
    }
    if(has_init==-1){
        cout<<warn_messages<<endl;
        return _chi2;
    }
    if(fabs(_chi2)>5.99||fabs(_f2edep)>5.99)
        ret=0;
    else{
        //cout<<hpdfele_Chi2_F2Edep<<", "<<hpdfele_Chi2_F2Edep->GetXaxis()->GetNbins()<<", "<<hpdfpro_Chi2_F2Edep<<", "<<hpdfpro_Chi2_F2Edep->GetXaxis()->GetNbins()<<endl;
        psig=hpdfele_Chi2_F2Edep->Interpolate(_chi2,_f2edep);
        pbkg=hpdfpro_Chi2_F2Edep->Interpolate(_chi2,_f2edep);
        //cout<<"EcalPDF::get_chi2plus psig= "<<psig<<", pbkg= "<<pbkg<<endl;
        if(psig>0)
            ret=1./(1.0+pow(pbkg/psig,factor));
        else if (pbkg>0.)
            ret=0.;
        else
            ret=nns(hpdfele_Chi2_F2Edep, hpdfpro_Chi2_F2Edep ,range,_chi2,_f2edep,factor);
    }
    return ret;
}
float EcalPDF::get_elik(float _chi2plus, float _bdt){
    float ret;
    double psig,pbkg;
    double range=.7,factor=2.;
    if(has_init==-1){
        cout<<warn_messages<<endl;
        return _chi2plus;
    }
    psig=hpdfele_Chi2plus_BDT->Interpolate(_chi2plus,_bdt);
    pbkg=hpdfpro_Chi2plus_BDT->Interpolate(_chi2plus,_bdt);
    if(psig>0)
        ret=1./(1.0+pow(pbkg/psig,factor));
    else if (pbkg>0.)
        ret=0.;
    else
        ret=nns(hpdfele_Chi2plus_BDT, hpdfpro_Chi2plus_BDT,range,_chi2plus,_bdt,factor);
    return ret;
}
double EcalPDF::nns(TH2D* hsig,TH2D* hbkg,double range,double x,double y,double order){
    double xmin=x-range>hsig->GetXaxis()->GetXmin()?x-range:hsig->GetXaxis()->GetXmin();
    double xmax=x+range<hsig->GetXaxis()->GetXmax()?x+range:hsig->GetXaxis()->GetXmax();
    double ymin=y-range>hsig->GetYaxis()->GetXmin()?y-range:hsig->GetYaxis()->GetXmin();
    double ymax=y+range<hsig->GetYaxis()->GetXmax()?y+range:hsig->GetYaxis()->GetXmax();
    double psig=0.,pbkg=0.,h=0.1;
    for(double rx=xmin;rx<xmax;rx+=h){
        for(double ry=ymin;ry<ymax;ry+=h){
            psig+=hsig->Interpolate(rx,ry);
            pbkg+=hbkg->Interpolate(rx,ry);
        }
    }
    //cout<<"EcalPDF::nns psig= "<<psig<<", pbkg= "<<pbkg<<endl;
    if(psig>0.0)
        return 1./(1.+pow(pbkg/psig,order));
    return 0.;
}
//End of Ecal PDF Class
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Class Ecal Chi2
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int EcalChi2::Version=2         ;
EcalChi2::EcalChi2(int ftype){
    char* amsdatadir=getenv("AMSDataDir");
    string tempname;
    if(!amsdatadir)
        amsdatadir="/afs/cern.ch/ams/Offline/AMSDataDir/";
    tempname=amsdatadir;
    if(Version==1)
        tempname+="/v5.00/EcalChi2CY_prof.20121106.1.root";
    else if(Version==2)
	tempname+="/v5.00/EcalChi2CY_prof.20130218.1.root";
    init(tempname.c_str(),ftype);
}
EcalChi2::EcalChi2(const char* fdatabase,int ftype){
    init(fdatabase,ftype);
}
EcalChi2::~EcalChi2(){
    if(ecalpdf){
        delete ecalpdf;
        ecalpdf=NULL;
    }
}
void EcalChi2::init(const char* database,int _type){
    EcalPDF::Version=EcalChi2::Version;
    ecalpdf=new EcalPDF(database);
    float _shiftxy_iss[]=   {0.049183,0.0547541,-0.127118,-0.129285,0.0567233,0.0589902,-0.125686,-0.12495,0.0588771,0.0580914,-0.13321,-0.135947,0.0553363,0.0543928,-0.132608,-0.130619,0.0509601,0.0504522};
    float _shiftxy_bt []=   {0.0649878,  0.0674102,    -0.18013,    -0.176749,   0.0684572,   0.0710825,   -0.175532,   -0.173065,   0.0631457,   0.0679332,   -0.172697,   -0.175777,   0.0636235,   0.0681259,  -0.166372,  -0.166112,  0.0657156,   0.0491425};
    float _shiftxy_sim[]=   {-0.036077,-0.0347022,-0.0483337,-0.0491348,0.0543024,0.0555286,-0.0786445,-0.0791036,0.00598481,0.00624232,-0.0977129,-0.0980029,0.0924151,0.0922796,-0.265521,-0.264891,0.131682,0.131873};
    float _ecalz[]      =   {-142.792,-143.658,-144.643,-145.508,-146.493,-147.357,-148.342,-149.208,-150.193,-151.058,-152.042,-152.908,-153.893,-154.758,-155.743,-156.607,-157.592,-158.458};
    float _ecalz_new[]  =   {-143.195,-144.115,-145.045,-145.965,-146.895,-147.815,-148.745,-149.665,-150.595,-151.515,-152.445,-153.365,-154.295,-155.215,-156.145,-157.065,-157.995,-158.915};
    float _shiftz_iss[] =   {-0.796973,-0.72069,-0.613334,-0.590829,-0.596496,-0.575594,-0.468351,-0.445498,-0.466901,-0.452407,-0.351328,-0.342198,-0.289828,-0.297303,-0.178367,-0.171907,-0.132327,-0.0401191};
    float _shiftz_sim[] =   {-0.399355,-0.275355,-0.207417,-0.163975,-0.140999,-0.103207,-0.0851771,-0.063238,-0.0402442,-0.02793,-0.00142254,0.0105943,0.0404541,0.0559551,0.0963537,0.122534,0.154541,0.22572 };

    for(int i1=0;i1<18;i1++){
        //ISS
        if(_type==0){
            shiftxy[i1]=-31.95-_shiftxy_iss[i1];
            ecalz0[i1]=_ecalz[i1];
            shiftz[i1]=_shiftz_iss[i1];
        }
        //Test Beam
        if(_type==1){
            shiftxy[i1]=-31.95-_shiftxy_bt [i1];
            ecalz0[i1]=_ecalz_new[i1];
            shiftz[i1]=0.        ;
        }
        //Simulation
        if(_type==2){
            shiftxy[i1]=-31.95-_shiftxy_sim[i1];
            ecalz0[i1]=_ecalz[i1];
            shiftz[i1]=_shiftz_sim[i1];
        }
    }
    for(int i1=0;i1<18;i1++)
        ecalz[i1]=ecalz0[i1]+shiftz[i1];

    for(int i1=0;i1<18;i1++){
        for(int i2=0;i2<72;i2++){
            fdead_cell[i1][i2]=0;
        }
    }
    //ISS
    if(_type==0){
        fdead_cell[14][65]=1;
        fdead_cell[6 ][16]=1;
        fdead_cell[6 ][17]=1;
        fdead_cell[7 ][16]=1;
        fdead_cell[7 ][17]=1;
        fdead_cell[6 ][50]=1;
    }
    //Test Beam

    //Simualtion
    if(_type==0){
        fdead_cell[14][65]=1;
    }
}
float EcalChi2::process(float* coo,float sign){
    if(coo==NULL)
        return -1;
    for(int i1=0;i1<18;i1++){
        pos[i1]=coo[i1];
    }
    float chi2,chi22,chi23,chi24;
    int   ndof;
    for(int i1=0;i1<18;i1++){
        int bin=(int)((coo[i1]-shiftxy[i1]+0.45)/SIZE);
        ndof             =cal_chi2(bin-20,bin+20,i1,coo[i1],chi2,chi22,chi23,chi24,sign);
        _ndofs[i1]       =ndof;
        _chi2_layer [i1] =chi2;
        _chi22_layer[i1] =chi22;
        _chi23_layer[i1] =chi23;
        _chi24_layer[i1] =chi24;
    }
    _chi2    =0.;
    _chi2x   =0.;
    _chi2y   =0.;
    _variance=0.;
    _skewness=0.;
    _kurtosis=0.;
    tot_ndof =0;
    tot_ndofx=0;
    tot_ndofy=0;
    for(int i1=0;i1<18;i1++){
        _chi2     +=_chi2_layer [i1];
        if(i1%4<2){
            _chi2y    +=_chi2_layer [i1];
            tot_ndofy +=_ndofs[i1];
        }
        else{
            _chi2x    +=_chi2_layer [i1];
            tot_ndofx +=_ndofs[i1];
        }
        tot_ndof  +=_ndofs[i1];
        _variance +=_chi22_layer[i1];
        _skewness +=_chi23_layer[i1];
        _kurtosis +=_chi24_layer[i1];
    }
    if(tot_ndof>0){
        _chi2  /=tot_ndof;
        if(tot_ndofy>0)
            _chi2y/=tot_ndofy;
        else
            _chi2y=50.;
        if(tot_ndofx>0)
            _chi2x/=tot_ndofx;
        else
            _chi2x=50.;
        _variance/=tot_ndof;
        _skewness/=tot_ndof;
        _kurtosis/=tot_ndof;

        _variance=sqrt(_variance-_chi2*_chi2);
        _skewness=(_skewness-3.*_chi2*_variance*_variance-_chi2*_chi2*_chi2)/(_variance*_variance*_variance);
        _kurtosis/=(_variance*_variance*_variance*_variance);
    }
    else{
        _chi2         =50.;
        _chi2x        =50.;
        _chi2y        =50.;
        _variance     =-1;
        _skewness     =-1;
        _kurtosis     =-1;
    }
    //cal_f2dep();
    return _chi2;
}
int   EcalChi2::set_edep(float* edeps, float erg,float erge){
    for(int i1=0;i1<18;i1++){
        for(int i2=0;i2<72;i2++){
            Edep_raw[i1*72+i2]=edeps[i1*72+i2];
        }
    }
    _erg=erg;
    _erge=erge;
    return 0;
}
float  EcalChi2::process(AMSEventR* ev, TrTrackR* trtrack, int iTrTrackPar){
#ifdef _PGTRACK_
    float sign=-1.;
    if(iTrTrackPar<0){
        cout<<"EcalChi2 Error : iTrTrackPar= "<<iTrTrackPar<<endl;
        return -1;
    }
    if(trtrack==NULL||ev==NULL){
        cout<<"EcalChi2 Error: trtrack= "<<trtrack<<" ev = "<<ev<<endl;
        return -1;
    }
    AMSPoint pnt;
    AMSDir   dir;
    if(trtrack->GetRigidity(iTrTrackPar)>0)
        sign=1.0;
    for(int i1=0;i1<18;i1++){
        trtrack->Interpolate(ecalz[i1],pnt,dir,iTrTrackPar);
        if(i1%4<2)
            pos[i1]=pnt.y();
        else
            pos[i1]=pnt.x();
    }
    for(int i1=0;i1<18;i1++){
        for(int i2=0;i2<72;i2++){
            Edep_raw[i1*72+i2]=0.;
        }
    }
    float maxe=-1;
    int  nmax =-1;
    if(ev->nEcalShower()<1)
        return -1;
    for(int i1=0;i1<ev->nEcalShower();i1++){
        if(ev->pEcalShower(i1)->EnergyE>maxe){
            maxe =ev->pEcalShower(i1)->EnergyE;
            nmax =i1                          ;
        }
    }
    _erg=ev->pEcalShower(nmax)->EnergyD/1000.;
    for(int i1=0;i1<ev->NEcalHit();i1++){
        Edep_raw[ev->pEcalHit(i1)->Plane*72+ev->pEcalHit(i1)->Cell]=ev->pEcalHit(i1)->Edep;
    }

    float chi2,chi22,chi23,chi24;
    int   ndof;
    for(int i1=0;i1<18;i1++){
        int bin=(int)((pos[i1]-shiftxy[i1]+0.45)/SIZE);
        ndof             =cal_chi2(bin-20,bin+20,i1,pos[i1],chi2,chi22,chi23,chi24,sign);
        _ndofs[i1]       =ndof;
        _chi2_layer [i1] =chi2;
        _chi22_layer[i1] =chi22;
        _chi23_layer[i1] =chi23;
        _chi24_layer[i1] =chi24;
    }
    _chi2    =0.;
    _chi2x   =0.;
    _chi2y   =0.;
    _variance=0.;
    _skewness=0.;
    _kurtosis=0.;
    tot_ndof =0 ;
    tot_ndofx=0 ;
    tot_ndofy=0 ;
    for(int i1=1;i1<18;i1++){
        _chi2     +=_chi2_layer [i1];
        if(i1%4<2){
            _chi2y    +=_chi2_layer [i1];
            tot_ndofy +=_ndofs[i1];
        }
        else{
            _chi2x    +=_chi2_layer [i1];
            tot_ndofx +=_ndofs[i1];
        }
        tot_ndof  +=_ndofs[i1];
        _variance +=_chi22_layer[i1];
        _skewness +=_chi23_layer[i1];
        _kurtosis +=_chi24_layer[i1];
    }
    if(tot_ndof>0){
        _chi2  /=tot_ndof;
        if(tot_ndofy>0)
            _chi2y/=tot_ndofy;
        else
            _chi2y=50.;
        if(tot_ndofx>0)
            _chi2x/=tot_ndofx;
        else
            _chi2x=50.;
        _variance/=tot_ndof;
        _skewness/=tot_ndof;
        _kurtosis/=tot_ndof;

        _variance=sqrt(_variance-_chi2*_chi2);
        _skewness=(_skewness-3.*_chi2*_variance*_variance-_chi2*_chi2*_chi2)/(_variance*_variance*_variance);
        _kurtosis/=(_variance*_variance*_variance*_variance);
    }
    else{
        _chi2         =50.;
        _chi2x        =50.;
        _chi2y        =50.;
        _variance     =-1;
        _skewness     =-1;
        _kurtosis     =-1;
    }
    //cal_f2dep();

    return _chi2;
#else
    return -1;
#endif

}
float EcalChi2::process(TrTrackR*  trtrack, EcalShowerR* esh, int iTrTrackPar){
#ifdef _PGTRACK_
    float sign=-1.;
    if(iTrTrackPar<0){
        cout<<"Error : iTrTrackPar= "<<iTrTrackPar<<endl;
        return -1;
    }
    if(trtrack==NULL||esh==NULL){
        cout<<"Ecal Chi2  Error: trtrack= "<<trtrack<<" esh = "<<esh<<endl;
        return -1;
    }
    AMSPoint pnt;
    AMSDir	 dir;
    if(trtrack->GetRigidity(iTrTrackPar)>0)
        sign=1.0;
    for(int i1=0;i1<18;i1++){
        trtrack->Interpolate(ecalz[i1],pnt,dir,iTrTrackPar);
        if(i1%4<2)
            pos[i1]=pnt.y();
        else
            pos[i1]=pnt.x();
    }
    for(int i1=0;i1<18;i1++){
        for(int i2=0;i2<72;i2++){
            Edep_raw[i1*72+i2]=0.;
        }
    }
    _erg=esh->EnergyD/1000.;
    for(int i1=0;i1<esh->NEcal2DCluster();i1++){
        Ecal2DClusterR * ecal2d=esh->pEcal2DCluster(i1);
        for(int i2=0;i2<ecal2d->NEcalCluster();i2++){
            EcalClusterR* ecalclt=ecal2d->pEcalCluster(i2);
            for(int i3=0;i3<ecalclt->NEcalHit();i3++){
                Edep_raw[ecalclt->pEcalHit(i3)->Plane*72+ecalclt->pEcalHit(i3)->Cell]=ecalclt->pEcalHit(i3)->Edep;
            }
        }
    }

    float chi2,chi22,chi23,chi24;
    int   ndof;
    for(int i1=0;i1<18;i1++){
        int bin=(int)((pos[i1]-shiftxy[i1]+0.45)/SIZE);
        ndof             =cal_chi2(bin-20,bin+20,i1,pos[i1],chi2,chi22,chi23,chi24,sign);
        _ndofs[i1]       =ndof;
        _chi2_layer [i1] =chi2;
        _chi22_layer[i1] =chi22;
        _chi23_layer[i1] =chi23;
        _chi24_layer[i1] =chi24;
    }
    _chi2    =0.;
    _chi2x   =0.;
    _chi2y   =0.;
    _variance=0.;
    _skewness=0.;
    _kurtosis=0.;
    tot_ndof =0;
    tot_ndofx=0;
    tot_ndofy=0;
    for(int i1=0;i1<18;i1++){
        _chi2     +=_chi2_layer [i1];
        if(i1%4<2){
            _chi2y    +=_chi2_layer [i1];
            tot_ndofy +=_ndofs[i1];
        }
        else{
            _chi2x    +=_chi2_layer [i1];
            tot_ndofx +=_ndofs[i1];
        }
        tot_ndof  +=_ndofs[i1];
        _variance +=_chi22_layer[i1];
        _skewness +=_chi23_layer[i1];
        _kurtosis +=_chi24_layer[i1];
    }
    if(tot_ndof>0){
        _chi2  /=tot_ndof;
        if(tot_ndofy>0)
            _chi2y/=tot_ndofy;
        else
            _chi2y=50.;
        if(tot_ndofx>0)
            _chi2x/=tot_ndofx;
        else
            _chi2x=50.;
        _variance/=tot_ndof;
        _skewness/=tot_ndof;
        _kurtosis/=tot_ndof;

        _variance=sqrt(_variance-_chi2*_chi2);
        _skewness=(_skewness-3.*_chi2*_variance*_variance-_chi2*_chi2*_chi2)/(_variance*_variance*_variance);
        _kurtosis/=(_variance*_variance*_variance*_variance);
    }
    else{
        _chi2         =50.;
        _chi2x        =50.;
        _chi2y        =50.;
        _variance     =-1;
        _skewness     =-1;
        _kurtosis     =-1;
    }
    //cal_f2dep();

    return _chi2;
#else
    return -1;
#endif
}
float EcalChi2::get_chi2(int ilayer){
    const double layerchi2_mean[18][4]= {{0.556226, -0.755122, 0.451302, -0.0634034 },
                                        {0.459205, -0.605575, 0.409162, -0.06698   },
                                        {0.544575, -0.767518, 0.529796, -0.0956603 },
                                        {0.310023, -0.30617, 0.260459, -0.0454493  },
                                        {0.234273, -0.0847922, 0.112312, -0.0150382},
                                        {0.12844, 0.145532, -0.0302078, 0.0145753  },
                                        {0.0832202, 0.23582, -0.0889377, 0.0289723  },
                                        {-0.164259, 0.708386, -0.376005, 0.0878599  },
                                        {-0.0524452, 0.595105, -0.329915, 0.0819065 },
                                        {-0.0852398, 0.649367, -0.353436, 0.084691  },
                                        {-0.238252, 0.849025, -0.433315, 0.0918773  },
                                        {0.0242082, 0.394811, -0.17313, 0.04205 },
                                        {0.00278083, 0.464479, -0.205462, 0.044993  },
                                        {-0.120806, 0.670635, -0.315587, 0.0633705  },
                                        {0.205496, 0.0640946, 0.0459995, -0.00734028},
                                        {0.442129, -0.367223, 0.304548, -0.0582486  },
                                        {0.563977, -0.544797, 0.393958, -0.0704523  },
                                        {0.792051, -1.00963, 0.685936, -0.125037    }};
    if(ilayer==-1)
        return _chi2;
    if(ilayer<-1||ilayer>17)
        return -1;
    if(Version==1){
        if(_ndofs[ilayer]==0)
            return -1;
        else
            return _chi2_layer[ilayer]/_ndofs[ilayer];
    }
    if(Version==2){
        if(_erge<15)
            _erge=15;
        if(_erge>400);
        _erge=400;
        if(_ndofs[ilayer]!=0){
            double x=log10(_erge);
            double layer_mean=layerchi2_mean[ilayer][0]+layerchi2_mean[ilayer][1]*x+layerchi2_mean[ilayer][2]*x*x+layerchi2_mean[ilayer][3]*x*x*x;
            return _chi2_layer[ilayer]/_ndofs[ilayer]-layer_mean+1.;
        }
        else return -1;
    }
    return -1;
}

float EcalChi2::cal_f2dep(){
    _f2dep=0.;
    for(int i1=0;i1<72;i1++){
        _f2dep+=Edep_raw[i1];
    }
    for(int i1=0;i1<72;i1++){
        _f2dep+=Edep_raw[72+i1];
    }
    return _f2dep;
}

int EcalChi2::cal_chi2(int start_cell,int end_cell,int layer,double coo,float& chi2,float& chi22,float& chi23,float& chi24,float sign){
    bool flag;
    int i1=0;
    double cell_mean[72],cell_rms[72], cell_prob[72],cell_probbar[72];
    double summ=0.,sumxm=0.;
    double chisq_prob,chisq_chi2,delta,chisq;
    int k1,k2;
    int count=0;
    if(start_cell<0) start_cell=0;
    if(start_cell>71) start_cell=71;
    if(end_cell<0) end_cell=0;
    if(end_cell>71) end_cell=71;
    /*
    k1=(int)((coo-shiftxy[layer]+0.45)/SIZE);
    k2=k1;
    flag=false;
    if(k1<start_cell)
        k1=start_cell;
    else{
        for(;k1>=start_cell;k1--){
            if(k1>0&&k1<72){
                if(Edep_raw[layer][k1-1]==0.0){
                    if(k1>1){
                        if(Edep_raw[layer][k1-2]==0.0)
                            flag=true;
                    }
                }
            }
            if(flag)
                break;
        }
    }
    flag=false;
    if(k2>end_cell)
        k2=end_cell;
    else{
        for(;k2<=end_cell;k2++){
            if(k2<71&&k2>-1){
                if(Edep_raw[layer][k2+1]==0.0){
                    if(k2<70){
                        if(Edep_raw[layer][k2+2]==0.0)
                            flag=true;
                    }
                }
            }
            if(flag)
                break;
        }
    }
    start_cell=k1;
    end_cell=k2;
    */
    for(i1=start_cell;i1<=end_cell;i1++){
        cell_mean[i1]=ecalpdf->get_mean(layer,-1.*sign*(i1*SIZE+shiftxy[layer]-coo),_erg,0);
        cell_rms[i1]=ecalpdf->get_rms(layer,-1.*sign*(i1*SIZE+shiftxy[layer]-coo),_erg,0);
        cell_prob[i1]=ecalpdf->get_prob(layer,-1.*sign*(i1*SIZE+shiftxy[layer]-coo),_erg,0);
        cell_probbar[i1]=ecalpdf->get_prob(layer,-1.*sign*(i1*SIZE+shiftxy[layer]-coo),_erg,1);
        if(fdead_cell[layer][i1]==0&&Edep_raw[layer*72+i1]>0){
            summ+=cell_mean[i1];
            sumxm+=Edep_raw[layer*72+i1];
            k1++;
        }
    }
    for(i1=start_cell;i1<=end_cell;i1++){
        if(cell_rms[i1]<=0.0){
            cell_rms[i1]+=ecalpdf->get_rms(layer,-1.*sign*(i1*SIZE+shiftxy[layer]-coo-0.18),_erg,0);
            cell_rms[i1]+=ecalpdf->get_rms(layer,-1.*sign*(i1*SIZE+shiftxy[layer]-coo-0.09),_erg,0);
            cell_rms[i1]+=ecalpdf->get_rms(layer,-1.*sign*(i1*SIZE+shiftxy[layer]-coo+0.09),_erg,0);
            cell_rms[i1]+=ecalpdf->get_rms(layer,-1.*sign*(i1*SIZE+shiftxy[layer]-coo+0.18),_erg,0);
            cell_rms[i1]/=4.0;
        }
    }
    count =0  ;
    chi2  =0. ;
    chi22 =0. ;
    chi23 =0. ;
    chi24 =0. ;
    for(i1=start_cell;i1<=end_cell;i1++){
        if(fdead_cell[layer][i1]==1)
            continue;
        count++;
        chisq_prob=0.;
        chisq_chi2=0.;
        if(Edep_raw[layer*72+i1]<=0){
            chisq_prob=cell_probbar[i1];
        }
        else{
            delta=(cell_mean[i1]-Edep_raw[layer*72+i1]*summ/sumxm);
            chisq_chi2=delta*delta/2.0/cell_rms[i1] ;
            chisq_prob=-1.*log(cell_prob[i1])	;
        }
        if(!(chisq_prob>=0.&&chisq_prob<50.))
            chisq_prob=50.;
        if(!(chisq_chi2>=0.&&chisq_chi2<50.))
            chisq_chi2=50.;
        //cout<<"+"<<cell_mean[i1]<<","<<Edep_raw[layer][i1]*summ/sumxm<<","<<cell_rms[i1]<<"; "<<cell_probbar[i1]<<", "<<-1.*log(cell_prob[i1])<<endl;
        chisq =chisq_chi2+chisq_prob;
        if(chisq>50.)
            chisq=50.;
        chi2 +=chisq;
        chi22+=chisq*chisq;
        chi23+=chisq*chisq*chisq;
        chi24+=chisq*chisq*chisq*chisq;
    }
    return count;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//End of Ecal Chi2
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Class Ecal Axis
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int EcalAxis::Version=2         ;
TVirtualFitter* EcalAxis::gMinuit_EcalAxis = NULL;
EcalAxis::EcalAxis(int ftype){
    char* amsdatadir=getenv("AMSDataDir");
    string tempname;
    if(!amsdatadir)
        amsdatadir="/afs/cern.ch/ams/Offline/AMSDataDir/";
    tempname=amsdatadir;
    if(Version==1)
        tempname+="/v5.00/EcalChi2CY_prof.20121106.1.root";
    else if(Version==2)
        tempname+="/v5.00/EcalChi2CY_prof.20130218.1.root";
    init(tempname.c_str(),ftype);
}
EcalAxis::EcalAxis(char* fdatabase, int ftype){
    init(fdatabase,ftype);
}
void EcalAxis::init(const char* fdatabase, int ftype){
    EcalChi2::Version=EcalAxis::Version;
    ecalchi2=new EcalChi2(fdatabase,ftype);
    ecalcr=new EcalCR(ftype,fdatabase);
    float _shiftxy_iss[]=   {0.049183,0.0547541,-0.127118,-0.129285,0.0567233,0.0589902,-0.125686,-0.12495,0.0588771,0.0580914,-0.13321,-0.135947,0.0553363,0.0543928,-0.132608,-0.130619,0.0509601,0.0504522};
    float _shiftxy_bt []=   {0.0649878,  0.0674102,    -0.18013,    -0.176749,   0.0684572,   0.0710825,   -0.175532,   -0.173065,   0.0631457,   0.0679332,   -0.172697,   -0.175777,   0.0636235,   0.0681259,  -0.166372,  -0.166112,  0.0657156,   0.0491425};
    float _shiftxy_sim[]=   {-0.036077,-0.0347022,-0.0483337,-0.0491348,0.0543024,0.0555286,-0.0786445,-0.0791036,0.00598481,0.00624232,-0.0977129,-0.0980029,0.0924151,0.0922796,-0.265521,-0.264891,0.131682,0.131873};
    float _ecalz[]      =   {-142.792,-143.658,-144.643,-145.508,-146.493,-147.357,-148.342,-149.208,-150.193,-151.058,-152.042,-152.908,-153.893,-154.758,-155.743,-156.607,-157.592,-158.458};
    float _ecalz_new[]  =   {-143.195,-144.115,-145.045,-145.965,-146.895,-147.815,-148.745,-149.665,-150.595,-151.515,-152.445,-153.365,-154.295,-155.215,-156.145,-157.065,-157.995,-158.915};
    float _shiftz_iss[] =   {-0.796973,-0.72069,-0.613334,-0.590829,-0.596496,-0.575594,-0.468351,-0.445498,-0.466901,-0.452407,-0.351328,-0.342198,-0.289828,-0.297303,-0.178367,-0.171907,-0.132327,-0.0401191};
    float _shiftz_sim[] =   {-0.399355,-0.275355,-0.207417,-0.163975,-0.140999,-0.103207,-0.0851771,-0.063238,-0.0402442,-0.02793,-0.00142254,0.0105943,0.0404541,0.0559551,0.0963537,0.122534,0.154541,0.22572 };
    _type=ftype;
    for(int i1=0;i1<18;i1++){
        //ISS
        if(_type==0){
            shiftxy[i1]=-31.95-_shiftxy_iss[i1];
            ecalz0[i1]=_ecalz[i1];
            shiftz[i1]=_shiftz_iss[i1];
        }
        //Test Beam
        if(_type==1){
            shiftxy[i1]=-31.95-_shiftxy_bt [i1];
            ecalz0[i1]=_ecalz[i1];
            shiftz[i1]=0.        ;
        }
        //Simulation
        if(_type==2){
            shiftxy[i1]=-31.95-_shiftxy_sim[i1];
            ecalz0[i1]=_ecalz_new[i1];
            shiftz[i1]=_shiftz_sim[i1];
        }
    }
    for(int i1=0;i1<18;i1++){
        for(int i2=0;i2<72;i2++){
            fdead_cell[i1][i2]=0;
        }
    }
    //ISS
    if(_type==0){
        fdead_cell[14][65]=1;
        fdead_cell[6 ][16]=1;
        fdead_cell[6 ][17]=1;
        fdead_cell[7 ][16]=1;
        fdead_cell[7 ][17]=1;
    }
    //Test Beam

    //Simualtion
    if(_type==2){
        fdead_cell[14][65]=1;
    }
}
EcalAxis::~EcalAxis(){
    if(ecalchi2)
        delete ecalchi2;
}
bool EcalAxis::init_lf(){
    TVirtualFitter::SetDefaultFitter("Minuit");
    gMinuit_EcalAxis=TVirtualFitter::Fitter(0,3);
    gMinuit_EcalAxis->SetFCN(fcn_EcalAxis_Chi2);
    gMinuit_EcalAxis->SetObjectFit(this);
    double arglist[10];
    int ret;
    arglist[0]=-1;
    gMinuit_EcalAxis->ExecuteCommand("SET PRINT", arglist, 1);
    gMinuit_EcalAxis->ExecuteCommand("SET NOW", arglist, 0);
    arglist[0]=0.1;
    gMinuit_EcalAxis->ExecuteCommand("SET ERR",arglist, 1);

    float init_x0=0;
    float init_y0=0;
    float init_dxdz=0;
    float init_dydz=0;
    if(use_ext==0){
        if((_status&8)!=8){
            if(init_cr2())
                _status+=8;
            else
                return false;
        }
        init_x0  =p0_cr2[0];
        init_y0  =p0_cr2[1];
        if(dir_cr2[2]==0.)
            dir_cr2[2]=1.;
        init_dxdz=dir_cr2[0]/dir_cr2[2];
        init_dydz=dir_cr2[1]/dir_cr2[2];
    }
    else{
        if(ext_d0[2]==0)
            ext_d0[2]=1.0;
        init_dxdz=ext_d0[0]/ext_d0[2];
        init_dydz=ext_d0[1]/ext_d0[2];
        init_x0  =ext_p0[0]+init_dxdz*(ecalz[8]-ext_p0[2]);
        init_y0  =ext_p0[1]+init_dydz*(ecalz[8]-ext_p0[2]);
    }
    gMinuit_EcalAxis->SetParameter(0,  "x0"  ,init_x0  ,0.2  ,init_x0-0.9 ,init_x0+0.9 );
    gMinuit_EcalAxis->SetParameter(1,  "y0"  ,init_y0  ,0.2  ,init_y0-0.9 ,init_y0+0.9 );
    gMinuit_EcalAxis->SetParameter(2,  "dxdz",init_dxdz,0.1   ,init_dxdz-.2,init_dxdz+.2);
    gMinuit_EcalAxis->SetParameter(3,  "dydz",init_dydz,0.1   ,init_dydz-.2,init_dydz+.2);

    if(_erg>5)
        arglist[0]=400;
    else
        arglist[0]=100 ;
    arglist[1]=10.;
    //Test before call Minuit
    int cflag=0 ;
    double p[4] ;
    double chi20;
    double chi21;
    srand(time(NULL));
    p[0]=init_x0  ;
    p[1]=init_y0  ;
    p[2]=init_dxdz;
    p[3]=init_dydz;
    chi20=GetChi2(p);
    if((Version==2&&EnergyE<15)||simple==1){
        p0_lf[0]=init_x0 ;
        p0_lf[1]=init_y0 ;
        p0_lf[2]=ecalz[8];
        dir_lf[0]=init_dxdz;
        dir_lf[1]=init_dydz;
        dir_lf[2]=1.0	   ;
        double r=sqrt(dir_lf[0]*dir_lf[0]+dir_lf[1]*dir_lf[1]+dir_lf[2]*dir_lf[2]);
        dir_lf[0]/=r;
        dir_lf[1]/=r;
        dir_lf[2]/=r;
	//reset simple to 0
	simple=0;
        return true;
    }
    for(int i1=0;i1<10;i1++){
        p[0]=init_x0+(rand()%200-100.)/200.*0.9  ;
        p[1]=init_y0+(rand()%200-100.)/200.*0.9	 ;
        p[2]=init_dxdz+(rand()%200-100.)/200.*0.4;
        p[3]=init_dydz+(rand()%200-100.)/200.*0.4;
        chi21=GetChi2(p);
        if(fabs(chi20-chi21)>0.1)
            cflag++;
    }
    if(cflag>0){
        ret=gMinuit_EcalAxis->ExecuteCommand("MINI", arglist, 2);
        //ret=gMinuit_EcalAxis->ExecuteCommand("SIMPLEX",0, 0);

        p0_lf[0]=gMinuit_EcalAxis->GetParameter(0);
        p0_lf[1]=gMinuit_EcalAxis->GetParameter(1);
        p0_lf[2]=ecalz[8];
        dir_lf[0]=gMinuit_EcalAxis->GetParameter(2);
        dir_lf[1]=gMinuit_EcalAxis->GetParameter(3);
        dir_lf[2]=1.0;
    }
    else{
        p0_lf[0]=init_x0 ;
        p0_lf[1]=init_y0 ;
        p0_lf[2]=ecalz[8];
        dir_lf[0]=init_dxdz;
        dir_lf[1]=init_dydz;
        dir_lf[2]=1.0	   ;
    }
    double r=sqrt(dir_lf[0]*dir_lf[0]+dir_lf[1]*dir_lf[1]+dir_lf[2]*dir_lf[2]);
    dir_lf[0]/=r;
    dir_lf[1]/=r;
    dir_lf[2]/=r;
    return true;
}

int EcalAxis::interpolate(float zpl       ,AMSPoint& p0   ,AMSDir  &dir, int algorithm){
    float x0,y0,dxdz,dydz;
    if((algorithm&4)==4){
        if((_status&4)!=4)
            return -1;
        x0    =p0_cg[0];
        y0    =p0_cg[1];
        dxdz  =dir_cg[0]/dir_cg[2];
        dydz  =dir_cg[1]/dir_cg[2];
        dir[0]=dir_cg[0];
        dir[1]=dir_cg[1];
        dir[2]=dir_cg[2];
    }
    else if((algorithm&1)==1){
        if((_status&1)!=1)
            return -1;
        x0    =p0_cr[0];
        y0    =p0_cr[1];
        dxdz  =dir_cr[0]/dir_cr[2];
        dydz  =dir_cr[1]/dir_cr[2];
        dir[0]=dir_cr[0];
        dir[1]=dir_cr[1];
        dir[2]=dir_cr[2];
    }
    else if((algorithm&2)==2){
        if((_status&2)!=2)
            return -1;
        x0    =p0_lf[0];
        y0    =p0_lf[1];
        dxdz  =dir_lf[0]/dir_lf[2];
        dydz  =dir_lf[1]/dir_lf[2];
        dir[0]=dir_lf[0];
        dir[1]=dir_lf[1];
        dir[2]=dir_lf[2];
    }
    else if((algorithm&8)==8){
	if((_status&8)!=8)
            return -1;
        x0    =p0_cr2[0];
        y0    =p0_cr2[1];
        dxdz  =dir_cr2[0]/dir_cr2[2];
        dydz  =dir_cr2[1]/dir_cr2[2];
        dir[0]=dir_cr2[0];
        dir[1]=dir_cr2[1];
        dir[2]=dir_cr2[2];
    }
    else if((algorithm&16)==16){
	x0    =ext_p0[0];
        y0    =ext_p0[1];
        dxdz  =ext_d0[0]/ext_d0[2];
        dydz  =ext_d0[1]/ext_d0[2];
        dir[0]=ext_d0[0];
        dir[1]=ext_d0[1];
        dir[2]=ext_d0[2];	
    }
    else
        return -2;
    p0[0]=x0+(zpl-ecalz[8])*dxdz;
    p0[1]=y0+(zpl-ecalz[8])*dydz;
    p0[2]=zpl;
    return 0;
}



void EcalAxis::fcn_EcalAxis_Chi2(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag){
    EcalAxis* ecalaxis=(EcalAxis*)gMinuit_EcalAxis->GetObjectFit();
    f=ecalaxis->GetChi2(par);
}
float EcalAxis::GetChi2(double* par){
    //par--->
    //0: x0  , 1: y0   @z=ecalz[8]
    //2: dxdz, 3: dydz z=1.
    float pos[18];
    for(int i1=0;i1<18;i1++){
        if(i1%4>1)
            pos[i1]=par[0]+(ecalz[i1]-ecalz[8])*par[2];
        else
            pos[i1]=par[1]+(ecalz[i1]-ecalz[8])*par[3];
    }
    float ret=ecalchi2->process(pos,_sign);
    return ret;
}
void EcalAxis::get_z(){
    for(int i1=0;i1<18;i1++){
        ecalz[i1]=ecalz0[i1]+shiftz[i1];
    }
}
int   EcalAxis::process(AMSEventR* ev, int algorithm, TrTrackR* trtrack){
#ifdef _PGTRACK_ 
    float fedep[1296];
    int   fcell[1296], fplane[1296],nEcalHits,ret;
    float EnergyD, _EnergyE, sign;
    nEcalHits=ev->nEcalHit()     ;
    for(int i1=0;i1<nEcalHits;i1++){
        fedep[i1] =ev->pEcalHit(i1)->Edep;
        fcell[i1] =ev->pEcalHit(i1)->Cell;
        fplane[i1]=ev->pEcalHit(i1)->Plane;
    }
    float maxe=-1;
    int  nmax =-1;
    if(ev->nEcalShower()<1)
        return -1;
    for(int i1=0;i1<ev->nEcalShower();i1++){
        if(ev->pEcalShower(i1)->EnergyE>maxe){
            maxe =ev->pEcalShower(i1)->EnergyE;
            nmax =i1                          ;
        }
    }
    _EnergyE=ev->pEcalShower(nmax)->EnergyE;
    EnergyD =ev->pEcalShower(nmax)->EnergyD;
    ext_d0[0]=ev->pEcalShower(nmax)->EMDir[0];
    ext_d0[1]=ev->pEcalShower(nmax)->EMDir[1];
    ext_d0[2]=ev->pEcalShower(nmax)->EMDir[2];

    ext_p0[0]=ev->pEcalShower(nmax)->CofG[0];
    ext_p0[1]=ev->pEcalShower(nmax)->CofG[1];
    ext_p0[2]=ev->pEcalShower(nmax)->CofG[2];
    use_ext=0;
    if(trtrack!=NULL)
        sign=trtrack->GetRigidity()>0?1.:-1.;
    else
        sign=-1;
    ret= process(fedep,fcell,fplane,nEcalHits,EnergyD,_EnergyE,algorithm,sign);
    return ret;
#else
    return -1;
#endif	
}
int EcalAxis::process(float* fedep,int* fcell,int* fplane, int nEcalhits,float EnergyD, float _EnergyE,int algorithm,float sign){
    _sign=sign;
    _algorithm=algorithm;
    for(int i1=0;i1<18;i1++){
        for(int i2=0;i2<72;i2++){
            Edep_raw[i1*72+i2]=0.;
        }
        Max_layer_edep[i1]= 0;
        Max_layer_cell[i1]=-1;
        Layer_quality [i1]= 0;
        layer_Edep    [i1]= 0;
    }
    _erg   =EnergyD/1000.;
    EnergyE=_EnergyE     ;
    for(int i1=0;i1<nEcalhits;i1++){
        Edep_raw[fplane[i1]*72+fcell[i1]]=fedep[i1];
        if(Max_layer_edep[fplane[i1]]<fedep[i1]){
            Max_layer_edep[fplane[i1]]=fedep[i1];
            Max_layer_cell[fplane[i1]]=fcell[i1];
        }
        layer_Edep[fplane[i1]]+=fedep[i1];
    }

    for(int i1=0;i1<18;i1++){
        if(Max_layer_cell[i1]-2>-1){
            if(Edep_raw[i1*72+Max_layer_cell[i1]-2]>0)
                Layer_quality [i1]+=1;
        }
        if(Max_layer_cell[i1]-1>-1){
            if(Edep_raw[i1*72+Max_layer_cell[i1]-1]>0)
                Layer_quality [i1]+=2;
        }
        if(Max_layer_cell[i1]>-1){
            if(Edep_raw[i1*72+Max_layer_cell[i1]]>0)
                Layer_quality [i1]+=4;
        }
        if(Max_layer_cell[i1]+1<72){
            if(Edep_raw[i1*72+Max_layer_cell[i1]+1]>0)
                Layer_quality [i1]+=8;
        }
        if(Max_layer_cell[i1]+2<72){
            if(Edep_raw[i1*72+Max_layer_cell[i1]+2]>0)
                Layer_quality [i1]+=16;
        }
    }
    get_z();
    ecalchi2->set_edep(Edep_raw,_erg,EnergyE);
    _status=0;
    if((algorithm&4)==4){
        if(init_cg()){
            _status+=4;
            double param[4]={p0_cg[0],p0_cg[1],dir_cg[0]/dir_cg[2],dir_cg[1]/dir_cg[2]};
            GetChi2(param);
        }
    }
    if((algorithm&2)==2){
        if(init_lf())
            _status+=2;
    }
    if((algorithm&1)==1){
        if(init_cr()){
            double param[4]={p0_cr[0],p0_cr[1],dir_cr[0]/dir_cr[2],dir_cr[1]/dir_cr[2]};
            GetChi2(param);
            _status+=1;
        }
    }
    if((algorithm&8)==8){
	if(init_cr2()){
		//cout<<ecalchi2->get_chi2()<<"---> ";
		double param[4]={p0_cr2[0],p0_cr2[1],dir_cr2[0]/dir_cr2[2],dir_cr2[1]/dir_cr2[2]};
		GetChi2(param);
		//cout<<ecalchi2->get_chi2()<<endl;
        	_status+=8;
	}		
    }
    if((algorithm&8)==16){
	double param[4]={ext_p0[0],ext_p0[1],ext_d0[0]/ext_d0[2],ext_d0[1]/ext_d0[2]};
        GetChi2(param);
        //cout<<ecalchi2->get_chi2()<<endl;
        _status+=16;	
    }
    use_ext=0;
    return _status;
}
bool EcalAxis::init_cr2(){
	ecalcr->get_esh_axis(Edep_raw,Max_layer_cell,init_raw_cr,Layer_quality,EnergyE);
        double r=sqrt(ecalcr->scfit_v0[0]*ecalcr->scfit_v0[0]+ecalcr->scfit_v0[1]*ecalcr->scfit_v0[1]+ecalcr->scfit_v0[2]*ecalcr->scfit_v0[2]);
        
	dir_cr2.setp(ecalcr->scfit_v0[0]/r,ecalcr->scfit_v0[1]/r,ecalcr->scfit_v0[2]/r);
        p0_cr2.setp(ecalcr->scfit_x0[0]+(ecalz[8]-ecalcr->scfit_x0[2])*dir_cr2[0]/dir_cr2[2],ecalcr->scfit_x0[1]+(ecalz[8]-ecalcr->scfit_x0[2])*dir_cr2[1]/dir_cr2[2],ecalcr->scfit_x0[2]);
	return true;
}
int EcalAxis::process(EcalShowerR* esh,int algorithm,float sign){
    if(!esh)
	return -1;
    float fedep[1296];
    int   fcell[1296], fplane[1296],nEcalHits,ret;
    float EnergyD, _EnergyE;
    nEcalHits=0;
    for(int i1=0;i1<esh->NEcal2DCluster();i1++){
        Ecal2DClusterR * ecal2d=esh->pEcal2DCluster(i1);
        for(int i2=0;i2<ecal2d->NEcalCluster();i2++){
            EcalClusterR* ecalclt=ecal2d->pEcalCluster(i2);
            for(int i3=0;i3<ecalclt->NEcalHit();i3++){
           	EcalHitR* hit=ecalclt->pEcalHit(i3);
		fedep[nEcalHits] =hit->Edep;
        	fcell[nEcalHits] =hit->Cell;
        	fplane[nEcalHits]=hit->Plane;
		nEcalHits++;	 
	    }
        }
    }
    _EnergyE=esh->EnergyE;
    EnergyD =esh->EnergyD;
    ext_d0[0]=esh->EMDir[0];
    ext_d0[1]=esh->EMDir[1];
    ext_d0[2]=esh->EMDir[2];

    ext_p0[0]=esh->CofG[0];
    ext_p0[1]=esh->CofG[1];
    ext_p0[2]=esh->CofG[2];
    use_ext=1;
    ret= process(fedep,fcell,fplane,nEcalHits,EnergyD,_EnergyE,algorithm,sign);
    return ret;
}
bool EcalAxis::init_cg(){
    bool ret;
    float x[18],y[18],ax,bx,ay,by;
    int npoints;
    for(int i1=0;i1<18;i1++){
        init_raw[i1]=0.;
    }
    for(int i1=0;i1<18;i1++){
        double n=0., sum=0.;
        if((Layer_quality [i1]&1)==1){
            n  +=Edep_raw[i1*72+Max_layer_cell[i1]-2];
            sum+=(shiftxy[i1]+(Max_layer_cell[i1]-2)*SIZE)*Edep_raw[i1*72+Max_layer_cell[i1]-2];
        }
        if((Layer_quality [i1]&2)==2){
            n  +=Edep_raw[i1*72+Max_layer_cell[i1]-1];
            sum+=(shiftxy[i1]+(Max_layer_cell[i1]-1)*SIZE)*Edep_raw[i1*72+Max_layer_cell[i1]-1];
        }
        if((Layer_quality [i1]&4)==4){
            n  +=Edep_raw[i1*72+Max_layer_cell[i1]];
            sum+=(shiftxy[i1]+(Max_layer_cell[i1])*SIZE)*Edep_raw[i1*72+Max_layer_cell[i1] ];
        }
        if((Layer_quality [i1]&8)==8){
            n  +=Edep_raw[i1*72+Max_layer_cell[i1]+1];
            sum+=(shiftxy[i1]+(Max_layer_cell[i1]+1)*SIZE)*Edep_raw[i1*72+Max_layer_cell[i1]+1];
        }
        if((Layer_quality [i1]&16)==16){
            n  +=Edep_raw[i1*72+Max_layer_cell[i1]+2];
            sum+=(shiftxy[i1]+(Max_layer_cell[i1]+2)*SIZE)*Edep_raw[i1*72+Max_layer_cell[i1]+2];
        }
        if(n>0.)
            init_raw[i1]=sum/n;
    }
    //Fit X direction
    npoints=0;
    for(int i1=2;i1<18;i1+=4){
        if(Layer_quality[i1]>0.&&((Layer_quality[i1]&4)==4)){
            y[npoints]=init_raw[i1];
            x[npoints]=ecalz[i1];
            npoints++;
        }
        if(Layer_quality[i1+1]>0.&&((Layer_quality[i1+1]&4)==4)){
            y[npoints]=init_raw[i1+1];
            x[npoints]=ecalz[i1+1];
            npoints++;
        }
    }
    ret=straight_line_fit(x,y,npoints,ax,bx,chi2x_cg);
    if(!ret)
        return false;
    //Fit Y direction
    npoints=0;
    for(int i1=0;i1<18;i1+=4){
        if(Layer_quality[i1]>0.&&((Layer_quality[i1]&4)==4)){
            y[npoints]=init_raw[i1];
            x[npoints]=ecalz[i1];
            npoints++;
        }
        if(Layer_quality[i1+1]>0.&&((Layer_quality[i1+1]&4)==4)){
            y[npoints]=init_raw[i1+1];
            x[npoints]=ecalz[i1+1];
            npoints++;
        }
    }
    ret=straight_line_fit(x,y,npoints,ay,by,chi2y_cg);
    if(!ret)
        return false;
    p0_cg[0]=ax+bx*ecalz[8];
    p0_cg[1]=ay+by*ecalz[8];
    p0_cg[2]=ecalz[8];
    double r=sqrt(bx*bx+by*by+1.);
    dir_cg[0]=bx/r;
    dir_cg[1]=by/r;
    dir_cg[2]=1./r;
    return true;
}
//***********My program for Cell Ratio Method 24/05/2012 MV
////***********Last update 13/07/2012 MV
bool EcalAxis::init_cr(){
    bool ret;
    float x[18],y[18],ax,bx,ay,by;
    float slope_cr[18], ratio_cr[18],Xminus1[18];
    float delta[18];
    int npoints;
    for(int i1=0;i1<18;i1++){
        init_raw_cr[i1]=0.;
    }
    for(int j=0;j<18;j++){
        if(((Layer_quality[j]&14)==14)&&layer_Edep[j]>50){
            slope_cr[j] = get_slope_cr(j)==0?-999999. : get_slope_cr(j);
            ratio_cr[j] = get_ratio_cr(j);
            delta[j] = get_deltaxy_cr(j);

            if(slope_cr[j]!=-999999.){
                Xminus1[j] =  (TMath::Log(Edep_raw[j*72+Max_layer_cell[j]-1]/Edep_raw[j*72+Max_layer_cell[j]+1])/slope_cr[j]) + ratio_cr[j];
            }
            else {
                Xminus1[j] = -999999.;
            }
            init_raw_cr[j]=-31.95 + SIZE*Max_layer_cell[j] + delta[j] + Xminus1[j];
        }
    }
    //exit(-1);
    //Fit X direction
    npoints=0;
    for(int j=3;j<16;j++){
        if(j%4>=2){
            if(((Layer_quality[j]&14)==14)&&layer_Edep[j]>50){
                y[npoints]=init_raw_cr[j];
                x[npoints]=ecalz[j];
                npoints++;
            }
        }
    }
    ret=straight_line_fit(x,y,npoints,ax,bx,chi2x_cr);
    if(!ret)
        return false;
    //Fit Y direction
    npoints=0;
    for(int j=4;j<16;j+=4){
        if(((Layer_quality[j]&14)==14)&&layer_Edep[j]>50){
            y[npoints]=init_raw_cr[j];
            x[npoints]=ecalz[j];
            npoints++;
        }
        if(((Layer_quality[j]&14)==14)&&layer_Edep[j]>50){
            y[npoints]=init_raw_cr[j+1];
            x[npoints]=ecalz[j+1];
            npoints++;
        }
    }
    ret=straight_line_fit(x,y,npoints,ay,by,chi2y_cr);
    if(!ret)
        return false;
    p0_cr[0]=ax+bx*ecalz[8];
    p0_cr[1]=ay+by*ecalz[8];
    p0_cr[2]=ecalz[8];
    double r=sqrt(bx*bx+by*by+1.);
    dir_cr[0]=bx/r;
    dir_cr[1]=by/r;
    dir_cr[2]=1./r;
    return true;
}
float EcalAxis::get_slope_cr(int flayer){
    float slope_cr[18];
    float p0[18]    = {0.,0.,0.,3.35297,7.12363,3.26488,-1.61848,-1.63469,-0.383966,-0.141785, -0.172576, 0.412143,0.799396, 1.08324, -0.10977, -0.0125473, 0., 0.};
    float errp0[18] = {0.,0.,0.,0.346605, 0.257378, 0.218957, 0.20647,0.195194, 0.185196, 0.185387, 0.190938, 0.200293, 0.204818, 0.218977, 0.273406, 0.283186, 0., 0.};
    float p1[18]    = {0.,0.,0.,-17.654,-27.0108,-18.2289, -7.57984, -6.19202, -6.6958, -6.46684,-5.97743, -6.23, -5.77442, -5.66404, -2.18609, -1.84701, 0. , 0.};
    float errp1[18] = {0., 0., 0., 0.870339, 0.636702, 0.537672, 0.508595, 0.479077, 0.449094, 0.447786, 0.462006, 0.48296, 0.489205, 0.521647,0.661204, 0.677242, 0., 0.};
    float p2[18]    = {0.,0.,0., 11.081,19.6107,13.1886, 5.59706 , 4.25144, 3.87708, 3.79357, 3.69053, 3.74557, 3.07479, 2.98225, 0.319258, 0.0521284,0.,0.};
    float errp2[18] = {0.,0.,0.,0.785294,0.566947, 0.475815, 0.451701, 0.424245, 0.393173, 0.39078,0.404119,0.421223, 0.422771,  0.449608, 0.578109, 0.585638, 0. ,0.};
    float p3[18]    = {0.,0.,0.,-2.9457, -6.20248, -4.22114, -1.9128, -1.4114, -1.09021 , -1.12017, -1.18253, -1.18025,-0.890959,-0.870741 ,-0.00477062,0.0734578   ,0.,0.};
    float errp3[18] = {1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1,1.,1.,1.,1.};//remember to complete this part!!!
    float p4[18]    = {0.,0.,0.,0.28909, 0.725524,0.501178, 0.243317, 0.176231, 0.116297,0.127854, 0.146521, 0.14473 , 0.104466, 0.103275, 5.78124e-05, -0.00796261  ,0.,0.};

    float errp4[18] = {1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1,1.,1.,1.,1.};//remember to complete this part!!!
    //****Define the slope as a function of logE - using 4th order polinomial
    slope_cr[flayer]= p0[flayer] + p1[flayer]*log10(EnergyE) +  p2[flayer]*pow(log10(EnergyE),2) + p3[flayer]*pow(log10(EnergyE),3) + p4[flayer]*pow(log10(EnergyE),4);

    return slope_cr[flayer];
}
float EcalAxis::get_ratio_cr(int flayer){
    float ratio_cr[18] ={0.,0.,0.,-0.005,0.02,0.02,-0.01,-0.002,0.005,0.,0.,0.003,0.,0.004,0.004,0.002,0.,0.};
    return ratio_cr[flayer];
}
float EcalAxis::get_deltaxy_cr(int flayer){
    float deltaxy_cr[18] ={-0.07,-0.07,0.13,0.13,-0.07,-0.07,0.13,0.13,-0.07,-0.07,0.13,0.13,-0.07,-0.07,0.13,0.13,-0.07,-0.07};
    return deltaxy_cr[flayer];
}
bool EcalAxis::straight_line_fit(float *x ,float*y, int npoints,float &a, float&b,float &fchi2,float* weight){
    double Ax[2][2],Bx[2];
    double w=1.0;
    double detx,ret;
    double err[18],sumerr=0.,sumerr2=0.,merr,errrms;
    int flag[18];
    Ax[0][0]=0.0;Ax[0][1]=0.0;Ax[1][0]=0.0;Ax[1][1]=0.0;Bx[0]=0.0;Bx[1]=0.0;
    for(int i1=0;i1<npoints;i1++){
        if(weight!=NULL)
            w=1./weight[i1];
        else
            w=1.;
        Ax[0][0]+=w;
        Ax[0][1]+=double(x[i1])*w;
        Ax[1][0]+=double(x[i1])*w;
        Ax[1][1]+=double(x[i1])*double(x[i1])*w;
        Bx[0]+=double(y[i1])*w;
        Bx[1]+=double(x[i1])*double(y[i1])*w;
    }
    detx=Ax[0][0]*Ax[1][1]-Ax[0][1]*Ax[1][0];
    if(detx==0)
        return false;
    a=1.0/detx*(Ax[1][1]*Bx[0]-Ax[0][1]*Bx[1]);
    b=1.0/detx*(-Ax[1][0]*Bx[0]+Ax[0][0]*Bx[1]);
    for(int i1=0;i1<npoints;i1++){
        err[i1]=(y[i1]-a-b*x[i1])/sqrt(1+b*b);
        sumerr+=err[i1];
        sumerr2+=err[i1]*err[i1];
    }
    merr=sumerr/npoints;
    errrms=(sumerr2-merr*merr);
    errrms=sqrt(errrms/npoints);

    vector<float> f;
    vector<int> f_ind;
    for(int i1=0;i1<npoints;i1++){
        f.push_back(fabs(err[i1]-merr));
        f_ind.push_back(i1);
    }
    bool flg;
    float temp;
    int tempi;
    for(int i1=0;i1<npoints;i1++){
        flg=false;
        for(int i2=npoints-1;i2>i1;i2--){
            if(f[i2]>f[i2-1]){
                temp=f[i2];
                f[i2]=f[i2-1];
                f[i2-1]=temp;
                tempi=f_ind[i2];
                f_ind[i2]=f_ind[i2-1];
                f_ind[i2-1]=tempi;
                flg=true;
            }
        }
        if(!flg)
            break;
    }

    int count=0;
    for(int i1=0;i1<npoints;i1++){
        if(f[i1]>2*errrms&&(npoints-count)>4){
            flag[f_ind[i1]]=0;
            count++;
        }
        else{
            flag[f_ind[i1]]=1;
        }
    }
    w=1.0;
    Ax[0][0]=0.0;Ax[0][1]=0.0;Ax[1][0]=0.0;Ax[1][1]=0.0;Bx[0]=0.0;Bx[1]=0.0;
    for(int i1=0;i1<npoints;i1++){
        if(flag[i1]==0)
            continue;
        if(weight!=NULL)
            w=1./weight[i1];
        else
            w=1.;
        Ax[0][0]+=w;
        Ax[0][1]+=double(x[i1])*w;
        Ax[1][0]+=double(x[i1])*w;
        Ax[1][1]+=double(x[i1])*double(x[i1])*w;
        Bx[0]+=double(y[i1])*w;
        Bx[1]+=double(x[i1])*double(y[i1])*w;
    }
    detx=Ax[0][0]*Ax[1][1]-Ax[0][1]*Ax[1][0];
    if(detx==0)
        return false;
    a=1.0/detx*(Ax[1][1]*Bx[0]-Ax[0][1]*Bx[1]);
    b=1.0/detx*(-Ax[1][0]*Bx[0]+Ax[0][0]*Bx[1]);

    for(int i1=0;i1<npoints;i1++){
        if(flag[i1]==1)
            ret+=(y[i1]-a-b*x[i1])*(y[i1]-a-b*x[i1]);
    }

    fchi2=(ret/(npoints-count))/errrms;
    return true;
}

///2012/11/07 Add EcalChi2 Plus, Ecal Electron Likelihood function
float EcalAxis::get_chi2plus(AMSEventR* ev,float& _nchi2,float& _nf2edep,float& _chi2plus){
    float _erg		;
    int   maxi,maxe         ;
    maxi=-1                 ;
    maxe=0                  ;
    for(int i1=0;i1<ev->nEcalShower();i1++){
        if(maxe<ev->pEcalShower(i1)->EnergyE){
            maxi=i1;
            maxe=ev->pEcalShower(i1)->EnergyE;
        }
    }
    if(maxi==-1){
        cout<<"EcalAxis-get_chi2plus: Seems No Ecalshower in this event!"<<endl;
        return -1.;
    }
    _erg=ev->pEcalShower(maxi)->EnergyE;
    process(ev,2);
    _nchi2	=ecalchi2->get_chi2();
    _nchi2	=ecalchi2->ecalpdf->normalize_chi2(_nchi2,_erg);
    _nf2edep=layer_Edep[0]+layer_Edep[1];
    _nf2edep=ecalchi2->ecalpdf->normalize_f2edep(_nf2edep,_erg);
    _chi2plus=ecalchi2->ecalpdf->get_chi2plus(_nchi2,_nf2edep);
    return _chi2plus;
}
float EcalAxis::get_elik(AMSEventR* ev,float& _nchi2,float& _nf2edep,float& _chi2plus, float& _elik){
    float 		_bdt	;
    int   maxi,maxe 	;
    maxi=-1         	;
    maxe=0          	;
    for(int i1=0;i1<ev->nEcalShower();i1++){
        if(maxe<ev->pEcalShower(i1)->EnergyE){
            maxi=i1;
            maxe=ev->pEcalShower(i1)->EnergyE;
        }
    }
    if(maxi==-1){
        cout<<"EcalAxis-get_elik: Seems No Ecalshower in this event!"<<endl;
        return -1.;
    }
    _bdt	 =ev->pEcalShower(maxi)->GetEcalBDT(4);
    get_chi2plus(ev, _nchi2,_nf2edep,_chi2plus);
    if(_chi2plus==-1){
        cout<<"EcalAxis-get_elik: Can not get chi2plus"<<endl;
        return -1;
    }
    _elik=ecalchi2->ecalpdf->get_elik(TMath::TanH(0.3*TMath::ATanH(_chi2plus)),TMath::TanH(0.3*TMath::ATanH(_bdt)));
    return _elik;
}
float EcalAxis::get_nchi2(AMSEventR* ev){
    int   maxi,maxe         ;
    maxi=-1                 ;
    maxe=0                  ;
    float _chi2,_erg	;
    for(int i1=0;i1<ev->nEcalShower();i1++){
        if(maxe<ev->pEcalShower(i1)->EnergyE){
            maxi=i1;
            maxe=ev->pEcalShower(i1)->EnergyE;
        }
    }
    if(maxi==-1){
        cout<<"EcalAxis-get_chi2plus: Seems No Ecalshower in this event!"<<endl;
        return -1.;
    }
    _erg=ev->pEcalShower(maxi)->EnergyE;
    process(ev,2);
    _chi2=ecalchi2->get_chi2();
    return ecalchi2->ecalpdf->normalize_chi2(_chi2,_erg);
}
int    EcalCR::est_nrange = 2;
double EcalCR::est_erange_low[2] = {10.,70.}    ;
double EcalCR::est_erange_high[2] = {70.,600.}  ;
double EcalCR::zCooEcal[18] = {-142.792496, -143.657501, -144.642502, -145.507507, -146.492508,
      -147.357498, -148.342499, -149.207504, -150.192505, -151.057510,
      -152.042496, -152.907501, -153.892502, -154.757507, -155.742508,
      -156.607498, -157.592499, -158.457504};
int    EcalCR::proj[18] = {1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1};
double EcalCR::MCsmear=0.;

EcalCR::EcalCR(int _runtype,const char* fdatabase){
	runtype=_runtype	;
	cout<<"EcalCR--Init: "<<fdatabase<<endl; 
	init_est(fdatabase)		;
}
EcalCR::~EcalCR(){

}

double EcalCR::local_shower_center(int layer,double el, double ec, double er, double energy){
  //***********************************************************************
  // Find the shower center localtion by energy ratio method
  // el, ec, er are the energy deposit of the 3 cells centered around maxcell.
  // ec is the energy of the most energetic cell (cell 1-70)
  // el is the energy in the left neighboring cell.(cell 0-69)
  // er is the energy in the right neighboring cell.(cell 2-71)
  // layer is the layer number (0-17)
  // energy is the energy measured by ECAL
  // The return value is the location of the shower center related to the
  // center of the most energetic cell (with energy ec)
  //***********************************************************************
  //
  if(ec<el || ec<er) {
    printf("ec is not the shower maximum\n");
    return -999.;
  }
  if(energy>600.) energy=600.;
  if(energy<10. ) energy=10. ;
  if(layer<0 || layer>=18){
    printf("get_shower_center routine error: wrong layer %d\n",layer);
    return -9999.;
  }
  if(ec==0 || (el==0 && er==0)){
    return 0.;
  }
  double *p_est_int, *p_est_slope, *p_est_curve;
  p_est_int=0;
  p_est_slope = 0;
  p_est_curve = 0;

  if(energy > est_erange_high[est_nrange-1]*1.05) {
    return 0.;
  }
  else {
    for(int ir =0; ir<est_nrange; ir++){
      if(energy>=est_erange_low[ir]*0.95 && energy<=est_erange_high[ir]*1.05){
        p_est_int = &est_int[ir][0][0];
        p_est_slope = &est_slope[ir][0][0];
        p_est_curve = &est_curve[ir][0][0];
        break;
      }
    }
    if(p_est_int == 0) {
    	return 0.;
    }
  }
  double dele=log10(energy)-log10(100.0);

  double xshower,rlr,cog,est;
  rlr = (el-er)/(el+er);
  cog = (el-er)/(el+er+ec);
  est = rlr+cog;
  const double est_binsize = 0.01;
  const int n_est_bin = 301;
  if(est<-1.5) est = -1.5;
  else if (est>1.5) est = 1.5;

  int ibin = floor((est+1.5)/est_binsize);
  double estlow = -1.5+ibin*est_binsize;
  int ib = layer*n_est_bin + ibin;
  double xlow = *(p_est_int+ib) + *(p_est_slope+ib)*dele +
                *(p_est_curve+ib)*dele*dele;
  double xhigh = *(p_est_int+ib+1) + *(p_est_slope+ib+1)*dele +
                *(p_est_curve+ib+1)*dele*dele;
  double xest = xlow+(xhigh-xlow)/est_binsize*(est-estlow);
  if(xest<0.0) xest=0.0;
  if(xest>0.9) xest=0.9;
  xshower = -xest+0.45;

  //if(runtype==2) xshower += MCsmear*rdm->Gaus();   //MC add 300 micron
  return xshower;
}
double EcalCR::ecalcoo_corrected(double tran_coo, double long_coo, double thetax, double thetay, int lay){
  //**************************************************************************
  //
  // correction to the ECAL positions
  //
  // Return: corrected ecal coordinate (in the measureing direction)
  // Input:
  //     tran_coo: uncorrected ECAL coordinate: -32.4 - +32.4
  //     long_coo: ECAL coordinate in the perpendicular direction.
  //     lay: layer number
  //     runtype: 0: iss, 1: mc 2: tb
  //
  //***************************************************************************

  static double Slope[18] = {0.000489, 0.000402, 0.000845, 0.000708, 0.000159,
                      0.000231, 0.000181, 0.000219,-0.000625,-0.000646,
                      4.9e-5,   1.94e-5, -0.000239,-0.000159, 0.000334,
                      0.000345,-0.001621,-0.001499};
  /*
  double Offset[18]= {-0.075785, -0.085085, 0.063735, 0.072154,-0.08138,
                      -0.085716, 0.092481, 0.091874, -0.058069,-0.060580,
                      0.09772,0.104093, -0.076070,-0.077813, 0.095522,
                      0.090256,-0.027785, -0.034400};
  */
  static double Offset[18] = {-0.07324,  -0.08149,  0.10311,  0.11116,  -0.07378,
                       -0.07702,  0.13018,  0.12953,  -0.04582,  -0.04731,
                        0.13439,  0.14032,  -0.05948,  -0.06012,  0.13106,
                       0.12534,  -0.00678,  -0.01242};

  static double delz[18] = {
    0.69878,  0.67685,  0.64967,  0.62664,  0.60627,  0.58448,
    0.55146,  0.52856,  0.51396,  0.49263,  0.45312,  0.43028,
    0.42159,  0.39966,  0.35460,  0.33199,  0.32901,  0.30764};

  /*
  const float Offset_mc[18] = {0.127,0.118,-0.0812,-0.0812,0.0227,0.0205,
                              -0.0515,-0.0511,0.0681,0.067,-0.03255,-0.03215,
                              -0.0201,-0.0201,0.1349,0.1346,-0.0614,-0.0628};
  */

  static float Offset_mc[18] = {
         0.013700,  -0.001009,  -0.027872,  -0.006446,  -0.014031,  -0.023413,
         0.056922,  0.069365,  -0.012313,  -0.019871,  0.117947,  0.137400,
        -0.051909,  -0.058700,  0.173418,  0.179588,  -0.025248,  -0.036930};

  static float delz_mc[18] = {
         0.288954,  0.212326,  0.246985,  0.170729,  0.196804,  0.120077,
         0.155832,  0.079470,  0.103800,  0.027058,  0.064383,  -0.011942,
         0.010782,  -0.065958,  -0.040393,  -0.102736,  -0.081880,  -0.158245};

  static float Offset_tb[18]= {-0.0792, -0.0670, 0.10205, 0.10553, -0.0728,
                      -0.0756, 0.12897, 0.12789, -0.06094,-0.064320,
                      0.132660,0.13932, -0.06639,-0.07077, 0.123455,
                      0.120981,-0.03080, -0.036510};

  double corr_coo = tran_coo;
  if(runtype == 2){  //mc data
    corr_coo += (long_coo+32.4)*Slope[lay]+ Offset_mc[lay];
    if(proj[lay] == 1) {
      corr_coo += delz_mc[lay]*thetay;
    }
    else {
      corr_coo += delz_mc[lay]*thetax;
    }
  }
  else if(runtype == 0) {   //iss
    corr_coo += (long_coo+32.4)*Slope[lay]+ Offset[lay];
    if(proj[lay] == 1) {
      corr_coo += delz[lay]*thetay;
    }
    else {
      corr_coo += delz[lay]*thetax;
    }
  }
  else if (runtype ==1) {  //testbeam
    if(proj[lay] == 1) {
      corr_coo += (long_coo+32.4)*Slope[lay] + Offset_tb[lay]
                   +  0.01268 + 0.4559*thetay;
    }
    else {
      corr_coo += (long_coo+32.4)*Slope[lay] + Offset_tb[lay]
                   + 0.03427 + 0.42076*thetax;
    }
  }
  return corr_coo;
}
int EcalCR::fit_shower_center_line(double *x0, double *v0, double *shower_center, int *shower_center_quality)
{
  //**************************************************************************
  //   Fit the shower center line, the coordinate of each layer obtained by cell-ratio
  //
  // Return: the status of the fit: 1:ok , -1:bad
  // Input:
  //       *shower_center: shower_center[18] - the shower center values obtained by
  //                                           cell ratio mathod
  //       *shower_center_quality: shower_center_quality[18] - quality of shower
  //                               center value.
  // Output:
  //       *x0: x0[3] - the shower center coordinate at z = zCooEcal[0];
  //       *v0: v0[3] - The direction vector of shower center line.
  //                    (dx/dz, dy/dz, 1);
  //
  //***************************************************************************

  const double std_weight[18] = {
    10.33, 30.52, 102.03, 210.04, 277.8, 369.8, 625.0, 692.5,
    540.8, 540.8, 540.8,  452.69, 297.26, 229.57, 198.37, 156.25,
    89.0, 62.0};
  double delta,sumz,sumz2,sumx,sumxz, sumw;
  int nlayerfit;
  double a[2],b[2];
  int returnvalue = 0;
  for(int iproj=0; iproj<2; iproj++){
    a[iproj]=0.0;
    b[iproj]=0.0;
    delta=0.0;
    sumz = 0.0;
    sumz2=0.0;
    sumx=0.0;
    sumxz=0.0;
    sumw = 0.0;
    nlayerfit = 0;
    for(int il=2; il<18; il++){
      if((shower_center_quality[il]&4)!=4) continue;
      if(proj[il]==iproj && fabs(shower_center[il])<100.){
        sumx+=shower_center[il]*std_weight[il];
        sumz+=zCooEcal[il]*std_weight[il];
        sumxz += shower_center[il]*zCooEcal[il]*std_weight[il];
        sumz2 += zCooEcal[il]*zCooEcal[il]*std_weight[il];
        sumw += std_weight[il];
        nlayerfit++;
        //        printf("layer %d, z= %f, x= %f\n",il,zCooEcal[il],center_layer[il]);
      }
    }
    //    printf("nlayer = %d \n",nlayerfit);
    delta = sumw*sumz2 - sumz*sumz;
    if(delta <=0.0||nlayerfit<2){
      returnvalue -= (1+iproj);
      continue;
    }
    a[iproj] = (sumx*sumz2 - sumz*sumxz)/delta;
    b[iproj] = (sumw*sumxz - sumz*sumx)/delta;
    //   printf("a,b = %f %f \n\n",a[iproj],b[iproj]);
  }
  *x0 = a[0]+b[0]*zCooEcal[0];
  *v0 = b[0];
  *(x0+1) = a[1] + b[1]*zCooEcal[0];
  *(v0+1) = b[1];
  *(x0+2) = zCooEcal[0];
  *(v0+2) = 1.0;
  return returnvalue;
}

void EcalCR::init_est(const char* fdatabase){
  TFile *f;
  char htitle[100];
  f=new TFile(fdatabase);
  for(int ir=0; ir<est_nrange; ir++){
    for(int il=0; il<18; il++){
      sprintf(htitle,"est_int_%d_%2.2d",ir,il);
      TH1F *h = (TH1F*)f->Get(htitle);
      if(!h)
	cout<<"EcalCR::Error-Cann't-find-"<<htitle<<endl;
      for(int ib=0; ib<301; ib++){
        est_int[ir][il][ib] = h->GetBinContent(ib+1);
      }
      sprintf(htitle,"est_slope_%d_%2.2d",ir,il);
      h = (TH1F*)f->Get(htitle);
      if(!h)
	cout<<"EcalCR::Error-Cann't-find-"<<htitle<<endl;
      for(int ib=0; ib<301; ib++){
        est_slope[ir][il][ib] = h->GetBinContent(ib+1);
      }
      sprintf(htitle,"est_curve_%d_%2.2d",ir,il);
      h = (TH1F*)f->Get(htitle);
      if(!h)
	cout<<"EcalCR::Error-Cann't-find-"<<htitle<<endl;
      for(int ib=0; ib<301; ib++){
        est_curve[ir][il][ib] = h->GetBinContent(ib+1);
      }
    }
  }
  f->Close();
  rdm = new TRandom3();
}

int EcalCR::get_esh_axis(float* Edep_raw,int* Max_layer_cell,float* init_raw_cr,int* Layer_quality,float _erg){
	erg=_erg;
	sc_local_quality=Layer_quality;
	sc_local_CoG=init_raw_cr;
	//Simple check of max cell compatibility
	bool goodsl[9];
	bool goodl[18];
	double dx=0.,dy=0.;
	int    ndx=0,ndy=0;
	for(int i1=0;i1<9;i1++){
		if(fabs(Max_layer_cell[2*i1]-Max_layer_cell[2*i1+1])<2)
			goodsl[i1]=true;
		else
			goodsl[i1]=false;
	}
		
	for(int il=0;il<18;il++){	
		if((sc_local_quality[il]&4)!=4)
			continue;
		if(!goodsl[il/2]){
			sc_local_quality[il]-=4;
			continue;
		}
		int icmax=Max_layer_cell[il];
		double ec =Edep_raw[il*72+icmax];
      		double el;
      		double er;
		if(icmax<1) el=0.;
		else el=Edep_raw[il*72+icmax-1];
		if(icmax>70)	er=0.;
		else	er=Edep_raw[il*72+icmax+1];
		sc_local[il] = local_shower_center(il, el, ec, er, erg)-31.95 + icmax*0.9;
		//cout<<"icmax= "<<icmax<<", "<<el<<", "<<ec<<", "<<er<<", "<<sc_local[il]<<endl;
		if(sc_local[il] < -100.) {
          		sc_local[il] = sc_local_CoG[il];
			if((sc_local_quality[il]&7)==4)
          			sc_local_quality[il]-=4;
        	}
	}
	int fit_status = fit_shower_center_line(scfit_x0, scfit_v0,sc_local, sc_local_quality);
  	if(fit_status == -3){
    		// printf("fit_shower_center_line failed\n");
    		return -299;
  	}
  	Theta_x = scfit_v0[0];
  	Theta_y = scfit_v0[1];
	
	/*cout<<"  sc_local= ";	
	// Now we have the coordinate of both direction, correct them to global.
	for(int i1=0;i1<18;i1++)
		cout<<sc_local[i1]<<", ";
	cout<<endl;
	*/
	for(int il=0; il<18; il++){
    		double ylayer = shower_center_layer_orthogonal(il);
    		sc_global[il] = ecalcoo_corrected(sc_local[il], ylayer,Theta_x,Theta_y,il);
  	}
	/*
	cout<<"  sc_global= ";
	for(int i1=0;i1<18;i1++)
                cout<<sc_global[i1]<<", ";
        cout<<endl;
	cout<<" sc_local_quality= ";
	for(int i1=0;i1<18;i1++)
                cout<<sc_local_quality[i1]<<", ";
        cout<<endl;
	*/
  	// Fit the shower center line again.
  	fit_status = fit_shower_center_line(scfit_x0, scfit_v0,sc_global, sc_local_quality);
	if(fit_status == -3){
                // printf("fit_shower_center_line failed\n");
                return -299;
        }
        Theta_x = scfit_v0[0];
        Theta_y = scfit_v0[1];
	//cout<<scfit_x0[0]<<", "<<scfit_x0[1]<<", "<<scfit_x0[2]<<", "<<scfit_v0[0]<<", "<<scfit_v0[1]<<", "<<scfit_v0[2]<<endl;	
	return 0;
}
double EcalCR::shower_center_layer_orthogonal(int layer_number){
  double a,b;
  if(layer_number>=18 || layer_number<0) return -9999.;
  if(proj[layer_number]==1){
    a = scfit_x0[0];
    b = scfit_v0[0];
  }
  else {
    a = scfit_x0[1];
    b = scfit_v0[1];
  }
  double xlayer = a+b*(zCooEcal[layer_number]-zCooEcal[0]);
  return xlayer;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//End of Ecal Axis 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

