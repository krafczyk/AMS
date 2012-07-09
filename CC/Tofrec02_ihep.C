//  $Id: Tofrec02_ihep.C,v 1.5 2012/07/09 22:25:23 qyan Exp $

// ------------------------------------------------------------
//      AMS TOF recontruction-> /*IHEP TOF cal+rec version*/
//      Not All finish: (BetaH BetaHC)
// ------------------------------------------------------------
//      History
//        Created:       2012-June-10  Q.Yan  qyan@cern.ch
//        Modified:      2012-July-7   Change Recontruction to TOFRawSide+Coo Calib Add
// -----------------------------------------------------------
#ifndef __ROOTSHAREDLIBRARY__
#include "tofsim02.h"
#include "tofrec02.h"
#include "Tofdbc.h"
#include "tofdbc02.h"
#include "tofanticonst.h"
#include "job.h"
#include "commons.h"
#include <limits.h>
#include "amsgobj.h"
#include "extC.h"
#include "upool.h"
#include "ntuple.h"
#include "trigger302.h"
#include "event.h"
#include "trdrec.h"
#include "charge.h"
#ifdef _PGTRACK_
#include "tkpatt.h"
#include "TrRecon.h"
#endif
#endif
#include <math.h>
#include "cern.h"
#include "Tofrec02_ihep.h"

#ifndef __ROOTSHAREDLIBRARY__
extern "C" void rzerowrapper_(number & z0, number & zb, number & x0, number & zmin,int & ierr);
#endif
//========================================================
TofBetaPar TofRecH::betapar;

#ifndef __ROOTSHAREDLIBRARY__
//========================================================
int TofRecH::BuildTofClusterH(){

//---  
  cout<<"begin tofclusterh"<<endl;
  integer i,j,hassid;
  integer idd=0,il=0,ib=0,is=0,idsoft=-1,pattern=0,nraws=0;
  int16u stat[2];
  uinteger sstatus[2]={0},status=0,recovsid[2];
  integer nadcd[2]={0};
  number sdtm[2]={0},adca[2]={0},timers[2]={0},timer=0,etimer=0,lcoo,scoo,elcoo,escoo,edepa,edepd,edep;
  number adcd[2][TOF2GC::PMTSMX]={{0}};
  vector<number>ltdcw[2];
  vector<number>htdcw[2];
  vector<number>shtdcw[2];
  AMSPoint coo,ecoo;
 
//-----
  TOF2RawSide *tfraw[2]={0};
  TOF2RawSide *ptr=(TOF2RawSide*)AMSEvent::gethead()
                                    ->getheadC("TOF2RawSide",0,1);
  
  while(ptr){
     idd=ptr->getsid();//LBBS
     il=idd/10/100-1;
     ib=idd/10%100-1;
     is=idd%10-1; 
     stat[is]=ptr->getstat();
     if(stat[is]%10!=0){ptr=ptr->next();continue;}//validation status(FTtime is required)
     if(!TOF2Brcal::scbrcal[il][ib].SideOK(is)||!TOFBPeds::scbrped[il][ib].PedAchOK(is)){ptr=ptr->next();continue;}//(LT&QAnode&(sumHT|noMatch) +Peds
//--already check get data
     tfraw[is]=ptr;nraws++;
     TofSideRec(ptr,adca[is],nadcd[is],adcd[is],sdtm[is],sstatus[is],ltdcw[is],htdcw[is],shtdcw[is]); 

//--different LBB// combine to one bar
    if((ptr->next()==0)||(ptr->next()->getsid()/10!=ptr->getsid()/10)){
      idsoft=il*1000+ib*100;
      if(((sstatus[0]&TOFDBcN::BAD)>0)||(tfraw[0]==0))status|=TOFDBcN::BADN;
      else                                            pattern|=TOFDBcN::SIDEN;
      if(((sstatus[1]&TOFDBcN::BAD)>0)||(tfraw[1]==0))status|=TOFDBcN::BADP;
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
        TimeCooRec(idsoft,sdtm,adca,timers,timer,etimer,coo[TOFGeom::Proj[il]],ecoo[TOFGeom::Proj[il]],status,0,0);

//-----other coo
       AMSPoint barco=TOFGeom::GetBarCoo(il,ib);
       coo[1-TOFGeom::Proj[il]] =barco[1-TOFGeom::Proj[il]];
       ecoo[1-TOFGeom::Proj[il]]=(TOFGeom::Sci_w[il][ib]/2.+0.3)/3.;
       coo[2]=barco[2];
       ecoo[2]=TOFGeom::Sci_t[il][ib]/2./3.;//z 3sigma out
//-----energy temp from RawCluster
       TOF2RawCluster *ptrc=(TOF2RawCluster*)AMSEvent::gethead()->getheadC("TOF2RawCluster",0);
       while(ptrc){
         if((ptrc->getntof()-1==il)&&(ptrc->getplane()-1==ib)){
           edepa=ptrc->getedepa();//temporaly
           edepd=ptrc->getedepd();//temp
           edep=edepa;            //temp
           break;
         }
       }
//--      
       AMSEvent::gethead()->addnext(AMSID("AMSTOFClusterH",il), new
         AMSTOFClusterH(sstatus,status,pattern,idsoft,adca,adcd,sdtm,timers,timer,etimer,coo,ecoo,edepa,edepd,edep,tfraw,nraws,ltdcw,htdcw,shtdcw));
      }
//--clear part
      nraws=status=pattern=0;
      timer=etimer=edepa=edepd=edep=0;
      for(is=0;is<2;is++){
        tfraw[is]=0; 
        ltdcw[is].clear();htdcw[is].clear();shtdcw[is].clear();
        sstatus[i]=0;
        timers[is]=adca[is]=sdtm[is]=0;
        nadcd[i]=0;
        for(int ipm=0;ipm<TOF2GC::PMTSMX;ipm++){adcd[is][ipm]=0;}
      }
   }//end one bar

   ptr=ptr->next();    
  }
     
 return 0;
}

