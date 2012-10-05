#include "TrdKCluster.h"

ClassImp(TrdKCluster)

using namespace TMath;


TVirtualFitter *TrdKCluster::gMinuit_TRDTrack = NULL;
TrdKCalib *TrdKCluster::_DB_instance = NULL;
TRD_ImpactParameter_Likelihood *TrdKCluster::TRDImpactlikelihood = NULL;
TrdKPDF *TrdKCluster::kpdf_e=NULL;
TrdKPDF *TrdKCluster::kpdf_p=NULL;
TrdKPDF *TrdKCluster::kpdf_h=NULL;
TrdKPDF *TrdKCluster::kpdf_q=NULL;

bool TrdKCluster::DebugOn=0;
int TrdKCluster::Default_TrPar[3];
int TrdKCluster::ForceReadAlignment=1;
int TrdKCluster::ForceReadCalibration=1;
int TrdKCluster::IsReadAlignmentOK=2;
int TrdKCluster::IsReadCalibOK=1;
int TrdKCluster::IsReadGlobalAlignment=1;
int TrdKCluster::IsReadPlaneAlignment=1;
Double_t TrdKCluster::LastProcessedRun_Alignment=0;
Double_t TrdKCluster::LastProcessedRun_Calibration=0;
vector <TrdKHit> TrdKCluster::TRDTubeCollection;
map<int, TRDOnline> TrdKCluster::map_TRDOnline;
float TrdKCluster::MinimumDistance=3;

/////////////////////////////////////////////////////////////////////

TrdKCluster::TrdKCluster()
{
    Init_Base();
}

/////////////////////////////////////////////////////////////////////

TrdKCluster::~TrdKCluster(){
}

/////////////////////////////////////////////////////////////////////

TrdKCluster::TrdKCluster(AMSEventR *evt, TrTrackR *track, int fitcode)
{
    _event=evt;
    Init_Base();

    Time=evt->UTime();

    if(Time-LastProcessedRun_Alignment>600 && IsReadAlignmentOK==0){
        printf("~~~TrdKCluster~~~, Current Time: %.0f , Last Alignment Time: %.0f \n",Time,LastProcessedRun_Alignment);
        IsReadAlignmentOK=2;
    }

    if(Time-LastProcessedRun_Calibration>600  && IsReadCalibOK==0 ){
        printf("~~~TrdKCluster~~~, Current Time: %.0f , Last Calibration Time: %.0f \n",Time,LastProcessedRun_Calibration);
        IsReadCalibOK=1;
    }

    int NTRDHit=evt->NTrdRawHit();
    if(!NTRDHit)return;

    for(int i=0;i<NTRDHit;i++){
        TrdRawHitR* _trd_hit=evt->pTrdRawHit(i);
        if(!_trd_hit) continue;
        TRDHitCollection.push_back(TrdKHit(_trd_hit,Zshift));
    }


    SetTrTrack(track, fitcode);

    DoHitPreselection(MinimumDistance); //=======Select +/- MinimumDistance (cm) volumn around TrTrack=============
    AddEmptyTubes(MinimumDistance);  //===========Add empty tubes to complete the geometry==========


    if(IsReadAlignmentOK>0 && ForceReadAlignment>0){
        IsReadAlignmentOK=DoAlignment(IsReadPlaneAlignment,IsReadGlobalAlignment);
        LastProcessedRun_Alignment=Time;
    }

    if(IsReadCalibOK && ForceReadCalibration>0){
        IsReadCalibOK=DoGainCalibration();
        LastProcessedRun_Calibration=Time;
    }


    HasTRDTrack=0;

    int getpressure=GetXePressure();
    if(getpressure==-1){
        cout<<"~~~~~WARNING~~~~~~  Get Xe Pressure Failed, Putting into Default value, result NOT Valid"<<endl;
        Pressure_Xe=780;
        IsValid=0;
    }else{
        IsValid=1;
    }

    return;
}


/////////////////////////////////////////////////////////////////////

TrdKCluster::TrdKCluster(vector<TrdKHit> _collection,AMSPoint *P0, AMSPoint *Dir,AMSPoint *TRDTrack_P0, AMSPoint *TRDTrack_Dir,AMSPoint *MaxSpan_P0, AMSPoint *MaxSpan_Dir):TRDHitCollection(_collection)
{

    Init_Base();

    //    DoHitPreselection(MinimumDistance); //=======Select +/- 5cm volumn around TrTrack=============
    //    AddEmptyTubes(MinimumDistance);  //===========Add empty tubes to complete the geometry==========

    float TRDCenter=115;
    Track_Rigidity=100000;

    track_extrapolated_Dir= *Dir;
    track_extrapolated_P0=  *P0;
    TrTrack_Pro=TrProp(track_extrapolated_P0,track_extrapolated_Dir,Track_Rigidity);

    TRDtrack_extrapolated_P0=*TRDTrack_P0;
    TRDtrack_extrapolated_Dir=*TRDTrack_Dir;
    HasTRDTrack=1;



}


/////////////////////////////////////////////////////////////////////

void TrdKCluster::Constrcut_TRDTube(){
    cout<<"Construct TRD Tube: "<<endl;
    TRDTubeCollection.clear();
    for(int i=0;i<5248;i++){
        int layer,ladder, tube;
        TrdHCalibR::gethead()->GetLLTFromTubeId(layer,ladder,tube,i);
        TrdRawHitR *_trdhit=new TrdRawHitR(layer,ladder,tube,0);
        TrdKHit hit(_trdhit,Zshift);
        TRDTubeCollection.push_back(hit);
        delete _trdhit;
    }
    cout<<"Number of TRD Tube constructed: "<<TRDTubeCollection.size()<<endl;

    return;
}


/////////////////////////////////////////////////////////////////////

void TrdKCluster::Init_Base(){

    IsValid=0;
    Zshift=0.0;
    Minimum_dR=2;
    TRDCenter=115;

    if(TRDTubeCollection.size()!=5248)Constrcut_TRDTube();
    if(map_TRDOnline.size()==0) InitXePressure();
    if(!TRDImpactlikelihood)TRDImpactlikelihood=new TRD_ImpactParameter_Likelihood();
    if(!_DB_instance) _DB_instance=new TrdKCalib();
    if(!kpdf_e)kpdf_e=new TrdKPDF("Electron");
    if(!kpdf_p)kpdf_p=new TrdKPDF("Proton");
    if(!kpdf_h)kpdf_h=new TrdKPDF("Helium");
    if(!kpdf_q)kpdf_q=new TrdKPDF("Nuclei");

}

/////////////////////////////////////////////////////////////////////

int TrdKCluster::DoGainCalibration(){
    int read=_DB_instance->readDB_Calibration(Time);
    if(read==0){
        cout<<"~~~~~~~WARNING, TrdKCluster~~~~~~~Can Not Read DBs for Gain Calibration, Returning 0"<<endl;
        return 0;
    }for(int i=0;i<NHits();i++){
        GetHit(i)->DoGainCalibration(_DB_instance->GetGainCorrectionFactorTube(GetHit(i)->tubeid,Time));
    }
    return read;
}

/////////////////////////////////////////////////////////////////////

int TrdKCluster::DoAlignment(int &readplane, int &readglobal){
    int read = _DB_instance->readDB_Alignment(Time,readplane,readglobal);
    if(read==0){
        cout<<"~~~~~~~WARNING, TrdKCluster~~~~~~~Can Not Read DBs for Alignment, Returning 0"<<endl;
        return 0;
    }else if(read==2){
        for(int i=0;i<NHits();i++){
            int layer=GetHit(i)->TRDHit_Layer;
            par_alignment=_DB_instance->GetAlignmentPar(layer,(int)Time);
            GetHit(i)->DoAlignment(&par_alignment);
        }
    }else if(read==1){
        for(int i=0;i<NHits();i++){
            int layer=GetHit(i)->TRDHit_Layer;
            par_alignment=*(_DB_instance->GetAlignmentPar_Plane(layer));
            GetHit(i)->DoAlignment(&par_alignment);
        }
    }
    return read;
}


/////////////////////////////////////////////////////////////////////

void TrdKCluster::DoHitPreselection(float cut_distance){
    vector<TrdKHit>::iterator  Hit_it=TRDHitCollection.begin();
    for (;Hit_it!=TRDHitCollection.end(); ) {
        if(fabs((*Hit_it).Tube_Track_Distance_3D(&track_extrapolated_P0,&track_extrapolated_Dir))>cut_distance){
            Hit_it =TRDHitCollection.erase(Hit_it);
        }else Hit_it++;
    }


}



/////////////////////////////////////////////////////////////////////

void TrdKCluster::AddEmptyTubes(float cut_distance){

    //========Include All Tubes===============================
    vector<TrdKHit> selected_tube_collection;

    for(int i=0;i<TRDTubeCollection.size();i++){
        TrdKHit *tube=&(TRDTubeCollection.at(i));
        if(fabs(tube->Tube_Track_Distance_3D(&track_extrapolated_P0,&track_extrapolated_Dir))>cut_distance)continue;
        selected_tube_collection.push_back(*tube);
    }


    //========Remove Duplication============
    vector<TrdKHit>::iterator  Hit_it=selected_tube_collection.begin();
    for (;Hit_it!=selected_tube_collection.end(); ) {
        bool removed=0;
        for(int i=0;i<NHits();i++){
            if((*Hit_it).tubeid==GetHit(i)->tubeid){
                Hit_it =selected_tube_collection.erase(Hit_it);
                removed=1;
                break;
            }
        }
        if(!removed)Hit_it++;
    }

    for(int i=0;i<selected_tube_collection.size();i++){
        TrdKHit *tube=&(selected_tube_collection.at(i));
        this->TRDHitCollection.push_back(*tube);
    }


}



