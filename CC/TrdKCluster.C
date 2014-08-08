#include "TrdKCluster.h"

ClassImp(TrdKCluster)

using namespace TMath;


TMinuit *TrdKCluster::gMinuit_TRDTrack = NULL;
TrdKCalib *TrdKCluster::_DB_instance = NULL;
TRD_ImpactParameter_Likelihood *TrdKCluster::TRDImpactlikelihood = NULL;
TrdKPDF *TrdKCluster::kpdf_e=NULL;
TrdKPDF *TrdKCluster::kpdf_p=NULL;
TrdKPDF *TrdKCluster::kpdf_h=NULL;
TrdKPDF *TrdKCluster::kpdf_q=NULL;

float TrdKCluster::threshold_ECAL_Energy_Hypothesis=2.0;
bool TrdKCluster::DebugOn=0;
int TrdKCluster::Default_TrPar[3];
int TrdKCluster::ForceReadAlignment=1;
int TrdKCluster::ForceReadCalibration=1;
int TrdKCluster::ForceReadXePressure=1;
float TrdKCluster::DefaultMCXePressure=780;

int TrdKCluster::IsReadAlignmentOK=2;
int TrdKCluster::IsReadCalibOK=1;
int TrdKCluster::IsReadGlobalAlignment=1;
int TrdKCluster::IsReadPlaneAlignment=1;
Double_t TrdKCluster::LastProcessedRun_Alignment=0;
Double_t TrdKCluster::LastProcessedRun_Calibration=0;
vector <TrdKHit> TrdKCluster::TRDTubeCollection;
map<int, TRDOnline> TrdKCluster::map_TRDOnline;
float TrdKCluster::DefaultRigidity=200;


TrdKCluster* TrdKCluster::_Head=0;
TrdKCalib* TrdKCluster::_HeadCalib=0;
AMSEventR* TrdKCluster::_HeadE=0;

/////////////////////////////////////////////////////////////////////

TrdKCluster::TrdKCluster() : TRDHitCollection(TRDTubeCollection)
{
    Init_Base();
}

/////////////////////////////////////////////////////////////////////

TrdKCluster::~TrdKCluster() {
}

//////////////////////////////////////////////////////////////////////
void TrdKCluster::Init(AMSEventR *evt){

    _event=evt;

    Time=evt->UTime();

    if(Time-LastProcessedRun_Alignment>600 && IsReadAlignmentOK==0){
        printf("~~~TrdKCluster~~~, Current Time: %.0f , Last Alignment Time: %.0f \n",Time,LastProcessedRun_Alignment);
        IsReadAlignmentOK=2;
    }

    if(Time-LastProcessedRun_Calibration>600  && IsReadCalibOK==0 ){
        printf("~~~TrdKCluster~~~, Current Time: %.0f , Last Calibration Time: %.0f \n",Time,LastProcessedRun_Calibration);
        IsReadCalibOK=1;
    }

    if(!evt->nTrdRawHit())return;

    FillHitCollection(evt);
    SetCorridorHits();

    if(IsReadAlignmentOK>0 && ForceReadAlignment>0){
        IsReadAlignmentOK=DoAlignment(IsReadPlaneAlignment,IsReadGlobalAlignment);
        LastProcessedRun_Alignment=Time;
    }

    if(IsReadCalibOK && ForceReadCalibration>0){
        IsReadCalibOK=DoGainCalibration();
        LastProcessedRun_Calibration=Time;
    }


    HasTRDTrack=false;

    int getpressure=GetXePressure();
    if(getpressure==-1|| Pressure_Xe>1100 || Pressure_Xe<600){
        cout<<"~~~~~WARNING~~~~~~  Get Xe Pressure Failed, Putting into Default value, result NOT Valid"<<endl;
        Pressure_Xe=780;
        IsValid=0;
    }else{
        IsValid=1;
    }

    return;
}

/////////////////////////////////////////////////////////////////////

TrdKCluster::TrdKCluster(AMSEventR *evt, TrTrackR *track, int fitcode) : TRDHitCollection(TRDTubeCollection)
{
    Init_Base();
    Init(evt);
    SetTrTrack(track, fitcode);
}


/////////////////////////////////////////////////////////////////////

TrdKCluster::TrdKCluster(AMSEventR *evt,AMSPoint *P0, AMSDir *Dir) : TRDHitCollection(TRDTubeCollection)
{
    Init_Base();
    Init(evt);
    SetTrTrack(P0, Dir, DefaultRigidity);
}


/////////////////////////////////////////////////////////////////////
TrdKCluster::TrdKCluster(AMSEventR *evt, TrdTrackR *trdtrack, float Rigidity) : TRDHitCollection(TRDTubeCollection) {
    AMSPoint *P0= new AMSPoint(trdtrack->Coo);
    AMSDir *Dir = new AMSDir(trdtrack->Theta,trdtrack->Phi);
    Init_Base();
    Init(evt);
    if(!Rigidity)Rigidity=DefaultRigidity;
    SetTrTrack(P0, Dir, Rigidity);
    delete P0;
    delete Dir;
}
/////////////////////////////////////////////////////////////////////
TrdKCluster::TrdKCluster(AMSEventR *evt, TrdHTrackR *trdtrack, float Rigidity) : TRDHitCollection(TRDTubeCollection) {
    AMSPoint *P0= new AMSPoint(trdtrack->Coo);
    AMSDir *Dir = new AMSDir(trdtrack->Dir);
    Init_Base();
    Init(evt);
    if(!Rigidity)Rigidity=DefaultRigidity;
    SetTrTrack(P0, Dir, Rigidity);
    delete P0;
    delete Dir;

}
/////////////////////////////////////////////////////////////////////
TrdKCluster::TrdKCluster(AMSEventR *evt, EcalShowerR *shower) : TRDHitCollection(TRDTubeCollection) {
    AMSPoint *P0= new AMSPoint(shower->CofG);
    AMSDir *Dir = new AMSDir(shower->Dir);
    Init_Base();
    Init(evt);
    SetTrTrack(P0, Dir, shower->EnergyC);
    delete P0;
    delete Dir;

}
/////////////////////////////////////////////////////////////////////
TrdKCluster::TrdKCluster(AMSEventR *evt, BetaHR *betah,float Rigidity) : TRDHitCollection(TRDTubeCollection) {
    AMSPoint *P0=new AMSPoint();
    AMSDir *Dir=new AMSDir();
    double dummy_time;
    betah->TInterpolate(TRDCenter,*P0,*Dir,dummy_time,false);
    Init_Base();
    Init(evt);
    if(!Rigidity)Rigidity=DefaultRigidity;
    SetTrTrack(P0, Dir, Rigidity);
    delete P0;
    delete Dir;

}
/////////////////////////////////////////////////////////////////////


TrdKCluster::TrdKCluster(const vector<TrdKHit>& _collection,AMSPoint *P0, AMSPoint *Dir,AMSPoint *TRDTrack_P0, AMSPoint *TRDTrack_Dir,AMSPoint *MaxSpan_P0, AMSPoint *MaxSpan_Dir):TRDHitCollection(_collection)
{

    Init_Base();

    Track_Rigidity=100000;

    track_extrapolated_Dir= *Dir;
    track_extrapolated_P0=  *P0;
    TrTrack_Pro=TrProp(track_extrapolated_P0,track_extrapolated_Dir,Track_Rigidity);

    TRDtrack_extrapolated_P0=*TRDTrack_P0;
    TRDtrack_extrapolated_Dir=*TRDTrack_Dir;
    HasTRDTrack=true;



}


/////////////////////////////////////////////////////////////////////

