#include <TrdKCalib.h>
#include <commonsi.h>
#include <timeid.h>
ClassImp(TrdKCalib)

//#include <TRDDBClass_ROOT.h>


//AMSTimeID* TrdKCalib::tid_Calib = 0;
//AMSTimeID* TrdKCalib::tid_Align_Plane = 0;
//AMSTimeID* TrdKCalib::tid_Align_Global = 0;

TrdKCalib* TrdKCalib::_Head=0;


TString TrdKCalib::GetEnv( const string & var ) {
    const char * val = getenv( var.c_str() );
    if ( val == 0 ) {
        return "";
    }
    else {
        return TString(val);
    }
}


template <class T>
void TrdKCalib::fillDB(TString s_type,  T *_db, Double_t time_start, Double_t time_end){
    alignment_last_valide_time=time_end;
    time_t starttime=(time_t)time_start;
    time_t endtime=(time_t)time_end;
    tm begin;
    tm end;
	localtime_r(&starttime,&begin);
	localtime_r(&endtime,&end);
    AMSTimeID *tid= new AMSTimeID(
                AMSID(s_type,1),
                begin,
                end,
                sizeof(*_db),
                _db,
                AMSTimeID::Standalone,
                1);
    tid->UpdateMe();
	tid->write(AMSDATADIR.amsdatabase);
    //        int pp=tid->write(getenv("TRDDataDir"));
    delete tid;
}


template <class T>
int TrdKCalib::readDB(TString s_type,  T *db, Double_t asktime){


    static AMSTimeID* tid_Calib=0;
    static AMSTimeID* tid_Align_Plane=0;
    static AMSTimeID* tid_Align_Global=0;

    AMSTimeID **tid;

    if(s_type=="TRDCalibration")tid=&tid_Calib;
    else     if(s_type=="TRDAlignmentPlane")tid=&tid_Align_Plane;
    else     if(s_type=="TRDAlignmentGlobal")tid=&tid_Align_Global;
    else{
        Error("TrdKCalib::readDB -E-", "No DB recode found for name: %s", s_type.Data());
        return -999;
    }

    time_t time=(time_t) asktime;
    if(!(*tid)){
//        cout<<"Pointer to tid not exist yet, create onece: "<<s_type<<endl;
        time_t endtime=time;
        time_t starttime=time+1;
        tm begin;
        tm end;
		localtime_r(&starttime,&begin);
		localtime_r(&endtime,&end);
        *tid= new AMSTimeID(AMSID(s_type,1),begin,end,sizeof(*db),db,AMSTimeID::Standalone,1);

//        cout<<"DEBUG: tid: " <<(*tid)<<endl;
//        cout<<"DEBUG: tid_Calib: " <<tid_Calib<<endl;
//        cout<<"DEBUG: tid_Align_Plane: " <<tid_Align_Plane<<endl;
//        cout<<"DEBUG: tid_Align_Global: " <<tid_Align_Global<<endl;

    }


//    cout<<"tid->Validate: "<<time<<endl;
    int read=(*tid)->validate(time);
    return read;
}



void TrdKCalib::fillDB_Calibration(TRDCalibPar *_db, Double_t time_start, Double_t time_end){
    fillDB("TRDCalibration",_db,time_start,time_end);
}


void TrdKCalib::fillDB_Alignment_Plane(TRDAlignmentDB_Plane *_db, Double_t time_start, Double_t time_end){
    fillDB("TRDAlignmentPlane",_db,time_start,time_end);
}

void TrdKCalib::fillDB_Alignment_Global(TRDAlignmentDB_Global *_db, Double_t time_start, Double_t time_end){
    fillDB("TRDAlignmentGlobal",_db,time_start,time_end);
}


int TrdKCalib::readDB_Calibration(Double_t asktime){

    //    if(asktime>=_trddb.Time_E){
    //        printf("Ask time: %f ,  Previos End Time: %f\n",asktime,_trddb.Time_E);

//    if(asktime <_trddb.Time_S || asktime>=_trddb.Time_E){
//        _trddb=TRDCalibPar();
        readDB("TRDCalibration",&_trddb,asktime);

        if(_trddb.Time_A==0){
            Error("TrdKCalib::readDB_GainCalibration-E-", "Gain Calibration Period average time return zero : %f", asktime);

            return 0;
        }

//    }


    return 1;
}