/////////////////////////////////////////////////////////////////////

void TrdKCluster::SelectClosest(){
    vector<TrdKHit>::iterator  Hit_it=TRDHitCollection.begin();
    for (;Hit_it!=TRDHitCollection.end(); ) {
        vector<TrdKHit>::iterator  Hit_it2=TRDHitCollection.begin();
        bool removed=0;
        for (;Hit_it2!=TRDHitCollection.end(); ) {
            if(Hit_it==Hit_it2)Hit_it2++;
            else if(int((*Hit_it).tubeid/16)!=int((*Hit_it2).tubeid/16))Hit_it2++;
            else if(abs((*Hit_it).Tube_Track_Distance_3D(&track_extrapolated_P0,&track_extrapolated_Dir))<abs((*Hit_it2).Tube_Track_Distance_3D(&track_extrapolated_P0,&track_extrapolated_Dir))) Hit_it2 =TRDHitCollection.erase(Hit_it2);
            else if (abs((*Hit_it).Tube_Track_Distance_3D(&track_extrapolated_P0,&track_extrapolated_Dir))>abs((*Hit_it2).Tube_Track_Distance_3D(&track_extrapolated_P0,&track_extrapolated_Dir))){
                Hit_it =TRDHitCollection.erase(Hit_it);
                removed=1;
            }else Hit_it2++;
        }
        if(!removed)Hit_it++;
    }
}

/////////////////////////////////////////////////////////////////////

void TrdKCluster::RemoveDuplicate(TrdKCluster *fired){
    vector<TrdKHit>::iterator  Hit_it=TRDHitCollection.begin();
    for (;Hit_it!=TRDHitCollection.end(); ) {
        bool removed=0;
        for(int i=0;i<fired->NHits();i++){
            if((*Hit_it).tubeid==fired->GetHit(i)->tubeid){
                //                cout<<"Removed"<<endl;
                Hit_it =TRDHitCollection.erase(Hit_it);
                removed=1;
                break;
            }
        }
        if(!removed)Hit_it++;
    }
}


/////////////////////////////////////////////////////////////////////

void TrdKCluster::Propogate_TrTrack(double z){
    TrTrack_Pro.Propagate(z);
}

/////////////////////////////////////////////////////////////////////

AMSPoint TrdKCluster::GetPropogated_TrTrack_P0(){
    return TrTrack_Pro.GetP0();
}

/////////////////////////////////////////////////////////////////////

AMSDir TrdKCluster::GetPropogated_TrTrack_Dir(){
    return TrTrack_Pro.GetDir();
}


/////////////////////////////////////////////////////////////////////

bool  TrdKCluster::IsCalibrated(){
    bool v=1;
    for(int i=0;i<TRDHitCollection.size();i++)
        if(!GetHit(i)->IsCalibrated)v=0;
    return v;
}

/////////////////////////////////////////////////////////////////////

bool  TrdKCluster::IsAligned(){
    bool v=1;
    for(int i=0;i<TRDHitCollection.size();i++)
        if(!GetHit(i)->IsAligned)v=0;
    return v;
}




/////////////////////////////////////////////////////////////////////

void TrdKCluster::fcn_TRDTrack(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag){
    TrdKCluster* cluster=( TrdKCluster*)gMinuit_TRDTrack->GetObjectFit();
    f=cluster->TRDTrack_ImpactChi2(par);
}




/////////////////////////////////////////////////////////////////////

void TrdKCluster::fcn_TRDTrack_PathLength(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag){
    TrdKCluster* cluster=( TrdKCluster*)gMinuit_TRDTrack->GetObjectFit();
    f=cluster->TRDTrack_PathLengthLikelihood(par);
}


/////////////////////////////////////////////////////////////////////

double TrdKCluster::TRDTrack_ImpactChi2(Double_t *par){

    int size= NHits();
    double result=0;
    AMSPoint temp_TrTrackP0(par[0],par[1],par[2]);
    AMSDir  temp_TrTrackDir(par[3],par[4],TMath::Sqrt(1-par[3]*par[3]-par[4]*par[4]));
    double impact_parameter,likelihood;
    TrdKHit* hit;
    for(int i=0;i<size;i++){
        hit=GetHit(i);
        impact_parameter=hit->Tube_Track_Distance_3D(&temp_TrTrackP0,&temp_TrTrackDir);
        likelihood = TRDImpactlikelihood->GetLikelihood(hit->TRDHit_Amp,impact_parameter);
        if(likelihood<0.0000001) likelihood=0.0000001;
        result-=2*log(likelihood);
    }
    return result;


}

/////////////////////////////////////////////////////////////////////

double TrdKCluster::TRDTrack_PathLengthLikelihood(Double_t *par){
    int size= NHits();
    double result=0;
    AMSPoint temp_TrTrackP0(par[0],par[1],par[2]);
    AMSDir  temp_TrTrackDir(par[3],par[4],TMath::Sqrt(1-par[3]*par[3]-par[4]*par[4]));
    double pathlength,likelihood;
    TrdKHit* hit;
    for(int i=0;i<size;i++){
        hit=GetHit(i);
        pathlength=hit->Tube_Track_3DLength(&temp_TrTrackP0,&temp_TrTrackDir);
        if(Refit_hypothesis==1) likelihood=kpdf_p->GetLikelihood(hit->TRDHit_Amp,abs(Track_Rigidity),pathlength,hit->TRDHit_Layer,Pressure_Xe/1000);
        else if(Refit_hypothesis==0) likelihood=kpdf_e->GetLikelihood(hit->TRDHit_Amp,abs(Track_Rigidity),pathlength,hit->TRDHit_Layer,Pressure_Xe/1000);
        else if(Refit_hypothesis==2) likelihood=kpdf_h->GetLikelihood(hit->TRDHit_Amp,abs(Track_Rigidity),pathlength,hit->TRDHit_Layer,Pressure_Xe/1000);
        else{
            cout<<"~~~~~WARNING~~~~TrdKCluster, Refit using PathLength Likelihood, Particle Hypothesis not found: "<<Refit_hypothesis<<endl;
            return -999;
        }
        result-=2*log(likelihood);

    }



    return result;
}


/////////////////////////////////////////////////////////////////////

void TRD_ImpactParameter_Likelihood::InitLikelihood(){
    TString s_func ="[0]*(TMath::ATan((0.3-(x-[1]))/[2])+TMath::ATan((0.3+(x-[1]))/[2]))";
    //    cout<<"Init Likelihood function: "<<s_func<<endl;

    d0_likelihood_func = new TF1("Residual_Function", s_func, -5,5);
    d0_likelihood_func->SetNpx(10000);
    d0_likelihood_func->SetParameter(0,1);
    d0_likelihood_func->SetParameter(1,0);
    d0_likelihood_func->SetParameter(2,0.03);
    d0_likelihood_func_integral=d0_likelihood_func->Integral(-5,5);

    TString s_func_missed ="[0]*([1]-(TMath::ATan((0.3-x)/[2])+TMath::ATan((0.3+x)/[2])))";

    d0_likelihood_func_missed = new TF1("Residual_Function_Missed", s_func_missed, -100,100);
    d0_likelihood_func_missed->SetParameter(0,1);
    d0_likelihood_func_missed->SetParameter(1,2.97750e+00);
    d0_likelihood_func_missed->SetParameter(2,2.59552e-02);
    d0_likelihood_func_missed->SetNpx(50000);
    d0_likelihood_func_missed_integral=d0_likelihood_func_missed->Integral(-100,100);

    //    cout<<"Integral for Missed Function: "<<d0_likelihood_func_missed_integral<<endl;


    return ;
}

/////////////////////////////////////////////////////////////////////

double TRD_ImpactParameter_Likelihood::GetLikelihood(float Amp, float d0){
    //    if(fabs(d0)>100){
    //        cout<<"~~~~~~WARNING~~~~~,  D0: "<<d0<<" exceeded tube range ~~~~~~~~~~~~~~   "<<endl;
    //        if(Amp==0)cout<<"Value: "<<d0_likelihood_func_missed->Eval(d0)/d0_likelihood_func_missed_integral<<endl;
    //    }

    if(Amp<=0)return d0_likelihood_func_missed->Eval(d0)/d0_likelihood_func_missed_integral;
    else return d0_likelihood_func->Eval(d0)/d0_likelihood_func_integral;
}



/////////////////////////////////////////////////////////////////////

void TrdKCluster::FitTRDTrack(int method, int hypothesis){

    if(method==1) FitTRDTrack_IPLikelihood();
    else  if(method==2) FitTRDTrack_Analytical();
    else  if(method==3) FitTRDTrack_PathLength(hypothesis);
    else{
        cout<<"~~~WARNING~~~TrdKCluster, Unrecognized Fit Method: "<<method<<endl;
        return ;
    }
    return;
};

