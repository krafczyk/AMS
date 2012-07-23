//  $Id: Tofrec02_ihep.C,v 1.14 2012/07/23 22:50:15 qyan Exp $

// ------------------------------------------------------------
//      AMS TOF recontruction-> /*IHEP TOF cal+rec version*/
//      Not All finish: (BetaH BetaHC)
// ------------------------------------------------------------
//      History
//        Created:       2012-June-10  Q.Yan  qyan@cern.ch
//        Modified:      2012-July-7   Change Recontruction to TOFRawSide+Coo Calib Add
//        Modified:      2012-july-18  Adding lib refit support module
// -----------------------------------------------------------
#ifndef __ROOTSHAREDLIBRARY__
#include "tofrec02.h"
#include "tofdbc02.h"
#include "job.h"
#include "commons.h"
#include <limits.h>
#include "tofsim02.h"
#include "tofhit.h"
#include "cern.h"
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
#endif

vector<TofClusterHR*> TofRecH::tofclh[4];

//--Track
vector<TrTrackR*>     TofRecH::track;
#ifndef __ROOTSHAREDLIBRARY__
vector<AMSTrTrack*>   TofRecH::amstrack;
#endif

vector<TrdTrackR>   TofRecH::trdtrack;
#ifndef __ROOTSHAREDLIBRARY__
vector<AMSTRDTrack*> TofRecH::amstrdtrack;
#endif


//========================================================
int TofRecH::ReBuild(){
   BuildTofClusterH();
   BuildBetaH();  
   return 0;
}

//========================================================
int TofRecH::Init(){
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
  tdvstat=TofAlignManager::GetHead(realdata)->Validate(trun);
#endif
  if(tdvstat!=0)cerr<<"Error TofRecH TDV Par Init Error"<<endl;
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
    track.clear();
    trdtrack.clear();
#ifndef __ROOTSHAREDLIBRARY__
    amstrack.clear();
    amstrdtrack.clear();
#endif
   return 0;
}

//========================================================
int TofRecH::BuildTofClusterH(){
  
  Init(); 
  ClearBuildTofClH();
//---  
  integer i,j,hassid;
  integer idd=0,il=0,ib=0,is=0,idsoft=-1,pattern=0,nraws=0,rawindex[2]={-1};
  integer stat[2];
  uinteger sstatus[2]={0},status=0,recovsid[2];
  integer nadcd[2]={0};
  number sdtm[2]={0},adca[2]={0},timers[2]={0},timer=0,etimer=0,edepa=0,edepd=0,edep=0;
  number adcd[2][TOF2GC::PMTSMX]={{0}};
  vector<number>ltdcw[2];
  vector<number>htdcw[2];
  vector<number>shtdcw[2];
  AMSPoint coo,ecoo;

//---
  vector<TofRawClusterR>tfrawcl;
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
//----   
   tfrawcl.clear();
   TOF2RawCluster *ptrc=(TOF2RawCluster*)AMSEvent::gethead()->getheadC("TOF2RawCluster",0);
   while(ptrc){
      tfrawcl.push_back(TofRawClusterR(ptrc));
      ptrc=ptrc->next(); 
   }

#else
    cont->eraseC();
    vector<pair <integer,integer > >sideid;
    tfraws=ev->TofRawSide();
    for(int i=0;i<tfraws.size();i++){
      sideid.push_back(make_pair<integer,integer>(i,tfraws.at(i).swid));
    }
//--increase id
    sort(tfraws.begin(),tfraws.end(),SideCompare);
    sort(sideid.begin(),sideid.end(),IdCompare);
    tfrawcl=ev->TofRawCluster();
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
//    rawindex[is]=i;
    tfhraws[is]=ev->pTofRawSide(sideid[i].first);
#endif

//--already check get data
     TofSideRec(&tfraws[i],adca[is],nadcd[is],adcd[is],sdtm[is],sstatus[is],ltdcw[is],htdcw[is],shtdcw[is]);

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
         if(((sstatus[0]|sstatus[1])&(TOFDBcN::NOHTRECOVCAD|TOFDBcN::NOMATCHRECOVCAD))>0){
            if((sstatus[0]&(TOFDBcN::NOHTRECOVCAD|TOFDBcN::NOMATCHRECOVCAD))>0)hassid=1;
            else                                                               hassid=0;
            LTRefind(idsoft,0,sdtm,adca,status,ltdcw[1-hassid],hassid);
          }

//---Time Rec
          TimeCooRec(idsoft,sdtm,adca,timers,timer,etimer,coo[TOFGeom::Proj[il]],ecoo[TOFGeom::Proj[il]],status);

//-----other coo
         AMSPoint barco=TOFGeom::GetBarCoo(il,ib);
         coo[1-TOFGeom::Proj[il]] =barco[1-TOFGeom::Proj[il]];
         ecoo[1-TOFGeom::Proj[il]]=(TOFGeom::Sci_w[il][ib]/2.+0.3)/3.;
         coo[2]=barco[2];
         ecoo[2]=TOFGeom::Sci_t[il][ib]/2./3.;//z 3sigma out

//-----energy temp from RawCluster
         for(j=0;j<tfrawcl.size();j++){
           if((tfrawcl[j].Layer-1==il)&&(tfrawcl[j].Bar-1==ib)){
            edepa=tfrawcl[j].edepa;
            edepd=tfrawcl[j].edepd;
            edep=edepa;
            break;
         }
        }
//----  
#ifndef __ROOTSHAREDLIBRARY__
        cont->addnext(new AMSTOFClusterH(sstatus,status,pattern,idsoft,adca,adcd,sdtm,timers,timer,etimer,coo,ecoo,edepa,edepd,tfhraws,tfhraws1));
#else
        cont->addnext(new TofClusterHR(sstatus,status,pattern,idsoft,adca,adcd,sdtm,timers,timer,etimer,coo,ecoo,edepa,edepd,tfhraws));
#endif
      }//at lease one side good build