// Default Build Function
int TrdKCluster::Build(){

    IsMC=0;
    IsTB=0;
    IsISS=0;

    Track_type=0;
    Event_type=0;

    pev=GetAMSEventRHead();
    if(!pev) return -1;

    if(pev->nMCEventg()>0)IsMC=1;
    else if(pev->Run()<1300000000)IsTB=1;
    else IsISS=1;


    if(IsMC){
        TrdKCluster::ForceReadAlignment=0;
        TrdKCluster::ForceReadCalibration=0;
        TrdKCluster::ForceReadXePressure=0;
        TrdKCluster::SetDefaultMCXePressure(900);
    } else if(IsTB){
        TrdKCluster::IsReadGlobalAlignment=0;
        TrdKCluster::ForceReadAlignment=1;
        TrdKCluster::ForceReadCalibration=1;
        TrdKCluster::ForceReadXePressure=1;
    }else{
        TrdKCluster::IsReadGlobalAlignment=1;
        TrdKCluster::ForceReadAlignment=1;
        TrdKCluster::ForceReadCalibration=1;
        TrdKCluster::ForceReadXePressure=1;
    }


    if(pev->NEcalShower()>0)FindBestMatch_FromECAL();
    else if (pev->NParticle()>0)FindBestMatch_FromParticle();
    else if (pev->NTrTrack()>0)FindBestMatch_FromTrTrack();
    else{
        ptrk  = pev->pTrTrack(0);
        ptrd  = pev->pTrdTrack(0);
        ptrdh = pev->pTrdHTrack(0);
        pecal = pev->pEcalShower(0);
        ptof = pev->pBetaH(0);
        mcpart=pev->pMCEventg(0);
    }


    Double_t rig=0;
    int myfitcode=0;

    if(ptrk){
        Int_t refit = 21;
        if( pev->nMCEventg() > 1 ) refit = 1;
                myfitcode= ptrk->iTrTrackPar(1, 0, refit); // MaxSpan
//        myfitcode= ptrk->iTrTrackPar(1, 3, refit); // InnerOnly
        if(!ptrk->ParExists(myfitcode))myfitcode=0;
        rig = ptrk->GetRigidity(myfitcode);
    }

    Double_t ene = 0.;
    if(pecal){ ene = fabs(pecal->EnergyE);	}

    float assume_rig=rig;
    if(abs(rig)<5 && ene > 5)assume_rig=ene;
    else assume_rig=100;

    if(ptrd){
        Build(ptrd,assume_rig);
        Track_type |= 1;
    }else if(ptrk && ptrk->ParExists(myfitcode)){
        Build(ptrk,myfitcode);
        Track_type |= 2;
    }else if(ptrdh){
        Build(ptrdh, assume_rig);
        Track_type |= 4;
    }else if(pecal){
        Build(pecal);
        Track_type |= 8;
    }else if(mcpart){
        Build(mcpart);
        Track_type |= 16;
    }else if(ptof){
        Build(ptof,assume_rig);
        Track_type |= 32;
    }
    else {
        return -1;
    }


    //=== Do Refit ====
    FitTRDTrack(1,0);

    //========TrTrack
    if(ptrk && ptrk->ParExists(myfitcode)){
        SetTrTrack(ptrk, myfitcode);
    }

    return 1;
}
/////////////////


int TrdKCluster::FindBestMatch_FromParticle(){

    int npart=pev->NParticle();

    int index=0;

    if(npart>1){
        float maxenergy=0;
        for(int i=0;i<npart;i++){
            ParticleR *part=pev->pParticle(i);
            if(fabs(part->Momentum)>maxenergy && (part->pTrdTrack() || part->pTrdHTrack())){
                maxenergy=fabs(part->Momentum);
                index=i;
            }
        }
    }

    ParticleR *part=pev->pParticle(index);

    ptrk  = part->pTrTrack();
    ptrd  = part->pTrdTrack();
    ptrdh = part->pTrdHTrack();
    pecal = part->pEcalShower();
    ptof = part->pBetaH();
    mcpart=pev->pMCEventg(0);

    Event_type=1;
    return 1;
}


int TrdKCluster::FindBestMatch_FromECAL(){
    ptrk  = 0;
    ptrd  =0;
    ptrdh = 0;
    pecal = 0;
    ptof = 0;
    mcpart=0;

    // 1. find the highest energetic shower
    Double_t e_candidate = 0.;
    Int_t    i_candidate_ecal = -1,
            i_candidate_trd  = -1,
            i_candidate_trdh = -1,
            i_candidate_trk  = -1,
            i_candidate_betah= -1;

    int necal = pev->nEcalShower();
    if(necal<=0)return 0;

    int ntrk  = pev->nTrTrack();

    // Most energetic Shower
    for(Int_t i=0; i<pev->nEcalShower(); i++){
        if( pev->pEcalShower(i)->EnergyD > e_candidate ){
            e_candidate = pev->pEcalShower(i)->EnergyD;
            i_candidate_ecal = i;
        }
    }

    AMSPoint ecal_coo( pev->pEcalShower(i_candidate_ecal)->CofG );
    AMSDir   ecal_dir( pev->pEcalShower(i_candidate_ecal)->Dir );

    // 2. find best matched TRD
    AMSPoint trd_coo, trd_coo_ecalcog;
    AMSDir   trd_dir;
    Double_t min_dist = 10000., dist;
    for(Int_t i=0; i<pev->nTrdTrack(); i++){
        trd_coo = AMSPoint( pev->pTrdTrack(i)->Coo );
        trd_dir = AMSDir( pev->pTrdTrack(i)->Theta, pev->pTrdTrack(i)->Phi );
        trd_coo_ecalcog[0] = trd_coo[0] + trd_dir[0]/trd_dir[2]*(ecal_coo[2] - trd_coo[2]);
        trd_coo_ecalcog[1] = trd_coo[1] + trd_dir[1]/trd_dir[2]*(ecal_coo[2] - trd_coo[2]);
        trd_coo_ecalcog[2] = ecal_coo[2];

        dist = trd_coo_ecalcog.dist( ecal_coo );
        if( dist < min_dist ){
            min_dist = dist;
            i_candidate_trd = i;
        }
    }
    min_dist = 10000.;
    for(Int_t i=0; i<pev->nTrdHTrack(); i++){
        trd_coo = AMSPoint( pev->pTrdHTrack(i)->Coo );
        trd_dir = AMSDir( pev->pTrdHTrack(i)->Dir );
        dist = trd_coo.dist( ecal_coo );
        trd_coo_ecalcog[0] = trd_coo[0] + trd_dir[0]/trd_dir[2]*(ecal_coo[2] - trd_coo[2]);
        trd_coo_ecalcog[1] = trd_coo[1] + trd_dir[1]/trd_dir[2]*(ecal_coo[2] - trd_coo[2]);
        trd_coo_ecalcog[2] = ecal_coo[2];

        if( dist < min_dist ){
            min_dist = dist;
            i_candidate_trdh = i;
        }
    }
    // 3. find best matched TrTrack
    min_dist = 10000.;
    AMSPoint trk_coo_ecalcog;
    AMSDir   trk_dir_ecalcog;
    for(Int_t i=0; i<ntrk; i++){
        Int_t fitid = pev->pTrTrack(i)->iTrTrackPar(1, 3, 1); // VC + Inner
        pev->pTrTrack(i)->Interpolate(ecal_coo[2], trk_coo_ecalcog, trk_dir_ecalcog, fitid);
        dist = trk_coo_ecalcog.dist( ecal_coo );
        if( dist < min_dist ){
            min_dist = dist;
            i_candidate_trk = i;
        }
    }
    // 4. find best matched BetaH
    for(Int_t i=0; i<pev->nBetaH(); i++){
        if(
                ( i_candidate_ecal >= 0 && pev->pBetaH(i)->iEcalShower() == i_candidate_ecal )
                || ( i_candidate_trk  >= 0 && pev->pBetaH(i)->iTrTrack()    == i_candidate_trk  )
                || ( i_candidate_trd  >= 0 && pev->pBetaH(i)->iTrdTrack()   == i_candidate_trd  )
                ){
            i_candidate_betah = i;
            break;
        }
    }


    ptrk  = _HeadE->pTrTrack(i_candidate_trk);
    ptrd  = _HeadE->pTrdTrack(i_candidate_trd);
    ptrdh = _HeadE->pTrdHTrack(i_candidate_trdh);
    pecal = _HeadE->pEcalShower(i_candidate_ecal);
    ptof  = _HeadE->pBetaH(i_candidate_betah);
    mcpart=_HeadE->pMCEventg(0);

    Event_type=0;
    return 1;
}