/////////////////////////////////////////////////////////////////////

void TrdKCluster::FitTRDTrack_IPLikelihood(int IsCharge){

    TVirtualFitter::SetDefaultFitter("Minuit");
    gMinuit_TRDTrack = TVirtualFitter::Fitter(0, 4);

    //    gMinuit_TRDTrack->SetFCN(fcn_TRDTrack);
    //Changed by Wei Sun
    if(IsCharge==0) gMinuit_TRDTrack->SetFCN(fcn_TRDTrack);
    else gMinuit_TRDTrack->SetFCN(fcn_TRDTrack_Charge);

    gMinuit_TRDTrack->SetObjectFit(this);
    double arglist[10];
    arglist[0]=-1;
    gMinuit_TRDTrack->ExecuteCommand("SET PRINT", arglist, 1);
    gMinuit_TRDTrack->ExecuteCommand("SET NOW", arglist, 0);


    float init_z0=115;
    float init_x0=0;
    float init_y0=0;
    float init_dx=0;
    float init_dy=0;

    Propogate_TrTrack(init_z0);
    AMSPoint P0=GetPropogated_TrTrack_P0();
    AMSPoint Dir=GetPropogated_TrTrack_Dir();
    init_x0=P0.x();
    init_y0=P0.y();
    init_dx=Dir.x();
    init_dy=Dir.y();

    //    printf("TrTrack : (%.2f, %.2f, %.2f), (%.2f, %.2f, %.2f)\n",P0.x(),P0.y(),P0.z(),Dir.x(),Dir.y(),Dir.z());

    gMinuit_TRDTrack->SetParameter(0,  "x0",  init_x0,  0.2,  init_x0-10,init_x0+10);
    gMinuit_TRDTrack->SetParameter(1,  "y0",  init_y0,  0.2,  init_y0-10,init_y0+10);
    gMinuit_TRDTrack->SetParameter(2,  "z0",  init_z0,0,init_z0,init_z0);
    gMinuit_TRDTrack->SetParameter(3,  "dx0", init_dx,  0.1,  -1,1);
    gMinuit_TRDTrack->SetParameter(4,  "dy0", init_dy,  0.1,  -1,1);

    arglist[0]=0;

    gMinuit_TRDTrack->FixParameter(2);
    gMinuit_TRDTrack->ExecuteCommand("MIGRAD", arglist, 0);


    float x0,y0,z0,dx,dy,dz;

    x0=gMinuit_TRDTrack->GetParameter(0);
    y0=gMinuit_TRDTrack->GetParameter(1);
    z0=gMinuit_TRDTrack->GetParameter(2);
    dx=gMinuit_TRDTrack->GetParameter(3);
    dy=gMinuit_TRDTrack->GetParameter(4);
    dz=TMath::Sqrt(1-dx*dx-dy*dy);


    AMSDir TRDDir(dx,dy,dz);
    AMSPoint TRDP0(x0,y0,z0);

    SetTRDTrack(&TRDP0,&TRDDir);

}


/////////////////////////////////////////////////////////////////////

void TrdKCluster::MergeWith(TrdKCluster *secondcluster){
    for(int i=0;i<secondcluster->NHits();i++){
        this->TRDHitCollection.push_back(*secondcluster->GetHit(i));
    }
}



/////////////////////////////////////////////////////////////////////

void TrdKCluster::AnalyticalFit_2D(int direction, double x, double z, double dx, double dz, double &TRDTrack_x,double &TRDTrack_dx){



    float tubex,tubez;

    double norm_dxdz=sqrt(dx*dx+dz*dz);

    dx/=norm_dxdz;
    dz/=norm_dxdz;


    double DD1[2]={0,0};
    double DD2[2][2]={0,0,0,0};

    float radius=0.3;
    float width=0.03;

    int tubecount=0;

    int factor_index=-1;
    float factor[2]={1./1.8478,-1./4.1522};

    for(int i=0;i<NHits();i++){

        TrdKHit* hit=GetHit(i);

        if(hit->TRDHit_Direction!=direction)continue;

        tubecount++;
        tubex=hit->TRDRawHit_R;
        tubez=hit->TRDRawHit_Z;

        if(hit->TRDHit_Amp>0)factor_index=0;
        else factor_index=1;

        DD1[0]+=factor[factor_index]*(dz/(width*(1 + Power (radius + dz*(-tubex + x) - dx*(-tubez + z),2)/Power(width,2))) - dz/(width*(1 + Power (radius - dz*(-tubex + x) + dx*(-tubez + z),2)/Power(width,2))));
        DD1[1]+=factor[factor_index]*((tubez - (dx*(-tubex + x))/Sqrt(1 - Power (dx,2)) - z)/(width*(1 + Power (radius + dz*(-tubex + x) - dx*(-tubez + z),2)/Power(width,2))) + (-tubez + (dx*(-tubex + x))/Sqrt(1 - Power (dx,2)) + z)/(width*(1 + Power (radius - dz*(-tubex + x) + dx*(-tubez + z),2)/Power(width,2))));
        DD2[0][0]+=factor[factor_index]*((-2*(1 - Power (dx,2))*(radius + dz*(-tubex + x) - dx*(-tubez + z)))/(Power (width,3)*Power (1 + Power (radius + dz*(-tubex + x) - dx*(-tubez + z),2)/Power(width,2),2)) - (2*(1 - Power (dx,2))*(radius - dz*(-tubex + x) + dx*(-tubez + z)))/(Power (width,3)*Power (1 + Power (radius - dz*(-tubex + x) + dx*(-tubez + z),2)/Power(width,2),2)));
        DD2[0][1]+=factor[factor_index]*((-2*dz*(tubez - (dx*(-tubex + x))/Sqrt(1 - Power (dx,2)) - z)*(radius + dz*(-tubex + x) - dx*(-tubez + z)))/(Power (width,3)*Power (1 + Power (radius + dz*(-tubex + x) - dx*(-tubez + z),2)/Power(width,2),2)) - dx/(dz*width*(1 + Power (radius + dz*(-tubex + x) - dx*(-tubez + z),2)/Power(width,2))) + (2*dz*(-tubez + (dx*(-tubex + x))/Sqrt(1 - Power (dx,2)) + z)*(radius - dz*(-tubex + x) + dx*(-tubez + z)))/(Power (width,3)*Power (1 + Power (radius - dz*(-tubex + x) + dx*(-tubez + z),2)/Power(width,2),2)) + dx/(dz*width*(1 + Power (radius - dz*(-tubex + x) + dx*(-tubez + z),2)/Power(width,2))));
        DD2[1][0]+=factor[factor_index]*((-2*dz*(tubez - (dx*(-tubex + x))/Sqrt(1 - Power (dx,2)) - z)*(radius + dz*(-tubex + x) - dx*(-tubez + z)))/(Power (width,3)*Power (1 + Power (radius + dz*(-tubex + x) - dx*(-tubez + z),2)/Power(width,2),2)) - dx/(dz*width*(1 + Power (radius + dz*(-tubex + x) - dx*(-tubez + z),2)/Power(width,2))) + (2*dz*(-tubez + (dx*(-tubex + x))/Sqrt(1 - Power (dx,2)) + z)*(radius - dz*(-tubex + x) + dx*(-tubez + z)))/(Power (width,3)*Power (1 + Power (radius - dz*(-tubex + x) + dx*(-tubez + z),2)/Power(width,2),2)) + dx/(dz*width*(1 + Power (radius - dz*(-tubex + x) + dx*(-tubez + z),2)/Power(width,2))));
        DD2[1][1]+=factor[factor_index]*((-2*Power (tubez - (dx*(-tubex + x))/Sqrt(1 - Power (dx,2)) - z,2)*(radius + dz*(-tubex + x) - dx*(-tubez + z)))/(Power (width,3)*Power (1 + Power (radius + dz*(-tubex + x) - dx*(-tubez + z),2)/Power(width,2),2)) + (-((Power (dx,2)*(-tubex + x))/Power(1 - Power (dx,2),1.5)) - (-tubex + x)/Sqrt(1 - Power (dx,2)))/(width*(1 + Power (radius + dz*(-tubex + x) - dx*(-tubez + z),2)/Power(width,2))) - (2*Power (-tubez + (dx*(-tubex + x))/Sqrt(1 - Power (dx,2)) + z,2)*(radius - dz*(-tubex + x) + dx*(-tubez + z)))/(Power (width,3)*Power (1 + Power (radius - dz*(-tubex + x) + dx*(-tubez + z),2)/Power(width,2),2)) + ((Power (dx,2)*(-tubex + x))/Power(1 - Power (dx,2),1.5) + (-tubex + x)/Sqrt(1 - Power (dx,2)))/(width*(1 + Power (radius - dz*(-tubex + x) + dx*(-tubez + z),2)/Power(width,2))));

    }

    if(tubecount<4){
        TRDTrack_x=x;
        TRDTrack_dx=dx;
        return;
    }




    M_MyDD=new TMatrixD(2,2);
    M_A = new TMatrixD(1,2);
    M_MyD=new TMatrixD(2,1);

    M_MyDD->Use(2,2,DD2[0]);
    M_MyD->Use(2,1,DD1);
    TMatrixD M_MyDDInvert=*M_MyDD;
    M_MyDDInvert.Invert();
    M_A->Mult(M_MyDDInvert,*M_MyD);
    *M_A *=-1;

    TRDTrack_x=x+TMatrixDRow(*M_A,0)(0);
    TRDTrack_dx=dx+TMatrixDRow(*M_A,0)(1);


    if(M_MyDD)delete M_MyDD;
    if(M_MyD)delete M_MyD;
    if(M_A)delete M_A;


    return;
}


