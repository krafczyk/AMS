//  $Id$

// ------------------------------------------------------------
//      AMS TOF recontruction-> /*IHEP TOF cal+rec version*/
// ------------------------------------------------------------
//      History
//        Created:       2012-June-10  Q.Yan  qyan@cern.ch
//        Modified:      2012-July-7   Change Recontruction to TOFRawSide+Coo Calib Add
//        Modified:      2012-july-18  Adding lib refit support module
//        Modified:      2012-Sep      Adding New Charge Reconstruction
//        Modified:      2012-Sep -12  Adding Tof Self Track Reconstruction
//        Modified:      2012-Oct -1   Update TOF Charge Part
//        Modified:      2013-Oct -28  Adding BetaH Counter Side Reconstruction
// -----------------------------------------------------------
#ifndef __ROOTSHAREDLIBRARY__
#include "tofrec02.h"
#include "tofdbc02.h"
#include "job.h"
#include "commons.h"
#include <limits>
#include "tofsim02.h"
#include "tofhit.h"
#include "cern.h"
#include "ecalrec.h"
#ifdef _PGTRACK_
#include "tkpatt.h"
#include "TrRecon.h"
#endif
#endif
#include <math.h>
#include "VCon.h"
#include <algorithm>
#include "Tofdbc.h"
#include "Tofrec02_ihep.h"

#ifndef __ROOTSHAREDLIBRARY__
extern "C" void rzerowrapper_(number & z0, number & zb, number & x0, number & zmin,int & ierr);
#endif
//========================================================
ClassImp(TofRecH)

TofBetaPar TofRecH::betapar;
AMSEventR *TofRecH::ev=0;
int        TofRecH::realdata=1;
//--Cluster
vector<TofRawSideR>   TofRecH::tfraws;
#ifndef __ROOTSHAREDLIBRARY__
vector<TOF2RawSide*>  TofRecH::tf2raws;
  #pragma omp threadprivate (TofRecH::tf2raws)
#endif
  bool  TofRecH::RebuildBetaHInReadHeader=true;

vector<TofClusterHR*> TofRecH::tofclh[4];
  #pragma omp threadprivate (TofRecH::tofclh)

vector<pair <TofClusterHR*,TofClusterHR* > > TofRecH::tofclp[2];
  #pragma omp threadprivate (TofRecH::tofclp)
 
vector<pair <TofClusterHR*,TofClusterHR* > > TofRecH::tofclc[2];
  #pragma omp threadprivate (TofRecH::tofclc)

//--Track
vector<TrTrackR*>     TofRecH::track;
  #pragma omp threadprivate ( TofRecH::track)
#ifndef __ROOTSHAREDLIBRARY__
vector<AMSTrTrack*>   TofRecH::amstrack;
  #pragma omp threadprivate ( TofRecH::amstrack)
#endif

vector<TrdTrackR>   TofRecH::trdtrack;
#ifndef __ROOTSHAREDLIBRARY__
vector<AMSTRDTrack*> TofRecH::amstrdtrack;
  #pragma omp threadprivate (TofRecH::amstrdtrack)
#endif

vector<EcalShowerR>   TofRecH::ecalshow;
#ifndef __ROOTSHAREDLIBRARY__
vector<AMSEcalShower*> TofRecH::amsecalshow;
  #pragma omp threadprivate (TofRecH::amsecalshow)
#endif

TF1  *TofRecH::BirkFun=0;

TF1  *TofRecH::BetaFun=0;

bool  TofRecH::BuildKey=1;

int   TofRecH::BuildOpt=0;

//========================================================
int TofRecH::ReBuild(int charge){
   BuildTofClusterH();
   BuildBetaH();  
   return 0;
}

//========================================================
int TofRecH::Init(){

  if(!BuildKey)return -1;
  static int nerr=0;
//---
  int tdvstat=0;
  unsigned int time,trun;
//----Initial TDV
#ifndef __ROOTSHAREDLIBRARY__
  realdata=AMSJob::gethead()->isRealData();
  time=AMSEvent::gethead()->gettime();
  trun=AMSEvent::gethead()->getrun();
#else
  ev=AMSEventR::Head();
  realdata=(ev->nMCEventg()==0)?1:0;
  time=ev->UTime();
  trun=ev->Run();
//--Adding Protection for MC
  if(realdata==0){
    if(trun<315532800||trun>1893456000)trun=1325376000;
  }
//--
  if(nerr<100){
    tdvstat=TofAlignManager::GetHead(realdata)->Validate(trun);
  }
  else tdvstat=-1;
#endif
  if(realdata){
     TofTdcPar::GetHead()->ntime=trun;
  }
  if(tdvstat!=0&&nerr++<100)cerr<<"<<-----Error TofRecH TDV Par Init Error"<<endl;
  return tdvstat;
}

//========================================================
int TofRecH::ClearBuildTofClH(){
   tfraws.clear();
#ifndef __ROOTSHAREDLIBRARY__
   tf2raws.clear();
#endif   
  return 0;
}

//========================================================
int TofRecH::ClearBuildBetaH(){
    for(int ilay=0;ilay<4;ilay++)tofclh[ilay].clear();
    for(int iud=0;iud<2;iud++){tofclp[iud].clear();tofclc[iud].clear();}
    track.clear();
    trdtrack.clear();
    ecalshow.clear();
#ifndef __ROOTSHAREDLIBRARY__
    amstrack.clear();
    amstrdtrack.clear();
    amsecalshow.clear();
#endif
   return 0;
}

//========================================================
int TofRecH::BuildTofClusterH(){

//--  
  if(Init()!=0)return -1; 
  ClearBuildTofClH();
//---  
  integer i,j,hassid;
  integer idd=0,il=0,ib=0,is=0,idsoft=-1,pattern=0,nraws=0;
  integer stat[2];
  uinteger sstatus[2]={0},status=0,recovsid[2];
  integer nadcd[2]={0};
  number sdtm[2]={0},adca[2]={0},timers[2]={0},timer=0,etimer=0,edepa=0,edepd=0,edep=0,q2pa[2]={0},q2pd[2][TOF2GC::PMTSMX]={{0}};
  number adcd[2][TOF2GC::PMTSMX]={{0}};
  vector<number>ltdcw[2];
  vector<number>htdcw[2];
  vector<number>shtdcw[2];
  AMSPoint coo,ecoo;

//-----new cont
  VCon* cont = GetVCon()->GetCont("AMSTOFClusterH");

#ifndef __ROOTSHAREDLIBRARY__
  TOF2RawSide *ptr=(TOF2RawSide*)AMSEvent::gethead()->getheadC("TOF2RawSide",0,1);
  while (ptr){
    tfraws.push_back(TofRawSideR(ptr)); 
    tf2raws.push_back(ptr);
    ptr=ptr->next();
  }
  TOF2RawSide *tfhraws[2]={0};
  TofRawSideR *tfhraws1[2]={0};

#else
    cont->eraseC();
    vector<pair <integer,integer > >sideid;
    tfraws=ev->TofRawSide();
    for(int i=0;i<tfraws.size();i++){
      sideid.push_back(make_pair<integer,integer>(i,tfraws.at(i).swid));
    }
//--increase id
    sort(tfraws.begin(),tfraws.end(),SideCompare);
    sort(sideid.begin(),sideid.end(),IdCompare<integer>);
    TofRawSideR *tfhraws[2]={0};
#endif  
//-----  
  for(int i=0;i<tfraws.size();i++){
     idd=tfraws.at(i).swid;
     il=idd/10/100-1;
     ib=idd/10%100-1;
     is=idd%10-1;
     stat[is]=tfraws[i].stat;
     if(stat[is]%10!=0)continue;//validation status(FTtime is required)
#ifndef __ROOTSHAREDLIBRARY__
    if(!TOF2Brcal::scbrcal[il][ib].SideOK(is)||!TOFBPeds::scbrped[il][ib].PedAchOK(is)){continue;}
    tfhraws[is]=tf2raws[i];
#else
    tfhraws[is]=ev->pTofRawSide(sideid[i].first);
#endif

//--already check get data
     TofSideRec(&tfraws[i],adca[is],nadcd[is],adcd[is],sdtm[is],sstatus[is],ltdcw[is],htdcw[is],shtdcw[is]);
     if(TOFGeom::Npmt[il][ib]==2&&adcd[is][TOFGeom::Npmt[il][ib]]>0){
        cerr<<"<<---Error Dynode Non Exist PMT-ADC>0"<<endl;adcd[is][TOFGeom::Npmt[il][ib]]=0;
     }

//--different LBB// combine to one bar
     if((i==tfraws.size()-1)||(tfraws[i+1].swid/10!=tfraws[i].swid/10)){
        idsoft=il*1000+ib*100;
        if(((sstatus[0]&TOFDBcN::BAD)>0)||(tfhraws[0]==0))status|=TOFDBcN::BADN;
        else                                            pattern|=TOFDBcN::SIDEN;
        if(((sstatus[1]&TOFDBcN::BAD)>0)||(tfhraws[1]==0))status|=TOFDBcN::BADP;
        else                                            pattern|=TOFDBcN::SIDEP;
//Two side bad abandon     
        if(((status&TOFDBcN::BADP)>0)&&((status&TOFDBcN::BADN)>0)){
           status|=TOFDBcN::BAD;
        }
   
//Reconstruction
      else {
//-- ReFind Lost  LT using other GOOD Side /*due to 30ns LT deadtime, it's enough to use other Side to ReFind LT
         if((((sstatus[0]|sstatus[1])&(TOFDBcN::NOHTRECOVCAD|TOFDBcN::NOMATCHRECOVCAD))>0)&&(BuildOpt>=0)){
            LTRefind(idsoft,0,sdtm,adca,status,ltdcw);
            TimeCooRec(idsoft,sdtm,adca,timers,timer,etimer,coo[TOFGeom::Proj[il]],ecoo[TOFGeom::Proj[il]],status);///Time Rec
//            if((status&TOFDBcN::BADTIME)==0)cout<<"refind LT="<<coo[TOFGeom::Proj[il]]<<endl;
          }

//-- NoNeed To ReFind Just Build
         else {
           uinteger ustatus=status;
           int tstat=TimeCooRec(idsoft,sdtm,adca,timers,timer,etimer,coo[TOFGeom::Proj[il]],ecoo[TOFGeom::Proj[il]],ustatus);
//--ReFind Again if also lost
           if((tstat!=-1&&(ustatus&TOFDBcN::BADTIME)>0)&&(BuildOpt>=0)){
             LTRefind(idsoft,0,sdtm,adca,status,ltdcw);
//             cout<<"misfind good coo="<<coo[TOFGeom::Proj[il]]<<endl;
             TimeCooRec(idsoft,sdtm,adca,timers,timer,etimer,coo[TOFGeom::Proj[il]],ecoo[TOFGeom::Proj[il]],status);
//             if((status&TOFDBcN::BADTIME)==0)cout<<"recover="<<coo[TOFGeom::Proj[il]]<<endl;
           }
           else  status=ustatus; 
//----
         }

//-----other coo
         AMSPoint barco=TOFGeom::GetBarCoo(il,ib);
         coo[1-TOFGeom::Proj[il]] =barco[1-TOFGeom::Proj[il]];
         ecoo[1-TOFGeom::Proj[il]]=(TOFGeom::Sci_w[il][ib]/2.+0.3)/3.;
         coo[2]=barco[2];
         ecoo[2]=TOFGeom::Sci_t[il][ib]/2./3.;//z 3sigma out

//---q2pa+q2pd are MIP unit Q^Q no birk corr, edepa+edepd unit MeV birk corr
        EdepRec(idsoft,adca,adcd,coo[TOFGeom::Proj[il]],q2pa,q2pd,edepa,edepd,sstatus);
//----  
#ifndef __ROOTSHAREDLIBRARY__
        cont->addnext(new AMSTOFClusterH(sstatus,status,pattern,idsoft,adca,adcd,sdtm,timers,timer,etimer,coo,ecoo,q2pa,q2pd,edepa,edepd,tfhraws,tfhraws1));
#else
        cont->addnext(new TofClusterHR(sstatus,status,pattern,idsoft,adca,adcd,sdtm,timers,timer,etimer,coo,ecoo,q2pa,q2pd,edepa,edepd,tfhraws));
#endif
      }//at lease one side good build

//--Clear part
      status=pattern=0;
      timer=etimer=edepa=edepd=edep=0;
      for(is=0;is<2;is++){
        tfhraws[is]=0; 
        ltdcw[is].clear();htdcw[is].clear();shtdcw[is].clear();
        sstatus[is]=0;
        timers[is]=adca[is]=sdtm[is]=0;
        nadcd[is]=0;
        q2pa[is]=0;
        for(int ipm=0;ipm<TOF2GC::PMTSMX;ipm++){adcd[is][ipm]=0;q2pd[is][ipm]=0;}
      }

//----
   }//end one bar

 } //end loop
 delete cont;    

 return 0;
}