int TrdKCalib::readDB_Alignment_Plane(Double_t asktime){

    //    if(asktime>_trdaligndb_Plane.time_end){

//    if(asktime<_trdaligndb_Plane.time_start || asktime>_trdaligndb_Plane.time_end){
//        _trdaligndb_Plane=TRDAlignmentDB_Plane();
        readDB("TRDAlignmentPlane",&_trdaligndb_Plane,asktime);


        //        printf("TRDAlignmentPlane DB read, Validity: %i - %i \n",_trdaligndb_Plane.time_start,_trdaligndb_Plane.time_end);
        TRDAlignmentPar * par= _trdaligndb_Plane.getplaneparp(0);
        if(par->dX==0 && par->dY==0 && par->dZ==0){
            //            printf("****ERROR****** Plane Alignment parameter return zero within validity period,  %f\n",asktime);
            Error("TrdKCalib::readDB_Alignment_Plane-E-", "Plane Alignment return zero : %f", asktime);
            return 0;
        }
//    }
    return 1;
}

int TrdKCalib::readDB_Alignment_Global(Double_t asktime){


    //   printf("Asking: %.0f , Current DB valid until: %.0f\n",asktime,_trdaligndb_Global.time_end);
    //    if(asktime>_trdaligndb_Global.time_end){
    //
//    if(asktime<_trdaligndb_Global.time_start || asktime>_trdaligndb_Global.time_end){

//        _trdaligndb_Global=TRDAlignmentDB_Global();
        readDB("TRDAlignmentGlobal",&_trdaligndb_Global,asktime);
        //        printf("TRDAlignmentGlobal Asking: %.0f \n",asktime);
        //        printf("TRDAlignmentGlobal DB read, Validity: %f - %f \n",_trdaligndb_Global.time_start,_trdaligndb_Global.time_end);

        if(!_trdaligndb_Global.size){
            Error("TrdKCalib::readDB_Alignment_Global-E-", "Global Alignment size zero : %f", asktime);

            return 0;

        }
            //    }
    /**
        float average[6]={0,0,0,0,0,0};
        int count=0;
        for(int i=0;i<_trdaligndb_Global.size;i++){
            if(_trdaligndb_Global.globalpar[i].dX==0)continue;
            average[0]+=_trdaligndb_Global.globalpar[i].dX;
            average[1]+=_trdaligndb_Global.globalpar[i].dY;
            average[2]+=_trdaligndb_Global.globalpar[i].dZ;
            average[3]+=_trdaligndb_Global.globalpar[i].alpha;
            average[4]+=_trdaligndb_Global.globalpar[i].beta;
            average[5]+=_trdaligndb_Global.globalpar[i].gamma;
            count++;
        }
        if(count!=0)for(int i=0;i<6;i++){
            average[i]/=count;
        }
        //        printf("TRDAlignmentGlobla, Average Alignment Parameter based on %i measurement \n",count);
        TRDAlignmentPar_Global_average=TRDAlignmentPar(-1,average[0],average[1],average[2],average[3],average[4],average[5],_trdaligndb_Global.globalpar[0].RotationCenter_X,_trdaligndb_Global.globalpar[0].RotationCenter_Y,_trdaligndb_Global.globalpar[0].RotationCenter_Z);
    }
    **/

    return 1;
}


int TrdKCalib::readDB_Alignment(Double_t asktime,int readplane, int readglobal){

    int r1=0;
    int r2=0;
    //    cout<<"Read ? "<<readplane<<", "<<readglobal<<endl;
    if(readplane==1)  r1=readDB_Alignment_Plane(asktime);
    if(readglobal==1) r2=readDB_Alignment_Global(asktime);


    if(r1==0 && r2==0)return 0;
    else if(r1!=0 && r2==0)return 1;
    else if(r1!=0 && r2!=0)return 2;
    else if(r1==0 && r2!=0) return -1;

    return -2;
}



double TrdKCalib::GetGainCorrectionFactorTube(int tubeid, double asktime){
    return _trddb.GetGainCorrectionFactorTube(tubeid,asktime);
}
double TrdKCalib::GetGainCorrectionFactorModule(int Moduleid, double asktime){
    return _trddb.GetGainCorrectionFactorModule(Moduleid,asktime);
}

