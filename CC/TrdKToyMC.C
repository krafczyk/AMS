#include "TrdKToyMC.h"
#include <TrTrack.h>


map<int, TRDOnline> TrdKToyMC::map_TRDOnline;

TrdKToyMC::TrdKToyMC()
{
}

TrdKToyMC::TrdKToyMC(int _seed, float Rigidity_Min,float Rigidity_Max)
{
    PrepareToyMC(_seed,Rigidity_Min,Rigidity_Max);
}


void TrdKToyMC::PrepareToyMC(int _seed, float Rigidity_Min,float Rigidity_Max){


    //==Prepare Tube Geometry==
    TRDAlignmentPar *par=new TRDAlignmentPar(0,0.00,0.00,0.00,0.000,0.000,0.000,0.00,0.00,115);
    for(int i=0;i<5248;i++){
        int layer,ladder, tube;
        TrdHCalibR::gethead()->GetLLTFromTubeId(layer,ladder,tube,i);
        TrdRawHitR *_trdhit=new TrdRawHitR(layer,ladder,tube,0);
        TrdKHit hit(_trdhit);
        hit.DoAlignment(par);
        hit.IsCalibrated=1;
        hit.IsAligned=1;
        Real_tube_collection.push_back(hit);
        delete _trdhit;
    }

    //==Prepare KPDF==
    kpdf_p=new TrdKPDF("Proton");
    kpdf_e=new TrdKPDF("Electron");
    kpdf_h=new TrdKPDF("Helium");

    //==Random Seed==
    gRandom=new TRandom3();
    Seed=_seed;
    gRandom -> SetSeed(Seed);
    kpdf_p->SetSeed(Seed);
    kpdf_e->SetSeed(Seed);
    kpdf_h->SetSeed(Seed);


    //==Prepare Spectrum==

    float mc_Rigidity_min=Rigidity_Min;
    float mc_Rigidity_max=Rigidity_Max;

    float par0[3]={0,0,0};
    float par1[3]={2.75e07,1.196e09,8.116e08};
    float par2[3]={-3.335,-2.801,-2.678};

    TF1* f_myrigidity[3];
    TString s_Particle[4]={"_Electron","_Proton","_Helium","_All"};
    for(int i=0;i<3;i++){
        f_myrigidity[i] = new TF1("f_rigidity"+s_Particle[i], Form("[0]+[1]*TMath::Power(x,[2])"),mc_Rigidity_min,mc_Rigidity_max);
        f_myrigidity[i]->SetParameter(0,par0[i]);
        f_myrigidity[i]->SetParameter(1,par1[i]);
        f_myrigidity[i]->SetParameter(2,par2[i]);
        f_myrigidity[i]->SetNpx(5000);
        //        norm_rigidity[i]=f_rigidity[i]->Eval(20);
    }

    SetSpectrum(f_myrigidity);

    MDR=1500;


    InitXePressure();



}