/////////////////////////////////////////////////////////////////////

void TrdKCluster::FitTRDTrack_PathLength(int particle_hypothesis){


    TVirtualFitter::SetDefaultFitter("Minuit");
    gMinuit_TRDTrack = TVirtualFitter::Fitter(0, 4);
    Refit_hypothesis=particle_hypothesis;
    gMinuit_TRDTrack->SetFCN(fcn_TRDTrack_PathLength);
    gMinuit_TRDTrack->SetObjectFit(this);
    double arglist[10];
    arglist[0]=-1;
    gMinuit_TRDTrack->ExecuteCommand("SET PRINT", arglist, 1);
    gMinuit_TRDTrack->ExecuteCommand("SET NOW", arglist, 0);

    float init_z0=115;
    float init_x0=0;
    float init_y0=0;
    float init_dx=0;
    float init_dy=0;

    Propogate_TrTrack(init_z0);
    AMSPoint P0=GetPropogated_TrTrack_P0();
    AMSPoint Dir=GetPropogated_TrTrack_Dir();
    init_x0=P0.x();
    init_y0=P0.y();
    init_dx=Dir.x();
    init_dy=Dir.y();

    //    printf("TrTrack : (%.2f, %.2f, %.2f), (%.2f, %.2f, %.2f)\n",P0.x(),P0.y(),P0.z(),Dir.x(),Dir.y(),Dir.z());

    gMinuit_TRDTrack->SetParameter(0,  "x0",  init_x0,  0.2,  init_x0-10,init_x0+10);
    gMinuit_TRDTrack->SetParameter(1,  "y0",  init_y0,  0.2,  init_y0-10,init_y0+10);
    gMinuit_TRDTrack->SetParameter(2,  "z0",  init_z0,0,init_z0,init_z0);
    gMinuit_TRDTrack->SetParameter(3,  "dx0", init_dx,  0.1,  -1,1);
    gMinuit_TRDTrack->SetParameter(4,  "dy0", init_dy,  0.1,  -1,1);

    arglist[0]=0;

    gMinuit_TRDTrack->FixParameter(2);
    gMinuit_TRDTrack->ExecuteCommand("MIGRAD", arglist, 0);


    float x0,y0,z0,dx,dy,dz;

    x0=gMinuit_TRDTrack->GetParameter(0);
    y0=gMinuit_TRDTrack->GetParameter(1);
    z0=gMinuit_TRDTrack->GetParameter(2);
    dx=gMinuit_TRDTrack->GetParameter(3);
    dy=gMinuit_TRDTrack->GetParameter(4);
    dz=TMath::Sqrt(1-dx*dx-dy*dy);

    AMSDir TRDDir(dx,dy,dz);
    AMSPoint TRDP0(x0,y0,z0);
    SetTRDTrack(&TRDP0,&TRDDir);
}








/////////////////////////////////////////////////////////////////////

void TrdKCluster::FitTRDTrack_Analytical(){

    int count=0;


    AMSPoint P0=track_extrapolated_P0;
    AMSDir Dir=track_extrapolated_Dir;


    while (count<2){
        double track_xz_x;
        double track_xz_z=P0.z();
        double track_xz_dx;
        AnalyticalFit_2D(0,P0.x(),P0.z(),Dir.x(),Dir.z(),track_xz_x,track_xz_dx);


        double track_yz_y;
        double track_yz_z=P0.z();
        double track_yz_dy;
        AnalyticalFit_2D(1,P0.y(),P0.z(),Dir.y(),Dir.z(),track_yz_y,track_yz_dy);



        P0= AMSPoint(track_xz_x,track_yz_y,track_xz_z);


        double track_yz_dz=sqrt(1-track_yz_dy*track_yz_dy);
        double track_xz_dz=sqrt(1-track_xz_dx*track_xz_dx);
        double ratio=track_xz_dz/track_yz_dz;
        track_yz_dy*=ratio;
        track_yz_dz*=ratio;
        double norm=sqrt(track_xz_dx*track_xz_dx+track_yz_dy*track_yz_dy+track_xz_dz*track_xz_dz);

        Dir= AMSDir(track_xz_dx/norm,track_yz_dy/norm,track_xz_dz/norm);
        count++;
    }

    SetTRDTrack(&P0,&Dir);
}



/////////////////////////////////////////////////////////////////////

void TrdKCluster::SetTrTrack(AMSPoint *P0, AMSDir *Dir, float Rigidity){

    track_extrapolated_Dir=*Dir;
    track_extrapolated_P0=*P0;
    TrTrack_Pro=TrProp(track_extrapolated_P0,track_extrapolated_Dir,Rigidity);
    if(Rigidity!=0)Track_Rigidity=Rigidity;
    if(Rigidity==0 && Track_Rigidity==0)cout<<"~~~WARNING~~~TrdKCluster,  Track Rigidity is 0"<<endl;


    //    for(int i=0;i<NHits();i++){
    //        GetHit(i)->PredictCrossing(&track_extrapolated_P0,&track_extrapolated_Dir);
    //    }

    return;
}


/////////////////////////////////////////////////////////////////////

void TrdKCluster::SetTRDTrack(AMSPoint *P0, AMSDir *Dir, float Rigidity){

    TRDtrack_extrapolated_Dir=*Dir;
    TRDtrack_extrapolated_P0=*P0;
    if(Rigidity!=0)TRDTrack_Rigidity=Rigidity;
    else TRDTrack_Rigidity=Track_Rigidity;
    HasTRDTrack=1;


    //    for(int i=0;i<NHits();i++){
    //        GetHit(i)->PredictCrossing(&TRDtrack_extrapolated_P0,&TRDtrack_extrapolated_Dir,1);
    //    }

    return;
}



/////////////////////////////////////////////////////////////////////

void TrdKCluster::SetTrTrack(TrTrackR* track, int fitcode){

    if(fitcode>=0){
        track_extrapolated_Dir=  AMSDir(0,0,0);
        track_extrapolated_P0= AMSPoint(0,0,0);
        track->Interpolate(TRDCenter,track_extrapolated_P0,track_extrapolated_Dir,fitcode);
        TrTrack_Pro=TrProp(track_extrapolated_P0,track_extrapolated_Dir,track->GetRigidity(fitcode));
        Track_Rigidity=track->GetRigidity(fitcode);
    }else{
        cout<<"~~~WARNING~~~ TrdKCluster Constructor, fitcode do not exist : "<<fitcode<<endl;
        IsValid=-1;
    }

    //    for(int i=0;i<NHits();i++){
    //        GetHit(i)->PredictCrossing(&track_extrapolated_P0,&track_extrapolated_Dir);
    //    }

    return;
}





/////////////////////////////////////////////////////////////////////

int TrdKCluster::GetLikelihoodRatio_TRDRefit(float threshold, double* LLR, int &nhits, int fitmethod, int particle_hypothesis){

    if(HasTRDTrack==0 || fitmethod>0)   FitTRDTrack(fitmethod,particle_hypothesis);

    return GetLikelihoodRatio(threshold,LLR,nhits,&TRDtrack_extrapolated_P0,&TRDtrack_extrapolated_Dir);
}


/////////////////////////////////////////////////////////////////////

int TrdKCluster::GetLikelihoodRatio_TrTrack(float threshold, double* LLR, int &nhits){
    return GetLikelihoodRatio(threshold,LLR,nhits,&track_extrapolated_P0,&track_extrapolated_Dir);
}


/////////////////////////////////////////////////////////////////////