TRDAlignmentPar * TrdKCalib::GetAlignmentPar_Global(int t){
    TRDAlignmentPar * par= _trdaligndb_Global.GetPar(t);
    if(par->dX!=0 && par->dY!=0 && par->dZ!=0 ){
        //            printf("DX: %f \n",par->dX);
        return par;
    }
    else {
        //            printf("****WARNING****** Alignment parameter return zero within validity period, try to return average value %i \n",t);
        //        return &TRDAlignmentPar_Global_average;
        return 0;
    }
}

TRDAlignmentPar *TrdKCalib::GetAlignmentPar_Plane(int plane){
    TRDAlignmentPar * par= _trdaligndb_Plane.getplaneparp(plane);
    if(par->dX!=0 && par->dY!=0 && par->dZ!=0 ) return par;
    else {
        //            printf("****ERROR****** Plane Alignment parameter return zero within validity period\n");
        return par;
    }
}

TRDAlignmentPar TrdKCalib::GetAlignmentPar(int plane,int t){

    TRDAlignmentPar * plane_par= GetAlignmentPar_Plane(plane);
    TRDAlignmentPar * plane0_par= GetAlignmentPar_Plane(0);
    TRDAlignmentPar *global_par=GetAlignmentPar_Global(t);

    if(!global_par)return *plane_par;
    //    if(global_par->dX==0 && global_par->dY==0 && global_par->dZ==0){
    //        return *plane_par;
    //    }


    AMSRotMat RGlobal=GetRotationMatrix(global_par->alpha,global_par->beta,global_par->gamma);
    AMSPoint TGlobal(global_par->dX,global_par->dY,global_par->dZ);
    AMSRotMat R1=GetRotationMatrix(plane_par->alpha,plane_par->beta,plane_par->gamma);
    AMSRotMat R2=GetRotationMatrix(plane0_par->alpha,plane0_par->beta,plane0_par->gamma);
    AMSPoint T1(plane_par->dX,plane_par->dY,plane_par->dZ);
    AMSPoint T2(plane0_par->dX,plane0_par->dY,plane0_par->dZ);
    AMSPoint TRDCenter(plane0_par->RotationCenter_X,plane0_par->RotationCenter_Y,plane0_par->RotationCenter_Z);


    AMSRotMat R2Inv=R2;
    R2Inv.Invert();
    AMSRotMat RR=R2Inv*R1;
    AMSPoint TT=R2Inv*(T1-T2);

    AMSRotMat R=RGlobal*RR;
    AMSPoint T=RGlobal*TT+TGlobal;

    double alpha, beta,gamma;
    R.GetRotAngles(alpha,beta,gamma);
    alpha*=-1;
    beta*=-1;
    gamma*=-1;
    TRDAlignmentPar par(-1,T.x(),T.y(),T.z(),alpha,beta,gamma,TRDCenter.x(),TRDCenter.y(),TRDCenter.z());
    return par;
}


AMSRotMat TrdKCalib::GetRotationMatrix(float angle_alpha, float angle_beta, float angle_gamma){
    AMSRotMat temp_RotMat_ALL;
    temp_RotMat_ALL.SetRotAngles(-angle_alpha,-angle_beta,-angle_gamma);
    return temp_RotMat_ALL;
}


void TrdKCalib::WriteDBFromRoot_Calib_TB(TString f_input){

    TChain *fChain=new TChain("tube");
    fChain->Add(f_input);

    // Declaration of leaf types
    Float_t         Ind[5248];
    Float_t         Par[5248];
    Float_t         Err[5248];

    // List of branches
    TBranch        *b_Ind;   //!
    TBranch        *b_Par;   //!
    TBranch        *b_Err;   //!

    fChain->SetBranchAddress("Ind", Ind, &b_Ind);
    fChain->SetBranchAddress("Par", Par, &b_Par);
    fChain->SetBranchAddress("Err", Err, &b_Err);




    if (fChain == 0) return;
    fChain->GetEntry(0);


    int Time_S=1281300000;
    int Time_E=1282400000;
    int Time_A=Time_S;
    float The_A=0;
    float Phi_A=0;
    float Gain[328];
    float Rate[328];

    for(int i=0;i<5248;i++){
        Par[i]*=1.30;
    }

    for(int i=0;i<328;i++){
        Rate[i]=-1*2.3e-7/1.30;
        Gain[i]=1;
    }

    TRDCalibPar _db(Time_S,Time_E,Time_A,The_A,Phi_A,Par,Err,Gain,Rate);
    fillDB_Calibration(&_db,(double)Time_S,(double)Time_E);







}