//========================================================Rec Side
int TofRecH::TofSideRec(TofRawSideR *ptr,number &adca, integer &nadcd,number adcd[],number &sdtm,uinteger &sstatus,
                        vector<number>&ltdcw, vector<number>&htdcw, vector<number>&shtdcw){
//---
     integer i,j;
     integer ltok=0;
     number ftdch[TOF2GC::SCTHMX1],ltdch[TOF2GC::SCTHMX3],htdch[TOF2GC::SCTHMX2],shtdch[TOF2GC::SCTHMX2];
     number ftdc[TOF2GC::SCTHMX1], ltdc[TOF2GC::SCTHMX3], htdc[TOF2GC::SCTHMX2], shtdc[TOF2GC::SCTHMX2];
     geant radcd[TOF2GC::PMTSMX];
//---init
     sstatus=0;

//----Get adca 
     adca=ptr->adca;
     nadcd=ptr->nadcd;
     for(i=0;i<TOF2GC::PMTSMX;i++){adcd[i]=ptr->adcd[i];}
     if(adca<=0)  {sstatus|=TOFDBcN::NOADC;}
     if(adca>=TofRecPar::PUXMXCH){adca=TofRecPar::PUXMXCH;sstatus|=TOFDBcN::AOVERFLOW;}//now just anode 

//---Get Tdc
     integer nft=ptr->nftdc;
     for(i=0;i<nft;i++)ftdch[i]=number(ptr->fftdc[i]);//FT
     integer nlt=ptr->nstdc;
     for(i=0;i<nlt;i++)ltdch[i]=number(ptr->fstdc[i]);//LT
     integer nht=ptr->nsumh;
     for(i=0;i<nht;i++)htdch[i]=number(ptr->fsumht[i]);//SumHT
     integer nsht=ptr->nsumsh;
     for(i=0;i<nsht;i++)shtdch[i]=number(ptr->fsumsht[i]);

//---TDC No-Lin correction
      integer hwidt=ptr->hwidt;//CSIIII
      integer crat=hwidt/100000;//1-4
      integer sslot=(hwidt%100000)/10000;//1-5
      integer tdcch[4];
      tdcch[0]=(hwidt%10000)/1000;//LTch#(1-5), 0 if missing
      tdcch[1]=(hwidt%1000)/100;//FTch#(6), 0 if missing
      tdcch[2]=(hwidt%100)/10;//SumHTch#(7), 0 if missing
      tdcch[3]=(hwidt%10);//SumSHTch#(8), 0 if missing
      number ftcor=0,ltcor=0,htcor=0,shtcor=0;
//---LT
  if(realdata>0&&nlt>0&&nft>0){
      TofTdcPar *TdcPar=TofTdcPar::GetHead(); 
      for(i=0;i<nlt;i++){
        if(TdcPar->IsValidate()){
           ltcor=TdcPar->getcor(crat-1,sslot-1,tdcch[0]-1,ptr->fstdc[i]); 
        }
        else {
#ifndef __ROOTSHAREDLIBRARY__
          if(tdcch[0]>0&&TofTdcCor::tdccor[crat-1][sslot-1].truech(tdcch[0]-1))ltcor=TofTdcCor::tdccor[crat-1][sslot-1].getcor(ptr->fstdc[i],tdcch[0]-1);
#else
          if(tdcch[0]>0&&TofTdcCorN::tdccor[crat-1][sslot-1].truech(tdcch[0]-1))ltcor=TofTdcCorN::tdccor[crat-1][sslot-1].getcor(ptr->fstdc[i],tdcch[0]-1);
#endif
         }
         if(BuildOpt>=0)ltdch[i]-=ltcor;
      }
//---FT
      for(i=0;i<nft;i++){
        if(TdcPar->IsValidate()){
           ftcor=TdcPar->getcor(crat-1,sslot-1,tdcch[1]-1,ptr->fftdc[i]);
        }
        else {
#ifndef __ROOTSHAREDLIBRARY__
          if(tdcch[1]>0&&TofTdcCor::tdccor[crat-1][sslot-1].truech(tdcch[1]-1))ftcor=TofTdcCor::tdccor[crat-1][sslot-1].getcor(ptr->fftdc[i],tdcch[1]-1);
#else
          if(tdcch[1]>0&&TofTdcCorN::tdccor[crat-1][sslot-1].truech(tdcch[1]-1))ftcor=TofTdcCorN::tdccor[crat-1][sslot-1].getcor(ptr->fftdc[i],tdcch[1]-1);
#endif
        }
        if(BuildOpt>=0)ftdch[i]-=ftcor;
      }
//---HT
      for(i=0;i<nht;i++){
        if(TdcPar->IsValidate()){
           htcor=TdcPar->getcor(crat-1,sslot-1,tdcch[2]-1,ptr->fsumht[i]);
        }
        else {
#ifndef __ROOTSHAREDLIBRARY__
          if(tdcch[2]>0&&TofTdcCor::tdccor[crat-1][sslot-1].truech(tdcch[2]-1))htcor=TofTdcCor::tdccor[crat-1][sslot-1].getcor(ptr->fsumht[i],tdcch[2]-1);
#else
          if(tdcch[2]>0&&TofTdcCorN::tdccor[crat-1][sslot-1].truech(tdcch[2]-1))htcor=TofTdcCorN::tdccor[crat-1][sslot-1].getcor(ptr->fsumht[i],tdcch[2]-1);
#endif
        }
         if(BuildOpt>=0)htdch[i]-=htcor;
      }
//---SHT
      for(i=0;i<nsht;i++){
        if(TdcPar->IsValidate()){
           shtcor=TdcPar->getcor(crat-1,sslot-1,tdcch[3]-1,ptr->fsumsht[i]);
        }
        else {
#ifndef __ROOTSHAREDLIBRARY__
         if(tdcch[3]>0&&TofTdcCor::tdccor[crat-1][sslot-1].truech(tdcch[3]-1))shtcor=TofTdcCor::tdccor[crat-1][sslot-1].getcor(ptr->fsumsht[i],tdcch[3]-1);
#else
         if(tdcch[3]>0&&TofTdcCorN::tdccor[crat-1][sslot-1].truech(tdcch[3]-1))shtcor=TofTdcCorN::tdccor[crat-1][sslot-1].getcor(ptr->fsumsht[i],tdcch[3]-1);
#endif
        }
         if(BuildOpt>=0)shtdch[i]-=shtcor;
      }
   }

//---Get Ref Tdc
      for(i=0;i<nft;i++)  ftdc[i]=ftdch[i]*TofRecPar::Tdcbin;//FTtime TDCch->ns
      number fttm=(nft>0)?ftdc[nft-1]:0;//choose last because this triggers LVL1 
      for(i=0;i<nlt;i++)  ltdc[i]=fttm-ltdch[i]*TofRecPar::Tdcbin;//Rel.LTtime(+ means befor FTtime)(+TDCch->ns)
      for(i=0;i<nht;i++)  htdc[i]=fttm-htdch[i]*TofRecPar::Tdcbin;//Rel.SumHTtime +TDCch->ns
      for(i=0;i<nsht;i++)shtdc[i]=fttm-shtdch[i]*TofRecPar::Tdcbin;//Rel.SumSHTtime +TDCch->ns

//---Put to calidate
      ltdcw.clear();htdcw.clear();shtdcw.clear();
      for(i=0;i<nlt;i++){//notTooYoung(>40ns befFT), notTooOld(<300 befFT)
         if(((ltdc[i]> TofRecPar::FTgate[0])&&(ltdc[i]<TofRecPar::FTgate[1]))||BuildOpt<0){
            ltdcw.push_back(ltdc[i]);
         }
       }
     for(i=0;i<nht;i++){//notTooYoung(>40ns befFT), notTooOld(<300 befFT)
         if(((htdc[i]> TofRecPar::FTgate[0])&& (htdc[i]<TofRecPar::FTgate[1]))||BuildOpt<0){
           htdcw.push_back(htdc[i]);
         }
       }
     for(i=0;i<nsht;i++){//notTooYoung(>40ns befFT), notTooOld(<300 befFT)
         if(((shtdc[i]> TofRecPar::FTgate[0])&& (shtdc[i]<TofRecPar::FTgate[1]))||BuildOpt<0){
           shtdcw.push_back(shtdc[i]);
         }
      }

//----Set Bad Status 
       if(nft<=0)         {sstatus|=TOFDBcN::NOFT;}
       if(ltdcw.size()<1) {sstatus|=TOFDBcN::NOWINDOWLT;}
       if(htdcw.size()<1) {sstatus|=TOFDBcN::NOWINDOWHT;}
       if(shtdcw.size()<1){sstatus|=TOFDBcN::NOWINDOWSHT;}
       if(ltdcw.size()>1) {sstatus|=TOFDBcN::LTMANY;}
       if(htdcw.size()>1) {sstatus|=TOFDBcN::HTMANY;}//HT MANY bad(due to 300ns block) but due to PMT-together Find itself
       if((sstatus&(TOFDBcN::NOFT|TOFDBcN::NOWINDOWLT|TOFDBcN::NOADC))>0){sstatus|=TOFDBcN::BAD;}

//---Get Time imformation----BAD Time
       sdtm=0;
       if((sstatus&TOFDBcN::BAD)>0){
         sdtm=0;
       }

//----For One LT set this as default (NO Need to ReFind Or will abandon this)
       else if(ltdcw.size()==1){
         sdtm=ltdcw[0];ltok=1;
       }

//----For One HT many LT //may not accurate
       else  {
         number minhldt=FLT_MAX,dt=0;
          //----if no HT and no SHT try to narrow
         if((htdcw.size()==0)&&(shtdcw.size()==0)){
           for(i=0;i<ltdcw.size();i++){//tight guide
             dt=ltdcw[i]-(TofRecPar::FTgate2[0]+TofRecPar::FTgate2[1])/2.;//using TF MPV gate to choose
             if(fabs(dt)<minhldt){sdtm=ltdcw[i];minhldt=fabs(dt);}
             if((ltdcw[i]>TofRecPar::FTgate2[0])&&(ltdcw[i]<TofRecPar::FTgate2[1])){ltok++;}
            }
            if(ltok!=1){sstatus|=(TOFDBcN::BAD|TOFDBcN::NOHTRECOVCAD);ltok=0;}
          }

          //---Using Ht Sht finding nearest as default match check
        else {
         //---First using ht match
         if(htdcw.size()>0){
           for(i=0;i<ltdcw.size();i++){//real impossible two LT match HT due to 30ns LT lock
              for(j=0;j<htdcw.size();j++){//With self associate j
                dt=ltdcw[i]-htdcw[j];//lt must >ht time
                if(fabs(dt-TofRecPar::LHMPV)<minhldt){sdtm=ltdcw[i];minhldt=fabs(dt-TofRecPar::LHMPV);}
                if((dt>TofRecPar::LHgate[0])&&(dt<TofRecPar::LHgate[1])){ltok++;}//find LT should match in window
               }
            }
          }
         //---Otherwise try using sht
          if((!ltok)&&(shtdcw.size()>0)){
           for(i=0;i<ltdcw.size();i++){//to find LT should match in windows
              for(j=0;j<shtdcw.size();j++)     
                dt=ltdcw[i]-shtdcw[j];//lt must >sht time 
                if(fabs(dt-TofRecPar::LHMPV)<minhldt){sdtm=ltdcw[i];minhldt=fabs(dt-TofRecPar::LHMPV);}
                if((dt>TofRecPar::LHgate[0])&&(dt<TofRecPar::LHgate[1])){ ltok++;}
            }
          }
//         if(!ltok)sstatus|=(TOFDBcN::BAD|TOFDBcN::NOMATCHRECOVCAD);
         if(!ltok)sstatus|=TOFDBcN::NOMATCHRECOVCAD;
       }
    }

   return 0;
}

//========================================================
int TofRecH::LTRefind(int idsoft,number trlcoo,number sdtm[2],number adca[2],uinteger &status, vector<number> ltdcw[]){

   if((adca[0]<=0)||(adca[1]<=0)||ltdcw[0].size()<=0||ltdcw[1].size()<=0)return -1;

//---Find Best LT
    number sdtm1[2],tms[2],tm,etm,lcoo,elcoo,mindis=FLT_MAX;
    uinteger ustatus=0;

//--search by mindis
    for(int i=0;i<ltdcw[0].size();i++){//side0
      sdtm1[0]=ltdcw[0].at(i);

      for(int j=0;j<ltdcw[1].size();j++){//side1
        sdtm1[1]=ltdcw[1].at(j);
        ustatus=0; 
        TimeCooRec(idsoft,sdtm1,adca,tms,tm,etm,lcoo,elcoo,ustatus);
        if((ustatus&TOFDBcN::BADTIME)>0)continue;//If In windows
        if(fabs(lcoo-trlcoo)<mindis){ //already find Mindis
           mindis=fabs(lcoo-trlcoo);
           sdtm[0]=ltdcw[0].at(i);sdtm[1]=ltdcw[1].at(j);//best time
           status|=(TOFDBcN::LTREFIND|TOFDBcN::RECOVERED);
         }
      }

    }
//----

   return 0;  
}

//========================================================
int TofRecH::TimeCooRec(int idsoft,number sdtm[], number adca[],number tms[2],number &tm,number &etm,number &lcoo,number &elcoo,uinteger &status){

    static int errcount=0;
    if(TofTAlignPar::Head==0){
       if(errcount<100)cerr<<"Error TofTAlignPar Head not Initial !!!!"<<endl;
    }
    TofTAlignPar *TPar=TofTAlignPar::GetHead();
   if(!TPar->Isload){
       if(errcount++<100)cerr<<"Error TofTAlignPar TDV not Load yet!!! Please load first"<<endl; 
       return -1;
   }
//----
    number sl1=  TPar->slew[idsoft];
    number sl2=  TPar->slew[idsoft+10];
    number c0=   TPar->delay[idsoft];
    number c1=   TPar->dt1[idsoft];
    number index=TPar->powindex;

    tms[0]=(((status&TOFDBcN::BADN)>0)&&((status&TOFDBcN::LTREFIND)==0))? 0: (sdtm[0]+2*sl1/pow(adca[0],index)+c0+c1);//s1 missing or not
    tms[1]=(((status&TOFDBcN::BADP)>0)&&((status&TOFDBcN::LTREFIND)==0))? 0: (sdtm[1]+2*sl2/pow(adca[1],index)+c0-c1);//s2 missing or not
  
 
//--Both two side to go next
   number dsl= TPar->dslew[idsoft];
   number vel= TPar->vel[idsoft];

   TofRecPar::IdCovert(idsoft);
   if((tms[0]==0)||(tms[1]==0)){
     lcoo=0;elcoo=TOFGeom::Sci_l[TofRecPar::iLay][TofRecPar::iBar]/2.;
     tm=(tms[0]!=0)?-tms[0]:-tms[1]; etm=fabs(elcoo/vel);
     status|=(TOFDBcN::BADTIME|TOFDBcN::BADTCOO);
     return -1;
    }

//--time
    tm=0.5*(tms[0]+tms[1]);
    tm=-tm;//convert to pos
    if((status&TOFDBcN::LTREFIND)>0)etm=TofRecPar::GetTimeSigma(idsoft,1);
    else                            etm=TofRecPar::GetTimeSigma(idsoft,1);
    
//--coo
    number tmsc[2];
    tmsc[0]=tms[0]-2*dsl/pow(adca[0],index);//coo compensate
    tmsc[1]=tms[1]+2*dsl/pow(adca[1],index);//coo compensate
    lcoo=0.5*(tmsc[0]-tmsc[1])*vel;
    if((status&TOFDBcN::LTREFIND)>0)elcoo=TofRecPar::GetCooSigma(idsoft,1);
    else                            elcoo=TofRecPar::GetCooSigma(idsoft,1);

//--New Adding Double Check
   if(fabs(lcoo)>TOFGeom::Sci_l[TofRecPar::iLay][TofRecPar::iBar]/2.+30.){//Still BAD
      lcoo=0; elcoo=TOFGeom::Sci_l[TofRecPar::iLay][TofRecPar::iBar]/2.;
      etm=fabs(elcoo/vel); 
      if(BuildOpt>=0)status|=(TOFDBcN::BADTIME|TOFDBcN::BADTCOO);
    }
//----

    return 0;
}