int TrdKCluster::GetLikelihoodRatio(float threshold, double* LLR, int &nhits, AMSPoint* P0, AMSDir* Dir){

    LLR[0]=-1;
    LLR[1]=-1;
    LLR[2]=-1;
    nhits=0;


    if(threshold>0){
        kpdf_e->SetNormalization(1,threshold);
        kpdf_p->SetNormalization(1,threshold);
        kpdf_h->SetNormalization(1,threshold);
    }


    if(NHits()<=0) return -10;

    int Track_nhits=0;
    double LL_pdf_track_particle[3]={1,1,1};
    double Likelihood_pdf_track_particle[3]={0,0,0};
    for(int i=0;i<NHits();i++){
        TrdKHit *hit=GetHit(i);

        if(ForceReadCalibration>0 && hit->IsCalibrated ==0 ){
            if(DebugOn)  cout<<"~~~WARNING~~~, Hit Not calibrated: "<<_event->Run()<<", "<<_event->Event()<<endl;
            continue;
        }
        if(ForceReadAlignment>0 && hit->IsAligned==0) {
            if(DebugOn) cout<<"~~~WARNING~~~, Hit Not Aligned: , "<<_event->Run()<<", "<<_event->Event()<<endl;
            continue;
        }

        float path_length=hit->Tube_Track_3DLength(P0,Dir);
        int Layer=hit->TRDHit_Layer;
        float Amp=hit->TRDHit_Amp;
        if(Amp>threshold && path_length>0){
            Track_nhits++;
            kpdf_e->GetPar(fabs(Track_Rigidity),path_length,Layer,Pressure_Xe/1000.);
            kpdf_p->GetPar(fabs(Track_Rigidity),path_length,Layer,Pressure_Xe/1000.);
            kpdf_h->GetPar(fabs(Track_Rigidity),path_length,Layer,Pressure_Xe/1000.);
            double kp=kpdf_p->GetLikelihood(Amp);
            double kh=kpdf_h->GetLikelihood(Amp);
            double ke=kpdf_e->GetLikelihood(Amp);
            LL_pdf_track_particle[0]*=(ke);
            LL_pdf_track_particle[1]*=(kp);
            LL_pdf_track_particle[2]*=(kh);

        }
    }


    if(Track_nhits<=0)  return -9;
    for(int i=0;i<3;i++) Likelihood_pdf_track_particle[i]=pow(LL_pdf_track_particle[i],(double)(1./(double)Track_nhits));
    LLR[0]=-1*log( Likelihood_pdf_track_particle[0]/( Likelihood_pdf_track_particle[0]+Likelihood_pdf_track_particle[1]));
    LLR[1]=-1*log( Likelihood_pdf_track_particle[0]/( Likelihood_pdf_track_particle[0]+Likelihood_pdf_track_particle[2]));
    LLR[2]=-1*log( Likelihood_pdf_track_particle[1]/( Likelihood_pdf_track_particle[1]+Likelihood_pdf_track_particle[2]));
    nhits=Track_nhits;

    return 1;
}


/////////////////////////////////////////////////////////////////////

void TrdKCluster::InitXePressure(){

    cout<<"Initilize Online TRD Parameters map..........."<<endl;

    //================================
    vector<ULong64_t> v_time;
    vector<Double_t> v_xe;
    vector<Double_t> v_co2;
    //TFile* f_online=new TFile("/afs/cern.ch/user/z/zweng02/public/trd_pressures.root");

    const char *amsdatadir=getenv("AMSDataDir");
    char local[]="/afs/cern.ch/ams/Offline/AMSDataDir";
     if(!(amsdatadir && strlen(amsdatadir))){
         amsdatadir=local;
    }

    TString name=TString(amsdatadir)+"/v5.00/trd_pressures.root";
    cout<<"Read TRD Pressure root files from: "<<name<<endl;
    TFile* f_online=new TFile(name);

    ULong64_t       time;
    Double_t        xe;
    Double_t        co2;
    // List of branches
    TBranch        *b_t;   //!
    TBranch        *b_xe;   //!
    TBranch        *b_co2;   //!

    TTree* fChain_presure=(TTree*)f_online->Get("trd_online");
    fChain_presure->SetBranchAddress("time", &time, &b_t);
    fChain_presure->SetBranchAddress("xe", &xe, &b_xe);
    fChain_presure->SetBranchAddress("co2", &co2, &b_co2);
    int n=fChain_presure->GetEntries();
    for(int i=0;i<n;i++){
        fChain_presure->GetEntry(i);
        v_time.push_back(time);
        v_xe.push_back(xe);
        v_co2.push_back(co2);
        map_TRDOnline[time]=TRDOnline(time,xe,co2);
    }
    for( map<int, TRDOnline>::iterator ii=map_TRDOnline.begin(); ii!=map_TRDOnline.end(); ++ii)
    {
        TRDOnline test=(*ii).second;
        printf("TRD Online:  %i  %.2f  %.2f\n",test.time,test.xe,test.co2);
    }


}



/////////////////////////////////////////////////////////////////////

int TrdKCluster::GetXePressure(){
    if(map_TRDOnline.size()==0)return -1;
    map<int, TRDOnline>::iterator  it=map_TRDOnline.upper_bound((int)Time);
    map<int, TRDOnline>::iterator  ithigh=it;
    map<int, TRDOnline>::iterator  itlow=--it;
    TRDOnline p_previous=(*itlow).second;
    TRDOnline p_next=(*ithigh).second;
    Pressure_Xe = p_previous.xe+(Time-p_previous.time)*(p_next.xe-p_previous.xe)/(p_next.time-p_previous.time);
    return Pressure_Xe;
}


/////////////////////////////////////////////////////////////////////

void TrdKCluster::GetOffTrackHit_TrTrack(int& nhits, float & amp){
    GetOffTrackHit(nhits,amp,&track_extrapolated_P0,&track_extrapolated_Dir);
}

/////////////////////////////////////////////////////////////////////

void TrdKCluster::GetOffTrackHit_TRDRefit(int& nhits, float & amp){
    if(HasTRDTrack==0){
        cout<<"~~~WARNING~~~~TrdKCluster, Get OffTrackHit from TRDRefit, TRDTrack not yet defined"<<endl;
        nhits=-1;
        amp=-1;
        return;
    }

    GetOffTrackHit(nhits,amp,&TRDtrack_extrapolated_P0,&TRDtrack_extrapolated_Dir);
}

/////////////////////////////////////////////////////////////////////

void TrdKCluster::GetOffTrackHit(int& nhits, float & amp,  AMSPoint* P0, AMSDir* Dir){
    nhits=0;
    amp=0;
    for(int i=0;i<NHits();i++){
        TrdKHit *hit=GetHit(i);
        float path_length=hit->Tube_Track_3DLength(P0,Dir);
        float Amp=hit->TRDHit_Amp;
        if(Amp>0 && path_length==0){
            nhits++;
            amp+=Amp;
        }
    }
    return;
}


/////////////////////////////////////////////////////////////////////

int TrdKCluster::GetTRDRefittedTrack(AMSPoint &P0, AMSDir &Dir){

    if(!HasTRDTrack){
        printf("~~~WARNING~~~TrdKCluster, TRD Track Refitted Not exist yet, please performthe refit before getting the track~~~\n");
        return 0;
    }

    P0=TRDtrack_extrapolated_P0;
    Dir=TRDtrack_extrapolated_Dir;
    return 1;

}

/////////////////////////////////////////////////////////////////////

int TrdKCluster::GetTrTrackExtrapolation(AMSPoint &P0, AMSDir &Dir){

    if(GetValidity()==-1){
        cout<<"~ERROR~ TrdKCluster, In valid Fitcode passed by SetTrTrack ."<<endl;
        return -1;
    }

    P0=track_extrapolated_P0;
    Dir=track_extrapolated_Dir;

    return 1;
}


//================TRD Charge Estimation=====================


void TrdKCluster::fcn_TRDTrack_Charge(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag)
{
    TrdKCluster *pTrdKCluster=(TrdKCluster*)gMinuit_TRDTrack->GetObjectFit();
    f=pTrdKCluster->TRDTrack_ImpactChi2_Charge(par);
}

/////////////////////////////////////////////////////////////////////

double TrdKCluster::TRDTrack_ImpactChi2_Charge(Double_t *par)
{
    double result=0;
    AMSPoint temp_TrTrackP0(par[0],par[1],par[2]);
    AMSDir  temp_TrTrackDir(par[3],par[4],sqrt(1-par[3]*par[3]-par[4]*par[4]));
    double Amp,impact_parameter,likelihood;
    for(vector<TrdKHit>::iterator it=QTRDHitCollectionRefit.begin();it!=QTRDHitCollectionRefit.end();it++)
    {
        impact_parameter=(*it).Tube_Track_Distance_3D(&temp_TrTrackP0,&temp_TrTrackDir);
        Amp=(*it).TRDHit_Amp;

        likelihood=TRDImpactlikelihood->GetLikelihood(Amp,impact_parameter);
        if(likelihood<0.0000001) likelihood=0.0000001;

        if(Amp<=0) result-=2*log(likelihood);
        else result-=2*Amp*log(likelihood);
    }
    return result;
}

/////////////////////////////////////////////////////////////////////