//--Clear part
      status=pattern=0;
      timer=etimer=edepa=edepd=edep=0;
      for(is=0;is<2;is++){
        tfhraws[is]=0; 
        rawindex[is]=-1;
        ltdcw[is].clear();htdcw[is].clear();shtdcw[is].clear();
        sstatus[is]=0;
        timers[is]=adca[is]=sdtm[is]=0;
        nadcd[is]=0;
        for(int ipm=0;ipm<TOF2GC::PMTSMX;ipm++){adcd[is][ipm]=0;}
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
     for(i=0;i<nadcd;i++){adcd[i]=ptr->adcd[i];}
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
  if(realdata>0){
      for(i=0;i<nlt;i++){
#ifndef __ROOTSHAREDLIBRARY__
         if(tdcch[0]>0&&TofTdcCor::tdccor[crat-1][sslot-1].truech(tdcch[0]-1))ltcor=TofTdcCor::tdccor[crat-1][sslot-1].getcor(ptr->fstdc[i],tdcch[0]-1);
#else
         if(tdcch[0]>0&&TofTdcCorN::tdccor[crat-1][sslot-1].truech(tdcch[0]-1))ltcor=TofTdcCorN::tdccor[crat-1][sslot-1].getcor(ptr->fstdc[i],tdcch[0]-1);
#endif
         ltdch[i]-=ltcor;
      }
//---FT
      for(i=0;i<nft;i++){
#ifndef __ROOTSHAREDLIBRARY__
         if(tdcch[1]>0&&TofTdcCor::tdccor[crat-1][sslot-1].truech(tdcch[1]-1))ftcor=TofTdcCor::tdccor[crat-1][sslot-1].getcor(ptr->fftdc[i],tdcch[1]-1);
#else
         if(tdcch[1]>0&&TofTdcCorN::tdccor[crat-1][sslot-1].truech(tdcch[1]-1))ftcor=TofTdcCorN::tdccor[crat-1][sslot-1].getcor(ptr->fftdc[i],tdcch[1]-1);
#endif
         ftdch[i]-=ftcor;
      }
//---HT
      for(i=0;i<nht;i++){
#ifndef __ROOTSHAREDLIBRARY__
         if(tdcch[2]>0&&TofTdcCor::tdccor[crat-1][sslot-1].truech(tdcch[2]-1))htcor=TofTdcCor::tdccor[crat-1][sslot-1].getcor(ptr->fsumht[i],tdcch[2]-1);
#else
         if(tdcch[2]>0&&TofTdcCorN::tdccor[crat-1][sslot-1].truech(tdcch[2]-1))htcor=TofTdcCorN::tdccor[crat-1][sslot-1].getcor(ptr->fsumht[i],tdcch[2]-1);
#endif
         htdch[i]-=htcor;
      }
//---SHT
      for(i=0;i<nsht;i++){
#ifndef __ROOTSHAREDLIBRARY__
         if(tdcch[3]>0&&TofTdcCor::tdccor[crat-1][sslot-1].truech(tdcch[3]-1))shtcor=TofTdcCor::tdccor[crat-1][sslot-1].getcor(ptr->fsumsht[i],tdcch[3]-1);
#else
         if(tdcch[3]>0&&TofTdcCorN::tdccor[crat-1][sslot-1].truech(tdcch[3]-1))shtcor=TofTdcCorN::tdccor[crat-1][sslot-1].getcor(ptr->fsumsht[i],tdcch[3]-1);
#endif
         shtdch[i]-=shtcor;
      }
   }

//---Get Ref Tdc
      for(i=0;i<nft;i++)  ftdc[i]=ftdch[i]*TofRecPar::Tdcbin;//FTtime TDCch->ns
      number fttm=ftdc[nft-1];//choose last because this triggers LVL1 
      for(i=0;i<nlt;i++)  ltdc[i]=fttm-ltdch[i]*TofRecPar::Tdcbin;//Rel.LTtime(+ means befor FTtime)(+TDCch->ns)
      for(i=0;i<nht;i++)  htdc[i]=fttm-htdch[i]*TofRecPar::Tdcbin;//Rel.SumHTtime +TDCch->ns
      for(i=0;i<nsht;i++)shtdc[i]=fttm-shtdch[i]*TofRecPar::Tdcbin;//Rel.SumSHTtime +TDCch->ns

//---Put to calidate
      ltdcw.clear();htdcw.clear();shtdcw.clear();
      for(i=0;i<nlt;i++){//notTooYoung(>40ns befFT), notTooOld(<300 befFT)
         if((ltdc[i]> TofRecPar::FTgate[0])&&(ltdc[i]<TofRecPar::FTgate[1])){
            ltdcw.push_back(ltdc[i]);
         }
       }
     for(i=0;i<nht;i++){//notTooYoung(>40ns befFT), notTooOld(<300 befFT)
         if((htdc[i]> TofRecPar::FTgate[0])&& (htdc[i]<TofRecPar::FTgate[1])){
           htdcw.push_back(htdc[i]);
         }
       }
     for(i=0;i<nsht;i++){//notTooYoung(>40ns befFT), notTooOld(<300 befFT)
         if((shtdc[i]> TofRecPar::FTgate[0])&& (shtdc[i]<TofRecPar::FTgate[1])){
           shtdcw.push_back(shtdc[i]);
         }
      }

//----Set Bad Status
       if(ltdcw.size()<1){sstatus|=TOFDBcN::NOWINDOWLT;}
       if(htdcw.size()<1){sstatus|=TOFDBcN::NOWINDOWHT;}
       if(ltdcw.size()>1){sstatus|=TOFDBcN::LTMANY;}
       if(htdcw.size()>1){sstatus|=TOFDBcN::HTMANY;}//HT MANY bad(due to 300ns block)
       if((sstatus&(TOFDBcN::NOWINDOWLT|TOFDBcN::HTMANY|TOFDBcN::NOADC)>0)){sstatus|=TOFDBcN::BAD;}

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
         number minhldt=999999,dt=0;
          //----if no HT and no SHT try to narrow
         if((htdcw.size()==0)&&(shtdcw.size()==0)){
           for(i=0;i<ltdcw.size();i++){//tight guide
             dt=ltdcw[i]-(TofRecPar::FTgate2[0]+TofRecPar::FTgate2[1])/2.;//using TF MPV gate to choose
             if(fabs(dt)<minhldt){sdtm=ltdcw[i];minhldt=fabs(dt);}
             if((ltdcw[i]>TofRecPar::FTgate2[0])&&(ltdcw[i]<TofRecPar::FTgate2[1])){ltok++;}
            }
            if(ltok!=1){sstatus=(TOFDBcN::BAD|TOFDBcN::NOHTRECOVCAD);ltok=0;}
          }

          //---Using Ht Sht finding nearest as default match check
        else {
         //---First using ht match
         if(htdcw.size()>0){
           for(i=0;i<ltdcw.size();i++){//real impossible two LT match HT due to 30ns LT lock
             dt=ltdcw[i]-htdcw[0];//lt must >ht time
             if(fabs(dt-TofRecPar::LHMPV)<minhldt){sdtm=ltdcw[i];minhldt=fabs(dt-TofRecPar::LHMPV);}
             if((dt>TofRecPar::LHgate[0])&&(dt<TofRecPar::LHgate[1])){ltok++;}//find LT should match in window
            }
          }
         //---Otherwise try using sht
          if((!ltok)&&(shtdcw.size()>0)){
           for(i=0;i<ltdcw.size();i++){//to find LT should match in windows
             dt=ltdcw[i]-shtdcw[0];//lt must >sht time 
             if(fabs(dt-TofRecPar::LHMPV)<minhldt){sdtm=ltdcw[i];minhldt=fabs(dt-TofRecPar::LHMPV);}
             if((dt>TofRecPar::LHgate[0])&&(dt<TofRecPar::LHgate[1])){ ltok++;}
            }
          }
         if(!ltok)sstatus|=(TOFDBcN::BAD|TOFDBcN::NOMATCHRECOVCAD);
       }
    }

   return 0;
}