//========================================================
int TofRecH::EdepRec(int idsoft,number adca[],number adcd[][TOF2GC::PMTSMX],number lcoo,number q2pa[],number q2pd[][TOF2GC::PMTSMX],number &edepa,number &edepd,uinteger sstatus[2]){

  number QD[TOFCSN::NSIDE][TOFCSN::NPMTM]={{0}},QA[TOFCSN::NSIDE]={0};
  int uid;
  edepa=edepd=0;
  for(int is=0;is<TOFCSN::NSIDE;is++){
     q2pa[is]=0;
     for(int ipm=0;ipm<TOFCSN::NPMTM;ipm++){q2pd[is][ipm]=0;}
  }

  for(int is=0;is<TOFCSN::NSIDE;is++){
//  anode Q*Q covertion+adc-non-linear-correction+attnuation correction
     uid=idsoft+is*10;
//--mark adc chip overflow
     if     (adca[is]>=TofRecPar::PUXMXCH){sstatus[is]|=TOFDBcN::AOVERFLOW; edepa=q2pa[is]=QA[is]=-3;}
     else if(adca[is]>0){
        QA[is]=GetQSignal(uid,1,(kQ2|kLinearCor|kAttCor),adca[is],lcoo);
        if(QA[is]==-1)   {sstatus[is]|=TOFDBcN::AOVERFLOWNONLC;}//Marking Non-Linear Correction Overflow
        edepa=q2pa[is]=QA[is];
     }

//--dynode Q*Q covertion+adc-non-linear-correction+attnuation correction
     for(int ipm=0;ipm<TOFCSN::NPMTM;ipm++){
         uid=idsoft+is*10+ipm; 
//--mark adc chip overflow
         if     (adcd[is][ipm]>=TofRecPar::PUXMXCH){sstatus[is]|=TOFDBcN::DOVERFLOW; edepd=q2pd[is][ipm]=QD[is][ipm]=-3;}
         else if(adcd[is][ipm]>TofRecPar::Dynodegate){
           QD[is][ipm]=GetQSignal(uid,0,(kQ2|kLinearCor|kAttCor),adcd[is][ipm],lcoo);
           if(QD[is][ipm]==-1){sstatus[is]|=TOFDBcN::DOVERFLOWNONLC;}//Marking Non-Linear Correction Overflow
           edepd=q2pd[is][ipm]=QD[is][ipm];
        }
     }
  }

/// Sum Anode Signal Dynode Signal
   number QSA=SumSignalA(idsoft,QA);
   number QSD=SumSignalD(idsoft,QD);
///--Birk Saturation Correction
   if(QSA>0)edepa=GetQSignal(idsoft,1,(kBirkCor|kQ2MeV),QSA);
   if(QSD>0)edepd=GetQSignal(idsoft,0,(kBirkCor|kQ2MeV),QSD);

  return 0;
}

//========================================================
int  TofRecH::EdepRecR(int ilay,int ibar,geant adca[],geant adcd[][TOF2GC::PMTSMX],number lcoo,geant q2pa[],geant q2pd[][TOF2GC::PMTSMX],geant &edepa,geant &edepd){
  number adca1[2]={0},adcd1[2][TOF2GC::PMTSMX]={0};
  number q2pa1[2]={0},q2pd1[2][TOF2GC::PMTSMX]={0};
  number edepa1=0,edepd1=0;
  uinteger sstatus[2]={0};
  TofRecPar::IdCovert(ilay,ibar);
  for(int is=0;is<2;is++){
     adca1[is]=adca[is];
     for(int ipm=0;ipm<TOF2GC::PMTSMX;ipm++){adcd1[is][ipm]=adcd[is][ipm];}
   }
   EdepRec(TofRecPar::Idsoft,adca1,adcd1,lcoo,q2pa1,q2pd1,edepa1,edepd1,sstatus);
///--Copy Par
   for(int is=0;is<2;is++){
     q2pa[is]=q2pa1[is];
     for(int ipm=0;ipm<TOF2GC::PMTSMX;ipm++){q2pd[is][ipm]=q2pd1[is][ipm];}
   }
   edepa=edepa1;edepd=edepd1;
   return 0;
}

//========================================================
number TofRecH::GetQSignal(int idsoft,int isanode,int optc,number signal,number lcoo,number cosz,number beta,number rig){

//---
  if(Init()!=0)return -1;

  if(TofPMAlignPar::Head==0||TofPMAlignPar::GetHead()->Isload!=1)return 0;
  if(TofPMDAlignPar::Head==0||TofPMDAlignPar::GetHead()->Isload!=1)return 0;
  if(TofAttAlignPar::Head==0||TofAttAlignPar::GetHead()->Isload!=1)return 0;
  if(TofCAlignPar::GetHead()->Isload!=1)return 0;
  if(signal<=0)return signal;//negtive singal overflow+No signal

  number signalc=signal;
//--Adding Gain to Convert To Q*Q (Proton Mip Unit) 0 Mean MaskBad
  if(optc&kQ2)       {signalc=CoverToQ2(idsoft,isanode,signalc);if(signalc<=0)return signalc;}
//--ADC Non Linear Correction -1 mean Overflow Non-Linear Correction faild
  if(optc&kLinearCor){signalc=NonLinearCor(idsoft,isanode,signalc);if(signalc<=0)return signalc;}
//--Attenuation Correction
  if(optc&kAttCor)   {signalc=SciAttCor(idsoft,lcoo,signalc);if(signalc<=0)return signalc;}
//---Path Length Theta Correction
  if(optc&kThetaCor) {signalc*=fabs(cosz);}
//--Birk Correction -2 mean Overflow Birk Correction faild
  if(optc&kBirkCor)  {signalc=BirkCor(idsoft,signalc,1);if(signalc<=0)return signalc;}
 //--ReAtt Correction
   if(optc&kReAttCor){signalc=SciReAttCor(idsoft,lcoo,signalc,1);}
//--Beta Correction
   if(optc&kBetaCor)   {signalc=BetaCor(idsoft,signalc,beta,rig);}
   if(optc&kRigidityCor){signalc=RigCor(idsoft,signalc,rig,isanode);} 
//--Conver from MeV to Q2
  if(optc&kMeVQ2)    signalc=signalc/TofCAlignPar::ProEdep;
//--Inverse Birk Correction
  if(optc&kVBirkCor)  {signalc=BirkCor(idsoft,signalc,0);if(signalc<=0)return signalc;}
//--Conver from Q2 To MeV
  if(optc&kQ2MeV)    signalc=signalc*TofCAlignPar::ProEdep;
//--Conver from MeV to Q2
  if(optc&kQ2Q)      signalc=sqrt(signalc);
//---

  return signalc;

}

//========================================================
number TofRecH::NonLinearCor(int idsoft,int isanode,number q2){//PM Level

  TofCAlignPar   *CPar=TofCAlignPar::GetHead();

///--Dynode Temp No ADC NonLinear Correction
   if(!isanode)return q2;

///--Anode 
   else {
     if(q2>=CPar->ansat[2][idsoft])return -1;
//--Non Linear expect
     double nlcq=log((q2-CPar->ansat[2][idsoft])/(-CPar->ansat[0][idsoft]))/(-CPar->ansat[1][idsoft]);
//--Linear expect //birk compensate  Let Scale QDynode=QAnode
//     double lcq=q2/(1.+CPar->birk[idsoft/100*100]);
     double lcq=q2*GetBirkFun(idsoft)->Eval(1);
//---Choose much larger one to decide non-saturation range
     double cq=lcq>nlcq?lcq:nlcq; 
     return cq;
   }
}

//========================================================
number TofRecH::CoverToQ2(int idsoft,int isanode,number adc){//PM Level

///-- Adding  Gain Correction+Normalizetion To Q*Q (Central MIP Unit)
  TofPMAlignPar  *PMPar= TofPMAlignPar::GetHead();
  TofPMDAlignPar *PMDPar=TofPMDAlignPar::GetHead(); 
  TofCAlignPar   *CPar=TofCAlignPar::GetHead();

//--Anode And Dynode Scale // QAnode=(1+k)*QDynode
  if(isanode){
      if(PMPar->gaina[idsoft]==0)return 0;//bad dynode mask not use
      return adc/PMPar->gaina[idsoft]/GetProMipAdc(idsoft,0); 
   }

//--Dynode Due to use Carbon Mip To Calib should*36
   else {
      if(PMDPar->gaind[idsoft]==0)return 0;//bad dynode mask not use
      return adc/(PMDPar->gaind[idsoft])/CPar->dycor[idsoft]*36.;
   }
}

//=======================================================
TF1  *TofRecH::GetBirkFun(int idsoft){

   if(!BirkFun)
#pragma omp critical(fun)
{
    int thread=0;
#ifdef _OPENMP
    thread=omp_get_thread_num();
#endif
    char title[80];
    sprintf(title,"TOF_birkfun%d",thread);
    BirkFun=new TF1(title,"x/(1.+[1]*atan([0]/[1]*x))",0,10000);
}

   TofCAlignPar   *CPar=TofCAlignPar::GetHead();
   idsoft=idsoft/100*100;

   BirkFun->SetParameter(0,CPar->birk[0][idsoft]);
   BirkFun->SetParameter(1,CPar->birk[1][idsoft]); 
   return BirkFun;
}

//========================================================
number TofRecH::BirkCor(int idsoft,number q2,int opt){//Counter Level

  if(q2<=0)return q2;

  TofCAlignPar   *CPar=TofCAlignPar::GetHead();
  idsoft=idsoft/100*100;//Counter Bar lever 
 
 //--Birk Saturation Correction
  if(opt==1){
    return GetBirkFun(idsoft)->GetX(q2);
    //--Birk Saturation Overflow 
    // if(1.-CPar->birk[idsoft]*q2<=0)return -2;
  }
  else {
    return GetBirkFun(idsoft)->Eval(q2);
  }
}

//========================================================
number TofRecH::RigCor(int idsoft,number q2,number rig,int isanode){

  if(q2<=0)return q2;

///--Finding Algorithem
  int nowch=TofCAlignParIon::RigCh[0];
   number cor1,cor2,ch1,ch2;
   number rigcor=1;
   while (1){
      cor1=GetRigCalCh(idsoft,nowch,rig,isanode);
      cor2=GetRigCalCh(idsoft,nowch+1,rig,isanode);
      ch1=sqrt(q2/cor1);
      ch2=sqrt(q2/cor2);
  ///--Find LowLimit
      if(ch1<TofCAlignParIon::RigCh[0]||ch2<TofCAlignParIon::RigCh[0]||nowch==0){
        rigcor=cor1;break;
      }
///--Find Gap
       else if((ch1>=nowch&&ch2<=nowch+1)||(ch1<=nowch&&ch2>=nowch+1)){
         number ww1=fabs(ch1*ch1-nowch*nowch);number ww2=fabs(ch2*ch2-(nowch+1)*(nowch+1));
         rigcor=(ww2*cor1+ww1*cor2)/(ww1+ww2);break;
      }
      else if(ch1>=nowch+1||ch2>=nowch+1){nowch=(ch1>ch2)? int(ch1):int(ch2);}
      else if(ch1<=nowch||ch2<=nowch)    {nowch--;}
      else {cerr<<"Error Rig Correction"<<endl;break;}
   }

    return q2/rigcor;
  
}
//========================================================
number TofRecH::GetRigCalCh(int idsoft,int charge,number rig,int isanode){

  if(charge<=0)return 1;

   number corvar=1;
   
   for(int ich=0;ich<TofCAlignParIon::nRigCh;ich++){
///---Find in arr or at end
       if(charge==TofCAlignParIon::RigCh[ich]||ich==TofCAlignParIon::nRigCh-1){
         corvar=GetRigCalI(idsoft,ich,rig,isanode); //Beta Correction
         break;
      }
///---Find In middle need interpolation
     else if(charge>TofCAlignParIon::RigCh[ich]&&charge<TofCAlignParIon::RigCh[ich+1]){
        number  ww1=charge*charge-TofCAlignParIon::RigCh[ich]*TofCAlignParIon::RigCh[ich];
        number  ww2=TofCAlignParIon::RigCh[ich+1]*TofCAlignParIon::RigCh[ich+1]-charge*charge;
        corvar=(ww2*GetRigCalI(idsoft,ich,rig,isanode)+ww1*GetRigCalI(idsoft,ich+1,rig,isanode))/(ww1+ww2);//Beta Correction
        break;
      }
   }

   return corvar;
}

//========================================================
number TofRecH::GetRigCalI(int idsoft,int chindex,number rig,int isanode){//Counter Level

   TofCAlignParIon  *CParI=TofCAlignParIon::GetHead();
///----
   const number rigcut=1.;////Const
   number rigp=fabs(rig)<rigcut? fabs(rigcut) :fabs(rig);
//---
   number xv=log(rigp);
   number par[7]={0};
   par[0]=CParI->rigcor[isanode][chindex][0][idsoft];
   par[1]=CParI->rigcor[isanode][chindex][1][idsoft];
   par[2]=CParI->rigcor[isanode][chindex][2][idsoft];
   par[3]=CParI->rigcor[isanode][chindex][3][idsoft];
   par[4]=CParI->rigcor[isanode][chindex][4][idsoft];
   par[5]=CParI->rigcor[isanode][chindex][5][idsoft];
   par[6]=CParI->rigcor[isanode][chindex][6][idsoft];
//--Rigidit Correction
   number rigcv=par[0]*atan(par[1]*xv)+par[2];
   if(xv<par[3]){
     rigcv=par[0]*atan(par[1]*par[3])+par[2];
     rigcv=rigcv+par[4]*(xv-par[3])+par[5]*pow((xv-par[3]),2)+par[6]*pow((xv-par[3]),3);
   }
   return rigcv;
}

//========================================================
number TofRecH::BetaCor(int idsoft,number q2,number beta,number rig){
 
   if(q2<=0)return q2;

///--Finding Algorithem
   int nowch=1;
   number cor1,cor2,ch1,ch2;
   number betacor=1;
   while (1){
      cor1=GetBetaCalCh(idsoft,0,beta,0,nowch,rig);
      cor2=GetBetaCalCh(idsoft,0,beta,0,nowch+1,rig);
      ch1=sqrt(q2/cor1);
      ch2=sqrt(q2/cor2);
///--Find LowLimit
      if(ch1<1||ch2<1||nowch==0){
        betacor=cor1;break;
      }
///--Find Gap
      else if((ch1>=nowch&&ch2<=nowch+1)||(ch1<=nowch&&ch2>=nowch+1)){
         number ww1=fabs(ch1*ch1-nowch*nowch);number ww2=fabs(ch2*ch2-(nowch+1)*(nowch+1));
         betacor=(ww2*cor1+ww1*cor2)/(ww1+ww2);break;
      }
      else if(ch1>=nowch+1||ch2>=nowch+1){nowch=(ch1>ch2)? int(ch1):int(ch2);}
      else if(ch1<=nowch||ch2<=nowch)    {nowch--;}
      else {cerr<<"Error Beta Correction"<<endl;break;}
   }

    return q2/betacor;
}