void TrdKToyMC::InitXePressure(){

    cout<<"Initilize Online TRD Parameters map..........."<<endl;

    //================================
    vector<ULong64_t> v_time;
    vector<Double_t> v_xe;
    vector<Double_t> v_co2;

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


float TrdKToyMC::GetXePressure(int Time){
    if(map_TRDOnline.size()==0)return -1;
    map<int, TRDOnline>::iterator  it=map_TRDOnline.upper_bound((int)Time);
    map<int, TRDOnline>::iterator  ithigh=it;
    map<int, TRDOnline>::iterator  itlow=--it;
    TRDOnline p_previous=(*itlow).second;
    TRDOnline p_next=(*ithigh).second;
    Pressure_Xe = p_previous.xe+(Time-p_previous.time)*(p_next.xe-p_previous.xe)/(p_next.time-p_previous.time);
    if(Pressure_Xe < 600)Pressure_Xe=750;
    return Pressure_Xe;
}



int TrdKToyMC::GenerateToyMCEvent(int Time, int particle){

    if(particle>=0)  MCParticle=particle;

    if(MCParticle>=3)cout<<"Warning, Particle Type Not Supported:  "<<MCParticle<<endl;

    mc_Rigidity=f_rigidity[MCParticle]->GetRandom();
    MCTrack_Rigidity=mc_Rigidity;
    TrTrack_Rigidity=1/((1/MCTrack_Rigidity)+(gRandom->Gaus(0,1)/MDR));

    AMSPoint mc_Origin(gRandom->Uniform(-200,200),gRandom->Uniform(-200,200),190);
    float temp_dz=gRandom->Uniform(-1,-0.7);
    float temp_dxy=sqrt(1-TMath::Power(temp_dz,2));
    float temp_phi=gRandom->Uniform(0,2*TMath::Pi());
    float temp_dx=temp_dxy*TMath::Cos(temp_phi);
    float temp_dy=temp_dxy*TMath::Sin(temp_phi);
    AMSDir mc_Dir(temp_dx,temp_dy,temp_dz);

    TrProp  *MCTrack_Pro= new TrProp(mc_Origin,mc_Dir,mc_Rigidity);
    static double Z_UpperTOF_Plane1=65.2;
    static double Z_LowerTOF_Plane4=-65.2;
    static double Z_TRD=115;
    MCTrack_Pro->Propagate(Z_UpperTOF_Plane1);
    AMSPoint hit_TOF1=MCTrack_Pro->GetP0();
    MCTrack_Pro->Propagate(Z_LowerTOF_Plane4);
    AMSPoint hit_TOF4=MCTrack_Pro->GetP0();
    if(fabs(hit_TOF1.x())>55 || fabs(hit_TOF1.y())>55 || fabs(hit_TOF4.x())>55 || fabs(hit_TOF4.y())>55 ){
        delete MCTrack_Pro;
        return -1;
    }


    MCTrack_Pro->Propagate(Z_TRD);
    MCTrack_P0=MCTrack_Pro->GetP0();
    MCTrack_Dir=MCTrack_Pro->GetDir();


    //=========Applied multiple scattering to Plane 2 measumenrt ============
    MCTrack_Pro->Propagate(0);
    AMSPoint temp_Track_P0=MCTrack_Pro->GetP0();
    AMSDir temp_Track_Dir=MCTrack_Pro->GetDir();
    static float Z_Plane2=55.0;
    MCTrack_Pro->Propagate(Z_Plane2);
    AMSPoint temp_Track_P0_Plane2=MCTrack_Pro->GetP0();
    AMSDir temp_Track_Dir_Plane2=MCTrack_Pro->GetDir();
    delete MCTrack_Pro;

    float charge=1;
    if(MCParticle==2)charge=2;
    float mass=0;
    if(MCParticle==0)mass=0.000511; //GeV
    else if(MCParticle==1)mass=0.938; //GeV
    else if(MCParticle==2)mass=3.7274; //GeV

    float frac_radiationlength=0.20;
    float momentum=fabs(mc_Rigidity)*charge;
    float velocity=sqrt(TMath::Power(momentum/mass,2)/(1+TMath::Power(momentum/mass,2)));
    float theta0=(0.0136/(momentum*velocity))*charge*sqrt(frac_radiationlength)*(1+0.038*TMath::Log(frac_radiationlength));
    float theta_x=TMath::ATan(temp_Track_Dir_Plane2.x()/temp_Track_Dir_Plane2.z());
    float theta_y=TMath::ATan(temp_Track_Dir_Plane2.y()/temp_Track_Dir_Plane2.z());
    theta_x+=gRandom->Gaus(0,theta0);
    theta_y+=gRandom->Gaus(0,theta0);

    float dz=1;
    float dx=dz*TMath::Tan(theta_x);
    float dy=dz*TMath::Tan(theta_y);
    float dd=sqrt(TMath::Power(dx,2)+TMath::Power(dy,2)+TMath::Power(dz,2));
    dz/=dd;
    dx/=dd;
    dy/=dd;

    AMSDir temp_Track_Dir_Plane2_Scattered=AMSDir(dx,dy,dz);

    TrProp  *TrTrack_Pro= new TrProp(temp_Track_P0_Plane2,temp_Track_Dir_Plane2_Scattered,TrTrack_Rigidity);
    TrTrack_Pro->Propagate(Z_TRD);
    TrTrack_P0=TrTrack_Pro->GetP0();
    TrTrack_Dir=TrTrack_Pro->GetDir();
    delete TrTrack_Pro;





    mc_Pressure=GetXePressure(Time);
    GeneratePattern(&MCTrack_P0,&MCTrack_Dir);
    _trdcluster=TrdKCluster(select_tube,&MCTrack_P0,&MCTrack_Dir,&MCTrack_P0,&MCTrack_Dir,&MCTrack_P0,&MCTrack_Dir);
    _trdcluster.Pressure_Xe=Pressure_Xe;
    _trdcluster.SetTrTrack(&MCTrack_P0,&MCTrack_Dir,MCTrack_Rigidity);

    return _trdcluster.NHits();
}


void TrdKToyMC::GeneratePattern(AMSPoint* P0, AMSDir *Dir){

   static Int_t    N_Modules[20] = {14, 14, 14, 14, 16, 16, 16, 16, 16, 16, 16, 16, 18, 18, 18, 18, 18, 18, 18, 18};
   static Double_t Module_Size   = 10.1;
   static Double_t Tube_Diameter = 0.606;


    Double_t X=P0->x();
    Double_t Y=P0->y();
    Double_t Z=P0->z();
    Double_t kX=Dir->x()/Dir->z();
    Double_t kY=Dir->y()/Dir->z();

    pdf=0;
    if(MCParticle==0)pdf=kpdf_e;
    else if(MCParticle==1)pdf=kpdf_p;
    else if(MCParticle==2)pdf=kpdf_h;


    Int_t i, j, k, l, m;
    Double_t x, y, z, v, d, p;

    int layer, ladder,dir;
    int tubeid;

    select_tube.clear();

    for (i=0; i<40; i++) {
        layer=i/2;
        if((layer>=16)||(layer<=3)) dir = 1;
        else                        dir = 0;
        z = 85.275 + 1.45*i;
        if(dir==0) x = X + kX*(z-Z);
        else x = Y + kY*(z-Z);
        m = (Int_t) ( x/Module_Size) + 0.5*N_Modules[i/2];
        for ( l=-1; l<2; l++){
            int module=m+l;
            if ( module>=0
                 && module<N_Modules[i/2]
                 && ( ( (i>=8 && i<=23) && (module%2)==(i%2)) ||  ( (i<8 || i>23) && (module%2)!=(i%2)) ) )
            {
                float module_start_position=(module-0.5*N_Modules[i/2])*Module_Size;
                k = (Int_t) ( (x - module_start_position)/Tube_Diameter);
                for ( j=-2; j<=2; j++) {
                    int tube=k+j;
                    if ( tube>=0 && tube<16 ) {
                        TrdHCalibR::gethead()->GetTubeIdFromLLT(layer,module,tube,tubeid);
                        TrdKHit hit=Real_tube_collection.at(tubeid);
                        float path=hit.Tube_Track_3DLength_New(P0,Dir);
                        if ( path > 0) {
                            pdf->GetPar(mc_Rigidity,path,layer,mc_Pressure/1000.);
                            hit.TRDHit_Amp=pdf->GenerateHit(0);
                        }else{
                            hit.TRDHit_Amp=0;
                        }
                        select_tube.push_back(hit);
                    }
                }
            }
        }

    }

    return;
}


TrdKCluster TrdKToyMC::GetKCluster(int TrackType){

    if(TrackType==0)_trdcluster.SetTrTrack(&MCTrack_P0,&MCTrack_Dir,MCTrack_Rigidity);
    if(TrackType==1)_trdcluster.SetTrTrack(&TrTrack_P0,&TrTrack_Dir,TrTrack_Rigidity);

    return _trdcluster;
}