//========================================================
int TofRecH::LTRefind(int idsoft,number trlcoo,number sdtm[2],number adca[2],uinteger &status, vector<number>&ltdcw,int hassid){

    if((adca[0]<=0)||(adca[1]<=0)||ltdcw.size()<=0)return -1;

//---Find Best LT
    number sdtm1[2],tms[2],tm,etm,lcoo,elcoo,mindis=999999;
    uinteger ustatus=0;
    int iminlt=-1;
    sdtm1[hassid]=sdtm[hassid];
    for(int i=0;i<ltdcw.size();i++){
       sdtm1[1-hassid]=ltdcw[i];
       ustatus=0; 
       TimeCooRec(idsoft,sdtm1,adca,tms,tm,etm,lcoo,elcoo,ustatus);
       if(fabs(lcoo-trlcoo)<mindis){mindis=fabs(lcoo-trlcoo);iminlt=i;}
    }

//---Set Par
    TofRecPar::IdCovert(idsoft);
    if(mindis<TOFGeom::Sci_l[TofRecPar::iLay][TofRecPar::iBar]/2.){
      sdtm[1-hassid]=ltdcw[iminlt];
      status|=(TOFDBcN::LTREFIND|TOFDBcN::RECOVERED);
    }
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
    if((status&TOFDBcN::LTREFIND)>0)etm=3*TofRecPar::GetTimeSigma(idsoft);
    else                            etm=TofRecPar::GetTimeSigma(idsoft);
    

//--coo
    number tmsc[2];
    tmsc[0]=tms[0]-2*dsl/pow(adca[0],index);//coo compensate
    tmsc[1]=tms[1]+2*dsl/pow(adca[1],index);//coo compensate
    lcoo=0.5*(tmsc[0]-tmsc[1])*vel;
    if((status&TOFDBcN::LTREFIND)>0)elcoo=3*TofRecPar::GetCooSigma(idsoft);
    else                            elcoo=TofRecPar::GetCooSigma(idsoft);
    return 0;
}