int TrdKCluster::CalculateTRDCharge(int Option, double Velocity)
{
    double BigNumber=100000000; 

    //For beta correction
    Beta=Velocity;

    //Refit TRD track
    if(QTRDHitCollection.size()==0)
    {
        HitSelectionBeforeRefit(3);
        FitTRDTrack_IPLikelihood(1);
        HitSelectionAfterRefit();
    }

    if(QTRDHitCollectionNuclei.size()<3)
    {
        TRDChargeValue=0;
  	TRDChargeError=BigNumber;
  	return -5;
    }

    //Find minimum of likelihood
    double QTRD,QTRD1;
    double QTRDLikelihood,QTRDLikelihoodMin;
    QTRDLikelihoodMin=BigNumber;
    for(double Z=1;Z<=49;Z=Z+1)
    {
        QTRDLikelihood=GetTRDChargeLikelihood(Z,Option);
        if(QTRDLikelihood>BigNumber) continue;
        if(QTRDLikelihood<QTRDLikelihoodMin)
        {
            QTRD1=Z;
            QTRDLikelihoodMin=QTRDLikelihood;
        }
    }
    QTRDLikelihoodMin=BigNumber;
    for(double Z=QTRD1-1;Z<=QTRD1+1;Z=Z+0.1)
    {
        if(Z==0) continue;
        QTRDLikelihood=GetTRDChargeLikelihood(Z,Option);
        if(QTRDLikelihood>BigNumber) continue;
        if(QTRDLikelihood<QTRDLikelihoodMin)
        {
            QTRD=Z;
            QTRDLikelihoodMin=QTRDLikelihood;
        }
    }

    if(QTRD==0)
    {
        TRDChargeValue=0;
        TRDChargeError=BigNumber;
        return -1;
    }

    //Estimate error using a simple parabola fit
    double x0,x1,x2,y0,y1,y2,a,b,c,ExpErr,ErrorTemp;
    x0=QTRD;
    ExpErr=0.1*x0;
    if(ExpErr>3) ExpErr=3;
    if(ExpErr<0.3) ExpErr=0.3;
    if(x0<ExpErr) x1=0;
    else x1=x0-ExpErr;
    x2=x0+ExpErr;
    y0=QTRDLikelihoodMin;
    y1=GetTRDChargeLikelihood(x1,Option);
    y2=GetTRDChargeLikelihood(x2,Option);
    a=((y1-y0)/(x1-x0)-(y2-y0)/(x2-x0))/(x1-x2);
    b=(y1-y0)*(x2+x0)/(x1-x0)/(x2-x1)-(y2-y0)*(x1+x0)/(x2-x0)/(x2-x1);
    c=y0-a*x0*x0-b*x0;
    //  TRDChargeValue=-b/a/2;
    //  TRDChargeError=1/sqrt(2*a);
    ErrorTemp=1/sqrt(2*a);

    //Parabola fit to get minimum and error
    double X[7],Y[7],NFitPoint;
    double X0,DD,alpha,beta,gamma;
    double m11,m12,m13,m22,m23,m33,a11,a12,a13,a22,a23,a33,b1,b2,b3;

    X[3]=QTRD;
    Y[3]=QTRDLikelihoodMin;

    double Sigma=0.1*((int)(ErrorTemp*10));
    if(Sigma<0.2) Sigma=0.2;
    if(Sigma>3) Sigma=3;
    X[0]=QTRD-0.1*((int)(Sigma*15));
    X[6]=QTRD+0.1*((int)(Sigma*15));
    X[1]=QTRD-0.1*((int)(Sigma*10));
    X[5]=QTRD+0.1*((int)(Sigma*10));
    X[2]=QTRD-0.1*((int)(Sigma*5));
    X[4]=QTRD+0.1*((int)(Sigma*5));

    X0=0;
    NFitPoint=0;
    for(int i=0;i<7;i++)
    {
        if(i==3)
        {
            NFitPoint++;
            continue;
        }
        if(X[i]<=0) continue;
        X0=X0+X[i];
        Y[i]=GetTRDChargeLikelihood(X[i],Option);
        NFitPoint++;
    }
    X0=X0/NFitPoint; //Average X

    m11=0;
    m12=0;
    m13=0;
    m23=0;
    b1=0;
    b2=0;
    b3=0;
    for(int i=0;i<7;i++) //Build matrix
    {
        if(X[i]<=0) continue;
        X[i]=X[i]-X0;
        m11=m11+X[i]*X[i]*X[i]*X[i];
        m12=m12+X[i]*X[i]*X[i];
        m13=m13+X[i]*X[i];
        m23=m23+X[i];
        b1=b1+Y[i]*X[i]*X[i];
        b2=b2+Y[i]*X[i];
        b3=b3+Y[i];
    }
    m22=m13;
    m33=NFitPoint;
    DD=m11*m22*m33+2*m12*m13*m23-m13*m13*m22-m12*m12*m33-m11*m23*m23;
    if(DD==0)
    {
        TRDChargeValue=0;
        TRDChargeError=BigNumber;
        return -2;
    }
    a11 = m22*m33 - m23*m23;
    a12 = m13*m23 - m12*m33;
    a13 = m12*m23 - m13*m22;
    a22 = m11*m33 - m13*m13;
    a23 = m12*m13 - m11*m23;
    a33 = m11*m22 - m12*m12;

    alpha = b1*a11 + b2*a12 + b3*a13;
    beta  = b1*a12 + b2*a22 + b3*a23;
    gamma = b1*a13 + b2*a23 + b3*a33;

    if(alpha/DD<=0)
    {
        TRDChargeValue=0;
        TRDChargeError=BigNumber;
        return -3;
    }

    TRDChargeValue=X0-0.5*beta/alpha;
    TRDChargeError=sqrt(DD/2/alpha);

    if(TRDChargeValue<0)
    {
      TRDChargeValue=0;
      TRDChargeError=BigNumber;
      return -4;
    }

    return 0;

}

/////////////////////////////////////////////////////////////////////

double TrdKCluster::GetTRDChargeLikelihood(double Z,int Option)
{
    double ADC,Length,Corr;
    int TRDLayer;
    double TRDChargeLikelihood=0;

    //If too many dE/dX tubes are saturated, then use delta ray PDF only
    if(NBelowThreshold<3 && Option==0) Option=2;

    //Only delta electron PDF
    if(Option==2)
    {
        double DAmpL=DAmp/20.0;
        TRDChargeLikelihood=TRDChargeLikelihood-log10(kpdf_q->GetLikelihoodDR(DAmpL,Z,Track_Rigidity));
        return TRDChargeLikelihood;
    }

    //Preparation for finding the hit with least likelihood
    double max_one=0;
    double max_two=0;
    double Lvalue;
    for(vector<TrdKHit>::iterator it=QTRDHitCollectionNuclei.begin();it!=QTRDHitCollectionNuclei.end();it++)
    {
      //Some parameters in TrdKPDF
      TRDLayer=(*it).TRDHit_Layer;
      ADC=(*it).TRDHit_Amp;
      Length=(*it).Tube_Track_3DLength(&TRDtrack_extrapolated_P0,&TRDtrack_extrapolated_Dir);
      Corr=_DB_instance->GetGainCorrectionFactorTube((*it).tubeid,Time);

      Lvalue=-log10(kpdf_q->GetLikelihood(ADC,Z,Corr,Track_Rigidity,Length,TRDLayer,Pressure_Xe/1000.0,Beta));
      if(Lvalue>max_one) max_one=Lvalue;
    }

    //Only dE/dX PDF
    if(Option==1)
    {
        for(vector<TrdKHit>::iterator it=QTRDHitCollectionNuclei.begin();it!=QTRDHitCollectionNuclei.end();it++)
        {
            //Some parameters in TrdKPDF
            TRDLayer=(*it).TRDHit_Layer;
            ADC=(*it).TRDHit_Amp;
            Length=(*it).Tube_Track_3DLength(&TRDtrack_extrapolated_P0,&TRDtrack_extrapolated_Dir);
            Corr=_DB_instance->GetGainCorrectionFactorTube((*it).tubeid,Time);

            Lvalue=-log10(kpdf_q->GetLikelihood(ADC,Z,Corr,Track_Rigidity,Length,TRDLayer,Pressure_Xe/1000.0,Beta));
            TRDChargeLikelihood=TRDChargeLikelihood+Lvalue;
	    if(Lvalue!=max_one && Lvalue>max_two) max_two=Lvalue;
        }
        TRDChargeLikelihood=TRDChargeLikelihood-max_one-max_two;
        return TRDChargeLikelihood;
    }

    //Both delta electron and dE/dX PDF
    for(vector<TrdKHit>::iterator it=QTRDHitCollectionNuclei.begin();it!=QTRDHitCollectionNuclei.end();it++)
    {
        //Some parameters in TrdKPDF
        TRDLayer=(*it).TRDHit_Layer;
        ADC=(*it).TRDHit_Amp;
        Length=(*it).Tube_Track_3DLength(&TRDtrack_extrapolated_P0,&TRDtrack_extrapolated_Dir);
        Corr=_DB_instance->GetGainCorrectionFactorTube((*it).tubeid,Time);

	Lvalue=-log10(kpdf_q->GetLikelihood(ADC,Z,Corr,Track_Rigidity,Length,TRDLayer,Pressure_Xe/1000.0,Beta));
	TRDChargeLikelihood=TRDChargeLikelihood+Lvalue;
	if(Lvalue!=max_one && Lvalue>max_two) max_two=Lvalue;
    }
    TRDChargeLikelihood=TRDChargeLikelihood-max_one-max_two;
    double DAmpL=DAmp/20.0;
    TRDChargeLikelihood=TRDChargeLikelihood-log10(kpdf_q->GetLikelihoodDR(DAmpL,Z,Track_Rigidity));

    return TRDChargeLikelihood;
}


/////////////////////////////////////////////////////////////////////



