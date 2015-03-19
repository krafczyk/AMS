
//! ECAL TDV Class
/*!
 * Source file for EcalKDbc.h
 * Tools to interface with DataBase
 * \author: hachen@cern.ch
*/

#include "commonsi.h"
#include "timeid.h"
#include <fstream>

#include "EcalKDbc.h"

//! EcalTDVTool class
/*!
 * Base class for all the calibration categories
 */

template  <typename T1>
int EcalTDVTool<T1>::ReadTDV(int rtime,int real){
    if(!TDVBlock || !TDVName){
        cerr<<"--->>TDV Read Problem!!!"<<endl;
        return -1;
    }
    time_t time=rtime;
    time_t brun=time+1;
    time_t erun=time;
    tm begin;
    tm end;
    gmtime_r(&brun, &begin);
    gmtime_r(&erun, &end);
    
    //--------Already load
    AMSTimeID* tdv=new AMSTimeID(AMSID(TDVName,real),begin,end,
                                 TDVSize,
                                 TDVBlock,
                                 AMSTimeID::Standalone,1);
    
    int status=tdv->validate(time);
    LoadTDVPar();
    delete tdv;
    return status;
}

//===========================================================
template  <typename T1>
int EcalTDVTool<T1>::WriteTDV(int brun,int erun,int real){
    if(!TDVBlock|| ! TDVName) {
        cerr<<"---->>TDV Write Problem!!!"<<endl;
        return -1;
    }
    
    time_t br=brun;
    time_t er=erun;
    tm begin;
    tm end;
    gmtime_r(&br, &begin);
    gmtime_r(&er, &end);
    cout << "Begin: " <<(int)brun<<"  " <<asctime(&begin)<<endl;
    cout << "End  : " <<(int)erun<<"  " <<asctime(&end  )<<endl;
    //--1 realdata  1--vertify  
    AMSTimeID *tdv=new AMSTimeID(AMSID(TDVName, real),begin,end,
                                 TDVSize,
                                 TDVBlock,
                                 AMSTimeID::Standalone,1
                                 );
    tdv->UpdateMe();
    int status=tdv->write(AMSDATADIR.amsdatabase);
    delete tdv;
    return status;
}


//! EcalAttPar class
/*!
 * Attenuation parameters
 * Long/Short/Fraction for each cell
 */

EcalAttPar* EcalAttPar::Head=0;

EcalAttPar *EcalAttPar::GetHead(){
    if(!Head)Head = new EcalAttPar();
    return Head;
}

EcalAttPar::EcalAttPar(){
    TDVName="EcalAttenuation";
    TDVParN=(ECNLayer*ECNCell*nattpar);
    TDVBlock=new float[TDVParN];
    TDVSize=TDVParN*sizeof(float);
}

EcalAttPar::EcalAttPar(float *arr,int brun,int erun){
    TDVName="EcalAttenuation";
    TDVParN=(ECNLayer*ECNCell*nattpar);
    TDVBlock=arr;
    TDVSize=TDVParN*sizeof(float);
    BRun=brun;
    ERun=erun;
    LoadTDVPar();
}

//===========================================================

void   EcalAttPar::LoadTDVPar(){
    int iblock=0;
    for(int ipar=0;ipar<nattpar;ipar++){
        for(int ilay=0;ilay<ECNLayer;ilay++){
            for(int icell=0;icell<ECNCell;icell++){
                int id=ilay*100+icell;
                iblock=nattpar*(ECNCell*ilay+icell)+ipar;
                attpar[ipar][id]=TDVBlock[iblock];
            }
        }
    }
    Isload=1;
}

//===========================================================
int  EcalAttPar::LoadFromFile(char *file){
    ifstream vlfile(file,ios::in);
    if(!vlfile){
        cerr <<"<---- Error: missing "<<file<<"--file !!: "<<endl;
        return -1;
    }
    //---load
    vlfile>>BRun>>ERun;
    int ib=0;
    for(int i=0;i<TDVParN;i++){
        vlfile>>TDVBlock[ib++];
    }
    LoadTDVPar();
    vlfile.close();
    return 0;
}

//===========================================================
void EcalAttPar::PrintTDV(){
    cout<<"<<----Print "<<TDVName<<endl;
    for(int i=0;i<TDVParN;i++){cout<<TDVBlock[i]<<" ";}
    cout<<'\n';
    cout<<"<<----end of Print "<<TDVName<<endl;
}