int TrdKCluster::FindBestMatch_FromTrTrack(){

    int ntrk=pev->NTrTrack();

    int index=0;

    if(ntrk>1){
        float maxenergy=0;
        for(int i=0;i<ntrk;i++){
            TrTrackR *trk=pev->pTrTrack(i);
            if(fabs(trk->GetRigidity())>maxenergy){
                maxenergy=fabs(trk->GetRigidity());
                index=i;
            }
        }
    }


    ptrk=_HeadE->pTrTrack(index);
    AMSPoint trk_coo;
    AMSDir trk_dir;
    ptrk->Interpolate(80,trk_coo,trk_dir);

    // 2. find best matched TRD
    int i_candidate_trd=-1;
    int i_candidate_trdh=-1;
    AMSPoint trd_coo, trd_coo_atTrTrack;
    AMSDir   trd_dir;
    Double_t min_dist = 10000., dist;



    for(Int_t i=0; i<pev->nTrdTrack(); i++){
        trd_coo = AMSPoint( pev->pTrdTrack(i)->Coo );
        trd_dir = AMSDir( pev->pTrdTrack(i)->Theta, pev->pTrdTrack(i)->Phi );
        trd_coo_atTrTrack[0] = trd_coo[0] + trd_dir[0]/trd_dir[2]*(trk_coo[2] - trd_coo[2]);
        trd_coo_atTrTrack[1] = trd_coo[1] + trd_dir[1]/trd_dir[2]*(trk_coo[2] - trd_coo[2]);
        trd_coo_atTrTrack[2] = trk_coo[2];
        dist = trd_coo_atTrTrack.dist( trk_coo );
        if( dist < min_dist ){
            min_dist = dist;
            i_candidate_trd = i;
        }
    }
    min_dist = 10000.;
    for(Int_t i=0; i<pev->nTrdHTrack(); i++){
        trd_coo = AMSPoint( pev->pTrdHTrack(i)->Coo );
        trd_dir = AMSDir( pev->pTrdHTrack(i)->Dir );
        dist = trd_coo.dist( trk_coo );
        trd_coo_atTrTrack[0] = trd_coo[0] + trd_dir[0]/trd_dir[2]*(trk_coo[2] - trd_coo[2]);
        trd_coo_atTrTrack[1] = trd_coo[1] + trd_dir[1]/trd_dir[2]*(trk_coo[2] - trd_coo[2]);
        trd_coo_atTrTrack[2] = trk_coo[2];

        if( dist < min_dist ){
            min_dist = dist;
            i_candidate_trdh = i;
        }
    }

    ptrd  = _HeadE->pTrdTrack(i_candidate_trd);
    ptrdh = _HeadE->pTrdHTrack(i_candidate_trdh);

    Event_type=2;
    return 1;

}



///////////////////////
void TrdKCluster::Construct_TRDTube(){
    cout<<"Construct TRD Tube: "<<endl;
    TRDTubeCollection.clear();
    TRDTubeCollection.reserve(TrdHCalibR::n_tubes);
    for(int i=0;i<TrdHCalibR::n_tubes;i++){
        int layer,ladder, tube;
        TrdHCalibR::GetLLTFromTubeId(layer,ladder,tube,i);
        TrdRawHitR *_trdhit=new TrdRawHitR(layer,ladder,tube,0.);
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
    corridor_radius=3.;
    corridor_p = AMSPoint(0,0,0);
    corridor_d = AMSDir(0,0,-1);
    if(DefaultMCXePressure<=0)SetDefaultMCXePressure(780);
    if(TRDTubeCollection.size()!=TrdHCalibR::n_tubes) {
        Construct_TRDTube();
        TRDHitCollection = TRDTubeCollection;
    }
    if(map_TRDOnline.size()==0) InitXePressure();
    if(!TRDImpactlikelihood)TRDImpactlikelihood=new TRD_ImpactParameter_Likelihood();
    if(!_DB_instance) _DB_instance=GetTRDKCalibHead();
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
    }for(int i=0;i<TrdHCalibR::n_tubes;i++){
        if(!GetHit(i)->IsCalibrated) {
            GetHit(i)->DoGainCalibration(_DB_instance->GetGainCorrectionFactorTube(GetHit(i)->tubeid,Time));
        }
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
	int old_layer = -1;
        for(int i=0;i<TrdHCalibR::n_tubes;++i) {
            if(!GetHit(i)->IsAligned) {
                int layer=GetHit(i)->TRDHit_Layer;
                if(old_layer != layer) {
                    par_alignment=_DB_instance->GetAlignmentPar(layer,(int)Time);
                    old_layer = layer;
                }
                GetHit(i)->DoAlignment(&par_alignment);
            }
        }
    }else if(read==1){
	int old_layer = -1;
        for(int i=0;i<TrdHCalibR::n_tubes;i++) {
            if(!GetHit(i)->IsAligned) {
                int layer=GetHit(i)->TRDHit_Layer;
                if(old_layer != layer) {
                    par_alignment=*(_DB_instance->GetAlignmentPar_Plane(layer));
                    old_layer = layer;
                }
                GetHit(i)->DoAlignment(&par_alignment);
            }
        }
    }
    return read;
}


/////////////////////////////////////////////////////////////////////

void TrdKCluster::FillHitCollection(AMSEventR* evt){
    int NTRDHit=evt->nTrdRawHit();
    if(!NTRDHit)return;

    //Add Raw Hits
    for(int i=0;i<NTRDHit;i++){
        TrdRawHitR* _trd_hit=evt->pTrdRawHit(i);
        if(!_trd_hit) continue;
	int id;
        TrdHCalibR::GetTubeIdFromLLT(_trd_hit->Layer,_trd_hit->Ladder,_trd_hit->Tube, id);
        TRDHitCollection[id] = TrdKHit(_trd_hit,Zshift);
    }
}

/////////////////////////////////////////////////////////////////////

