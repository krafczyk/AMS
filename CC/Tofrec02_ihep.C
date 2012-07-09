//  $Id: Tofrec02_ihep.C,v 1.4.2.1 2012/07/09 09:23:13 choutko Exp $

// ------------------------------------------------------------
//      AMS TOF recontruction-> /*IHEP TOF cal+rec version*/
//      Not All finish: Now focus on time caliberation (BetaH BetaHC)
//                      Temp use TOFRawCluster to Rec//(will change future)
// ------------------------------------------------------------
//      History
//        Created:       2012-June-10  Q.Yan  qyan@cern.ch
//        Modified:
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
  
  TOF2RawCluster *ptrc=(TOF2RawCluster*)AMSEvent::gethead()->getheadC("TOF2RawCluster",0);
  if(!ptrc)return -1;
  
  int il=0,ib=0,idsoft=-1,rstatus=0,status=0,pattern,bmax,nraws;
  bool ismiss=0;
  number sdtm[2],adca[2],timer,etimer,lcoo,scoo,elcoo,escoo,edepa,edepd,edep,barw;
  AMSPoint coo,ecoo;
  while(ptrc){ // loop over counters(raw clusters)
      rstatus=ptrc->getstatus();//TOFGC::SCBADB2 missing
      il=ptrc->getntof()-1;
      ib=ptrc->getplane()-1;
      ptrc->getadca(adca);
      idsoft=il*1000+ib*100;
      status=0;ismiss=0;pattern=0; TOF2RawSide *tfraw[2]={0};nraws=0;
      TOF2RawSide* ptr=(TOF2RawSide*)AMSEvent::gethead()->getheadC("TOF2RawSide",0,1);
      while(ptr){
        if((ptr->getsid()/100-1==il)&&(ptr->getsid()%100-1==ib)){tfraw[ptr->getsid()%10-1]=ptr;nraws++;}
        ptr=ptr->next();
      }
      if(adca[0]>0)pattern|=0x1;//S1 has signal
      if(adca[1]>0)pattern|=0x2;//S2 has signal
      if((rstatus&TOFGC::SCBADB2)>0){ // 1-side counter found --not candidate
          if((rstatus&TOFGC::SCBADB4)>0)pattern&=0x1;//S2 missing try S1 
          else                          pattern&=0x2;//S1 missing try S2
          ismiss=1;
      }
      if(pattern==0){ptrc=ptrc->next();continue;}//both 2 iside has problem
      status=rstatus;
//-----time
      ptrc->getsdtm(sdtm); 
      TimeRec(idsoft,sdtm,adca,timer,etimer,0,0,pattern);
//-----coo
      coo[2]=TOF2DBc::getzsc(il,ib);
      scoo=TOF2DBc::gettsc(il,ib);  //short coo(transv coo)
      lcoo=ptrc->gettimeD(); //temp long coo will change 
//-----ecoo
      ecoo[2]=TOF2DBc::plnstr(6)/sqrt(12.);//bar thickness/sqrt(12)
      bmax=TOF2DBc::getbppl(il);
      if(ib>0 && (ib+1)<bmax)barw= TOF2DBc::plnstr(5);//stand  bar width
      else                   barw= TOF2DBc::outcp(il,1);//outer counter width
      escoo=(barw/2+0.5)/3;//3signma=barw/2 //0.5cm compesate
      if(ismiss==1)elcoo=TOF2DBc::brlen(il,ib)/sqrt(12.);//bleng/sqrt(12)
      else         elcoo=ptrc->getetimeD();//
//---       
      if(il==0||il==3){coo[0]=lcoo;ecoo[0]=elcoo; coo[1]=scoo;ecoo[1]=escoo;}
      else            {coo[0]=scoo;ecoo[0]=escoo; coo[1]=lcoo;ecoo[1]=elcoo;}
//-----energy
      edepa=ptrc->getedepa();//temporaly
      edepd=ptrc->getedepd();//temp
      edep=edepa;            //temp
//--      
      AMSEvent::gethead()->addnext(AMSID("AMSTOFClusterH",il), new
        AMSTOFClusterH(status,pattern,idsoft,adca,sdtm,timer,etimer,coo,ecoo,edepa,edepd,edep,tfraw,nraws));
//------
      ptrc=ptrc->next();
  }
  
 return 0;
}