//! EcalEquilPar class
/*!
 * High gain equilization parameters
 * TestBeam 400GeV proton MIP as reference
 * Daily basis
 */

EcalEquilPar* EcalEquilPar::Head=0;

EcalEquilPar* EcalEquilPar::GetHead(){
    if(!Head)Head = new EcalEquilPar();
    return Head;
}

//===========================================================
EcalEquilPar::EcalEquilPar(){
    TDVName="EcalEquilization";
    TDVParN=(ECNLayer*ECNCell*nequil);
    TDVBlock=new float[TDVParN];
    TDVSize=TDVParN*sizeof(float);
}

//===========================================================
EcalEquilPar::EcalEquilPar(float *arr,int brun,int erun){
    TDVName="EcalEquilization";
    TDVParN=(ECNLayer*ECNCell*nequil);
    TDVBlock=arr;
    TDVSize=TDVParN*sizeof(float);
    BRun=brun;
    ERun=erun;
    LoadTDVPar();
}
//===========================================================
void  EcalEquilPar::LoadTDVPar(){
    int iblock=0;
    //----load par
    for(int ilay=0;ilay<ECNLayer;ilay++){
        for(int icell=0;icell<ECNCell;icell++){
            int id=ilay*100+icell;
            equil[id]=TDVBlock[iblock++];
        }
    }
    Isload=1;
}
//==========================================================
int  EcalEquilPar::LoadFromFile(char *file){
    ifstream vlfile(file,ios::in);
    if(!vlfile){
        cerr <<"<---- Error: missing "<<file<<"--file !!: "<<endl;
        return -1;
    }
    //---load
    vlfile>>BRun>>ERun;
    int ib=0;
    for(int i=0;i<TDVParN;i++){
        vlfile>>TDVBlock[ib++];
    }
    LoadTDVPar();
    vlfile.close();
    return 0;
}

//==========================================================
void EcalEquilPar::PrintTDV(){
    cout<<"<<----Print "<<TDVName<<endl;
    for(int i=0;i<TDVParN;i++){cout<<TDVBlock[i]<<" ";}
    cout<<'\n';
    cout<<"<<----end of Print "<<TDVName<<endl;
}


//! EcalGainRatioPar class
/*!
 * Gain ratio parameters for each cell
 * Static correction
 */

EcalGainRatioPar* EcalGainRatioPar::Head=0;

//==========================================================
EcalGainRatioPar *EcalGainRatioPar::GetHead(){
    if(!Head)Head = new EcalGainRatioPar();
    return Head;
}

EcalGainRatioPar::EcalGainRatioPar(){
    TDVName="EcalGainRatio";
    TDVParN=(ECNLayer*ECNCell*ngainpar);
    TDVBlock=new float[TDVParN];
    TDVSize=TDVParN*sizeof(float);
}

//==========================================================
EcalGainRatioPar::EcalGainRatioPar(float *arr,int brun,int erun){
    TDVName="EcalGainRatio";
    TDVParN=(ECNLayer*ECNCell*ngainpar);
    TDVBlock=arr;
    TDVSize=TDVParN*sizeof(float);
    BRun=brun;
    ERun=erun;
    LoadTDVPar();
}

//==========================================================
void   EcalGainRatioPar::LoadTDVPar(){
    int iblock=0;
    //----load par
    for(int ilay=0;ilay<ECNLayer;ilay++){
        for(int icell=0;icell<ECNCell;icell++){
            int id=ilay*100+icell;
            gain[id]=TDVBlock[iblock++];
        }
    }
    Isload=1;
}

//==========================================================
int  EcalGainRatioPar::LoadFromFile(char *file){
    ifstream vlfile(file,ios::in);
    if(!vlfile){
        cerr <<"<---- Error: missing "<<file<<"--file !!: "<<endl;
        return -1;
    }
    //---load
    vlfile>>BRun>>ERun;
    int ib=0;
    for(int i=0;i<TDVParN;i++){
        vlfile>>TDVBlock[ib++];
    }
    LoadTDVPar();
    vlfile.close();
    return 0;
}