//========================================================
int TofRecH::EdepRec(){
  return 0;
}

//========================================================
int TofRecH::BuildBetaH(int mode){
//---
    Init();
    ClearBuildBetaH();
//---
   
    integer i;
    AMSPoint tkcoo,tfcoo,tfecoo;
    number theta,phi,sleng,mscoo,mlcoo,dscoo,dlcoo,mintm,barw;
    int iscoo[4]={1,0,0,1},nowbar,prebar,nextbar,pretm,nexttm,tminbar;//short coo Trans
    bool leftf=0,rightf=0;
    number Beta,InvErrBeta,BetaC,InvErrBetaC,Chi2;
    int found=0;//
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
//----

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
   
   for(int iktr=0;iktr<2;iktr++){
//---if trdtrack
     if(iktr==1){ntracks=trdtrack.size();}
//----
     for(int itr=0;itr<ntracks;itr++){
       uinteger status=(itr==0)?0:TOFDBcN::TRDTRACK;

#if (defined _PGTRACK_) || (defined __ROOTSHAREDLIBRARY__)
       TrTrackR *ptrack;  TrTrackR *usetr=0;
       if(iktr==0){ptrack=track.at(itr);usetr=ptrack;}
#if (defined _PGTRACK_)
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
      TrdTrackR *usetrd=0;
      if(iktr==1)usetrd=ev->pTrdTrack(itr);
#else
      AMSTRDTrack *usetrd=0;
      if(iktr==1)usetrd=amstrdtrack.at(itr);
#endif
//--- 
         TofClusterHR *phit[4]={0}; int pattern[4]={0};number len[4]={0};number tklcoo[4]={1000};
         number cres[4][2]={{1000}}; 
         for(int ilay=0;ilay<4;ilay++){
            BetaFindTOFCl(ptrack,ilay,&phit[ilay],len[ilay],tklcoo[ilay],cres[ilay],pattern[ilay]);
        }
//----then select+Fit
       int xylay[2]={0,0};//x y direction check
       int udlay[2]={0,0};//up and down check
       for(int ilay=0;ilay<4;ilay++){
          if(phit[ilay]==0)continue;
          if(ilay==0||ilay==1){if((pattern[ilay]%10==4))udlay[0]++;}//no mising
          else                {if((pattern[ilay]%10==4))udlay[1]++;}//no mising
          if(ilay==0||ilay==3){xylay[0]++;}
          else                {xylay[1]++;}
        }
//-----then fit+Check
       if((xylay[0]>=1)&&(xylay[1]>=1)&&(udlay[0]>=1)&&(udlay[1]>=1)){//pattern found
          if((xylay[0]+xylay[1]>=TofRecPar::BetaHMinL[0])&&(udlay[0]+udlay[1]>TofRecPar::BetaHMinL[1])){
           betapar.Init();//first initial
//---first to recover
           TRecover(phit,tklcoo);
           BetaFitC(phit,cres,pattern,betapar,1);
           BetaFitT(phit,len,pattern,betapar,1);
           BetaFitCheck(betapar);
           betapar.Status|=status;
#ifndef __ROOTSHAREDLIBRARY__
          cont->addnext(new AMSBetaH(phit,dynamic_cast<AMSTrTrack *>(usetr),usetrd,betapar));
#else
          cont->addnext(new  BetaHR(phit,usetr,usetrd,betapar));
           BetaHLink(usetr,usetrd);
#endif
           found++;
          }
        }
        
       if(iktr==1)delete ptrack;
     }//ntrack;
    if(found>0)break;//for track or trd has found all
  }//2 type

  delete cont;
  return 0;

}
//========================================================
#if (defined _PGTRACK_) || (defined __ROOTSHAREDLIBRARY__)
int  TofRecH::BetaFindTOFCl(TrTrackR *ptrack,   int ilay,TofClusterHR **tfhit,number &tklen,number &tklcoo,number cres[2],int &pattern){
#else
int  TofRecH::BetaFindTOFCl(AMSTrTrack *ptrack,int ilay,TofClusterHR **tfhit,number &tklen,number &tklcoo,number cres[2],int &pattern){
#endif
//---init
    int nowbar=0,prebar=-1000,nextbar=-1000,tminbar=0,layhit=0;
    uinteger tfhstat;
    number mscoo=1000,mlcoo=100,mintm=1000,barw=0,dscoo=1000,dlcoo=1000,tfhtime;
    number theta,phi,sleng;
    bool leftf,rightf;
    int iscoo=1-TOFGeom::Proj[ilay];//short coo y x x y
    (*tfhit)=0;tklen=0;pattern=0;cres[0]=1000,cres[1]=1000;
//---  
    for(int i=0;i<tofclh[ilay].size();i++){
       AMSPoint tfcoo (tofclh[ilay].at(i)->Coo);
       AMSPoint tfecoo(tofclh[ilay].at(i)->ECoo);
       nowbar=         tofclh[ilay].at(i)->Bar;
       tfhstat=        tofclh[ilay].at(i)->Status;
       tfhtime=        tofclh[ilay].at(i)->Time;
       pattern=        tofclh[ilay].at(i)->Pattern%10;
       if(i==tofclh[ilay].size()-1)nextbar=-1000;
       else                        nextbar=tofclh[ilay].at(i+1)->Bar;

//---Intepolate 
       AMSDir tkdir(0,0,1);AMSPoint tkcoo;   
#if (defined _PGTRACK_) || (! defined __ROOTSHAREDLIBRARY__)
       ptrack->interpolate(tfcoo,tkdir,tkcoo,theta,phi,sleng);
#else
      return -1;
#endif

//---time min bar not used now
       if((tfhstat&TOFDBcN::BADTIME)==0){ //not bad time
         if(mintm==1000||tfhtime<mintm){
           mintm=tfhtime;//find earliest(tm) in this layer
           tminbar=nowbar;//tag earliest bar
          }
       }

      dscoo=fabs(tkcoo[iscoo]-tfcoo[iscoo]);
      if(dscoo<mscoo){//first compare Trans
          mscoo=dscoo;
          if(mscoo<TofRecPar::BetaHReg[0]*tfecoo[iscoo]){//0.5cm offset tof+tkhit match in same counter 3sigma S flag
             (*tfhit)=tofclh[ilay].at(i);tklen=sleng;
             dlcoo=fabs(tkcoo[1-iscoo]-tfcoo[1-iscoo]);
             tklcoo=tkcoo[1-iscoo];
//------------
             for(int ip=0;ip<2;ip++){
                cres[ip]=tkcoo[ip]-tfcoo[ip];
                if(((tfhstat&TOFDBcN::BADTCOO)>0)&&(ip==1-iscoo)){cres[ip]=TOFGeom::Sci_l[ilay][nowbar]/2.;};
              }
             if(TofRecPar::BetaHLMatch==0||dlcoo<TofRecPar::BetaHReg[1]*tfecoo[1-iscoo]){
               if((pattern%10)==3)pattern=4;
             }//3sigma L flag
             leftf=0;rightf=0;
//----previous bar
             if(prebar>=0)       {leftf=1;}//left counter fire
             else                {leftf=0;}
             if(leftf)pattern+=(nowbar-prebar)*100;
//----next bar
             if(nextbar>=0)       {rightf=1;}//right counter fire
             else                 {rightf=0;}
             if(rightf)pattern+=(nextbar-nowbar)*10;
//---
         }
    }
   prebar=nowbar;
   layhit++;
  }//end tof loop
   pattern+=1000*layhit;

   return 0;
}


//======================================================== 
int TofRecH::TRecover(TofClusterHR *tfhit[4],number tklcoo[4]){
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
   par.Chi2C=chisl/nhitl;
//  par.Chi2C=sqrt((chisxy[0]+chisxy[1])/(nhitxy[0]+nhitxy[1]));
  return 0;
}

//========================================================
int TofRecH::BetaFitT(TofClusterHR *tfhit[4],number lenr[4],int pattern[4], TofBetaPar &par,int mode){
//---
  number time [4];
  number etime[4];
  number len  [4];

  int selhit=0;
//---select+Fit
  for(int ilay=0;ilay<4;ilay++){
     par.Pattern[ilay]=pattern[ilay];
     if(tfhit[ilay]==0)par.Pattern[ilay]=0;
     if(par.Pattern[ilay]!=0)par.SumHit++;
     if(par.Pattern[ilay]%10!=4)continue;
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

//--then Fit
   BetaFitT(time,etime,len,selhit,par,mode);
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
  etm=3*TofRecPar::GetTimeSigma(idsoft)*sqrt(2.);//1sid sqrt(2)
  status|=TOFDBcN::RECOVERED;
  return 0;
}

//========================================================
int TofRecH::BetaFitT(number time[],number etime[],number len[],const int nhits,TofBetaPar &par,int mode){
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
     ey[i]=(mode==0)?0.15:etime[i];//0.15ns if same weight
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
   if(fabs(Beta)>2){
     static int nerr=0;
     if(nhits>2 && nerr++<100){
       cout<<"<--- AMSBeta::SimpleFit BetaOut-OfRange "<<Beta<<endl;
       cout<<"TimeRec=";for(i=0;i<nhits;i++)cout<<y[i]<<" ";cout<<endl;
     }
     if(Beta>0)Beta=2;
     else Beta=-2;
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
   InvErrBetaC=0;
   BetaC=0;
#endif
//--now ready to par
  par.Status|=status;
  par.SetFitPar(Beta,BetaC,InvErrBeta,InvErrBetaC,Chi2,T0);
  return 0;
//---
}

//========================================================
int  TofRecH::BetaFitCheck(TofBetaPar &par){
/* if(par.Beta
 else {
    par.Beta
 }
*/
  return 0;
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
int  TofRecH::BetaHLink(TrTrackR* ptrack,TrdTrackR *trdtrack){
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
   return 0;
}

//========================================================