void TrdKCluster::HitSelectionBeforeRefit(double DistanceCut)
{
    //Collect possible useful hits into TRDHitCollection, do calibration and alignment
    double ADC,Distance,ADC_Max[20];
    int TRDLayer;
    for(int i=0;i<20;i++) ADC_Max[i]=0;
    for(vector<TrdKHit>::iterator it=TRDHitCollection.begin();it!=TRDHitCollection.end();it++)
    {
        //TRDHit variables
        ADC=(*it).TRDHit_Amp;
        TRDLayer=(*it).TRDHit_Layer;
        Distance=fabs((*it).Tube_Track_Distance_3D(&track_extrapolated_P0,&track_extrapolated_Dir));

        //Remove distance > DistanceCutBig and ADC <= 0
        if(Distance>DistanceCut || ADC<=0) continue;

        QTRDHitCollection.push_back(*it);

        //Find the highest amplitude in each layer
        if(ADC>ADC_Max[TRDLayer]) ADC_Max[TRDLayer]=ADC;
    }

    //Find the second highest amplitude and coordinate of the hit with highest amplitude in each layer
    double ADC_SecMax[20];
    for(int i=0;i<20;i++) ADC_SecMax[i]=0;
    AMSPoint TRDHitCoo_MaxADC[20];
    for(int i=0;i<20;i++) TRDHitCoo_MaxADC[i]=AMSPoint(0,0,0);
    for(vector<TrdKHit>::iterator it=QTRDHitCollection.begin();it!=QTRDHitCollection.end();it++)
    {
        //TRDHit variables
        ADC=(*it).TRDHit_Amp;
        TRDLayer=(*it).TRDHit_Layer;

        //Search
        if(ADC==ADC_Max[TRDLayer]) TRDHitCoo_MaxADC[TRDLayer]=(*it).TRDTube_Center;
        else if(ADC>ADC_SecMax[TRDLayer]) ADC_SecMax[TRDLayer]=ADC;
    }

    //TRDHitCollectionRefit
    double Distance1D;
    for(vector<TrdKHit>::iterator it=QTRDHitCollection.begin();it!=QTRDHitCollection.end();it++)
    {
        //TRDHit variables
        ADC=(*it).TRDHit_Amp;
        TRDLayer=(*it).TRDHit_Layer;

        //Distance1D
        if(TRDLayer<16 && TRDLayer>3)
        {
            Distance1D=(*it).TRDTube_Center.x()-TRDHitCoo_MaxADC[TRDLayer].x();
        }
        else
        {
            Distance1D=(*it).TRDTube_Center.y()-TRDHitCoo_MaxADC[TRDLayer].y();
        }

        //Collect hits used for refit
        if(ADC==ADC_Max[TRDLayer]) QTRDHitCollectionRefit.push_back(*it);
        else if(ADC==ADC_SecMax[TRDLayer] && Distance1D<0.9) QTRDHitCollectionRefit.push_back(*it);
    }
}

/////////////////////////////////////////////////////////////////////

void TrdKCluster::HitSelectionAfterRefit()
{
    double Length;
    int TRDLayer;
    double ADC,Corr;
    DAmp=0;
    DAmpUpper=0; 
    DAmpLower=0; 
    NBelowThreshold=0;
    NBelowThresholdUpper=0; 
    NBelowThresholdLower=0; 
    for(vector<TrdKHit>::iterator it=QTRDHitCollection.begin();it!=QTRDHitCollection.end();it++)
    {
        Length=(*it).Tube_Track_3DLength(&TRDtrack_extrapolated_P0,&TRDtrack_extrapolated_Dir);
        TRDLayer=(*it).TRDHit_Layer;
	ADC=(*it).TRDHit_Amp;
	Corr=_DB_instance->GetGainCorrectionFactorTube((*it).tubeid,Time);
        if(Length>0) 
	{
	  if(3400*Corr>ADC) NBelowThreshold++;
	  QTRDHitCollectionNuclei.push_back(*it);
	  if(TRDLayer<10)
	  {
	    if(3400*Corr>ADC) NBelowThresholdLower++;
	    QTRDHitCollectionNucleiLower.push_back(*it);
	  }
	  else
	  {
	    if(3400*Corr>ADC) NBelowThresholdUpper++;
	    QTRDHitCollectionNucleiUpper.push_back(*it);
	  }
	}
        else
        {
            QTRDHitCollectionDeltaRay.push_back(*it);
            DAmp=DAmp+(*it).TRDHit_Amp;
	    if(TRDLayer<10) DAmpLower=DAmpLower+(*it).TRDHit_Amp;
	    else DAmpUpper=DAmpUpper+(*it).TRDHit_Amp;
        }
    }
}

/////////////////////////////////////////////////////////////////////

double TrdKCluster::GetTRDChargeLikelihoodUpper(double Z)
{
    double ADC,Length,Corr;
    int TRDLayer;
    double TRDChargeLikelihood=0;

    //If too many dE/dX tubes are saturated, then use delta ray PDF only
    if(NBelowThresholdUpper<2)
    {
        double DAmpL=DAmpUpper/10.0;
        TRDChargeLikelihood=TRDChargeLikelihood-log10(kpdf_q->GetLikelihoodDR(DAmpL,Z,Track_Rigidity));
        return TRDChargeLikelihood;
    }

    //Both delta electron and dE/dX PDF
    double max_one=0; //Remove the least likelihood for each charge assumption
    double Lvalue;
    for(vector<TrdKHit>::iterator it=QTRDHitCollectionNucleiUpper.begin();it!=QTRDHitCollectionNucleiUpper.end();it++)
    {
        //Some parameters in TrdKPDF
        TRDLayer=(*it).TRDHit_Layer;
        ADC=(*it).TRDHit_Amp;
        Length=(*it).Tube_Track_3DLength(&TRDtrack_extrapolated_P0,&TRDtrack_extrapolated_Dir);
        Corr=_DB_instance->GetGainCorrectionFactorTube((*it).tubeid,Time);

        Lvalue=-log10(kpdf_q->GetLikelihood(ADC,Z,Corr,Track_Rigidity,Length,TRDLayer,Pressure_Xe/1000.0,Beta));
        TRDChargeLikelihood=TRDChargeLikelihood+Lvalue;
        if(Lvalue>max_one) max_one=Lvalue;
    }
    TRDChargeLikelihood=TRDChargeLikelihood-max_one;
    double DAmpL=DAmpUpper/10.0;
    TRDChargeLikelihood=TRDChargeLikelihood-log10(kpdf_q->GetLikelihoodDR(DAmpL,Z,Track_Rigidity));
    return TRDChargeLikelihood;
}

/////////////////////////////////////////////////////////////////////

double TrdKCluster::GetTRDChargeLikelihoodLower(double Z)
{
    double ADC,Length,Corr;
    int TRDLayer;
    double TRDChargeLikelihood=0;

    //If too many dE/dX tubes are saturated, then use delta ray PDF only
    if(NBelowThresholdLower<2)
    {
        double DAmpL=DAmpLower/10.0;
        TRDChargeLikelihood=TRDChargeLikelihood-log10(kpdf_q->GetLikelihoodDR(DAmpL,Z,Track_Rigidity));
        return TRDChargeLikelihood;
    }

    //Both delta electron and dE/dX PDF
    double max_one=0; //Remove the least likelihood for each charge assumption
    double Lvalue;
    for(vector<TrdKHit>::iterator it=QTRDHitCollectionNucleiLower.begin();it!=QTRDHitCollectionNucleiLower.end();it++)
    {
        //Some parameters in TrdKPDF
        TRDLayer=(*it).TRDHit_Layer;
        ADC=(*it).TRDHit_Amp;
        Length=(*it).Tube_Track_3DLength(&TRDtrack_extrapolated_P0,&TRDtrack_extrapolated_Dir);
        Corr=_DB_instance->GetGainCorrectionFactorTube((*it).tubeid,Time);

        Lvalue=-log10(kpdf_q->GetLikelihood(ADC,Z,Corr,Track_Rigidity,Length,TRDLayer,Pressure_Xe/1000.0,Beta));
        TRDChargeLikelihood=TRDChargeLikelihood+Lvalue;
        if(Lvalue>max_one) max_one=Lvalue;
    }
    TRDChargeLikelihood=TRDChargeLikelihood-max_one;
    double DAmpL=DAmpLower/10.0;
    TRDChargeLikelihood=TRDChargeLikelihood-log10(kpdf_q->GetLikelihoodDR(DAmpL,Z,Track_Rigidity));
    return TRDChargeLikelihood;
}

/////////////////////////////////////////////////////////////////////