//==========================================================
void EcalGainRatioPar::PrintTDV(){
    cout<<"<<----Print "<<TDVName<<endl;
    for(int i=0;i<TDVParN;i++){cout<<TDVBlock[i]<<" ";}
    cout<<'\n';
    cout<<"<<----end of Print "<<TDVName<<endl;
}

//! EcalPedPar class
/*!
 * Pedestal for each run
 * To check the operation of ECAL
 */

EcalPedPar* EcalPedPar::Head=0;

//==========================================================
EcalPedPar *EcalPedPar::GetHead(){
    if(!Head)Head = new EcalPedPar();
    return Head;
}

EcalPedPar::EcalPedPar(){
    TDVName="EcalPedestal";
    TDVParN=(ECNLayer*ECNCell*npedpar);
    TDVBlock=new float[TDVParN];
    TDVSize=TDVParN*sizeof(float);
}

//==========================================================
EcalPedPar::EcalPedPar(float *arr,int brun,int erun){
    TDVName="EcalPedestal";
    TDVParN=(ECNLayer*ECNCell*npedpar);
    TDVBlock=arr;
    TDVSize=TDVParN*sizeof(float);
    BRun=brun;
    ERun=erun;
    LoadTDVPar();
}

//==========================================================
void  EcalPedPar::LoadTDVPar(){
    int iblock=0;
    //----load par
    for(int ipar=0;ipar<npedpar;ipar++){
        for(int ilay=0;ilay<ECNLayer;ilay++){
            for(int icell=0;icell<ECNCell;icell++){
                int id=ilay*100+icell;
                iblock=npedpar*(ECNCell*ilay+icell)+ipar;
                ped[ipar][id]=TDVBlock[iblock];
            }
        }
    }
    Isload=1;
}

//==========================================================
int  EcalPedPar::LoadFromFile(char *file){
    ifstream vlfile(file,ios::in);
    if(!vlfile){
        cerr <<"<---- Error: missing "<<file<<"--file !!: "<<endl;
        return -1;
    }
    //---load
    vlfile>>BRun>>ERun;
    int ib=0;
    for(int i=0;i<TDVParN;i++){
        vlfile>>TDVBlock[ib++];
    }
    LoadTDVPar();
    vlfile.close();
    return 0;
}

//==========================================================
void EcalPedPar::PrintTDV(){
    cout<<"<<----Print "<<TDVName<<endl;
    for(int i=0;i<TDVParN;i++){cout<<TDVBlock[i]<<" ";}
    cout<<'\n';
    cout<<"<<----end of Print "<<TDVName<<endl;
}

/*
/////////////////////////////////////
// EcalAlignPar
/////////////////////////////////////

EcalAlignPar* EcalAlignPar::Head=0;

EcalAlignPar *EcalAlignPar::GetHead(){
    if(!Head)Head = new EcalAlignPar();
    return Head;
}

float EcalAlignPar::alignpar[4]={0};

EcalAlignPar::EcalAlignPar(){
    TDVName="EcalAlignment";
    TDVParN=nalignpar;
    TDVBlock=new float[TDVParN];
    TDVSize=TDVParN*sizeof(float);
}

EcalAlignPar::EcalAlignPar(float *arr,int brun,int erun){
    TDVName="EcalAlignment";
    TDVParN=nalignpar;
    TDVBlock=arr;
    TDVSize=TDVParN*sizeof(float);
    BRun=brun;
    ERun=erun;
    LoadTDVPar();
}

void   EcalAlignPar::LoadTDVPar(){
    int iblock=0;
    //----load par
    for(int ipar=0;ipar<nalignpar;ipar++){
        alignpar[ipar]=TDVBlock[iblock++];
    }
    Isload=1;
}

int  EcalAlignPar::LoadFromFile(char *file){
    ifstream vlfile(file,ios::in);
    if(!vlfile){
        cerr <<"<---- Error: missing "<<file<<"--file !!: "<<endl;
        return -1;
    }
    //---load
    vlfile>>BRun>>ERun;
    int ib=0;
    for(int i=0;i<TDVParN;i++){
        vlfile>>TDVBlock[ib++];
    }
    LoadTDVPar();
    vlfile.close();
    return 0;
}

void EcalAlignPar::PrintTDV(){
    cout<<"<<----Print "<<TDVName<<endl;
    for(int i=0;i<TDVParN;i++){cout<<TDVBlock[i]<<" ";}
    cout<<'\n';
    cout<<"<<----end of Print "<<TDVName<<endl;
}
*/