//========================================================Rec Side
int TofRecH::TofSideRec(TOF2RawSide *ptr,number &adca, integer &nadcd,number adcd[],number &sdtm,uinteger &sstatus,
                        vector<number>&ltdcw, vector<number>&htdcw, vector<number>&shtdcw){
//---
     integer i,j;
     integer ltok=0;
     integer ftdch[TOF2GC::SCTHMX1],ltdch[TOF2GC::SCTHMX3],htdch[TOF2GC::SCTHMX2],shtdch[TOF2GC::SCTHMX2];
     number  ftdc[TOF2GC::SCTHMX1], ltdc[TOF2GC::SCTHMX3], htdc[TOF2GC::SCTHMX2], shtdc[TOF2GC::SCTHMX2];
     geant radcd[TOF2GC::PMTSMX];
//---init
     sstatus=0;

//----Get adca 
     adca=ptr->getadca();
     nadcd=ptr->getadcd(radcd);
     for(i=0;i<nadcd;i++){adcd[i]=radcd[i];}
     if(adca<=0)  {sstatus|=TOFDBcN::NOADC;}
     if(adca>=TofRecPar::PUXMXCH){adca=TofRecPar::PUXMXCH;sstatus|=TOFDBcN::AOVERFLOW;}//now just anode 

//---Get Tdc
     integer nft=ptr->getftdc(ftdch);//FT
     integer nlt=ptr->getstdc(ltdch);//LT
     integer nht=ptr->getsumh(htdch);//SumHT
     integer nsht=ptr->getsumsh(shtdch);//SumSHT
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
int TofRecH::LTRefind(int idsoft,number trlcoo,number sdtm[2],number adca[2],uinteger &status, vector<number>ltdcw,int hassid){

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
int TofRecH::TimeCooRec(int idsoft,number sdtm[], number adca[],number tms[2],number &tm,number &etm,number &lcoo,number &elcoo,uinteger &status,int run,int force){

    static int errcount=0;
    if(TofTAlignPar::Head==0){
       if(errcount<100)cerr<<"Error TofTAlignPar Head not Initial !!!!"<<endl;
    }
    TofTAlignPar *TPar=TofTAlignPar::GetHead();
    if(force==1){
       TPar->ReadTDV(run,AMSJob::gethead()->isRealData());
    }
   else if(!TPar->Isload){
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

    AMSPoint tkcoo,tfcoo,tfecoo;
    number theta,phi,sleng,mscoo,mlcoo,dscoo,dlcoo,mintm,barw;
    int iscoo[4]={1,0,0,1},nowbar,prebar,nextbar,pretm,nexttm,tminbar;//short coo Trans
    bool leftf=0,rightf=0;
    number Beta,InvErrBeta,BetaC,InvErrBetaC,Chi2;
//----
    int found=0;//
    int ntr=0;
    int ntracks= AMSEvent::gethead()->getC(AMSID("AMSTrTrack",0))->getnelem();
    int ntrackTs=AMSEvent::gethead()->getC(AMSID("AMSTRDTrack",0))->getnelem();
    AMSTrTrack  *ptrack= (AMSTrTrack*)  AMSEvent::gethead()->getheadC("AMSTrTrack",0,1);
    AMSTRDTrack *ptrackT=(AMSTRDTrack *)AMSEvent::gethead()->getheadC("AMSTRDTrack",0,1);

//---build now
   for(int iktr=0;iktr<2;iktr++){
      if(iktr==1)ntracks=ntrackTs;
      for(int itr=0;itr<ntracks;itr++){
         uinteger status=0;
         AMSTrTrack *track=0;AMSTRDTrack *trdtrack=0;AMSCharge *amscharge=0;
         if(iktr==0){track=ptrack;trdtrack=0;}
         else if(iktr==1){
           if(!BETAFITFFKEY.FullReco && ptrackT->checkstatus(AMSDBc::USED)){ptrackT=ptrackT->next();continue;}
           track = new AMSTrTrack(ptrackT->gettheta(),ptrackT->getphi(), ptrackT->getcoo());
           status|=AMSDBc::TRDTRACK;
           trdtrack=ptrackT;
         }
         AMSTOFClusterH* phit[4]={0};int pattern[4]={0};number len[4]={0};number tklcoo[4]={1000};
         number cres[4][2]={{1000}};
//-----first find
         for(int ilay=0;ilay<4;ilay++){
           BetaFindTOFCl(track,ilay,&phit[ilay],len[ilay],tklcoo[ilay],cres[ilay],pattern[ilay],BETAFITFFKEY.HSearchLMatch);
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
          if((xylay[0]+xylay[1]>=BETAFITFFKEY.HMinLayer[0])&&(udlay[0]+udlay[1]>=BETAFITFFKEY.HMinLayer[1])){
           betapar.Init();//first initial
//---first to recover
           TRecover(phit,tklcoo);
           BetaFitC(phit,cres,pattern,betapar,1);
           BetaFitT(phit,len,pattern,betapar,1);
           BetaFitCheck(betapar);
           AMSEvent::gethead()->addnext(AMSID("AMSBetaH",0),new AMSBetaH(status,phit,track,trdtrack,amscharge,betapar));
           found++;
          }
        }
       if(iktr==0)     {ptrack=ptrack->next();}//track
       else if(iktr==1){delete track;ptrackT=ptrackT->next();}//trd make track delete
     }
    if(found>0)break;//for track or trd has found all
  }
  return 0;    
}

//========================================================
int  TofRecH::BetaFindTOFCl(AMSTrTrack *ptrack,int ilay,AMSTOFClusterH** tfhit,number &tklen,number &tklcoo,number cres[2],int &pattern,int mode){
//---init
    int nowbar=0,prebar=-1000,nextbar=-1000,tminbar=0,layhit=0;
    number mscoo=1000,mlcoo=100,mintm=0,barw=0,dscoo=1000,dlcoo=1000;
    number theta,phi,sleng;
    bool leftf,rightf;
    AMSPoint tfcoo,tfecoo;
    int iscoo=1-TOFGeom::Proj[ilay];//short coo y x x y
    (*tfhit)=0;tklen=0;pattern=0;cres[0]=1000,cres[1]=1000;
//---
    AMSTOFClusterH * tofhit=(AMSTOFClusterH*)AMSEvent::gethead()->getheadC("AMSTOFClusterH",ilay,1);
    for( ; tofhit;tofhit=tofhit->next()) {
       AMSDir tkdir(0,0,1);AMSPoint tkcoo;
       tfcoo=tofhit->getcoo();
       tfecoo=tofhit->getecoo();
       nowbar=tofhit->getbarid()/100%10;
//---
       ptrack->interpolate(tfcoo,tkdir,tkcoo,theta,phi,sleng);
       if(ptrack->getpattern()<0){//trd track finding
           //ecoo[0]+=1.;  // add one cm fot trd/tof
	   //ecoo[1]+=1.;  // add one cm fot trd/tof
       }
//---time min bar not used now
       if((tofhit->getstatus()&TOFDBcN::BADTIME)==0){ //not bad time
	 if(mintm==0||tofhit->gettime()<mintm){
           mintm=tofhit->gettime();//find earliest(tm) in this layer
	   tminbar=nowbar;//tag earliest bar
	  }
       }
      dscoo=fabs(tkcoo[iscoo]-tfcoo[iscoo]);
      if(dscoo<mscoo){//first compare Trans
          mscoo=dscoo;
          if(mscoo<BETAFITFFKEY.HSearchReg[0]*tfecoo[iscoo]){//0.5cm offset tof+tkhit match in same counter 3sigma S flag
             (*tfhit)=tofhit;tklen=sleng;
             pattern=(*tfhit)->getpattern()%10;//two side information
	     dlcoo=fabs(tkcoo[1-iscoo]-tfcoo[1-iscoo]);
             tklcoo=tkcoo[1-iscoo];
//------------
             for(int ip=0;ip<2;ip++){
                cres[ip]=tkcoo[ip]-tfcoo[ip];
                if((((*tfhit)->getstatus()&TOFDBcN::BADTCOO)>0)&&(ip==1-iscoo)){cres[ip]=TOFGeom::Sci_l[ilay][nowbar]/2.;};
              }
             if(mode==0||dlcoo<BETAFITFFKEY.HSearchReg[1]*tfecoo[1-iscoo]){
               if((pattern%10)==3)pattern=4;
             }//3sigma L flag
	     leftf=0;rightf=0;
	     if(prebar>=0)       {leftf=1;}//left counter fire
	     else                {leftf=0;}
             if(leftf)pattern+=(nowbar-prebar)*100;
	     if((tofhit->next()==0)||(tofhit->next()->getbarid()/1000%10!=ilay))nextbar=-1000;//next not same lay
	     else                                   nextbar=tofhit->next()->getbarid()/100%10;
	     if(nextbar>=0)       {rightf=1;}//right counter fire
	     else                 {rightf=0;}
             if(rightf)pattern+=(nextbar-nowbar)*10;
	 }
    }
   prebar=nowbar;
   layhit++;
  }//end tof loop
   pattern+=1000*layhit;

   return 0;
}

//======================================================== 
int TofRecH::TRecover(AMSTOFClusterH *tofclh[4],number tklcoo[4]){
  for(int ilay=0;ilay<4;ilay++){
    if(!tofclh[ilay])continue;
    if((tofclh[ilay]->getstatus()&TOFDBcN::BADTIME)>0){
      int hassid=((tofclh[ilay]->getstatus()&TOFDBcN::BADN)>0)?1:0;//NBAD try PSIDE
      TRecover(tofclh[ilay]->_idsoft,tklcoo[ilay],tofclh[ilay]->_timers,tofclh[ilay]->_timer,tofclh[ilay]->_etimer,tofclh[ilay]->_status,hassid);
    }
  }
  return 0;
}

//========================================================
int TofRecH::BetaFitC(AMSTOFClusterH *tofclh[4],number res[4][2],int pattern[4], TofBetaPar &par,int mode){
   int iscoo[4]={1,0,0,1};
   AMSPoint tfecoo;
   int nhitxy[2]={0},nhitl=0;
   number chisxy[2]={0};
   number chisl=0;
   for(int ilay=0;ilay<4;ilay++){//temp exclude missing time measument lay S side
     for(int is=0;is<2;is++){par.CResidual[ilay][is]=res[ilay][is];}
     if(tofclh[ilay]){
       tfecoo=tofclh[ilay]->getecoo();
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
int TofRecH::BetaFitT(AMSTOFClusterH *tofclh[4],number lenr[4],int pattern[4], TofBetaPar &par,int mode){
//---
  number time [4];
  number etime[4];
  number len  [4];

  int selhit=0;
//---select+Fit
  for(int ilay=0;ilay<4;ilay++){
     par.Pattern[ilay]=pattern[ilay];
     if(tofclh[ilay]==0)par.Pattern[ilay]=0;
     if(par.Pattern[ilay]!=0)par.SumHit++;
     if(par.Pattern[ilay]%10!=4)continue;
//--fill to par
     par.Time[ilay]= tofclh[ilay]->gettime();//ns
     par.ETime[ilay]=tofclh[ilay]->getetime();//ns  
     par.Len[ilay]=  lenr[ilay];
//---fill to arr
     time [selhit]=par.Time[ilay];//ns
     etime[selhit]=par.ETime[ilay];//ns
     len  [selhit]=par.Len[ilay];
     selhit++;
   }

  par.UseHit=selhit;
  if(selhit<2||selhit>=5){par.Status|=AMSDBc::BAD;return -1;}

//--then Fit
   BetaFitT(time,etime,len,selhit,par,mode);
   par.CalFitRes();
   return 0;
  
}
#endif

//========================================================
int TofRecH::TRecover(int idsoft,number tklcoo,number tms[2],number &tm,number &etm,uinteger &status,int hassid){

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
#endif
//--now ready to par
  par.Status|=status;
  par.SetFitPar(Beta,BetaC,InvErrBeta,InvErrBetaC,Chi2,T0);
  return 0;
//---
}

#ifndef __ROOTSHAREDLIBRARY__
//========================================================
int  TofRecH::BetaFitCheck(TofBetaPar &par){
 if(BETAFITFFKEY.HBetaCheck==0)return 0;
/* if(par.Beta
 else {
    par.Beta
 }
*/
  return 0;
}

//========================================================
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

#ifndef __ROOTSHAREDLIBRARY__
//========================================================
void AMSTOFClusterH::_writeEl(){
#ifdef __WRITEROOT__
    AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif
}

//========================================================
void AMSTOFClusterH::_copyEl(){
#ifdef __WRITEROOT__
 if(PointerNotSet())return;
 TofClusterHR & ptr = AMSJob::gethead()->getntuple()->Get_evroot02()->TofClusterH(_vpos);
   for(int i=0;i<2;i++){
      if(_tfraws[i])ptr.fTofRawSide.push_back(_tfraws[i]->GetClonePointer());
    }
#endif
}
//=======================================================
integer AMSTOFClusterH::Out(integer status){
static integer init=0;
static integer WriteAll=1;
if(init == 0){
 init=1;
 integer ntrig=AMSJob::gethead()->gettriggerN();
 for(int n=0;n<ntrig;n++){
   if(strcmp("AMSTOFClusterH",AMSJob::gethead()->gettriggerC(n))==0){
     WriteAll=1;
     break;
   }
 }
} 
return (WriteAll || status);
}
 
//======================================================
void AMSBetaH::_writeEl(){
#ifdef __WRITEROOT__
    AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif
}

//======================================================
void AMSBetaH::_copyEl(){
#ifdef __WRITEROOT__
 if(PointerNotSet())return;
    BetaHR & ptr=AMSJob::gethead()->getntuple()->Get_evroot02()->BetaH(_vpos);
    if(_ptrack){
         ptr.fTrTrack=_ptrack->GetClonePointer();
     }
    else {ptr.fTrTrack=-1;}
//-----
    if(_ptrdtrack){
         ptr.fTrdTrack=_ptrdtrack->GetClonePointer();
     }
    else {ptr.fTrdTrack=-1;}
//-----
    if(_pcharge){
      ptr.fCharge=_pcharge->GetClonePointer();
    }
    else {ptr.fCharge=-1;}
//-----
    ptr.fTofClusterH.clear(); 
    for  (int i=0; i<4; i++) {
      if(_phith[i]){
         ptr.fTofClusterH.push_back(_phith[i]->GetClonePointer());
         ptr.fLayer[i]=_phith[i]->GetClonePointer();
        }
      else ptr.fLayer[i]=-1;
    }
#endif
}

//======================================================
integer AMSBetaH::Out(integer status){
static integer init=0;
static integer WriteAll=1;
if(init == 0){
 init=1;
 integer ntrig=AMSJob::gethead()->gettriggerN();
 for(int n=0;n<ntrig;n++){
   if(strcmp("AMSBetaH",AMSJob::gethead()->gettriggerC(n))==0){
     WriteAll=1;
     break;
   }
 }
}
return (WriteAll || status);
}
//======================================================
#endif