//========================================================
int TofRecH::TimeRec(int idsoft,number sdtm[], number adca[],number &tm,number &etm, int run,int force,int pattern){
    if(pattern%10==1){sdtm[1]=0;tm=0;etm=4*TFMCFFKEY.TimeSigma/sqrt(2.);return -1;}//s2 missing s1 have
    if(pattern%10==2){sdtm[0]=0;tm=0;etm=4*TFMCFFKEY.TimeSigma/sqrt(2.);return -1;}//s1 missing s2 have

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
    number sl1=  TPar->slew[idsoft];
    number sl2=  TPar->slew[idsoft+10];
    number t0=   TPar->delay[idsoft];
    number index=TPar->powindex;
    tm=0.5*(sdtm[0]+sdtm[1])+sl1/pow(adca[0],index)+sl2/pow(adca[1],index)+t0;//ns
//    time=-time/1e9;
    tm=-tm;//convert to pos 
    etm=TFMCFFKEY.TimeSigma/sqrt(2.);;//will change //temp 0.15ns
    return 0;
//    return time;
}
//========================================================
int TofRecH::CooRec(){
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
         AMSTOFClusterH* phit[4]={0};int pattern[4]={0};number len[4]={0};
         number cres[4][2]={{1000}};
//-----first find
         for(int ilay=0;ilay<4;ilay++){
           BetaFindTOFCl(track,ilay,&phit[ilay],len[ilay],cres[ilay],pattern[ilay],BETAFITFFKEY.HSearchLMatch);
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
int  TofRecH::BetaFindTOFCl(AMSTrTrack *ptrack,int ilay,AMSTOFClusterH** tfhit,number &tklen,number cres[2],int &pattern,int mode){
//---init
    int nowbar=0,prebar=-1000,nextbar=-1000,tminbar=0,layhit=0;
    number mscoo=1000,mlcoo=100,mintm=0,barw=0,dscoo=1000,dlcoo=1000;
    number theta,phi,sleng;
    bool leftf,rightf;
    AMSPoint tfcoo,tfecoo;
    int iscoo[4]={1,0,0,1};
    (*tfhit)=0;tklen=0;pattern=0;cres[0]=1000,cres[1]=1000;
    int bmax=TOF2DBc::getbppl(ilay);
//---
    AMSTOFClusterH * tofhit=(AMSTOFClusterH*)AMSEvent::gethead()->getheadC("AMSTOFClusterH",ilay,1);
    for( ; tofhit;tofhit=tofhit->next()) {
       AMSDir tkdir(0,0,1);AMSPoint tkcoo;
       tfcoo=tofhit->getcoo();
       tfecoo=tofhit->getecoo();
       nowbar=tofhit->getbarid()/100%10;
       if(nowbar>0 && (nowbar+1)<bmax)barw= TOF2DBc::plnstr(5);//stand  bar width
       else                           barw= TOF2DBc::outcp(ilay,1);//outer counter width
//---
       ptrack->interpolate(tfcoo,tkdir,tkcoo,theta,phi,sleng);
       if(ptrack->getpattern()<0){//trd track finding
           //ecoo[0]+=1.;  // add one cm fot trd/tof
	   //ecoo[1]+=1.;  // add one cm fot trd/tof
       }
//---time min bar not used now
       if(tofhit->gettime()!=0){ //bad time
	 if(mintm==0||tofhit->gettime()<mintm){
           mintm=tofhit->gettime();//find earliest(tm) in this layer
	   tminbar=nowbar;//tag earliest bar
	  }
       }
      dscoo=fabs(tkcoo[iscoo[ilay]]-tfcoo[iscoo[ilay]]);
      if(dscoo<mscoo){//first compare Trans
          mscoo=dscoo;
          if(mscoo<BETAFITFFKEY.HSearchReg[0]*tfecoo[iscoo[ilay]]){//0.5cm offset tof+tkhit match in same counter 3sigma S flag
             (*tfhit)=tofhit;tklen=sleng;
             pattern=(*tfhit)->getpattern()%10;//two side information
	     dlcoo=fabs(tkcoo[1-iscoo[ilay]]-tfcoo[1-iscoo[ilay]]);
//------------
             for(int ip=0;ip<2;ip++){
                cres[ip]=tkcoo[ip]-tfcoo[ip];
                if(((pattern%10)!=3)&&(ip==1-iscoo[ilay])){cres[ip]=500;};
              }
             if(mode==0||dlcoo<BETAFITFFKEY.HSearchReg[1]*tfecoo[1-iscoo[ilay]]){
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
int TofRecH::BetaFitC(AMSTOFClusterH *tofclh[4],number res[4][2],int pattern[4], TofBetaPar &par,int mode){
   int iscoo[4]={1,0,0,1};
   AMSPoint tfecoo;
   int nhitxy[2]={0,0};
   number chisxy[2]={0,0};
   for(int ilay=0;ilay<4;ilay++){//temp exclude missing time measument lay S side
     for(int is=0;is<2;is++){par.CResidual[ilay][is]=res[ilay][is];}
     if(tofclh[ilay]){
       tfecoo=tofclh[ilay]->getecoo();
       int isco=iscoo[ilay];
       int ilco=1-iscoo[ilay];
       nhitxy[isco]++;
       chisxy[isco]+=res[ilay][isco]*res[ilay][isco]/tfecoo[isco]/tfecoo[isco];
       if(pattern[ilay]%10==4){//not missing layer
             nhitxy[ilco]++;
             chisxy[ilco]+=res[ilay][ilco]*res[ilay][ilco]/tfecoo[ilco]/tfecoo[ilco];
          }
      }
   }
  par.Chi2C=sqrt(chisxy[0]/nhitxy[0]+chisxy[1]/nhitxy[1]);
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