//! EcalTDV Manager class
/*! Manage all the ECAL TDV classes
 */

//==========================================================
EcalTDVManager *EcalTDVManager::Head=0;

EcalTDVManager *EcalTDVManager::GetHead(int real){
    if(!Head){Head = new EcalTDVManager(real);}
    else if(real!=Head->isreal){
        delete Head;
        Head = new EcalTDVManager(real);
    }
    return Head;
}
//==========================================================
EcalTDVManager::EcalTDVManager(int real){
    //---validate time
    isreal=real;
    ntime=0;
    time_t brun=1;
    time_t erun=0;
    tm begin;
    tm end;
    gmtime_r(&brun, &begin);
    gmtime_r(&erun, &end);
    AMSTimeID::CType server=AMSTimeID::Standalone;
    
    //----Insert Ecal Attenuation TDV into tdvmap
    EcalAttPar *EcalAtt=EcalAttPar::GetHead();
    AMSTimeID *tdv=new AMSTimeID(AMSID(EcalAtt->TDVName,isreal),begin,end,
                                 EcalAtt->TDVSize,
                                 EcalAtt->TDVBlock,
                                 server,1,EcalAttPar::HeadLoadTDVPar);
    tdvmap.insert(pair<string,AMSTimeID*>(EcalAtt->TDVName,tdv));
    
    //---Insert Ecal PM Equilization TDV into tdvmap
    EcalEquilPar *EcalEquil=EcalEquilPar::GetHead();
    tdv= new AMSTimeID(AMSID(EcalEquil->TDVName,isreal),begin,end,
                       EcalEquil->TDVSize,
                       EcalEquil->TDVBlock,
                       server,1,EcalEquilPar::HeadLoadTDVPar);
    tdvmap.insert(pair<string,AMSTimeID*>(EcalEquil->TDVName,tdv));
    
    //----Insert Ecal GainRatio TDV into tdvmap
    EcalGainRatioPar *EcalGainRatio=EcalGainRatioPar::GetHead();
    tdv=new AMSTimeID(AMSID(EcalGainRatio->TDVName,isreal),begin,end,
                      EcalGainRatio->TDVSize,
                      EcalGainRatio->TDVBlock,
                      server,1,EcalGainRatioPar::HeadLoadTDVPar);
    tdvmap.insert(pair<string,AMSTimeID*>(EcalGainRatio->TDVName,tdv));

/*
    //----Insert Ecal Pedestal TDV into tdvmap
    EcalPedPar *EcalPed=EcalPedPar::GetHead();
    tdv=new AMSTimeID(AMSID(EcalPed->TDVName,isreal),begin,end,
                      EcalPed->TDVSize,
                      EcalPed->TDVBlock,
                      server,1,EcalPedPar::HeadLoadTDVPar);
    tdvmap.insert(pair<string,AMSTimeID*>(EcalPed->TDVName,tdv));
    
    //----Insert Ecal Alignment TDV into tdvmap
    EcalAlignPar *EcalAlign=EcalAlignPar::GetHead();
    tdv=new AMSTimeID(AMSID(EcalAlign->TDVName,isreal),begin,end,
                      EcalAlign->TDVSize,
                      EcalAlign->TDVBlock,
                      server,1,EcalAlignPar::HeadLoadTDVPar);
    tdvmap.insert(pair<string,AMSTimeID*>(EcalAlign->TDVName,tdv));
*/
}

//==========================================================
int  EcalTDVManager::Validate(unsigned int time){
    static int tdvstat=-1;
    if(time==ntime){return tdvstat;}
    else {
        tdvstat=0;
        time_t vtime=time;
        for(map<string,AMSTimeID*>::iterator it=tdvmap.begin();it!=tdvmap.end();it++)
            if(!it->second->validate(vtime))tdvstat=-1;
        ntime=time;
        return tdvstat;
    }
}

//==========================================================
void  EcalTDVManager::Clear(){
    for(map<string,AMSTimeID*>::iterator it=tdvmap.begin();it!=tdvmap.end();it++){
        delete it->second;
    }
    tdvmap.clear();
}