//========================================================
number TofRecH::GetBetaCalCh(int idsoft,int opt,number beta,number q2,int charge,number rig){

  if(charge<=0)return 1;

   number corvar=1;
   for(int ich=0;ich<TofCAlignPar::nBetaCh;ich++){
///---Find in arr or at end
      if(charge==TofCAlignPar::BetaCh[ich]||ich==TofCAlignPar::nBetaCh-1){
         if(opt==0)corvar=GetBetaCalI(idsoft,0,beta,0.,ich,rig); //Beta Correction
         else      {
            q2=q2/(charge*charge);
            corvar=GetBetaCalI(idsoft,1,beta,q2,ich,rig);//Edep Beta
          }
         break;
      }
///---Find In middle need interpolation
      else if(charge>TofCAlignPar::BetaCh[ich]&&charge<TofCAlignPar::BetaCh[ich+1]){
        number  ww1=charge*charge-TofCAlignPar::BetaCh[ich]*TofCAlignPar::BetaCh[ich];
        number  ww2=TofCAlignPar::BetaCh[ich+1]*TofCAlignPar::BetaCh[ich+1]-charge*charge;
        if(opt==0)corvar=(ww2*GetBetaCalI(idsoft,0,beta,0.,ich,rig)+ww1*GetBetaCalI(idsoft,0,beta,0.,ich+1,rig))/(ww1+ww2);//Beta Correction
        else     {
            q2=q2/(charge*charge);
            corvar=(ww2*GetBetaCalI(idsoft,1,beta,q2,ich,rig)+ww1*GetBetaCalI(idsoft,1,beta,q2,ich+1,rig))/(ww1+ww2);//Edep Beta
        }
        break;
      }
   }

   return corvar;
}

//========================================================
TF1 *TofRecH::GetBetaFun(){

   if(!BetaFun)
#pragma omp critical(fun)
{ 
    int thread=0;
#ifdef _OPENMP
    thread=omp_get_thread_num();
#endif
    char title[80];
    sprintf(title,"TOF_VE%d",thread);
    BetaFun=new TF1(title,"pol4",0.2,1.1);
}

   return BetaFun;
}

//========================================================
number TofRecH::GetBetaCalI(int idsoft,int opt,number beta,number q2norm,int chindex,number rig){//Counter Level

   TofCAlignPar   *CPar=TofCAlignPar::GetHead();
   idsoft=idsoft/100*100;
///----
   TofRecPar::IdCovert(idsoft);
   int lay=TofRecPar::iLay; int bar=TofRecPar::iBar;

//--Minus case Invert Layer
   if(beta<0){
     if(lay==1||lay==2){
       if     (lay==1&&bar==TOFGeom::Nbar[lay]-1){bar=TOFGeom::Nbar[2]-1;}
       else if(lay==2&&bar==TOFGeom::Nbar[lay]-1){bar=TOFGeom::Nbar[1]-1;}
       else if(lay==2&&bar>=TOFGeom::Nbar[1]-1)  {bar=TOFGeom::Nbar[1]-2;}
     }
     lay=(TOFGeom::NLAY-1)-lay;
     TofRecPar::IdCovert(lay,bar);
     idsoft=TofRecPar::Idsoft/100*100;
   }

//--Normal Beta 
   const number rigcut=20;//>20GeV not Apply Beta Correction
//   const number betacut=0.96;
   const number betacut=1.;
   number betap=fabs(beta)>betacut? betacut :fabs(beta);
   if(fabs(rig)>rigcut)betap=1;
//--Protection
   if(betap<0.38)betap=0.38;

//---beta correction var
   number betacv=1;

//---Correction
    TF1 *fun=GetBetaFun();
//    fun=new TF1("TOF_VE","pol4",0.2,1.1);
    for(int ipar=0;ipar<=4;ipar++){
       fun->SetParameter(ipar,CPar->betacorn[chindex][ipar][idsoft]);
    }

//-----Beta Correction
  if(opt==0){
    betacv=fun->Eval(betap);
  }
  else   {  //Edpe Beta
     number q2=q2norm;
     if     (q2<fun->Eval(0.94))betacv=beta>0?1:-1;
     else if(q2>fun->Eval(0.38))betacv=beta>0?0.38:-0.38;
     else                       betacv=beta>0?fun->GetX(q2):-fun->GetX(q2);
  }
  return betacv;
}

//========================================================
number TofRecH::GetProMipAdc(int idsoft,number lpos){//Side Level

//---Attenuation TDV
   TofAttAlignPar *AttPar=TofAttAlignPar::GetHead();
//    AttPar->PrintTDV();
//---Geometry
   idsoft=idsoft/10*10;//Anode Two Side
   TofRecPar::IdCovert(idsoft);
   bool istrap=TOFGeom::IsTrapezoid(TofRecPar::iLay,TofRecPar::iBar);
   number lpos1=(TofRecPar::iSide==0?lpos:-lpos);

//---Fast +Slow Photon Component
   number sci_hl=AttPar->attpar[0][idsoft]/2;
   number decayl=sci_hl+lpos1;
   number direct=AttPar->attpar[1][idsoft]*((AttPar->attpar[2][idsoft])*exp(-decayl/AttPar->attpar[3][idsoft])
                +(1.-AttPar->attpar[2][idsoft])*exp(-decayl/AttPar->attpar[4][idsoft]));
   if(!istrap){ return direct;}
//--Adding Reflection Part
   else  {
      number decayl2=sci_hl-lpos1;
      number reflect=AttPar->attpar[5][idsoft]*exp(-decayl2/AttPar->attpar[6][idsoft]);
      return direct+reflect;
    }
}

//========================================================
number TofRecH::SumSignalA(int idsoft,number signal[],int useweight){//Counter Level

///--Temp No weight For Anode
     double sums=0,sumw=0;
     int usesid=0,overid=0;
     for(int is=0;is<TOFCSN::NSIDE;is++){
        if     (signal[is]>0){sums+=signal[is];sumw+=1;usesid++;}
        else if(signal[is]<0){overid++;}
     }
     if(usesid==0){
       if(overid>=1)return -1;
       else         return 0;
     } 
     return  sums/sumw;
}

//========================================================
number TofRecH::SumSignalD(int idsoft,number signal[][TOFCSN::NPMTM],int useweight,bool minpmcut){//Counter Level

  if(Init()!=0)return -1;
  TofCAlignPar *CPar=TofCAlignPar::GetHead();
///--Dynode weight
   double sums=0,sumw=0,ww=0;
   int usepm=0,overpm=0,uid;
   for(int is=0;is<TOFCSN::NSIDE;is++){
     for(int ipm=0;ipm<TOFCSN::NPMTM;ipm++){
       uid=idsoft+is*10+ipm;
       if(signal[is][ipm]>0){
          if(useweight)ww=1./(CPar->dypmw[uid]*CPar->dypmw[uid]);
          else         ww=1.;
          sums+=ww*signal[is][ipm];
          sumw+=ww;
          usepm++;
       }
       else if(signal[is][ipm]<0){overpm++;}
     }
  }
//--Dynode Need Min PMT //cut due to dynode not-linear for begin
  if((minpmcut&&usepm<CPar->RecMinPmD)||(usepm==0)){
     if(overpm>=2)return -1;
     else         return 0;
  }
  return sums/sumw;
}

//========================================================
number TofRecH::SciAttCor(int idsoft,number lpos,number q2){//Side Level

//---
   number  attpos=lpos;
//--Att For Two Side
   idsoft=idsoft/10*10;
   TofRecPar::IdCovert(idsoft);
//--Out Range Check
   if     (attpos> TOFGeom::Sci_l[TofRecPar::iLay][TofRecPar::iBar]/2.){attpos= TOFGeom::Sci_l[TofRecPar::iLay][TofRecPar::iBar]/2.;}
   else if(attpos<-TOFGeom::Sci_l[TofRecPar::iLay][TofRecPar::iBar]/2.){attpos=-TOFGeom::Sci_l[TofRecPar::iLay][TofRecPar::iBar]/2.;}
   if     (TOFGeom::IsTrapezoid(TofRecPar::iLay,TofRecPar::iBar)){
//          if     (attpos>TofRecPar::AttLenLimit){attpos=TofRecPar::AttLenLimit;}
//          else if(attpos<-TofRecPar::AttLenLimit){attpos=-TofRecPar::AttLenLimit;}
//---Each Tune
         if     (TofRecPar::iLay==0&&attpos>50.5) attpos=50.5;
         else if(TofRecPar::iLay==0&&attpos<-50.5)attpos=-50.5;
//----
         else if(TofRecPar::iLay==1&&attpos>46.) attpos=46.;
         else if(TofRecPar::iLay==1&&attpos<-46.)attpos=-46.;
//---
         else if(TofRecPar::iLay==2&&attpos>45.) attpos=45.;
         else if(TofRecPar::iLay==2&&attpos<-45.)attpos=-45.;
//---
         else if(TofRecPar::iLay==3&&attpos>50.) attpos=50.;
         else if(TofRecPar::iLay==3&&attpos<-50.)attpos=-50.;
   }
//---
   return q2*GetProMipAdc(idsoft,0.)/GetProMipAdc(idsoft,attpos);
}

//========================================================
number TofRecH::SciReAttCor(int idsoft,number lpos,number q2,int qopt){//Attuantion Global ReCorrection

   if(lpos==0)return q2;///Default No Correction
   TofCAlignPar *CPar=TofCAlignPar::GetHead();
//----
   number attpos=lpos;
//--Att For Counter
   idsoft=idsoft/100*100;//Counter Level
   TofRecPar::IdCovert(idsoft);
//--Attnuation Range Check and Protection
   if     (attpos>60) {attpos=60;}
   else if(attpos<-60){attpos=-60;}
   if     (TOFGeom::IsTrapezoid(TofRecPar::iLay,TofRecPar::iBar)){
          if     (attpos>45) {attpos=45;}
          else if(attpos<-45){attpos=-45;}
   }
//---
   number recor=CPar->reanti[0][idsoft]+CPar->reanti[1][idsoft]*attpos+CPar->reanti[2][idsoft]*pow(attpos,2)
               +CPar->reanti[3][idsoft]*pow(attpos,3)+CPar->reanti[4][idsoft]*pow(attpos,4);
   recor=recor/6.;//Carbon Q-Scale Due to Use Carbon To Tune
   if(qopt==0)return q2/recor;//Q Correction
   else       return q2/recor/recor;//Q2 Correction
}

//========================================================