double TrdKCluster::GetTRDChargeUpper()
{
    double TRDChargeValueUpper;

    if(QTRDHitCollectionNucleiUpper.size()<2) return 0;

    //Find minimum of likelihood
    double QTRD,QTRD1;
    double QTRDLikelihood,QTRDLikelihoodMin;
    double BigNumber=100000000;
    QTRDLikelihoodMin=BigNumber;
    for(double Z=1;Z<=49;Z=Z+1)
    {
        QTRDLikelihood=GetTRDChargeLikelihoodUpper(Z);
        if(QTRDLikelihood>BigNumber) continue;
        if(QTRDLikelihood<QTRDLikelihoodMin)
        {
            QTRD1=Z;
            QTRDLikelihoodMin=QTRDLikelihood;
        }
    }
    QTRDLikelihoodMin=BigNumber;
    for(double Z=QTRD1-1;Z<=QTRD1+1;Z=Z+0.1)
    {
        if(Z==0) continue;
        QTRDLikelihood=GetTRDChargeLikelihoodUpper(Z);
        if(QTRDLikelihood>BigNumber) continue;
        if(QTRDLikelihood<QTRDLikelihoodMin)
        {
            QTRD=Z;
            QTRDLikelihoodMin=QTRDLikelihood;
        }
    }

    if(QTRD==0) return 0;

    //Estimate error using a simple parabola fit
    double x0,x1,x2,y0,y1,y2,a,b,c,ExpErr,ErrorTemp;
    x0=QTRD;
    ExpErr=0.1*x0;
    if(ExpErr>3) ExpErr=3;
    if(ExpErr<0.3) ExpErr=0.3;
    if(x0<ExpErr) x1=0;
    else x1=x0-ExpErr;
    x2=x0+ExpErr;
    y0=QTRDLikelihoodMin;
    y1=GetTRDChargeLikelihoodUpper(x1);
    y2=GetTRDChargeLikelihoodUpper(x2);
    a=((y1-y0)/(x1-x0)-(y2-y0)/(x2-x0))/(x1-x2);
    b=(y1-y0)*(x2+x0)/(x1-x0)/(x2-x1)-(y2-y0)*(x1+x0)/(x2-x0)/(x2-x1);
    c=y0-a*x0*x0-b*x0;
    //  TRDChargeValue=-b/a/2;
    //  TRDChargeError=1/sqrt(2*a);
    ErrorTemp=1/sqrt(2*a);

    //Parabola fit to get minimum and error
    double X[7],Y[7],NFitPoint;
    double X0,DD,alpha,beta,gamma;
    double m11,m12,m13,m22,m23,m33,a11,a12,a13,a22,a23,a33,b1,b2,b3;

    X[3]=QTRD;
    Y[3]=QTRDLikelihoodMin;

    double Sigma=0.1*((int)(ErrorTemp*10));
    if(Sigma<0.2) Sigma=0.2;
    if(Sigma>3) Sigma=3;
    X[0]=QTRD-0.1*((int)(Sigma*15));
    X[6]=QTRD+0.1*((int)(Sigma*15));
    X[1]=QTRD-0.1*((int)(Sigma*10));
    X[5]=QTRD+0.1*((int)(Sigma*10));
    X[2]=QTRD-0.1*((int)(Sigma*5));
    X[4]=QTRD+0.1*((int)(Sigma*5));

    X0=0;
    NFitPoint=0;
    for(int i=0;i<7;i++)
    {
        if(i==3)
        {
            NFitPoint++;
            continue;
        }
        if(X[i]<=0) continue;
        X0=X0+X[i];
        Y[i]=GetTRDChargeLikelihoodUpper(X[i]);
        NFitPoint++;
    }
    X0=X0/NFitPoint; //Average X

    m11=0;
    m12=0;
    m13=0;
    m23=0;
    b1=0;
    b2=0;
    b3=0;
    for(int i=0;i<7;i++) //Build matrix
    {
        if(X[i]<=0) continue;
        X[i]=X[i]-X0;
        m11=m11+X[i]*X[i]*X[i]*X[i];
        m12=m12+X[i]*X[i]*X[i];
        m13=m13+X[i]*X[i];
        m23=m23+X[i];
        b1=b1+Y[i]*X[i]*X[i];
        b2=b2+Y[i]*X[i];
        b3=b3+Y[i];
    }
    m22=m13;
    m33=NFitPoint;
    DD=m11*m22*m33+2*m12*m13*m23-m13*m13*m22-m12*m12*m33-m11*m23*m23;

    if(DD==0) return 0;

    a11 = m22*m33 - m23*m23;
    a12 = m13*m23 - m12*m33;
    a13 = m12*m23 - m13*m22;
    a22 = m11*m33 - m13*m13;
    a23 = m12*m13 - m11*m23;
    a33 = m11*m22 - m12*m12;

    alpha = b1*a11 + b2*a12 + b3*a13;
    beta  = b1*a12 + b2*a22 + b3*a23;
    gamma = b1*a13 + b2*a23 + b3*a33;

    if(alpha/DD<=0) return 0;

    TRDChargeValueUpper=X0-0.5*beta/alpha;

    if(TRDChargeValueUpper<0) return 0;

    return TRDChargeValueUpper;
}

/////////////////////////////////////////////////////////////////////

double TrdKCluster::GetTRDChargeLower()
{
    double TRDChargeValueLower;

    if(QTRDHitCollectionNucleiLower.size()<2) return 0;

    //Find minimum of likelihood
    double QTRD,QTRD1;
    double QTRDLikelihood,QTRDLikelihoodMin;
    double BigNumber=100000000;
    QTRDLikelihoodMin=BigNumber;
    for(double Z=1;Z<=49;Z=Z+1)
    {
        QTRDLikelihood=GetTRDChargeLikelihoodLower(Z);
        if(QTRDLikelihood>BigNumber) continue;
        if(QTRDLikelihood<QTRDLikelihoodMin)
        {
            QTRD1=Z;
            QTRDLikelihoodMin=QTRDLikelihood;
        }
    }
    QTRDLikelihoodMin=BigNumber;
    for(double Z=QTRD1-1;Z<=QTRD1+1;Z=Z+0.1)
    {
        if(Z==0) continue;
        QTRDLikelihood=GetTRDChargeLikelihoodLower(Z);
        if(QTRDLikelihood>BigNumber) continue;
        if(QTRDLikelihood<QTRDLikelihoodMin)
        {
            QTRD=Z;
            QTRDLikelihoodMin=QTRDLikelihood;
        }
    }

    if(QTRD==0) return 0;

    //Estimate error using a simple parabola fit
    double x0,x1,x2,y0,y1,y2,a,b,c,ExpErr,ErrorTemp;
    x0=QTRD;
    ExpErr=0.1*x0;
    if(ExpErr>3) ExpErr=3;
    if(ExpErr<0.3) ExpErr=0.3;
    if(x0<ExpErr) x1=0;
    else x1=x0-ExpErr;
    x2=x0+ExpErr;
    y0=QTRDLikelihoodMin;
    y1=GetTRDChargeLikelihoodLower(x1);
    y2=GetTRDChargeLikelihoodLower(x2);
    a=((y1-y0)/(x1-x0)-(y2-y0)/(x2-x0))/(x1-x2);
    b=(y1-y0)*(x2+x0)/(x1-x0)/(x2-x1)-(y2-y0)*(x1+x0)/(x2-x0)/(x2-x1);
    c=y0-a*x0*x0-b*x0;
    //  TRDChargeValue=-b/a/2;
    //  TRDChargeError=1/sqrt(2*a);
    ErrorTemp=1/sqrt(2*a);

    //Parabola fit to get minimum and error
    double X[7],Y[7],NFitPoint;
    double X0,DD,alpha,beta,gamma;
    double m11,m12,m13,m22,m23,m33,a11,a12,a13,a22,a23,a33,b1,b2,b3;

    X[3]=QTRD;
    Y[3]=QTRDLikelihoodMin;

    double Sigma=0.1*((int)(ErrorTemp*10));
    if(Sigma<0.2) Sigma=0.2;
    if(Sigma>3) Sigma=3;
    X[0]=QTRD-0.1*((int)(Sigma*15));
    X[6]=QTRD+0.1*((int)(Sigma*15));
    X[1]=QTRD-0.1*((int)(Sigma*10));
    X[5]=QTRD+0.1*((int)(Sigma*10));
    X[2]=QTRD-0.1*((int)(Sigma*5));
    X[4]=QTRD+0.1*((int)(Sigma*5));

    X0=0;
    NFitPoint=0;
    for(int i=0;i<7;i++)
    {
        if(i==3)
        {
            NFitPoint++;
            continue;
        }
        if(X[i]<=0) continue;
        X0=X0+X[i];
        Y[i]=GetTRDChargeLikelihoodLower(X[i]);
        NFitPoint++;
    }
    X0=X0/NFitPoint; //Average X

    m11=0;
    m12=0;
    m13=0;
    m23=0;
    b1=0;
    b2=0;
    b3=0;
    for(int i=0;i<7;i++) //Build matrix
    {
        if(X[i]<=0) continue;
        X[i]=X[i]-X0;
        m11=m11+X[i]*X[i]*X[i]*X[i];
        m12=m12+X[i]*X[i]*X[i];
        m13=m13+X[i]*X[i];
        m23=m23+X[i];
        b1=b1+Y[i]*X[i]*X[i];
        b2=b2+Y[i]*X[i];
        b3=b3+Y[i];
    }
    m22=m13;
    m33=NFitPoint;
    DD=m11*m22*m33+2*m12*m13*m23-m13*m13*m22-m12*m12*m33-m11*m23*m23;

    if(DD==0) return 0;

    a11 = m22*m33 - m23*m23;
    a12 = m13*m23 - m12*m33;
    a13 = m12*m23 - m13*m22;
    a22 = m11*m33 - m13*m13;
    a23 = m12*m13 - m11*m23;
    a33 = m11*m22 - m12*m12;

    alpha = b1*a11 + b2*a12 + b3*a13;
    beta  = b1*a12 + b2*a22 + b3*a23;
    gamma = b1*a13 + b2*a23 + b3*a33;

    if(alpha/DD<=0) return 0;

    TRDChargeValueLower=X0-0.5*beta/alpha;

    if(TRDChargeValueLower<0) return 0;

    return TRDChargeValueLower;
}

/////////////////////////////////////////////////////////////////////