void TrdKCluster::SelectClosest(){
    vector<unsigned int>::iterator  Hit_it=corridor_hits.begin();
    for (;Hit_it!=corridor_hits.end(); ) {
        vector<unsigned int>::iterator  Hit_it2=corridor_hits.begin();
        bool removed=false;
        for (;Hit_it2!=corridor_hits.end(); ) {
            TrdKHit* hit1 = GetCorridorHit(*Hit_it);
	    TrdKHit* hit2 = GetCorridorHit(*Hit_it2);
            if(Hit_it==Hit_it2)++Hit_it2;
            else if(int(hit1->tubeid/16)!=int(hit2->tubeid/16))Hit_it2++;
            else if(abs(hit1->Tube_Track_Distance_3D(&track_extrapolated_P0,&track_extrapolated_Dir))<abs(hit2->Tube_Track_Distance_3D(&track_extrapolated_P0,&track_extrapolated_Dir))) Hit_it2 =corridor_hits.erase(Hit_it2);
            else if (abs(hit1->Tube_Track_Distance_3D(&track_extrapolated_P0,&track_extrapolated_Dir))>abs(hit2->Tube_Track_Distance_3D(&track_extrapolated_P0,&track_extrapolated_Dir))){
                Hit_it =corridor_hits.erase(Hit_it);
                removed=true;
            }else ++Hit_it2;
        }
        if(!removed)++Hit_it;
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

bool  TrdKCluster::IsCalibrated() const {
    bool v=1;
    for(unsigned int i=0;i<TRDHitCollection.size();i++)
        if(!GetConstHit(i)->IsCalibrated)v=0;
    return v;
}

/////////////////////////////////////////////////////////////////////

bool  TrdKCluster::IsAligned() const {
    bool v=1;
    for(unsigned int i=0;i<TRDHitCollection.size();i++)
        if(!GetConstHit(i)->IsAligned)v=0;
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

    int size= corridor_hits.size();
    double result=0;
    AMSPoint temp_TrTrackP0(par[0],par[1],par[2]);
    AMSDir  temp_TrTrackDir(par[3],par[4],TMath::Sqrt(1-par[3]*par[3]-par[4]*par[4]));
    double impact_parameter,likelihood;
    TrdKHit* hit;
    for(int i=0;i<size;i++){
        hit=GetCorridorHit(i);
        impact_parameter=hit->Tube_Track_Distance_3D(&temp_TrTrackP0,&temp_TrTrackDir);
        likelihood = TRDImpactlikelihood->GetLikelihood(hit->TRDHit_Amp,impact_parameter);
        if(likelihood<0.0000001) likelihood=0.0000001;
        result-=2*log(likelihood);
    }
    return result;


}

/////////////////////////////////////////////////////////////////////

double TrdKCluster::TRDTrack_PathLengthLikelihood(Double_t *par){
    int size= corridor_hits.size();
    double result=0;
    AMSPoint temp_TrTrackP0(par[0],par[1],par[2]);
    AMSDir  temp_TrTrackDir(par[3],par[4],TMath::Sqrt(1-par[3]*par[3]-par[4]*par[4]));
    double pathlength,likelihood;
    TrdKHit* hit;
    for(int i=0;i<size;i++){
        hit=GetCorridorHit(i);
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

Double_t TrdKCluster::trd_parabolic_fit(Int_t N, Double_t *X, Double_t *Y, Double_t *V) {

    Int_t    i;
    Double_t X0, DD, x_min, x_max, y_min, alpha, beta, gamma;
    Double_t m11, m12, m13, m22, m23, m33, a11, a12, a13, a22, a23, a33, b1, b2, b3;

    if ( N < 4 ) return -1.;

    X0    = 0.;
    y_min = Y[0];


    // define average X
    V[0] = X[0];
    for ( i=0; i<N; i++ ) {
        X0 += X[i];
        if ( Y[i] < y_min ) {
            V[0]  = X[i];
            y_min = Y[i];
        }
    }
    X0   = X0 / N;

    x_min = 0.;
    x_max = 0.;

    m11 = 0.;
    m12 = 0.;
    m13 = 0.;
    m23 = 0.;
    m33 = (Double_t)N;

    b1  = 0.;
    b2  = 0.;
    b3  = 0.;

    // build matrix
    for ( i=0; i<N; i++ ) {

        X[i] -= X0;

        if ( X[i] > x_max ) x_max = X[i];
        if ( X[i] < x_min ) x_min = X[i];

        m11 += X[i]*X[i]*X[i]*X[i];
        m12 += X[i]*X[i]*X[i];
        m13 += X[i]*X[i];
        m23 += X[i];

        b1 += Y[i]*X[i]*X[i];
        b2 += Y[i]*X[i];
        b3 += Y[i];

    }
    m22 = m13;

    V[1] = x_max - x_min;

    //inverse matrix
    DD  = m11*m22*m33 + 2.*m12*m13*m23 - m13*m13*m22 - m12*m12*m33 - m11*m23*m23;
    if ( DD == 0. ) return y_min;

    a11 = m22*m33 - m23*m23;
    a12 = m13*m23 - m12*m33;
    a13 = m12*m23 - m13*m22;
    a22 = m11*m33 - m13*m13;
    a23 = m12*m13 - m11*m23;
    a33 = m11*m22 - m12*m12;


    // fitted parameters
    alpha = b1*a11 + b2*a12 + b3*a13;
    beta  = b1*a12 + b2*a22 + b3*a23;
    gamma = b1*a13 + b2*a23 + b3*a33;

    if ( alpha <= 0. ) return y_min;

    if ( -0.5*beta/alpha > x_min && -0.5*beta/alpha < x_max ) {
        V[0] = X0 - 0.5*beta/alpha;
        V[1] = DD/alpha;
        return (gamma - 0.25*beta*beta/alpha)/DD;
    }

    if ( -0.5*beta/alpha <= x_min ) V[0] = X0 + x_min;
    else                            V[0] = X0 + x_max;

    return y_min;

}

/////////////////////////////////////////////////////////////////////

void TrdKCluster::KounineRefit(AMSPoint& P_fit, AMSDir& D_fit,
                               const AMSPoint& P_init,
                               const AMSDir& D_init) {
    Double_t dDD  = 0.04;
    Double_t dAA  = 0.004;
    Double_t D_Tol = 0.03;
    Double_t K_Tol = 0.002;
    Int_t MaxItr = 4;

    Double_t X_0  = P_init[0];
    Double_t Y_0  = P_init[1];
    Double_t Z = P_init[2];

    Double_t kX_init, kY_init;
    Bool_t Up;
    dir_to_k(kX_init, kY_init, Up, D_init);

    Double_t kX_0 = kX_init;
    Double_t kY_0 = kY_init;
    Double_t X_fit,Y_fit,kX_fit,kY_fit;
    Double_t kX,kY;
    Double_t V[10],W[10],Q[5];
    Double_t par[5];
    AMSDir temp_d;
    Int_t Itr  = 0;

    par[2] = Z;

refit:

    ///////////////////////////
    //         Fit X0        //
    ///////////////////////////
    par[1] = Y_0;
    kX = kX_0;
    kY = kY_0;
    for (Int_t m=0; m<5; m++ ) {

        par[0] = X_0 + dDD*(m-2.);
        V[m] = par[0];
        k_to_dir(temp_d,kX,kY,Up);
        par[3] = temp_d[0];
        par[4] = temp_d[1];

        W[m] = TRDTrack_PathLengthLikelihood(par);
    }

    if( trd_parabolic_fit(5, V, W, Q) < 0. ) {
        X_fit = P_init[0];
    } else {
        X_fit = Q[0];
    }

    ///////////////////////////
    //         Fit Y0        //
    ///////////////////////////
    par[0] = X_0;
    kX = kX_0;
    kY = kY_0;
    for (Int_t m=0; m<5; m++ ) {

        par[1] = Y_0 + dDD*(m-2.);
        V[m] = par[1];
        k_to_dir(temp_d,kX,kY,Up);
        par[3] = temp_d[0];
        par[4] = temp_d[1];

        W[m] = TRDTrack_PathLengthLikelihood(par);
    }

    if( trd_parabolic_fit(5, V, W, Q) < 0. ) {
        Y_fit = P_init[1];
    } else {
        Y_fit = Q[0];
    }

    ///////////////////////////
    //         Fit kX        //
    ///////////////////////////
    par[0] = X_0;
    par[1] = Y_0;
    kY = kY_0;
    for (Int_t m=0; m<5; m++ ) {

        kX = kX_0 + dAA*(m-2.);
        V[m] = kX;
        k_to_dir(temp_d,kX,kY,Up);
        par[3] = temp_d[0];
        par[4] = temp_d[1];

        W[m] = TRDTrack_PathLengthLikelihood(par);
    }

    if( trd_parabolic_fit(5, V, W, Q) < 0. ) {
        kX_fit = kX_init;
    } else {
        kX_fit = Q[0];
    }

    ///////////////////////////
    //         Fit kY        //
    ///////////////////////////
    par[0] = X_0;
    par[1] = Y_0;
    kX = kX_0;
    for (Int_t m=0; m<5; m++ ) {

        kY = kY_0 + dAA*(m-2.);
        V[m] = kY;
        k_to_dir(temp_d,kX,kY,Up);
        par[3] = temp_d[0];
        par[4] = temp_d[1];

        W[m] = TRDTrack_PathLengthLikelihood(par);
    }

    if( trd_parabolic_fit(5, V, W, Q) < 0. ) {
        kY_fit = kY_init;
    } else {
        kY_fit = Q[0];
    }

    if ( TMath::Abs(X_fit-X_0)>D_Tol || TMath::Abs(Y_fit-Y_0)>D_Tol || TMath::Abs(kX_fit-kX_0)>K_Tol || TMath::Abs(kY_fit-kY_0)>K_Tol ) {

        //	printf("Init: %f %f %f %f\n", X_0, Y_0,   kX_0,   kY_0);
        //	printf("Fit:%f %f %f %f\n", X_fit, Y_fit, kX_fit, kY_fit);

        dDD *= 0.7;
        dAA *= 0.7;
        X_0= X_fit;
        Y_0= Y_fit;
        kX_0 = kX_fit;
        kY_0 = kY_fit;

        if ( Itr++ < MaxItr) goto refit;

    }

    P_fit[0] = X_fit;
    P_fit[1] = Y_fit;
    P_fit[2] = Z;
    k_to_dir(D_fit,kX_fit,kY_fit,Up);
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
    else  if(method==4) FitTRDTrack_PathLength_KFit(hypothesis);
    else{
        cout<<"~~~WARNING~~~TrdKCluster, Unrecognized Fit Method: "<<method<<endl;
        return ;
    }
    return;

};

/////////////////////////////////////////////////////////////////////

void TrdKCluster::FitTRDTrack_IPLikelihood(int IsCharge){

    //    TVirtualFitter::SetDefaultFitter("Minuit");
    //    gMinuit_TRDTrack = TVirtualFitter::Fitter(0, 4);
    if(!gMinuit_TRDTrack)  gMinuit_TRDTrack=new TMinuit(4);
    //    gMinuit_TRDTrack->SetFCN(fcn_TRDTrack);
    //Changed by Wei Sun
    if(IsCharge==0) gMinuit_TRDTrack->SetFCN(fcn_TRDTrack);
    else gMinuit_TRDTrack->SetFCN(fcn_TRDTrack_Charge);

    gMinuit_TRDTrack->SetObjectFit(this);
    double arglist[10];


    //    arglist[0]=-1;
    //    gMinuit_TRDTrack->ExecuteCommand("SET PRINT", arglist, 1);
    //    gMinuit_TRDTrack->ExecuteCommand("SET NOW", arglist, 0);
    //    arglist[0]=0;
    //    gMinuit_TRDTrack->ExecuteCommand("SET NOWarnings",arglist,1);
    //    gMinuit_TRDTrack->SetPrintLevel(-1);

    arglist[0] = -1;
    gMinuit_TRDTrack->SetPrintLevel(-1);
    //    gMinuit_TRDTrack->ExecuteCommand("SET PRINT", arglist,1);
    //    arglist[0] = 0;
    //    gMinuit_TRDTrack->ExecuteCommand("SET NOW",   arglist,0);

    float init_z0=115;
    float init_x0=0;
    float init_y0=0;
    float init_dx=0;
    float init_dy=0;

    Propogate_TrTrack(init_z0);
    AMSPoint P0=GetPropogated_TrTrack_P0();
    AMSDir Dir=GetPropogated_TrTrack_Dir();
    init_x0=P0.x();
    init_y0=P0.y();
    init_dx=Dir.x();
    init_dy=Dir.y();

    if(DebugOn){
        cout<<"FitTRDTrack_IPLikelihood ,  TrTrack : "<<track_extrapolated_P0<<", "<<track_extrapolated_Dir<<endl;
        printf("TrTrack : (%.2f, %.2f, %.2f), (%.2f, %.2f, %.2f)\n",P0.x(),P0.y(),P0.z(),Dir.x(),Dir.y(),Dir.z());
    }

    //
    //    gMinuit_TRDTrack->SetParameter(0,  "x0",  init_x0,  0.2,  init_x0-10,init_x0+10);
    //    gMinuit_TRDTrack->SetParameter(1,  "y0",  init_y0,  0.2,  init_y0-10,init_y0+10);
    //    gMinuit_TRDTrack->SetParameter(2,  "z0",  init_z0,0,init_z0,init_z0);
    //    gMinuit_TRDTrack->SetParameter(3,  "dx0", init_dx,  0.1,  -1,1);
    //    gMinuit_TRDTrack->SetParameter(4,  "dy0", init_dy,  0.1,  -1,1);

    int ierr=0;
    gMinuit_TRDTrack->mnparm(0,  "x0",  init_x0,  0.2,  init_x0-10,init_x0+10,ierr);
    gMinuit_TRDTrack->mnparm(1,  "y0",  init_y0,  0.2,  init_y0-10,init_y0+10,ierr);
    gMinuit_TRDTrack->mnparm(2,  "z0",  init_z0,0,init_z0,init_z0,ierr);
    gMinuit_TRDTrack->mnparm(3,  "dx0", init_dx,  0.1,  -1,1,ierr);
    gMinuit_TRDTrack->mnparm(4,  "dy0", init_dy,  0.1,  -1,1,ierr);

    arglist[0]=0;

    gMinuit_TRDTrack->FixParameter(2);
    //    gMinuit_TRDTrack->ExecuteCommand("MIGRAD", arglist, 0);
    gMinuit_TRDTrack->mnexcm("MIGRAD", arglist, 0,ierr);


    float x0,y0,z0,dx,dy,dz;

    //    x0=gMinuit_TRDTrack->GetParameter(0);
    //    y0=gMinuit_TRDTrack->GetParameter(1);
    //    z0=gMinuit_TRDTrack->GetParameter(2);
    //    dx=gMinuit_TRDTrack->GetParameter(3);
    //    dy=gMinuit_TRDTrack->GetParameter(4);

    double out[5],err[5];
    TString s_name[5]={"x0","y0","z0","dx0","dy0"};
    for(int i=0;i<5;i++)   {
        double bnd1, bnd2;
        int ivar;
        gMinuit_TRDTrack->mnpout(i, s_name[i], out[i], err[i], bnd1, bnd2, ivar);
    }

    x0=out[0];
    y0=out[1];
    z0=out[2];
    dx=out[3];
    dy=out[4];


    dz=TMath::Sqrt(1-dx*dx-dy*dy);

    if(DebugOn){
        printf("TRDTrack Refit : (%.2f, %.2f, %.2f), (%.2f, %.2f, %.2f)\n",x0,y0,z0,dx,dy,dz);
    }


    AMSDir TRDDir(dx,dy,dz);
    AMSPoint TRDP0(x0,y0,z0);

    SetTRDTrack(&TRDP0,&TRDDir);

}


/////////////////////////////////////////////////////////////////////

void TrdKCluster::MergeWith(TrdKCluster *secondcluster){
    for(int i=0;i<secondcluster->NHits();i++){
        this->TRDHitCollection.push_back(*secondcluster->GetHit(i));
    }
    SetCorridorHits();
}



/////////////////////////////////////////////////////////////////////

void TrdKCluster::AnalyticalFit_2D(int direction, double x, double z, double dx, double dz, double &TRDTrack_x,double &TRDTrack_dx){



    float tubex,tubez;

    double norm_dxdz=sqrt(dx*dx+dz*dz);

    dx/=norm_dxdz;
    dz/=norm_dxdz;


    double DD1[2]={0,0};
    double DD2[2][2]={{0,0},{0,0}};

    float radius=0.3;
    float width=0.03;

    int tubecount=0;

    int factor_index=-1;
    float factor[2]={1./1.8478,-1./4.1522};

    for(size_t i=0;i<corridor_hits.size();i++){

        TrdKHit* hit=GetHit(corridor_hits[i]);

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


    //    TVirtualFitter::SetDefaultFitter("Minuit");
    //    gMinuit_TRDTrack = TVirtualFitter::Fitter(0, 4);

    if(!gMinuit_TRDTrack)  gMinuit_TRDTrack=new TMinuit(4);

    Refit_hypothesis=particle_hypothesis;
    gMinuit_TRDTrack->SetFCN(fcn_TRDTrack_PathLength);
    gMinuit_TRDTrack->SetObjectFit(this);
    double arglist[10];

    //    arglist[0]=-1;
    //    gMinuit_TRDTrack->ExecuteCommand("SET PRINT", arglist, 1);
    //    gMinuit_TRDTrack->ExecuteCommand("SET NOW", arglist, 0);

    arglist[0] = -1;
    gMinuit_TRDTrack->SetPrintLevel(-1);

    float init_z0=115;
    float init_x0=0;
    float init_y0=0;
    float init_dx=0;
    float init_dy=0;

    Propogate_TrTrack(init_z0);
    AMSPoint P0=GetPropogated_TrTrack_P0();
    AMSDir Dir=GetPropogated_TrTrack_Dir();
    init_x0=P0.x();
    init_y0=P0.y();
    init_dx=Dir.x();
    init_dy=Dir.y();

    //    printf("TrTrack : (%.2f, %.2f, %.2f), (%.2f, %.2f, %.2f)\n",P0.x(),P0.y(),P0.z(),Dir.x(),Dir.y(),Dir.z());

    //    gMinuit_TRDTrack->SetParameter(0,  "x0",  init_x0,  0.2,  init_x0-10,init_x0+10);
    //    gMinuit_TRDTrack->SetParameter(1,  "y0",  init_y0,  0.2,  init_y0-10,init_y0+10);
    //    gMinuit_TRDTrack->SetParameter(2,  "z0",  init_z0,0,init_z0,init_z0);
    //    gMinuit_TRDTrack->SetParameter(3,  "dx0", init_dx,  0.1,  -1,1);
    //    gMinuit_TRDTrack->SetParameter(4,  "dy0", init_dy,  0.1,  -1,1);


    int ierr=0;
    gMinuit_TRDTrack->mnparm(0,  "x0",  init_x0,  0.2,  init_x0-10,init_x0+10,ierr);
    gMinuit_TRDTrack->mnparm(1,  "y0",  init_y0,  0.2,  init_y0-10,init_y0+10,ierr);
    gMinuit_TRDTrack->mnparm(2,  "z0",  init_z0,0,init_z0,init_z0,ierr);
    gMinuit_TRDTrack->mnparm(3,  "dx0", init_dx,  0.1,  -1,1,ierr);
    gMinuit_TRDTrack->mnparm(4,  "dy0", init_dy,  0.1,  -1,1,ierr);

    arglist[0]=0;

    gMinuit_TRDTrack->FixParameter(2);
    //    gMinuit_TRDTrack->ExecuteCommand("MIGRAD", arglist, 0);
    gMinuit_TRDTrack->mnexcm("MIGRAD", arglist, 0,ierr);


    float x0,y0,z0,dx,dy,dz;

    //    x0=gMinuit_TRDTrack->GetParameter(0);
    //    y0=gMinuit_TRDTrack->GetParameter(1);
    //    z0=gMinuit_TRDTrack->GetParameter(2);
    //    dx=gMinuit_TRDTrack->GetParameter(3);
    //    dy=gMinuit_TRDTrack->GetParameter(4);

    double out[5],err[5];
    TString s_name[5]={"x0","y0","z0","dx0","dy0"};
    for(int i=0;i<5;i++)   {
        double bnd1, bnd2;
        int ivar;
        gMinuit_TRDTrack->mnpout(i, s_name[i], out[i], err[i], bnd1, bnd2, ivar);
    }

    x0=out[0];
    y0=out[1];
    z0=out[2];
    dx=out[3];
    dy=out[4];


    dz=TMath::Sqrt(1-dx*dx-dy*dy);

    AMSDir TRDDir(dx,dy,dz);
    AMSPoint TRDP0(x0,y0,z0);
    SetTRDTrack(&TRDP0,&TRDDir);


}






/////////////////////////////////////////////////////////////////////

void TrdKCluster::FitTRDTrack_PathLength_KFit(int particle_hypothesis){

    Refit_hypothesis=particle_hypothesis;
    float init_z0=115;

    Propogate_TrTrack(init_z0);
    AMSPoint P0=GetPropogated_TrTrack_P0();
    AMSDir Dir=GetPropogated_TrTrack_Dir();
    AMSPoint TRDP0;
    AMSDir   TRDDir;

    KounineRefit(TRDP0,TRDDir,P0,Dir);

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
    SetCorridor(track_extrapolated_P0, track_extrapolated_Dir);


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
    HasTRDTrack=true;


    //    for(int i=0;i<NHits();i++){
    //        GetHit(i)->PredictCrossing(&TRDtrack_extrapolated_P0,&TRDtrack_extrapolated_Dir,1);
    //    }

    return;
}



/////////////////////////////////////////////////////////////////////

void TrdKCluster::SetTrTrack(TrTrackR* track, int fitcode){

    if(!track->ParExists(fitcode)) {
        fitcode=track->Gettrdefaultfit();
        cout<<"~~WARNING~~TrdKCluster::SetTrTrack : Input fitcode do not exist, use default fitcode : "<<fitcode<<endl;
    }
        track_extrapolated_Dir=  AMSDir(0,0,0);
        track_extrapolated_P0= AMSPoint(0,0,0);
        track->Interpolate(TRDCenter,track_extrapolated_P0,track_extrapolated_Dir,fitcode);
        TrTrack_Pro=TrProp(track_extrapolated_P0,track_extrapolated_Dir,track->GetRigidity(fitcode));
        Track_Rigidity=track->GetRigidity(fitcode);
        SetCorridor(track_extrapolated_P0, track_extrapolated_Dir);

    return;
}





/////////////////////////////////////////////////////////////////////


int TrdKCluster::GetLikelihoodRatio_TRDRefit(float threshold, double* LLR, int &nhits){
    float ECAL_Energy_Hypothesis=0;
    double dummy_L[3];
    int fitmethod=1;
    int particle_hypothesis=1;
    return GetLikelihoodRatio_TRDRefit(threshold, LLR,nhits,ECAL_Energy_Hypothesis,dummy_L,fitmethod,particle_hypothesis);
}

int TrdKCluster::GetLikelihoodRatio_TRDRefit(float threshold, double* LLR, int &nhits, float ECAL_Energy_Hypothesis, double *LL, int fitmethod, int particle_hypothesis){

    if(!HasTRDTrack && !fitmethod)fitmethod=1;
    if(!HasTRDTrack || fitmethod>0)   FitTRDTrack(fitmethod,particle_hypothesis);
    double dummy_L[3];
    float dummy_length;
    float dummy_amp;
    if(!LL)return GetLikelihoodRatio(threshold, LLR,dummy_L,nhits,dummy_length,dummy_amp,&TRDtrack_extrapolated_P0,&TRDtrack_extrapolated_Dir,ECAL_Energy_Hypothesis);
    else return GetLikelihoodRatio(threshold, LLR,LL,nhits,dummy_length,dummy_amp,&TRDtrack_extrapolated_P0,&TRDtrack_extrapolated_Dir,ECAL_Energy_Hypothesis);
}

int TrdKCluster::GetLikelihoodRatio_TRDRefit(float threshold, double* LLR, double* L, int &nhits,float &total_pathlength, float &total_amp , int fitmethod, int particle_hypothesis,int flag_debug, float ECAL_Energy_Hypothesis){
    if(!HasTRDTrack && !fitmethod)fitmethod=1;
    if(!HasTRDTrack || fitmethod>0)   FitTRDTrack(fitmethod,particle_hypothesis);
    if(flag_debug<0)return GetLikelihoodRatio(threshold, LLR,L,nhits,total_pathlength,total_amp,&TRDtrack_extrapolated_P0,&TRDtrack_extrapolated_Dir);
    else return GetLikelihoodRatio_DEBUG(threshold, LLR,L,nhits,total_pathlength,total_amp,&TRDtrack_extrapolated_P0,&TRDtrack_extrapolated_Dir,flag_debug,ECAL_Energy_Hypothesis);
}

/////////////////////////////////////////////////////////////////////


int TrdKCluster::GetLikelihoodRatio_TrTrack(float threshold, double* LLR, int &nhits){
    float ECAL_Energy_Hypothesis=0;
    double dummy_L[3];
    return GetLikelihoodRatio_TrTrack(threshold, LLR,nhits,ECAL_Energy_Hypothesis,dummy_L);
}

int TrdKCluster::GetLikelihoodRatio_TrTrack(float threshold, double* LLR, int &nhits, float ECAL_Energy_Hypothesis, double *LL){
    double dummy_L[3];
    float dummy_length;
    float dummy_amp;
    if(!LL) return GetLikelihoodRatio(threshold, LLR,dummy_L,nhits,dummy_length,dummy_amp,&track_extrapolated_P0,&track_extrapolated_Dir,ECAL_Energy_Hypothesis);
    else return GetLikelihoodRatio(threshold, LLR,LL,nhits,dummy_length,dummy_amp,&track_extrapolated_P0,&track_extrapolated_Dir,ECAL_Energy_Hypothesis);

}

int TrdKCluster::GetLikelihoodRatio_TrTrack(float threshold, double* LLR, double* L, int &nhits, float &total_pathlength, float &total_amp,int flag_debug, float ECAL_Energy_Hypothesis){
    if(flag_debug<0)return GetLikelihoodRatio(threshold, LLR,L,nhits,total_pathlength,total_amp,&track_extrapolated_P0,&track_extrapolated_Dir);
    else return GetLikelihoodRatio_DEBUG(threshold, LLR,L,nhits,total_pathlength,total_amp,&track_extrapolated_P0,&track_extrapolated_Dir,flag_debug,ECAL_Energy_Hypothesis);
}


/////////////////////////////////////////////////////////////////////

int TrdKCluster::GetLikelihoodRatio(float threshold, double* LLR, double * L , int &nhits, float &total_pathlength, float &total_amp, AMSPoint* P0, AMSDir* Dir,float ECAL_Energy_Hypothesis){

    LLR[0]=-1;
    LLR[1]=-1;
    LLR[2]=-1;
    nhits=0;
    total_pathlength=0;
    total_amp=0;

    if(threshold>0){
        kpdf_e->SetNormalization(1,threshold);
        kpdf_p->SetNormalization(1,threshold);
        kpdf_h->SetNormalization(1,threshold);
    }


    if(NHits()<=0) return -10;

    int Track_nhits=0;
    double LL_pdf_track_particle[3]={1,1,1};
    double Likelihood_pdf_track_particle[3]={0,0,0};
    for(unsigned int i=0;i<corridor_hits.size();i++){
        TrdKHit *hit=GetCorridorHit(i);

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
            total_amp+=Amp;
            total_pathlength+=path_length;

            if(ECAL_Energy_Hypothesis<threshold_ECAL_Energy_Hypothesis){
                kpdf_e->GetPar(fabs(Track_Rigidity),path_length,Layer,Pressure_Xe/1000.);
                kpdf_p->GetPar(fabs(Track_Rigidity),path_length,Layer,Pressure_Xe/1000.);
                kpdf_h->GetPar(fabs(Track_Rigidity),path_length,Layer,Pressure_Xe/1000.);
            }else{
                if(DebugOn)   cout<<"Using ECAL Energy Hypothesis for electron/positron,  *2 for proton/Helium: "<<ECAL_Energy_Hypothesis<<endl;
                kpdf_e->GetPar(fabs(ECAL_Energy_Hypothesis),path_length,Layer,Pressure_Xe/1000.);
                kpdf_p->GetPar(fabs(ECAL_Energy_Hypothesis*2),path_length,Layer,Pressure_Xe/1000.);
                kpdf_h->GetPar(fabs(ECAL_Energy_Hypothesis*2),path_length,Layer,Pressure_Xe/1000.);
            }

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


    L[0]=Likelihood_pdf_track_particle[0];
    L[1]=Likelihood_pdf_track_particle[1];
    L[2]=Likelihood_pdf_track_particle[2];

    return 1;
}


int TrdKCluster::GetLikelihoodRatio_DEBUG(float threshold, double* LLR, double * L , int &nhits, float &total_pathlength, float &total_amp, AMSPoint* P0, AMSDir* Dir, int start_index,float ECAL_Energy_Hypothesis){

    LLR[0]=-1;
    LLR[1]=-1;
    LLR[2]=-1;
    nhits=0;
    total_pathlength=0;
    total_amp=0;

    if(threshold>0){
        kpdf_e->SetNormalization(1,threshold);
        kpdf_p->SetNormalization(1,threshold);
        kpdf_h->SetNormalization(1,threshold);
    }


    if(NHits()<=0) return -10;

    int Track_nhits=0;
    double LL_pdf_track_particle[3]={1,1,1};
    double Likelihood_pdf_track_particle[3]={0,0,0};
    vector<LikelihoodObject> v_p,v_e,v_h;
    for(unsigned int i=0;i<corridor_hits.size();i++){
        TrdKHit *hit=GetCorridorHit(i);

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
            if(ECAL_Energy_Hypothesis<threshold_ECAL_Energy_Hypothesis){
                kpdf_e->GetPar(fabs(Track_Rigidity),path_length,Layer,Pressure_Xe/1000.);
                kpdf_p->GetPar(fabs(Track_Rigidity),path_length,Layer,Pressure_Xe/1000.);
                kpdf_h->GetPar(fabs(Track_Rigidity),path_length,Layer,Pressure_Xe/1000.);
            }else{
                if(DebugOn)   cout<<"Using ECAL Energy Hypothesis for electron/positron,  *2 for proton/Helium: "<<ECAL_Energy_Hypothesis<<endl;
                kpdf_e->GetPar(fabs(ECAL_Energy_Hypothesis),path_length,Layer,Pressure_Xe/1000.);
                kpdf_p->GetPar(fabs(ECAL_Energy_Hypothesis*2),path_length,Layer,Pressure_Xe/1000.);
                kpdf_h->GetPar(fabs(ECAL_Energy_Hypothesis*2),path_length,Layer,Pressure_Xe/1000.);
            }
            double kp=kpdf_p->GetLikelihood(Amp);
            double kh=kpdf_h->GetLikelihood(Amp);
            double ke=kpdf_e->GetLikelihood(Amp);
            LikelihoodObject lp(1,Amp,path_length,kp);
            LikelihoodObject lh(2,Amp,path_length,kh);
            LikelihoodObject le(0,Amp,path_length,ke);

            v_p.push_back(lp);
            v_e.push_back(le);
            v_h.push_back(lh);
        }
    }

    if(Track_nhits<=start_index)  return -9;
    //    cout<<"Original vector: "<<endl;
    //    for(int i=0;i<v_p.size();i++){cout<<i<<", "<<v_p.at(i).likelihood<<endl;}
    sort(v_p.begin(),v_p.end(),LikelihoodObject::comp_likelihood);
    sort(v_e.begin(),v_e.end(),LikelihoodObject::comp_likelihood);
    sort(v_h.begin(),v_h.end(),LikelihoodObject::comp_likelihood);
    //    cout<<"Sorted vector: "<<endl;
    //    for(int i=0;i<v_p.size();i++){cout<<i<<", "<<v_p.at(i).likelihood<<endl;}

    Track_nhits=0;
    for(unsigned int i=start_index;i<v_p.size();i++){
        LL_pdf_track_particle[0]*=(v_e.at(i).likelihood);
        LL_pdf_track_particle[1]*=(v_p.at(i).likelihood);
        LL_pdf_track_particle[2]*=(v_h.at(i).likelihood);
        total_amp+=v_p.at(i).amp;
        total_pathlength+=v_p.at(i).l;
        Track_nhits++;
    }


    if(Track_nhits<=0)  return -9;
    for(int i=0;i<3;i++) Likelihood_pdf_track_particle[i]=pow(LL_pdf_track_particle[i],(double)(1./(double)Track_nhits));
    LLR[0]=-1*log( Likelihood_pdf_track_particle[0]/( Likelihood_pdf_track_particle[0]+Likelihood_pdf_track_particle[1]));
    LLR[1]=-1*log( Likelihood_pdf_track_particle[0]/( Likelihood_pdf_track_particle[0]+Likelihood_pdf_track_particle[2]));
    LLR[2]=-1*log( Likelihood_pdf_track_particle[1]/( Likelihood_pdf_track_particle[1]+Likelihood_pdf_track_particle[2]));
    nhits=Track_nhits;

    L[0]=Likelihood_pdf_track_particle[0];
    L[1]=Likelihood_pdf_track_particle[1];
    L[2]=Likelihood_pdf_track_particle[2];

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
        if(DebugOn) printf("TRD Online:  %i  %.2f  %.2f\n",test.time,test.xe,test.co2);
    }


}



/////////////////////////////////////////////////////////////////////

int TrdKCluster::GetXePressure(){
    if(ForceReadXePressure==0){
        Pressure_Xe=GetDefaultMCXePressure();
        return Pressure_Xe;
    }
    if(map_TRDOnline.size()==0)return -1;

    //TB Pressure
    if(Time<1300000000) {
        Pressure_Xe = 1000;
        return Pressure_Xe;
    }

    map<int, TRDOnline>::iterator  it=map_TRDOnline.upper_bound((int)Time);
    map<int, TRDOnline>::iterator  ithigh=it;
    map<int, TRDOnline>::iterator  itlow=--it;
    TRDOnline p_previous=(*itlow).second;
    TRDOnline p_next=(*ithigh).second;
    Pressure_Xe = p_previous.xe+(Time-p_previous.time)*(p_next.xe-p_previous.xe)/(p_next.time-p_previous.time);
    return Pressure_Xe;
}


/////////////////////////////////////////////////////////////////////

void TrdKCluster::GetOffTrackHit_TrTrack(int& nhits, float & amp, float threshold) const {
    GetOffTrackHit(nhits,amp,&track_extrapolated_P0,&track_extrapolated_Dir, threshold);
}

/////////////////////////////////////////////////////////////////////

void TrdKCluster::GetOffTrackHit_TRDRefit(int& nhits, float & amp, float threshold) const {
    if(!HasTRDTrack){
        cout<<"~~~WARNING~~~~TrdKCluster, Get OffTrackHit from TRDRefit, TRDTrack not yet defined"<<endl;
        nhits=-1;
        amp=-1;
        return;
    }

    GetOffTrackHit(nhits,amp,&TRDtrack_extrapolated_P0,&TRDtrack_extrapolated_Dir, threshold);
}

/////////////////////////////////////////////////////////////////////

void TrdKCluster::GetOffTrackHit(int& nhits, float & amp, const AMSPoint* P0, const AMSDir* Dir, float threshold) const {
    nhits=0;
    amp=0;
    for(int i=0;i<TrdHCalibR::n_tubes;i++){
        const TrdKHit *hit=GetConstHit(i);
        float path_length=hit->Tube_Track_3DLength(P0,Dir);
        //float dist=fabs(hit->Tube_Track_Distance_3D(P0,Dir));
        float Amp=hit->TRDHit_Amp;
        if(Amp > threshold && path_length == 0.){
            nhits++;
            amp+=Amp;
        }
    }
    return;
}

/////////////////////////////////////////////////////////////////////

void TrdKCluster::GetOnTrackHit_TrTrack(int& nhits, float & amp, float threshold) const {
    GetOnTrackHit(nhits,amp,&track_extrapolated_P0,&track_extrapolated_Dir, threshold);
}

/////////////////////////////////////////////////////////////////////

void TrdKCluster::GetOnTrackHit_TRDRefit(int& nhits, float & amp, float threshold) const {
    if(!HasTRDTrack){
        cout<<"~~~WARNING~~~~TrdKCluster, Get OnTrackHit from TRDRefit, TRDTrack not yet defined"<<endl;
        nhits=-1;
        amp=-1;
        return;
    }

    GetOnTrackHit(nhits,amp,&TRDtrack_extrapolated_P0,&TRDtrack_extrapolated_Dir, threshold);
}

/////////////////////////////////////////////////////////////////////

void TrdKCluster::GetOnTrackHit(int& nhits, float & amp, const AMSPoint* P0, const AMSDir* Dir, float threshold) const {
    nhits=0;
    amp=0;
    for(int i=0;i<TrdHCalibR::n_tubes;i++){
        const TrdKHit *hit=GetConstHit(i);
        float path_length=hit->Tube_Track_3DLength(P0,Dir);
        //float dist=fabs(hit->Tube_Track_Distance_3D(P0,Dir));
        float Amp=hit->TRDHit_Amp;
        if(Amp > threshold && path_length != 0.){
            nhits++;
            amp+=Amp;
        }
    }
    return;
}


/////////////////////////////////////////////////////////////////////

void TrdKCluster::GetNearTrackHit_TrTrack(int& nhits, float & amp, float radius, float threshold) const {
    GetNearTrackHit(nhits,amp,&track_extrapolated_P0,&track_extrapolated_Dir, radius, threshold);
}

/////////////////////////////////////////////////////////////////////

void TrdKCluster::GetNearTrackHit_TRDRefit(int& nhits, float & amp, float radius, float threshold) const {
    if(!HasTRDTrack){
        cout<<"~~~WARNING~~~~TrdKCluster, Get NearTrackHit from TRDRefit, TRDTrack not yet defined"<<endl;
        nhits=-1;
        amp=-1;
        return;
    }

    GetNearTrackHit(nhits,amp,&TRDtrack_extrapolated_P0,&TRDtrack_extrapolated_Dir, radius, threshold);
}

/////////////////////////////////////////////////////////////////////

void TrdKCluster::GetNearTrackHit(int& nhits, float & amp, const AMSPoint* P0, const AMSDir* Dir, float radius, float threshold) const {
    nhits=0;
    amp=0;
    for(int i=0;i<TrdHCalibR::n_tubes;i++){
        const TrdKHit *hit=GetConstHit(i);
        float dist=fabs(hit->Tube_Track_Distance_3D(P0,Dir));
        float Amp=hit->TRDHit_Amp;
        if(Amp > threshold && dist<=radius && dist > TrdKHit::Tube_radius){
            nhits++;
            amp+=Amp;
        }
    }
    return;
}

/////////////////////////////////////////////////////////////////////

int TrdKCluster::GetTRDRefittedTrack(AMSPoint &P0, AMSDir &Dir) const {

    if(!HasTRDTrack){
        printf("~~~WARNING~~~TrdKCluster, TRD Track Refitted Not exist yet, please performthe refit before getting the track~~~\n");
        return 0;
    }

    P0=TRDtrack_extrapolated_P0;
    Dir=TRDtrack_extrapolated_Dir;
    return 1;

}

/////////////////////////////////////////////////////////////////////

int TrdKCluster::GetTrTrackExtrapolation(AMSPoint &P0, AMSDir &Dir) const {

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
    double QTRD=0,QTRD1=0;
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
    double x0,x1,x2,y0,y1,y2,a,ExpErr,ErrorTemp;
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
    //  TRDChargeValue=-b/a/2;
    //  TRDChargeError=1/sqrt(2*a);
    ErrorTemp=1/sqrt(2*a);

    //Parabola fit to get minimum and error
    double X[7],Y[7],NFitPoint;
    double X0,DD,alpha,beta;
    double m11,m12,m13,m22,m23,m33,a11,a12,a13,a22,a23,b1,b2,b3;

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

    alpha = b1*a11 + b2*a12 + b3*a13;
    beta  = b1*a12 + b2*a22 + b3*a23;

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
    double QTRD=0,QTRD1=0;
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
    double x0,x1,x2,y0,y1,y2,a,ExpErr,ErrorTemp;
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
    //  TRDChargeValue=-b/a/2;
    //  TRDChargeError=1/sqrt(2*a);
    ErrorTemp=1/sqrt(2*a);

    //Parabola fit to get minimum and error
    double X[7],Y[7],NFitPoint;
    double X0,DD,alpha,beta;
    double m11,m12,m13,m22,m23,m33,a11,a12,a13,a22,a23,b1,b2,b3;

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

    alpha = b1*a11 + b2*a12 + b3*a13;
    beta  = b1*a12 + b2*a22 + b3*a23;

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
    double QTRD=0,QTRD1=0;
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
    double x0,x1,x2,y0,y1,y2,a,ExpErr,ErrorTemp;
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
    //  TRDChargeValue=-b/a/2;
    //  TRDChargeError=1/sqrt(2*a);
    ErrorTemp=1/sqrt(2*a);

    //Parabola fit to get minimum and error
    double X[7],Y[7],NFitPoint;
    double X0,DD,alpha,beta;
    double m11,m12,m13,m22,m23,m33,a11,a12,a13,a22,a23,b1,b2,b3;

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

    alpha = b1*a11 + b2*a12 + b3*a13;
    beta  = b1*a12 + b2*a22 + b3*a23;

    if(alpha/DD<=0) return 0;

    TRDChargeValueLower=X0-0.5*beta/alpha;

    if(TRDChargeValueLower<0) return 0;

    return TRDChargeValueLower;
}

/////////////////////////////////////////////////////////////////////

double TrdKCluster::GetIPChi2()
{
    double Par[5];
    Par[0]=TRDtrack_extrapolated_P0.x();
    Par[1]=TRDtrack_extrapolated_P0.y();
    Par[2]=TRDtrack_extrapolated_P0.z();
    double theta=TRDtrack_extrapolated_Dir.gettheta();
    double phi=TRDtrack_extrapolated_Dir.getphi();
    Par[3]=sin(theta)*cos(phi);
    Par[4]=sin(theta)*sin(phi);

    double TotalAmpRefit=0;
    for(vector<TrdKHit>::iterator it=QTRDHitCollectionRefit.begin();it!=QTRDHitCollectionRefit.end();it++)
    {
        TotalAmpRefit=TotalAmpRefit+(*it).TRDHit_Amp;
    }

    double IPChi2=TRDTrack_ImpactChi2_Charge(Par);
    IPChi2=IPChi2/TotalAmpRefit;
    IPChi2=IPChi2+8.982074956/10.0;

    return IPChi2;
}

/////////////////////////////////////////////////////////////////////

double TrdKCluster::GetAsyNormE()
{
    int Layer;
    double ADC;
    double Length;

    double UpE=0;
    double LowE=0;
    double UpPL=0;
    double LowPL=0;

    for(vector<TrdKHit>::iterator it=QTRDHitCollectionNuclei.begin();it!=QTRDHitCollectionNuclei.end();it++)
    {
        ADC=(*it).TRDHit_Amp;
        Layer=(*it).TRDHit_Layer;
        Length=(*it).Tube_Track_3DLength(&TRDtrack_extrapolated_P0,&TRDtrack_extrapolated_Dir);
        if(Layer>=10)
        {
            UpE+=ADC;
            UpPL+=Length;
        }
        else
        {
            LowE+=ADC;
            LowPL+=Length;
        }
    }

    if(UpPL) UpE=UpE/UpPL;
    else UpE=0;
    if(LowPL) LowE=LowE/LowPL;
    else LowE=0;
    if(UpE+LowE) return (UpE-LowE)/(UpE+LowE);
    else return -100;
}

/////////////////////////////////////////////////////////////////////

double TrdKCluster::GetAsyD()
{
    if(DAmpUpper+DAmpLower) return (DAmpUpper-DAmpLower)/(DAmpUpper+DAmpLower);
    else return -100;
}

/////////////////////////////////////////////////////////////////////

void TrdKCluster::SetCorridorHits() {
	corridor_hits.clear();
	corridor_hits.reserve(200);
	for(unsigned int i=0;i<TRDHitCollection.size();++i) {
		if(IsHitGood(&(TRDHitCollection[i]))) {
			corridor_hits.push_back(i);
		}
	}
}