void TrdKCalib::WriteDBFromRoot_Calib(vector<TString> f_input){
    //        printf("Write into AMSDataDir:  %s \n",AMSDATADIR_DEF::amsdatabase);
    TChain *fChain=new TChain("calib");
    for(int i=0;i<f_input.size();i++){
        //            printf("Read Calibration Constant from  Input File: %s \n",f_input.at(i));
        fChain->Add(f_input.at(i));
    }
    // Declaration of leaf types
    Int_t           Time_S;
    Int_t           Time_E;
    Double_t        Time_A;
    Double_t        The_A;
    Double_t        Phi_A;
    Float_t         Par[5248];
    Float_t         Err[5248];
    Float_t         Gain[328];
    Float_t         Rate[328];

    // List of branches
    TBranch        *b_Time_S;   //!
    TBranch        *b_Time_E;   //!
    TBranch        *b_Time_A;   //!
    TBranch        *b_The_A;   //!
    TBranch        *b_Phi_A;   //!
    TBranch        *b_Par;   //!
    TBranch        *b_Err;   //!
    TBranch        *b_Gain;   //!
    TBranch        *b_Rate;   //!

    fChain->SetBranchAddress("Time_S", &Time_S, &b_Time_S);
    fChain->SetBranchAddress("Time_E", &Time_E, &b_Time_E);
    fChain->SetBranchAddress("Time_A", &Time_A, &b_Time_A);
    fChain->SetBranchAddress("The_A", &The_A, &b_The_A);
    fChain->SetBranchAddress("Phi_A", &Phi_A, &b_Phi_A);
    fChain->SetBranchAddress("Par", Par, &b_Par);
    fChain->SetBranchAddress("Err", Err, &b_Err);
    fChain->SetBranchAddress("Gain", Gain, &b_Gain);
    fChain->SetBranchAddress("Rate", Rate, &b_Rate);



    if (fChain == 0) return;
    Long64_t nentries = fChain->GetEntries();
    Long64_t nbytes = 0, nb = 0;
    //    printf("Nentries: %i \n",nentries);
    for (Long64_t jentry=0; jentry<nentries;jentry++) {
        nb = fChain->GetEntry(jentry);   nbytes += nb;
        TRDCalibPar _db(Time_S,Time_E,Time_A,The_A,Phi_A,Par,Err,Gain,Rate);
        fillDB_Calibration(&_db,(double)Time_S,(double)Time_E);
    }


}


void TrdKCalib::WriteDBFromRoot_Alignment(TTree *fChain){
    //#ifdef __TRDDB_ROOT__

    /**
    TRDAlignmentDB_Global_ROOT *t_TRDAlignmentDB_Global;
    Double_t        t_writetime;
    TBranch         *b_t_TRDAlignmentDB_Global;
    TBranch        *b_t_writetime;

    t_TRDAlignmentDB_Global=0;
    fChain->SetMakeClass(1);
    fChain->SetBranchAddress("t_TRDAlignmentDB_Global",&t_TRDAlignmentDB_Global,&b_t_TRDAlignmentDB_Global);
    fChain->SetBranchAddress("t_writetime", &t_writetime, &b_t_writetime);

    int nentries=fChain->GetEntries();
    cout<<"Entries: "<<nentries<<endl;
    for(int i=0;i<nentries;i++){
        fChain->GetEntry(i);

        TRDAlignmentDB_Global *constant =t_TRDAlignmentDB_Global->MakeDB();
                new TRDAlignmentDB_Global(t_TRDAlignmentDB_Global);
        printf("Time in DB: %.0f %.0f \n",constant->time_start,constant->time_end);

        double start_time=constant->time_start;
        double end_time=constant->time_end;
        this->fillDB_Alignment_Global(constant,start_time,end_time);

    }
    **/
    //#endif
}