int TofRecH::BuildBetaH(int verse){
//---
   if(Init()!=0)return -1;
   ClearBuildBetaH();
//---
   
    integer i;
    AMSPoint tkcoo,tfcoo,tfecoo;
    number theta,phi,sleng,mscoo,mlcoo,dscoo,dlcoo,mintm,barw;
    int iscoo[4]={1,0,0,1},nowbar,prebar,nextbar,pretm,nexttm,tminbar;//short coo Trans
    bool leftf=0,rightf=0;
    number Beta,InvErrBeta,BetaC,InvErrBetaC,Chi2;
    int found=0;//
    int tktrdflag=0;
    int ntracks=0;

    VCon* cont=0;
//--Track Part V5
#ifdef _PGTRACK_
    cont = GetVCon()->GetCont("AMSTrTrack");
    if (cont){
       for (int itr=0;itr<cont->getnelem();itr++){
         TrTrackR* tr=(TrTrackR*) cont->getelem(itr);
         track.push_back(tr);
       }
    }
   ntracks=track.size();
   delete cont;

//--V4 Part
#else
#ifndef __ROOTSHAREDLIBRARY__
    AMSTrTrack  *ptrack= (AMSTrTrack*)  AMSEvent::gethead()->getheadC("AMSTrTrack",0);
    while (ptrack){
       amstrack.push_back(ptrack);
       ptrack=ptrack->next();
    }
    ntracks=amstrack.size();
#else 
//---V4.00 root mode no interpolate
   return -1;
#endif
#endif

//--Then TrdTrack
#ifndef __ROOTSHAREDLIBRARY__
   AMSTRDTrack *ptrackT=(AMSTRDTrack *)AMSEvent::gethead()->getheadC("AMSTRDTrack",0,1);
   while(ptrackT){ 
      trdtrack.push_back(TrdTrackR(ptrackT));
      amstrdtrack.push_back(ptrackT);
      ptrackT=ptrackT->next();
   }
#else
   trdtrack=ev->TrdTrack();
#endif

//----Then Ecal
#ifndef __ROOTSHAREDLIBRARY__
      AMSEcalShower * pecalshow=(AMSEcalShower*)AMSEvent::gethead()->getheadC("EcalShower",0,1);
      while(pecalshow){
        ecalshow.push_back(EcalShowerR(pecalshow));
        amsecalshow.push_back(pecalshow);
        pecalshow=pecalshow->next();
      }
#else
     ecalshow=ev->EcalShower();
#endif


//--Then ClusterH
    cont = GetVCon()->GetCont("AMSTOFClusterH");
    for(i=0;i<cont->getnelem();i++){
       TofClusterHR *tofhit=(TofClusterHR *)cont->getelem(i);
       tofclh[tofhit->Layer].push_back(tofhit);
    }
   delete cont;
//----   

  cont = GetVCon()->GetCont("AMSBetaH");
#ifdef __ROOTSHAREDLIBRARY__
  cont->eraseC(); 
#endif

tktrdf:   
   for(int iktr=0;iktr<2;iktr++){
//---Not Use Track
     if((BuildOpt==1)&&(iktr==0))continue;
     if((BuildOpt>=10&&(BuildOpt/10000%10==0))&&(iktr==0))continue;
//---Not Use TRD
     if((BuildOpt>=10&&(BuildOpt/1000%10==0))&&(iktr==1))continue;
//---Not Use Track+TRD
     if(BuildOpt==2)break;
//---if trdtrack
     if     (iktr==0){ntracks=track.size();}
     else if(iktr==1){ntracks=trdtrack.size();}
//----
     for(int itr=0;itr<ntracks;itr++){
       uinteger status=(iktr==0)?(TOFDBcN::TKTRACK):(TOFDBcN::TRDTRACK);
       if(tktrdflag!=0)status|=(iktr==0)?(TOFDBcN::TKTRACK2):(TOFDBcN::TRDTRACK2);
#if defined (_PGTRACK_) || defined (__ROOTSHAREDLIBRARY__)
       TrTrackR *ptrack;  TrTrackR *usetr=0;
       if(iktr==0){ptrack=track.at(itr);usetr=ptrack;}
#if defined (_PGTRACK_)
       else        ptrack=new TrTrackR(number(trdtrack.at(itr).Theta),number(trdtrack.at(itr).Phi),AMSPoint(trdtrack.at(itr).Coo));
#else
       else        return -1;
#endif
#else
       AMSTrTrack *ptrack; AMSTrTrack *usetr=0;
       if(iktr==0){ptrack=amstrack.at(itr);usetr=ptrack;}
       else        ptrack=new AMSTrTrack(number(trdtrack.at(itr).Theta),number(trdtrack.at(itr).Phi),AMSPoint(trdtrack.at(itr).Coo));
#endif

//----
#ifdef  __ROOTSHAREDLIBRARY__
      TrdTrackR *usetrd=0; EcalShowerR *useshow=0;
      if(iktr==1)usetrd=ev->pTrdTrack(itr);
#else
      AMSTRDTrack *usetrd=0;AMSEcalShower *useshow=0;
      if(iktr==1)usetrd=amstrdtrack.at(itr);
#endif
//--- 
         TofClusterHR *phit[4]={0}; int pattern[4]={0};number len[4]={0},tklcoo[4]={0},tkcosz[4]={1.,1.,1.,1.},cres[4][2]={{0}};
         for(int ilay=0;ilay<4;ilay++){
            BetaFindTOFCl(ptrack,ilay,&phit[ilay],len[ilay],tklcoo[ilay],tkcosz[ilay],cres[ilay],pattern[ilay],0);
        }
//----then select+Fit
       int xylay[2]={0,0};//x y direction check
       int udlay[2]={0,0};//up and down check
       for(int ilay=0;ilay<4;ilay++){
          if(phit[ilay]==0)continue;
          if(ilay==0||ilay==1){if((pattern[ilay]%10==4)||(tktrdflag!=0))udlay[0]++;}//no mising
          else                {if((pattern[ilay]%10==4)||(tktrdflag!=0))udlay[1]++;}//no mising
          if(ilay==0||ilay==3){xylay[0]++;}//Fire
          else                {xylay[1]++;}//Fire
        }
//----then fit+Check
       if((xylay[0]>=1)&&(xylay[1]>=1)&&(udlay[0]>=1)&&(udlay[1]>=1)){//Layer Require Fire
          if((xylay[0]+xylay[1]>=TofRecPar::BetaHMinL[0])&&(udlay[0]+udlay[1]>=TofRecPar::BetaHMinL[1])){//sum layer
//----2nd Association
         if(BuildOpt>=0){
           TofClusterHR *phitb[4]={0};int patternb[4]={0};number lenb[4]={0},tklcoob[4]={0},tkcoszb[4]={1.,1.,1.,1.},cresb[4][2]={{0}};
           for(int ilay=0;ilay<4;ilay++){
              int fopt=(phit[ilay]==0)?10:11;//2nd search largest or second
              if(iktr==1)fopt=fopt+100;//TrdTracker
              BetaFindTOFCl(ptrack,ilay,&phitb[ilay],lenb[ilay],tklcoob[ilay],tkcoszb[ilay],cresb[ilay],patternb[ilay],fopt);
           }
           TofClusterHR *phits[4]={0};
           TOFClSel(phit,phitb,phits,tklcoo,tklcoob,tkcosz,tkcoszb); 
           for(int ilay=0;ilay<4;ilay++){
//              if(phit[ilay]&&iktr==1)cout<<"ilay="<<ilay<<" RT="<<phit[ilay]->Time<<" RQ="<<phit[ilay]->GetQSignal(-1)<<endl;
              if(phit[ilay]!=phits[ilay]){//new found
                len[ilay]=lenb[ilay];
                tklcoo[ilay]=tklcoob[ilay];
                tkcosz[ilay]=tkcoszb[ilay];
                cres[ilay][0]=cresb[ilay][0];cres[ilay][1]=cresb[ilay][1];
                pattern[ilay]=patternb[ilay];
                phit[ilay]=phits[ilay];
//                if(iktr==1)cout<<"ilay="<<ilay<<" NT="<<phit[ilay]->Time<<" NQ="<<phit[ilay]->GetQSignal(-1)<<endl;
             }
           }
         }
//---first to recover 
           betapar.Init();//first initial
           int mode=(tktrdflag==0)?1:11;
           int tstat=TRecover(phit,tklcoo,pattern,mode);
           if((tktrdflag==0)||(tstat==0)){
             BetaFitC(phit,cres,pattern,betapar,1);
             BetaFitT(phit,len,pattern,betapar,mode,verse);
//--Addtional Check
             tstat=0;
             if(BuildOpt>=0)tstat=BetaFitCheck(phit,cres,len,pattern,betapar,mode);
             if((tktrdflag==0)||(tstat==0)){
               EdepTkAtt(phit,tklcoo,tkcosz,betapar);
               betapar.Status|=status;
          //     if(tktrdflag!=0)cout<<"beta="<<betapar.Beta<<endl;
#ifndef __ROOTSHAREDLIBRARY__
               cont->addnext(new AMSBetaH(phit,dynamic_cast<AMSTrTrack *>(usetr),usetrd,useshow,betapar));
#else
               cont->addnext(new  BetaHR(phit,usetr,usetrd,useshow,betapar));
               BetaHLink(usetr,usetrd,useshow);
#endif
               found++;
             }
            }
          }
        }
        
       if(iktr==1)delete ptrack;
     }//ntrack;

//---After Track Find Continue
    if(iktr==0&&((BuildOpt==3)||(BuildOpt/10000%10==3)))found=0;//Opt=3 Build Continue After Track Find
    if(iktr==1&&(BuildOpt/1000%10==3))found=0;//Opt=3 Build Continue After Trd Find
//----
    if(found>0)break;
    
  }//2 type
 
// 2nd Search Tk-Trd exit 
 if(tktrdflag!=0||BuildOpt<0)found=-1; //faild    
 

//No Tk or Trd Match, Tof Self Track Recontruction
  int npairu=0;
  int npaird=0; 
  if(found==0){
     npairu=TOFClMakePair(0,1,0);//Layer0 and 1 Make pair 
     npaird=TOFClMakePair(2,3,1);
   }
  
  if(found==0&&(npairu>=1||npaird>=1)){   
///---If EcalShower
    if((ecalshow.size()>0)&&(!(BuildOpt>=10&&(BuildOpt/100%10==0)))){
       number cooshow[3];
       for(int ish=0;ish<ecalshow.size();ish++){
         if(ecalshow.at(ish).Entry[2]>ecalshow.at(ish).Exit[2]){
           cooshow[0]=ecalshow.at(ish).Entry[0];
           cooshow[1]=ecalshow.at(ish).Entry[1];
           cooshow[2]=ecalshow.at(ish).Entry[2];
         }
         else {
           cooshow[0]=ecalshow.at(ish).Exit[0];
           cooshow[1]=ecalshow.at(ish).Exit[1];
           cooshow[2]=ecalshow.at(ish).Exit[2];
         }
         AMSDir showdir(ecalshow.at(ish).Dir);
//-----Build Up+Down
         TofClusterHR *tfhitu[2]={0},*tfhitd[2]={0};
         TOFPairPreSel(0,cooshow,showdir,TofRecPar::TFEcalAMatch);
         TOFPairSel(0,tfhitu);
         if(!tfhitu[0])continue;
         EcalSearchD(tfhitu,tfhitd,cooshow);
         if(!tfhitd[0])continue;
         TofClusterHR *phit[4]={0};
///----Assemble
         phit[0]=tfhitu[0];phit[1]=tfhitu[1];
         phit[2]=tfhitd[0];phit[3]=tfhitd[1];
///---FitC
         betapar.Init();
         TofTrackFit(phit,betapar,1,1); //
         betapar.Status|=TOFDBcN::ECALTRACK;
#ifndef __ROOTSHAREDLIBRARY__
         AMSTrTrack *usetr=0; AMSTRDTrack *usetrd=0; AMSEcalShower *useshow=amsecalshow.at(ish);
         cont->addnext(new AMSBetaH(phit,dynamic_cast<AMSTrTrack *>(usetr),usetrd,useshow,betapar));
#else
         TrTrackR *usetr=0; TrdTrackR *usetrd=0; EcalShowerR *useshow=ev->pEcalShower(ish);
         cont->addnext(new  BetaHR(phit,usetr,usetrd,useshow,betapar));
         BetaHLink(usetr,usetrd,useshow);
#endif
         TOFClErase(phit);
///---
         found++; 
     }//show loop

    }//end if

///---Dump Track Tof Self Reconstruction
    if(found==0&&(!(BuildOpt>=10&&(BuildOpt/10%10==0)))){
//--Pair+Pair
       for(int iclu=0;iclu<tofclp[0].size();iclu++){

//----Up Search Down
         int bestdid=PairSearchUD(tofclp[0].at(iclu),0,0);
         if(bestdid<0)continue;

//----Down Search Up again to see if up is the same best match
         int bestuid=PairSearchUD(tofclp[1].at(bestdid),1,0);
         if(bestuid!=iclu)continue;
//---
         TofClusterHR *phit[4]={0};
         phit[0]=tofclp[0].at(iclu).first;    phit[1]=tofclp[0].at(iclu).second;
         phit[2]=tofclp[1].at(bestdid).first; phit[3]=tofclp[1].at(bestdid).second;
         betapar.Init();
         TofTrackFit(phit,betapar,1,1);
         betapar.Status|=TOFDBcN::TOFTRACK;
#ifndef __ROOTSHAREDLIBRARY__
         AMSTrTrack *usetr=0; AMSTRDTrack *usetrd=0; AMSEcalShower *useshow=0;
         cont->addnext(new AMSBetaH(phit,dynamic_cast<AMSTrTrack *>(usetr),usetrd,useshow,betapar));
#else
         TrTrackR *usetr=0; TrdTrackR *usetrd=0; EcalShowerR *useshow=0;
         cont->addnext(new  BetaHR(phit,usetr,usetrd,useshow,betapar));
         BetaHLink(usetr,usetrd,useshow);
#endif
         TOFClErase(phit);iclu=-1;//Vector size change-Go back to Re-search Best Match
         found++;
//-----
       }//loop1

//----Pair+1Counter
      if(found==0&&(tofclp[0].size()>=1||tofclp[1].size()>=1)){
//---
       for(int iud=0;iud<2;iud++){
//---sort by Time
         if(tofclp[iud].size()==0)continue;
         sort(tofclp[iud].begin(),tofclp[iud].end(),PairCompare);
//---best Pair+1Layer
         for(int icl=0;icl<tofclp[iud].size();icl++){
           int bestdid=PairSearchUD(tofclp[iud].at(icl),iud,1);
           if(bestdid<0)continue;

//--found+prepare
           TofClusterHR *phit[4]={0};
           phit[iud*2]=tofclp[iud].at(icl).first;    phit[iud*2+1]=tofclp[iud].at(icl).second;
           phit[bestdid/1000]=tofclh[bestdid/1000].at(bestdid%1000); 
           betapar.Init();
           TofTrackFit(phit,betapar,1,1);
           betapar.Status|=TOFDBcN::TOFTRACK;
#ifndef __ROOTSHAREDLIBRARY__
           AMSTrTrack *usetr=0; AMSTRDTrack *usetrd=0; AMSEcalShower *useshow=0;
           cont->addnext(new AMSBetaH(phit,dynamic_cast<AMSTrTrack *>(usetr),usetrd,useshow,betapar));
#else
           TrTrackR *usetr=0; TrdTrackR *usetrd=0; EcalShowerR *useshow=0;
           cont->addnext(new  BetaHR(phit,usetr,usetrd,useshow,betapar));
           BetaHLink(usetr,usetrd,useshow);
#endif
           TOFClErase(phit);icl=-1;//Vector size change-Go back to Re-search Best Match
           found++;
          }
        }
 //----
      }
//----
#ifdef __ROOTSHAREDLIBRARY__
       if(found>0)BetaHReLink();
#endif 
    }//end if
  }//---found


  if(found==0&&BuildOpt>=0){tktrdflag=1; goto tktrdf;} //No Tof Sel Track, Search Tk-Trd Again (more loose)

  delete cont;
  return 0;

}
//========================================================
#if defined (_PGTRACK_) || defined (__ROOTSHAREDLIBRARY__)
int  TofRecH::BetaFindTOFCl(TrTrackR *ptrack,  int ilay,TofClusterHR **tfhit,number &tklen,number &tklcoo,number &tkcosz,number cres[2],int &pattern,int opt){
#else
int  TofRecH::BetaFindTOFCl(AMSTrTrack *ptrack,int ilay,TofClusterHR **tfhit,number &tklen,number &tklcoo,number &tkcosz,number cres[2],int &pattern,int opt){
#endif
//---init
    int nowbar=0,prebar=-1000,nextbar=-1000,npattern=0;
    uinteger tfhstat=0;
    number mscoo=1000,dscoo=1000;
    number theta,phi,sleng;
    int iscoo=1-TOFGeom::Proj[ilay];//short coo y x x y
     (*tfhit)=0; tklen=0;pattern=0;cres[0]=cres[1]=1000;

///--First search
    vector<pair <integer,number> >fbarid;
    for(int i=0;i<tofclh[ilay].size();i++){
       AMSPoint tfcoo (tofclh[ilay].at(i)->Coo);
       nowbar=         tofclh[ilay].at(i)->Bar;
//---Intepolate 
       AMSDir tkdir(0,0,1);AMSPoint tkcoo;   
#if defined (_PGTRACK_) || ! defined (__ROOTSHAREDLIBRARY__)
       ptrack->interpolate(tfcoo,tkdir,tkcoo,theta,phi,sleng);
#else
       return -1;
#endif
       dscoo=fabs(tkcoo[iscoo]-tfcoo[iscoo])-TOFGeom::Sci_w[ilay][nowbar]/2.;
       fbarid.push_back(make_pair<integer,number>(i,dscoo)); 
     }

//--sort id by distance+fix index     
     sort(fbarid.begin(),fbarid.end(),IdCompare<number>);
     integer idb=-1,ib=-1;
     if     (opt%10==0&&fbarid.size()>=1)idb=0;//0 or 10 barid cmin
     else if(opt%10==1&&fbarid.size()>=2)idb=1;//11  barid cmin+

//--selection
     if(idb>=0){
        ib=fbarid.at(idb).first; dscoo=fbarid.at(idb).second;
        bool transcut=0;
        if  (opt%100==0)transcut=(dscoo<TofRecPar::TkLSMatch);//1nd search
        else   {//2nd search
          if(opt/100==0)transcut=(dscoo<TofRecPar::TkLSMatch2);//Tk Case
          else          transcut=(dscoo<TofRecPar::TRDLSMatch2);//TRD Case
        }
//---
        if(transcut){
          AMSPoint tfcoo (tofclh[ilay].at(ib)->Coo);
          nowbar=         tofclh[ilay].at(ib)->Bar;
          tfhstat=        tofclh[ilay].at(ib)->Status;
          npattern=       tofclh[ilay].at(ib)->Pattern%10;
          if(ib==0)                    prebar=-1000;
          else                         prebar=tofclh[ilay].at(ib-1)->Bar;
          if(ib==tofclh[ilay].size()-1)nextbar=-1000;
          else                         nextbar=tofclh[ilay].at(ib+1)->Bar;
//---Intepolate 
          AMSDir tkdir(0,0,1);AMSPoint tkcoo;
#if defined (_PGTRACK_) || ! defined (__ROOTSHAREDLIBRARY__)
          ptrack->interpolate(tfcoo,tkdir,tkcoo,theta,phi,sleng);
#else
          return -1;
#endif
//---
          (*tfhit)=tofclh[ilay].at(ib);
          tklen=sleng;
          tklcoo=tkcoo[1-iscoo]; tkcosz=cos(theta);//tkcosz=fabs(tkdir[2]);
          pattern=npattern;
          for(int ip=0;ip<2;ip++){
             cres[ip]=tkcoo[ip]-tfcoo[ip];
             if(((tfhstat&TOFDBcN::BADTCOO)>0)&&(ip==1-iscoo)){cres[ip]=TOFGeom::Sci_l[ilay][nowbar]/2.;}
           }
          if(((tfhstat&TOFDBcN::BADTIME)==0)||BuildOpt<0)pattern=4;
//----previous next bar
          if(prebar>=0) pattern+=(nowbar-prebar)*100;
          if(nextbar>=0)pattern+=(nextbar-nowbar)*10;
        }
    }
     
   pattern+=1000*tofclh[ilay].size();

   return 0;
}

//========================================================
int  TofRecH::TOFClSel(TofClusterHR *tfhit[4],TofClusterHR *tfhitb[4],TofClusterHR *tfhits[4],number tklcoo[4],number tklcoob[4],number tkcosz[4],number tkcoszb[4]){

   for(int iud=0;iud<2;iud++){
//--
     TofClusterHR *phit[2][2]={{0}};
     phit[0][0]=tfhits[2*iud]=tfhit[2*iud];
     phit[0][1]=tfhitb[2*iud];
     phit[1][0]=tfhits[2*iud+1]=tfhit[2*iud+1];   
     phit[1][1]=tfhitb[2*iud+1]; 
     number qsmax=0,qdmin=FLT_MAX,qa=0,qb=0,qd=0,qsum=0;
     int qpat[2]={-1,-1}; 
     TofClusterHR *phits[2]={0},*phitsb[2]={0};
//---
     for(int ib=0;ib<2;ib++){
       if(phit[0][ib]==0||(phit[1][0]==0&&phit[1][1]==0))continue;
       if(ib==0)qa=phit[0][ib]->GetQSignal(2,TofClusterHR::DefaultQOpt,tkcosz[2*iud],1,tklcoo[2*iud]);
       else     qa=phit[0][ib]->GetQSignal(2,TofClusterHR::DefaultQOpt,tkcoszb[2*iud],1,tklcoob[2*iud]);
       bool tmatch=((phit[0][ib]->Status&TOFDBcN::BADTIME)==0);
       for(int ib1=0;ib1<2;ib1++){
          if(phit[1][ib1]==0)continue;
          if(ib1==0)qb=phit[1][ib1]->GetQSignal(2,TofClusterHR::DefaultQOpt,tkcosz[2*iud+1],1,tklcoo[2*iud+1]);
          else      qb=phit[1][ib1]->GetQSignal(2,TofClusterHR::DefaultQOpt,tkcoszb[2*iud+1],1,tklcoob[2*iud+1]);
//--TMatch
          if(tmatch)tmatch=((phit[1][ib1]->Status&TOFDBcN::BADTIME)==0);
          if(tmatch){
             number ndt=phit[0][ib]->Time-phit[1][ib1]->Time;
             number dcoz=phit[0][ib]->Coo[2]-phit[1][ib1]->Coo[2];
             tmatch=(fabs(ndt)<fabs(dcoz)*1.414/(TofRecPar::NonTkBetaCutL*cvel)+TofRecPar::PairTMatch);//45degree 0.3c slow   
          }
//---EMax-Counter
          qsum=qa+qb;
          qd=fabs((qa-qb)/(qa+qb));
          if(tmatch){//TMatch Case, Use Max-Edep Counter
            if(qsum>qsmax){phits[0]=phit[0][ib];phits[1]=phit[1][ib1];qpat[0]=ib*10+ib1;qsmax=qsum;}
            if(qd<qdmin)  {phitsb[0]=phit[0][ib];phitsb[1]=phit[1][ib1];qpat[1]=ib*10+ib1;qdmin=qd;}
          }
       }
     }
//---
      if(phits[0]!=0&&phits[1]!=0){
        if(qpat[1]==0){phits[0]=phitsb[0];phits[1]=phitsb[1];}//Best-EMatch+PosMatch
        tfhits[2*iud]=phits[0];tfhits[2*iud+1]=phits[1];//Largest-E
      }  
   }
     

   return 0;
}

//========================================================
int TofRecH::TOFPairSel(int ud,TofClusterHR* tfhit[2]){

    int candid=-1,qmaxid=-1,tminid=-1;//
    number qmax[2]={0.},tmin=FLT_MAX;
    int mysize=tofclc[ud].size();
    for(int icl=0;icl<tofclc[ud].size();icl++){//Up part
       TofClusterHR *cl0=tofclc[ud].at(icl).first;
       TofClusterHR *cl1=tofclc[ud].at(icl).second;
///--Find Emax
       if(cl0->GetQSignal(-1)+cl1->GetQSignal(-1)>qmax[0]){
         qmaxid=icl;
         qmax[1]=qmax[0];qmax[0]=cl0->GetQSignal(-1)+cl1->GetQSignal(-1);
       }
///--Find Tmin
       if((cl1->Time+cl0->Time)/2.<tmin){//earliest time to avoid backsplish
         tminid=icl;
         tmin=(cl1->Time+cl0->Time)/2.;
         }
     }
///-----
     if(qmaxid>=0){
       if(qmax[1]<=0||qmax[0]<0||(qmax[0]/qmax[1]>TofRecPar::PairQRgate))candid=qmaxid;  //Ion Case
       else candid=tminid;//Back Spalish suppression
       tfhit[0]=tofclc[ud].at(candid).first; tfhit[1]=tofclc[ud].at(candid).second;
     }
   return 0;
}

//=======================================================
int  TofRecH::TOFPairPreSel(int ud,number coref[],AMSDir diref,number cutangle){

    tofclc[ud].clear();
///---
    number cop[3],disa;//pair coo
    for(int iclp=0;iclp<tofclp[ud].size();iclp++){//Up part
       TofClusterHR *cl0=tofclp[ud].at(iclp).first;
       TofClusterHR *cl1=tofclp[ud].at(iclp).second;
       cop[cl0->GetDirection()]= cl0->Coo[cl0->GetDirection()];
       cop[cl1->GetDirection()]= cl1->Coo[cl1->GetDirection()];
       cop[2]=(cl0->Coo[2]+cl1->Coo[2])/2.;
       AMSDir dirnew=AMSDir(cop[0]-coref[0],cop[1]-coref[1],cop[2]-coref[2]);
       disa=fabs(dirnew.prod(diref));
       disa=acos(disa)*180./3.1415926;
       if(disa<cutangle){tofclc[ud].push_back(tofclp[ud].at(iclp));}
   }
   return 0;
}

//========================================================
int TofRecH::EcalSearchD(TofClusterHR* tfhitu[2], TofClusterHR* tfhitd[2],number cooshow[3]){

    number cop[3],timeup,coodis[2],tmind[2]={FLT_MAX,FLT_MAX};
    bool tmatch[2];
    cop[tfhitu[0]->GetDirection()]=tfhitu[0]->Coo[tfhitu[0]->GetDirection()];
    cop[tfhitu[1]->GetDirection()]=tfhitu[1]->Coo[tfhitu[1]->GetDirection()];
    cop[2]=(tfhitu[0]->Coo[2]+tfhitu[1]->Coo[2])/2.;
    timeup=(tfhitu[0]->Time+tfhitu[1]->Time)/2.;
///--Dir
   AMSDir dirnew=AMSDir(cop[0]-cooshow[0],cop[1]-cooshow[1],cop[2]-cooshow[2]);
   for(int ilay=2;ilay<4;ilay++){
      for(int i=0;i<tofclh[ilay].size();i++){
         if(!(tofclh[ilay].at(i)->IsGoodSide(0))||!(tofclh[ilay].at(i)->IsGoodSide(1)))continue;
         AMSPoint tfcoo (tofclh[ilay].at(i)->Coo);
         coodis[0]=dirnew[0]/dirnew[2]*(tfcoo[2]-cooshow[2])+cooshow[0]-tfcoo[0];
         coodis[1]=dirnew[1]/dirnew[2]*(tfcoo[2]-cooshow[2])+cooshow[1]-tfcoo[1];
//---Coo Match
         if(fabs(coodis[TOFGeom::Proj[ilay]])>TofRecPar::PairLSMatch)continue;//Project
         if(fabs(coodis[1-TOFGeom::Proj[ilay]])>tofclh[ilay].at(i)->GetBarWidth())continue;
///---TMatch
         tmatch[0]=tofclh[ilay].at(i)->Time-timeup<((cop[2]-tfcoo[2])*1.414/(TofRecPar::NonTkBetaCutL*cvel)+TofRecPar::PairTMatch);
         tmatch[1]=tofclh[ilay].at(i)->Time-timeup>((cop[2]-tfcoo[2])/(TofRecPar::NonTkBetaCutU*cvel)-TofRecPar::PairTMatch);
         if(!(tmatch[0]&&tmatch[1]))continue;//time match windows
         if(tofclh[ilay].at(i)->Time<tmind[ilay-2]){tmind[ilay-2]=tofclh[ilay].at(i)->Time;tfhitd[ilay-2]=tofclh[ilay].at(i);}
       }
    }
   return 0;
}

//=====================================================
int TofRecH::TofTrackFit(TofClusterHR *tfhit[4],TofBetaPar &par,int attrefit,int verse){
  
   number coo[3][4]={{0}},ecoo[3][4]={{0}};//X Y Z 4Layer;
   int nhits=0,hitl[4]={0},pattern[4]={0};
   
///--Get Par
   for(int ilay=0;ilay<4;ilay++){
     pattern[ilay]=1000*tofclh[ilay].size();
     if(tfhit[ilay]==0)continue;
     coo[0][nhits]= tfhit[ilay]->Coo[0];
     coo[1][nhits]= tfhit[ilay]->Coo[1];
     coo[2][nhits]= tfhit[ilay]->Coo[2];
     ecoo[0][nhits]=tfhit[ilay]->ECoo[0];
     ecoo[1][nhits]=tfhit[ilay]->ECoo[1];
     ecoo[2][nhits]=tfhit[ilay]->ECoo[2];
     hitl[nhits]=ilay;
     pattern[ilay]+=4;
//--fill to par
     for(int i=0;i<tofclh[ilay].size();i++){
       if(tofclh[ilay].at(i)==tfhit[ilay]){
        if(i!=0)                    {pattern[ilay]+=(tfhit[ilay]->Bar-tofclh[ilay].at(i-1)->Bar)*100;}
        if(i!=tofclh[ilay].size()-1){pattern[ilay]+=(tofclh[ilay].at(i+1)->Bar-tfhit[ilay]->Bar)*10;}
      }
     }
//----
     nhits++;
  }

///--Space Line Fit x=az+b y=az+b
   number ax,bx,ay,by;
   LineFit(nhits,coo[2],coo[0],ecoo[0],ax,bx);
   LineFit(nhits,coo[2],coo[1],ecoo[1],ay,by);
///--Length+Time Fit
   number len[4]={0},res[4][2]={0},tklcoo[4]={0},tkcosz[4]={1,1,1,1};
   for(int ihit=0;ihit<nhits;ihit++){
     AMSPoint  p1(ax*coo[2][ihit]+bx,ay*coo[2][ihit]+by,coo[2][ihit]);
     AMSPoint  p0(ax*0.+bx,ay*0.+by,0.);
     AMSPoint  p2=(p1-p0);
     int ilay=hitl[ihit];
     len[ilay]=p2.norm();
     if(coo[2][ihit]>0)len[ilay]*=-1;
     res[ilay][0]=p1[0]-coo[0][ihit];
     res[ilay][1]=p1[1]-coo[1][ihit];
     tklcoo[ilay]=p1[TOFGeom::Proj[ilay]];
     tkcosz[ilay]=fabs(AMSDir(p2)[2]);
   }
   BetaFitC(tfhit,res,pattern,par,1);
   BetaFitT(tfhit,len,pattern,par,1,verse);
   if(attrefit)EdepTkAtt(tfhit,tklcoo,tkcosz,par);
   return 0;
}

//=====================================================
int TofRecH::LineFit(int nhits,number x[],number y[],number ey[],number &a,number &b){//line y=ax+b

   number xy=0,x2=0,xa=0,ya=0,e2=0;//line y=ax+b x=kz+b
//--Fit
   for(int i=0;i<nhits;i++){
      xy+=x[i]*y[i]/ey[i]/ey[i];
      x2+=x[i]*x[i]/ey[i]/ey[i];
      xa+=x[i]/ey[i]/ey[i];
      ya+=y[i]/ey[i]/ey[i];
      e2+=1/ey[i]/ey[i];
   }
   xy=xy/e2;x2=x2/e2;
   xa=xa/e2;ya=ya/e2;
   double kk=(x2-xa*xa);
   if(kk==0) kk=0.000001;
   a=(xy-xa*ya)/kk;b=ya-a*xa;
   return 0;
}


//========================================================
int TofRecH::TOFClMakePair(int il0,int il1,int isdown){

   number q0,q1,dco[3];
   bool   ematch,cmatch[2],tmatch;
///--one layer
   for(int i=0;i<tofclh[il0].size();i++){
       if(!(tofclh[il0].at(i)->IsGoodSide(0))||!(tofclh[il0].at(i)->IsGoodSide(1)))continue;
       int isig=1;
       q0=tofclh[il0].at(i)->GetQSignal(-1);
///--other laye
       for(int j=0;j<tofclh[il1].size();j++){
         if(!(tofclh[il1].at(j)->IsGoodSide(0))||!(tofclh[il1].at(j)->IsGoodSide(1)))continue;
         q1=tofclh[il1].at(j)->GetQSignal(-1); 
//--qgate cut  0
         if(q0<TofRecPar::PairQgate||q1<TofRecPar::PairQgate)continue;
//--qmatch cut 1
         ematch=(q1/q0<TofRecPar::PairQRgate&&q0/q1<TofRecPar::PairQRgate);
         if(!ematch)continue;
//--coo match cut ilay0 Lcoo- ilay1 Scoo
         dco[0]=tofclh[il0].at(i)->Coo[TOFGeom::Proj[il0]]-tofclh[il1].at(j)->Coo[TOFGeom::Proj[il0]];//lay0 lcoo -lay1 scoo <lay1 offset
         dco[1]=tofclh[il1].at(j)->Coo[TOFGeom::Proj[il1]]-tofclh[il0].at(i)->Coo[TOFGeom::Proj[il1]]; //lay1 lcoo- lay0 scoo <lay0 offset
         dco[2]=tofclh[il0].at(i)->Coo[2]-tofclh[il1].at(j)->Coo[2];
         cmatch[0]=(fabs(dco[0])<tofclh[il1].at(j)->GetBarWidth()/2.+TofRecPar::PairLSMatch+fabs(dco[2]));//45 degree
         cmatch[1]=(fabs(dco[1])<tofclh[il0].at(i)->GetBarWidth()/2.+TofRecPar::PairLSMatch+fabs(dco[2]));
         if(!cmatch[0]||!cmatch[1])continue;
//---time match cut
         tmatch=fabs(tofclh[il0].at(i)->Time-tofclh[il1].at(j)->Time)<(fabs(dco[2])*1.414/(TofRecPar::NonTkBetaCutL*cvel)+TofRecPar::PairTMatch);//45degree 0.3c slow
         if(!tmatch)continue;
         tofclp[isdown].push_back(make_pair<TofClusterHR*,TofClusterHR*>(tofclh[il0].at(i),tofclh[il1].at(j)));
    }
  }

  return tofclp[isdown].size(); 
}


//========================================================
bool TofRecH::PairMatchUD(pair <TofClusterHR*,TofClusterHR*>upair,pair <TofClusterHR*,TofClusterHR*>dpair,number &edis){

     TofClusterHR *cl[4]={0};
     cl[0]=upair.first;cl[1]=upair.second;
     cl[2]=dpair.first;cl[3]=dpair.second;
//-----Get QT
     number qu=0,qd=0,tu=0,td=0,cu=0,cd=0;
     int nu=0,nd=0;
     for(int il=0;il<4;il++){//Glue Up2 or Down2
       if(!cl[il])continue;
       if(il<2){qu+=cl[il]->GetQSignal(-1);tu+=cl[il]->Time;cu+=cl[il]->Coo[2];nu++;}
       else    {qd+=cl[il]->GetQSignal(-1);td+=cl[il]->Time;cd+=cl[il]->Coo[2];nd++;}  
     }
     if(nu==0||nd==0)return false;
     qu/=nu; qd/=nd; 
     tu/=nu; td/=nd; 
     cu/=nu; cd/=nd;
     edis=fabs(qu-qd)/(qu+qd);
//----UP+Down Match
     bool tmatch[2];
     tmatch[0]=fabs(td-tu)<fabs(cu-cd)*1.414/(TofRecPar::NonTkBetaCutL*cvel)+TofRecPar::PairTMatch;
     tmatch[1]=fabs(td-tu)>fabs(cu-cd)/(TofRecPar::NonTkBetaCutU*cvel)-TofRecPar::PairTMatch;
     bool ematch=((qu/qd<TofRecPar::PairQRgate)&&(qd/qu<TofRecPar::PairQRgate));
     return (ematch&&tmatch[0]&&tmatch[1]);
}


//=======================================================
int  TofRecH::PairSearchUD(pair <TofClusterHR*,TofClusterHR* >sedpair,int sedud,int opt){
  
   number chistmax=TofRecPar::DPairChi2TCut,edismin=1,betamax=0,edis;
   int bestid=-1;

//--pair+pair
   if(opt==0){
    for(int icl=0;icl<tofclp[1-sedud].size();icl++){

//--match
       if(PairMatchUD(sedpair,tofclp[1-sedud].at(icl),edis)){
	  TofClusterHR *phitc[4]={0};
	  phitc[2*sedud]=      sedpair.first;
	  phitc[2*sedud+1]=    sedpair.second;
	  phitc[2*(1-sedud)]=  tofclp[1-sedud].at(icl).first;
	  phitc[2*(1-sedud)+1]=tofclp[1-sedud].at(icl).second;
	  betapar.Init();
	  TofTrackFit(phitc,betapar,0,0);
	  if(betapar.Chi2C>TofRecPar::DPairChi2CCut)continue;
	  if(fabs(betapar.Beta)<TofRecPar::NonTkBetaCutL)continue;//Too fast
	  if(fabs(betapar.Beta)>TofRecPar::NonTkBetaCutU)continue;//Too slow
	  if(betapar.Chi2T<chistmax){
            chistmax=betapar.Chi2T;
            bestid=icl;
	  }
       }
    }

  }

//---pair+1 layer
  else { 
     int slay=2*(1-sedud);

//--using ematch else using min-beta
     bool emath=0;
     if((sedpair.first->GetQSignal(-1)+sedpair.second->GetQSignal(-1))/2.>TofRecPar::PairQRgate)emath=1;
     for(int ilay=slay;ilay<=slay+1;ilay++){

//---all layer candidate
       for(int i=0;i<tofclh[ilay].size();i++){
         if(!(tofclh[ilay].at(i)->IsGoodSide(0))||!(tofclh[ilay].at(i)->IsGoodSide(1)))continue;//Side OK
         if(fabs(tofclh[ilay].at(i)->Coo[TOFGeom::Proj[ilay]])>TOFGeom::Sci_l[ilay][tofclh[ilay].at(i)->Bar]/2.+15)continue;//5sigma 15cm
         TofClusterHR *clnu=0; 

///--check match
         if(PairMatchUD(sedpair,make_pair(tofclh[ilay].at(i),clnu),edis)){
            TofClusterHR *phitc[4]={0};
            phitc[2*sedud]=      sedpair.first;
            phitc[2*sedud+1]=    sedpair.second;
            phitc[ilay]=tofclh[ilay].at(i);
            betapar.Init();
            TofTrackFit(phitc,betapar,0,0);
            if(betapar.Chi2C>TofRecPar::DPairChi2CCut2)continue;
            if(fabs(betapar.Beta)<TofRecPar::NonTkBetaCutL2)continue;//Too fast
            if(fabs(betapar.Beta)>TofRecPar::NonTkBetaCutU2)continue;//Too slow
            if(betapar.Chi2T>TofRecPar::DPairChi2TCut2)continue;
//---emath
            if(emath){
               if(edis<edismin){edismin=edis; bestid=ilay*1000+i;} //Using Energy
             }
            else {//max beta
               if(fabs(betapar.Beta)>betamax){betamax=fabs(betapar.Beta); bestid=ilay*1000+i;} //Using Max Beta
            }
//----
         }
       }
//----
     }

   }
//--
   return bestid;
}


//========================================================
int TofRecH::TOFClErase(TofClusterHR *tfhit[4]){

//---Erase From Pair
   for(int ilay=0;ilay<4;ilay++){
      if(tfhit[ilay]==0)continue;
      for(int iclp=0;iclp<tofclp[ilay/2].size();iclp++){
        if(tofclp[ilay/2].at(iclp).first==tfhit[ilay]||tofclp[ilay/2].at(iclp).second==tfhit[ilay]){
          tofclp[ilay/2].erase(tofclp[ilay/2].begin()+iclp);
          iclp=-1;//Size Change-From Begin Re-search To Erase
        }
       }
//--From From Layer vector
    for(int iclh=0;iclh<tofclh[ilay].size();iclh++){
       if(tofclh[ilay].at(iclh)==tfhit[ilay]){
         tofclh[ilay].erase(tofclh[ilay].begin()+iclh);break;
        }
      }
    }
   return 0;
}

//========================================================
bool TofRecH::PairCompare(const pair<TofClusterHR*,TofClusterHR*> &a,const pair<TofClusterHR*,TofClusterHR*> &b){

//--Time
    float dta=(a.first)->Time-(a.second)->Time;
    float dtd=(b.first)->Time-(b.second)->Time;
//--Edep
    float qp[2][2];
    qp[0][0]=(a.first)->GetQSignal(-1);qp[0][1]=(a.second)->GetQSignal(-1);
    qp[1][0]=(b.first)->GetQSignal(-1);qp[1][1]=(b.second)->GetQSignal(-1);
    bool emath=0;
    for(int ip=0;ip<2;ip++){
      if((qp[ip][0]+qp[ip][1])/2.>TofRecPar::PairQRgate){emath=1;break;}
    }
//--- 
   if(emath){return (qp[0][0]+qp[0][1])>(qp[1][0]+qp[1][1]);}//Largest
   return fabs(dta)<fabs(dtd);//Small
}

//======================================================== 
int TofRecH::TRecover(TofClusterHR *tfhit[4],number tklcoo[4],int pattern[4],int mode){

//--Directly Recover
 if(mode/10==0){//tktrdf
  for(int ilay=0;ilay<4;ilay++){
    if(!tfhit[ilay])continue;
    if((tfhit[ilay]->Status&TOFDBcN::BADTIME)>0){
      int hassid=((tfhit[ilay]->Status&TOFDBcN::BADN)>0)?1:0;//NBAD try PSIDE
      TofRecPar::IdCovert(ilay,tfhit[ilay]->Bar);
      TRecover(TofRecPar::Idsoft,geant(tklcoo[ilay]),tfhit[ilay]->Stime,tfhit[ilay]->Time,tfhit[ilay]->ETime,tfhit[ilay]->Status,hassid);
    }
  }
  return 0;
 } 

//--Using For Pattern Finding
  else  {

     uinteger status;
     geant tm,etm,time[4]={0},etime[4]={0},st[2]={0};
     int npattern[4]={0};
     bool tmatch[2]={0};
//----Mark pattern 
    for(int ilay=0;ilay<4;ilay++){
       if(!tfhit[ilay])continue;  
       npattern[ilay]=pattern[ilay]%10;
       time[ilay]=tfhit[ilay]->Time;
       etime[ilay]=tfhit[ilay]->ETime;
     }

     for(int iud=0;iud<2;iud++){
//----Recover Side
       number tms[2][2]={{0}},etms[2][2]={{0}};
       for(int il=0;il<2;il++){
          int ilay=iud*2+il;
          for(int is=0;is<2;is++){
            if(npattern[ilay]<3&&npattern[ilay]!=is+1)continue;
            if(!tfhit[ilay])continue;
            TofRecPar::IdCovert(ilay,tfhit[ilay]->Bar);
            st[0]=tfhit[ilay]->Stime[0];
            st[1]=tfhit[ilay]->Stime[1];
            TRecover(TofRecPar::Idsoft,geant(tklcoo[ilay]),st,tm,etm,status,is);
            tms[il][is]=tm; etms[il][is]=etm;
//           cout<<"ilay="<<ilay<<" is="<<is<<" tms="<< tms[il][is]<<endl;
         }
       }
       number dmint=FLT_MAX;
       int il0=iud*2, il1=iud*2+1;
       for(int is0=0;is0<2;is0++){
        for(int is1=0;is1<2;is1++){
          if(tms[0][is0]==0||tms[1][is1]==0)continue;
          number ndt=tms[0][is0]-tms[1][is1]; 
          if(fabs(ndt)<dmint){
            if(npattern[il0]!=4){npattern[il0]=is0+1;time[il0]=tms[0][is0];etime[il0]=etms[0][is0];}
            if(npattern[il1]!=4){npattern[il1]=is1+1;time[il1]=tms[1][is1];etime[il1]=etms[1][is1];}
            dmint=fabs(ndt);
          }
        }
       }
       tmatch[iud]=(npattern[il0]!=0&&npattern[il1]!=0);
       if(tmatch[iud]){
         double dcoz=tfhit[il0]->Coo[2]-tfhit[il1]->Coo[2];
         tmatch[iud]=(fabs(dmint)<fabs(dcoz)*1.414/(TofRecPar::NonTkBetaCutL*cvel)+1.414*TofRecPar::PairTMatch);//45degree 0.3c slow
       }
       if (!tmatch[iud]){
           if     (npattern[il0]==4&&npattern[il1]!=4)npattern[il1]=0;
           else if(npattern[il1]==4&&npattern[il0]!=4)npattern[il0]=0;
           if     (npattern[il0]==4||npattern[il1]==4)tmatch[iud]=1;
       }
       
      if(!tmatch[iud])return -1;
     }
//---Already Find
    for(int ilay=0;ilay<4;ilay++){
       pattern[ilay]=pattern[ilay]/10*10+npattern[ilay];
       if(!tfhit[ilay])continue;
       tfhit[ilay]->Time=time[ilay];
       tfhit[ilay]->ETime=etime[ilay];
       if(npattern[ilay]==1||npattern[ilay]==2)tfhit[ilay]->Status|=TOFDBcN::RECOVERED;
    }
    
   }
 
  
  return 0;
}

//======================================================== 
int TofRecH::EdepTkAtt(TofClusterHR *tfhit[4],number tklcoo[4],number tkcosz[4],TofBetaPar &par){
  for(int ilay=0;ilay<4;ilay++){
    if(!tfhit[ilay])continue;
    par.CosZ[ilay]=tkcosz[ilay];
    EdepRecR(ilay,tfhit[ilay]->Bar,tfhit[ilay]->Aadc,tfhit[ilay]->Dadc,tklcoo[ilay],par.AQ2L[ilay],par.DQ2L[ilay],par.AEdepL[ilay],par.DEdepL[ilay]);
  }
  return 0;
}

//========================================================
int TofRecH::BetaFitC(TofClusterHR *tfhit[4],number res[4][2],int pattern[4], TofBetaPar &par,int mode){
   int iscoo[4]={1,0,0,1};
   int nhitxy[2]={0},nhitl=0;
   number chisxy[2]={0};
   number chisl=0;
   for(int ilay=0;ilay<4;ilay++){//temp exclude missing time measument lay S side
     for(int is=0;is<2;is++){par.CResidual[ilay][is]=res[ilay][is];}
     if(tfhit[ilay]){
       AMSPoint tfecoo(tfhit[ilay]->ECoo);
       int isco=iscoo[ilay];
       int ilco=1-iscoo[ilay];
       nhitxy[isco]++;
       chisxy[isco]+=res[ilay][isco]*res[ilay][isco]/tfecoo[isco]/tfecoo[isco];
       if(pattern[ilay]%10==4){//not missing layer
             nhitxy[ilco]++;nhitl++;
             chisxy[ilco]+=res[ilay][ilco]*res[ilay][ilco]/tfecoo[ilco]/tfecoo[ilco];
             chisl+=res[ilay][ilco]*res[ilay][ilco]/tfecoo[ilco]/tfecoo[ilco];
          }
      }
   }
   par.Chi2C=(nhitl==0)?0:chisl/nhitl;
//  par.Chi2C=sqrt((chisxy[0]+chisxy[1])/(nhitxy[0]+nhitxy[1]));
  return 0;
}

//========================================================
int TofRecH::BetaFitT(TofClusterHR *tfhit[4],number lenr[4],int pattern[4], TofBetaPar &par,int mode,int verse){
//---
  number time [4];
  number etime[4];
  number len  [4];

  int selhit=0;
//---select+Fit
  for(int ilay=0;ilay<4;ilay++){
     par.Pattern[ilay]=pattern[ilay];
//     if(tfhit[ilay]==0)par.Pattern[ilay]=0;
     if(par.Pattern[ilay]%10>0)par.SumHit++;
     if(par.Pattern[ilay]%10!=4&&!(mode/10==1&&par.Pattern[ilay]%10>0))continue;//tktrdf
//--fill to par
     par.Time[ilay]= tfhit[ilay]->Time;//ns
     par.ETime[ilay]=tfhit[ilay]->ETime;//ns  
     par.Len[ilay]=  lenr[ilay];
//---fill to arr
     time [selhit]=par.Time[ilay];//ns
     etime[selhit]=par.ETime[ilay];//ns
     len  [selhit]=par.Len[ilay];
     selhit++;
   }

  par.UseHit=selhit;
  if(selhit<2||selhit>=5){par.Status|=TOFDBcN::BAD;return -1;}
  if(mode/10==1&&selhit>=2)par.UseHit=2;//tktrdf   

//--then Fit
   BetaFitT(time,etime,len,selhit,par,mode,verse);
   par.CalFitRes();
   return 0;
  
}

//========================================================
int TofRecH::TRecover(int idsoft,geant tklcoo,geant tms[2],geant &tm,geant &etm,uinteger &status,int hassid){

  if(TofTAlignPar::Head==0||TofTAlignPar::GetHead()->Isload!=1)return -1;
  TofTAlignPar *TPar=TofTAlignPar::GetHead();

//----tm recover for 1side
  number vel= TPar->vel[idsoft];
  number phdt=2.*tklcoo/vel;//photon time
  if(hassid==0){tms[1]=tms[0]-phdt;}
  else         {tms[0]=tms[1]+phdt;}
  tm=0.5*(tms[0]+tms[1]);
  tm=-tm;
//  etm=3*TofRecPar::GetTimeSigma(idsoft,1)*sqrt(2.);//1sid sqrt(2)
  etm=TofRecPar::GetTimeSigma(idsoft,1)*sqrt(2.);//1sid sqrt(2)
  status|=TOFDBcN::RECOVERED;
  return 0;
}

//========================================================
int TofRecH::BetaFitT(number time[],number etime[],number len[],const int nhits,TofBetaPar &par,int mode,int verse){
//---
  if(nhits<2||nhits>=5)return -1;
 
//--par
  number y[nhits],x[nhits];//y time /x length(no err)
  number ey[nhits];//etime
  number xy=0,x2=0,xa=0,ya=0,e2=0,a,b;//line y=ax+b
  number Beta=0,BetaC=0,InvErrBeta=0,InvErrBetaC=0,Chi2=0,T0=0;
  uinteger status=0;
  int i;
  for(i=0;i<nhits;i++){
     ey[i]=(mode%10==0)?0.15:etime[i];//0.15ns if same weight
     ey[i]=ey[i]*cvel;
     y[i]=time[i]*cvel;
     x[i]=len[i];
   }

//--Fit
   for(i=0;i<nhits;i++){
      xy+=x[i]*y[i]/ey[i]/ey[i];
      x2+=x[i]*x[i]/ey[i]/ey[i];
      xa+=x[i]/ey[i]/ey[i];
      ya+=y[i]/ey[i]/ey[i];
      e2+=1/ey[i]/ey[i];
   }
   xy=xy/e2;x2=x2/e2;
   xa=xa/e2;ya=ya/e2;
   double kk=(x2-xa*xa);
   if(kk==0) kk=0.000001;
   a=(xy-xa*ya)/kk;b=ya-a*xa;

//---par Beta
   if (a==0)Beta=100;
   else     Beta=1/a;
   if(fabs(Beta)>2&&verse>0){
     static int nerr=0;
     if(nhits>2 && nerr++<100){
       cout<<"<--- AMSBetaH::SimpleFit BetaOut-OfRange "<<Beta<<endl;
       cout<<"TimeRec=";for(i=0;i<nhits;i++)cout<<y[i]<<" ";cout<<endl;
     }
//     if(Beta>0)Beta=2;
//     else Beta=-2;
   }
  T0=b/cvel;

//--par Chis+EBeta
   for(i=0;i<nhits;i++)Chi2+=(y[i]-a*x[i]-b)/ey[i]*(y[i]-a*x[i]-b)/ey[i];
   if(nhits>2)Chi2=Chi2/(nhits-2);
   else       Chi2=0; 
   InvErrBeta=sqrt(1./e2)/sqrt(kk);
#ifndef __ROOTSHAREDLIBRARY__
//--par BetaC+EBetaC
    if(InvErrBeta==InvErrBeta && fabs(InvErrBeta)<FLT_MAX && nhits>2){
       number xibme=fabs(1/Beta);
       number z0=(xibme-1)/InvErrBeta/sqrt(2.);
       number zint=DERFC(z0);
       number zprim=BetaCorr(zint,z0,0.5,status);
       number xibcor=zprim*sqrt(2.)*InvErrBeta+1;
       BetaC=1/xibcor;
       if(Beta<0)BetaC*=-1;
       number zprima=BetaCorr(zint,z0,1./6.,status);
       number zprimb=BetaCorr(zint,z0,5./6.,status);
       number xibcora=zprima*sqrt(2.)*InvErrBeta+1;
       number xibcorb=zprimb*sqrt(2.)*InvErrBeta+1;
       InvErrBetaC=fabs(xibcorb-xibcora)/2;
  }
  else InvErrBetaC=0;
  if(InvErrBetaC==0){
    BetaC=Beta;
    InvErrBetaC=InvErrBeta;
  }
#else
   if(fabs(Beta)<=1){BetaC=Beta;}
   else             {BetaC=1./(2.-1./fabs(Beta));if(Beta<0)BetaC=-BetaC;}
   InvErrBetaC=InvErrBeta;
#endif
//--now ready to par
  par.Status|=status;
  par.SetFitPar(Beta,BetaC,InvErrBeta,InvErrBetaC,Chi2,T0);
  return 0;
//---
}

//========================================================
int  TofRecH::BetaFitCheck(TofClusterHR *tfhit[4],number res[4][2],number lenr[4],int pattern[4],TofBetaPar &par,int mode){

//----double check don't need recover or unable to recover
  const float bcutl=0.3;
  const float bcuth=1.5;

//----set status
  bool bstat=(fabs(par.Beta)>bcutl&&fabs(par.Beta)<bcuth);
  bool cstat=(par.Chi2T<TofRecPar::DPairChi2TCut);
  if(!bstat)par.Status|=(TOFDBcN::BETAOVERFLOW|TOFDBcN::BAD);
  if(!cstat)par.Status|=(TOFDBcN::BETABADCHIT|TOFDBcN::BAD);

  if(mode/10==1){//tktrdf
     par.Status|=TOFDBcN::RECOVERED;
     if((par.Status&TOFDBcN::BAD)>0)return -1;
     else  return 0; 
  }

//--try to recover
  if((bstat&&cstat)||par.UseHit<=2){return 0;}
//-----remove one to see whether much better
  else {
     number mbeta=FLT_MAX;   
     TofBetaPar fitpar;
     for(int ilay=0;ilay<4;ilay++){

       if(pattern[ilay]%10!=4)continue;
//----now remove
       int npattern=pattern[ilay];
       pattern[ilay]=tfhit[ilay]->Pattern%10+pattern[ilay]/10*10;
       fitpar.Init();
       BetaFitC(tfhit,res,pattern,fitpar,1);
       BetaFitT(tfhit,lenr,pattern,fitpar,1,0);

       bool bstat1=(fabs(fitpar.Beta)>bcutl&&fabs(fitpar.Beta)<bcuth);
//----beta overflow
      if(!bstat&&bstat1){//First order
         par=fitpar;
         par.Status|=(TOFDBcN::BETAOVERFLOW|TOFDBcN::RECOVERED);break;
      }
//----big chis
      else if(!cstat&&bstat1&&fitpar.UseHit>=3){//too big chis-try to move---only for one-layer bad
        if((fitpar.Chi2T<TofRecPar::DPairChi2TCut2)&&(fabs(fabs(fitpar.Beta)-1)<mbeta)){//big-improve+select fastest beta
          par=fitpar;
          par.Status|=(TOFDBcN::BETABADCHIT|TOFDBcN::RECOVERED); mbeta=fabs(fabs(fitpar.Beta)-1);
        }
      } 

//----back pattern
      pattern[ilay]=npattern;
     }
   }
   

  return 1;
}

//========================================================
#ifndef __ROOTSHAREDLIBRARY__
number TofRecH::BetaCorr(number zint,number z0,number part,uinteger &status){//Vitaly
 if(zint<1./FLT_MAX){
   return 1;
 }
 number zmin=zint*part;
 number zb=z0+1;
 number x0;
 integer ntry=0;
 integer ntrymax=99;
 while(DERFC(zb)>zmin && ntry++<ntrymax)zb=zb+1;
 integer ierr;
 rzerowrapper_(z0,zb,x0,zmin,ierr);
 if(ierr || ntry>ntrymax){
  cerr<<"AMSBeta::BetaCorr-E-UnableToCorrectBeta "<<zint<<" "<<z0<<" "<<part<<" "<<ierr<<" "<<ntry<<endl;
  status|=AMSDBc::BAD;
  return 1;
 }
 else     return x0;
}
#endif

//========================================================
int TofRecH::MassRec(TofBetaPar &par,number rig,number charge,number evrig,int isubetac){
//-----
   number Momentum=rig*charge;
   number EMomentum=evrig*rig*rig*charge;
   if(par.Beta<0)Momentum=-Momentum;
   if(fabs(par.Beta)<1.e-10){par.EMass=FLT_MAX;par.Mass=FLT_MAX;}
   else                     {
      number RBeta=(isubetac>0)?par.BetaC:par.Beta;
      number EBeta=(isubetac>0)?par.InvErrBetaC:par.InvErrBeta;
      EBeta=RBeta*RBeta*EBeta;
      number CBeta=1./fabs(RBeta);
      if(fabs(RBeta)>1){CBeta=2.-CBeta;}
      number gamma2=(CBeta!=1)?1./(1.-RBeta*RBeta):FLT_MAX;
      number mass2=Momentum*Momentum*(CBeta*CBeta-1);
      par.Mass=(fabs(RBeta)<1)?sqrt(mass2):-sqrt(mass2);
      par.EMass=par.Mass*sqrt(EMomentum/Momentum*EMomentum/Momentum+gamma2*EBeta/RBeta*gamma2*EBeta/RBeta);
   }
   return 0;
}

//========================================================
int  TofRecH::BetaHLink(TrTrackR* ptrack,TrdTrackR *trdtrack,EcalShowerR *ecalshow){
  if(ptrack){
      for(int ii=0;ii<ev->NCharge();ii++)  {
         if(ev->pCharge(ii)->pBeta()->pTrTrack()==ptrack){ev->pCharge(ii)->setBetaH(ev->NBetaH()-1);break;}
       }
        for(int ii=0;ii<ev->NParticle();ii++){
         if(ev->pParticle(ii)->pTrTrack()==ptrack)       {ev->pParticle(ii)->setBetaH(ev->NBetaH()-1);break;}
       }
   }
   else if(trdtrack){
      for(int ii=0;ii<ev->NParticle();ii++){
         if(ev->pParticle(ii)->pTrdTrack()==trdtrack)    {ev->pParticle(ii)->setBetaH(ev->NBetaH()-1);break;}
       }
   }
   else if(ecalshow){
       for(int ii=0;ii<ev->NParticle();ii++){
         if(ev->pParticle(ii)->pEcalShower()==ecalshow)    {ev->pParticle(ii)->setBetaH(ev->NBetaH()-1);break;}
       }
   }
   return 0;
}

//========================================================
int  TofRecH::BetaHReLink(){

///---Find Candidate Not to attached to particle
    vector<int >fBetaH;
    for(int ii=0;ii<ev->nBetaH();ii++){//candidate
       bool used=0; 
       for(int jj=0;jj<ev->NParticle();jj++){
         if(ev->pParticle(jj)->iBetaH()==ii)used=1;
      }
      if(!used)fBetaH.push_back(ii);
   }

///----Then Assemble
   for(int ii=0;ii<ev->NParticle();ii++){
      if(ev->pParticle(ii)->iBetaH()>=0)continue;
//----Refind  TRD or TRDH or EcalShower
      int ibest=0; 
      number mindis=999999,nowdis,time; 
      AMSPoint pnt; AMSDir dir;
      for(int jj=0;jj<fBetaH.size();jj++){
///---Trd
         if(ev->pParticle(ii)->iTrdTrack()>=0){
            TrdTrackR *ptrd=ev->pParticle(ii)->pTrdTrack();
            ev->pBetaH(fBetaH[jj])->TInterpolate(ptrd->Coo[2],pnt,dir,time);
            nowdis=sqrt((ptrd->Coo[0]-pnt[0])*(ptrd->Coo[0]-pnt[0])+(ptrd->Coo[1]-pnt[1])*(ptrd->Coo[1]-pnt[1]));
            if(nowdis<mindis){mindis=nowdis;ibest=jj;}
         }
///---TrdH
         else if (ev->pParticle(ii)->iTrdHTrack()>=0){
            TrdHTrackR *ptrdh=ev->pParticle(ii)->pTrdHTrack();
            ev->pBetaH(fBetaH[jj])->TInterpolate(ptrdh->Coo[2],pnt,dir,time);
            nowdis=sqrt((ptrdh->Coo[0]-pnt[0])*(ptrdh->Coo[0]-pnt[0])+(ptrdh->Coo[1]-pnt[1])*(ptrdh->Coo[1]-pnt[1]));
            if(nowdis<mindis){mindis=nowdis;ibest=jj;}
         }
//---EcalShow
         if(ev->pParticle(ii)->iEcalShower()>=0){
            EcalShowerR *pshow=ev->pParticle(ii)->pEcalShower();
            number cooshow[3];
            if(pshow->Entry[2]>pshow->Exit[2]){
              cooshow[0]=pshow->Entry[0];cooshow[1]=pshow->Entry[1]; cooshow[2]=pshow->Entry[2];
            }
            else {
              cooshow[0]=pshow->Exit[0];cooshow[1]=pshow->Exit[1];cooshow[2]=pshow->Exit[2];
            } 
            ev->pBetaH(fBetaH[jj])->TInterpolate(cooshow[2],pnt,dir,time);
            nowdis=sqrt((cooshow[0]-pnt[0])*(cooshow[0]-pnt[0])+(cooshow[1]-pnt[1])*(cooshow[1]-pnt[1]));
            if(nowdis<mindis){mindis=nowdis;ibest=jj;}
          }
       }//out betah loop
       if(fBetaH.size()>0){ ev->pParticle(ii)->setBetaH(fBetaH[ibest]);fBetaH.erase(fBetaH.begin()+ibest);}//if no match adding betah one by one
   }

  return 0;
}
