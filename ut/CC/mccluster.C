// Author V. Choutko 24-may-1996
 
#include <trid.h>
#include <mccluster.h>
#include <extC.h>
#include <commons.h>
#include <math.h>
#include <job.h>
#include <event.h>
#include <amsstl.h>
#include <cont.h>
#include <ntuple.h>

integer AMSTrMCCluster::debug(1);
  
AMSTrMCCluster::AMSTrMCCluster
(const AMSTrIdGeom & id,integer side,integer nel, number ss[], integer itra){
_next=0;
_idsoft=id.cmpt();
_xca=0;
_xcb=0;
_xgl=0;
_itra=itra;
_sum=0;
if(nel>5)nel=5;
VZERO(_ss,10*sizeof(geant)/4);
_left[0]=max(0,id.getstrip(0)-nel/2); 
_left[1]=max(0,id.getstrip(1)-nel/2); 
_right[0]=min(id.getstrip(0)+nel/2,id.getmaxstrips(0)-1); 
_right[1]=min(id.getstrip(1)+nel/2,id.getmaxstrips(1)-1); 
_center[0]=id.getstrip(0);
_center[1]=id.getstrip(1);
for(int i=_left[side];i<_right[side]+1;i++)
 _ss[side][i-_left[side]]=ss[i+nel-_right[side]-1];
}

void AMSTrMCCluster::addcontent(const AMSTrIdSoft &  idl, geant *adc){
integer i;
integer side=idl.getside();
if(side==0){
   for(i=_left[side];i<=_right[side];i++){
    AMSTrIdSoft id(AMSTrIdGeom(_idsoft,i,0),side);
    if(idl.getaddr() != id.getaddr())return;
    if(id.dead())continue; 
    adc[id.getstrip()]=  adc[id.getstrip()]+_ss[side][i-_left[side]];
   }
}  
else {
   for(i=_left[side];i<=_right[side];i++){
    AMSTrIdSoft id(AMSTrIdGeom(_idsoft,0,i),side);
    if(idl.getaddr() != id.getaddr()  )return;
    if(id.dead())continue; 
    adc[id.getstrip()]=  adc[id.getstrip()]+_ss[side][i-_left[side]];
   }
}
}

void AMSTrMCCluster::addcontent(char xy, geant ** p){
 integer i;
 if(xy=='x'){
   for(i=_left[0];i<=_right[0];i++){
    AMSTrIdSoft id(AMSTrIdGeom(_idsoft,i,0),0);
    if(id.dead()){
     continue;
    }
    if(p[id.getaddr()]==0){
     integer isize=sizeof(geant)*id.getmaxstrips();
     p[id.getaddr()]=(geant*)UPool.insert(isize);
     if(p[id.getaddr()]==0){
       cerr <<" AMSTrMCCLUSTER-S-Memory Alloc Failure"<<endl;
       return;
     }
     VZERO(p[id.getaddr()],isize/4);
    }
    *(p[id.getaddr()]+id.getstrip())= *(p[id.getaddr()]+id.getstrip())+
    _ss[0][i-_left[0]];
    //    cout <<id<<" side 0 adc + "<<id.getstrip()<<" "<<_ss[0][i-_left[0]]<< endl;
   }
 }
 else{
   for(i=_left[1];i<=_right[1];i++){
    AMSTrIdSoft id(AMSTrIdGeom(_idsoft,0,i),1);
    if(id.dead()){
     continue;
    }
    if(p[id.getaddr()]==0){
     integer isize=sizeof(geant)*id.getmaxstrips();
     p[id.getaddr()]=(geant*)UPool.insert(isize);
     if(p[id.getaddr()]==0){
       cerr <<" AMSTrMCCLUSTER-S-Memory Alloc Failure"<<endl;
       return;
     }
     VZERO(p[id.getaddr()],isize/4);
    }
    *(p[id.getaddr()]+id.getstrip())= *(p[id.getaddr()]+id.getstrip())+
    _ss[1][i-_left[1]];
    //    cout <<id <<" side 1 adc + "<<id.getstrip()<<" "<<_ss[1][i-_left[1]]<< endl;
   }
 }
}

void AMSTrMCCluster::_shower(){
  //  cout <<"*** start shower " <<endl;
  AMSgObj::BookTimer.start("TrMCCluster");
  //   Non active silicon as geant volume introduced ; skip some lines below
  //
  //  // As we have no non-active silicon as geant volume 
  //  // we should do something manually in case we are in it;
  //  // Very crude method (but fast)
  //  // gives some overestimation of dead space by ~100-200 mkm == ~10-20%
  //     number sa[2],sb[2];
  //     if((_xca[0]<0 || _xca[0]>id.getmaxsize(0)) ||
  //        (_xca[1]<0 || _xca[1]>id.getmaxsize(1)) ||
  //        (_xcb[0]<0 || _xcb[0]>id.getmaxsize(0)) ||
  //        (_xcb[1]<0 || _xcb[1]>id.getmaxsize(1))   )_sum=0; 
  //
  //  // End active check
  //  //

AMSTrIdGeom id(_idsoft);
AMSPoint entry=(_xca+_xcb)/2.;
AMSPoint dentry=(_xcb-_xca)/2;
AMSDir dir=_xcb-_xca;
geant cofg=0;
geant esumx=0;
geant esumy=0;
integer ierr;
integer i;
geant xpr=_sum*TRMCFFKEY.dedx2nprel;
for(i=0;i<_nel;i++)_ss[0][i]=0;
for(i=0;i<_nel;i++)_ss[1][i]=0;
_center[0]=id.size2strip(0,entry[0]);
_center[1]=id.size2strip(1,entry[1]);
_left[0]= max(0,_center[0]-(_nel/2));
_left[1]= max(0,_center[1]-(_nel/2));
_right[0]=min(id.getmaxstrips(0)-1,_center[0]+(_nel/2));
_right[1]=min(id.getmaxstrips(1)-1,_center[1]+(_nel/2));
if(xpr >0.){
for (integer k=0;k<2;k++){
  if(fabs(dentry[k])/(xpr)<TRMCFFKEY.fastswitch){
    // fast algo
     id.upd(k,_center[k]);  // update geom id
     number e=entry[k]-id.strip2size(k);
     number s=id.strip2size(k);
     for (int ii=_left[k];ii<=_right[k];ii++){
       number a1,a2;
       id.upd(k,ii);  // update geom id
       a1=id.strip2size(k)-s;
       a2=a1+id.getsize(k);
       number r=a2-a1;
      _ss[k][ii-_left[k]]=TRMCFFKEY.delta[k]*xpr*sitkfint2_(a1,a2,e,dentry[k],r);
     }     
  }
  else{
    //slow algo
    for(int kk=0;kk<floor(xpr);kk++){
     geant e,dummy;
     e=entry[k]-dentry[k]+2*RNDM(dummy)*dentry[k];
     number a1,a2;
     for (int ii=_left[k];ii<=_right[k];ii++){
       id.upd(k,ii);  // update geom id
       a1=id.strip2size(k)-e;
       a2=a1+id.getsize(k);
       _ss[k][ii-_left[k]]=_ss[k][ii-_left[k]]+TRMCFFKEY.delta[k]*sitkfint_s_(a1,a2);
     }
    }
  }
  integer nedx;
  for(int ii=_left[k];ii<=_right[k];ii++){
   _ss[k][ii-_left[k]]=_ss[k][ii-_left[k]]*(1+rnormx()*TRMCFFKEY.gammaA[k]);
  }
}
}

  AMSgObj::BookTimer.stop("TrMCCluster");
  // cout <<"** end shower"<<endl;
}




void AMSTrMCCluster::_printEl(ostream & stream){
   AMSTrIdGeom id(_idsoft);
   stream << "AMSTrMCCluster-Shower x "<<_itra<<" "<<id.getlayer()<<" "
   <<_ss[0][0]<<" "<<_ss[0][1]<<" "<<_ss[0][2]<<" "
   <<_ss[0][3]<<" "<<_ss[0][4]<<endl;
   stream << "AMSTrMCCluster-Shower y "<<_itra<<" "<<id.getlayer()<<" "
   <<_ss[1][0]<<" "<<_ss[1][1]<<" "<<_ss[1][2]<<" "
   <<_ss[1][3]<<" "<<_ss[1][4]<<endl;
   stream << "AMSTrMCCluster-Coo "<<_itra<<" "<<id.getlayer()<<" "
   <<_xca<<" "<<_xcb<<" "<<_xgl<<endl;
}

void AMSTrMCCluster::_copyEl(){
}

void AMSTrMCCluster::_writeEl(){

static integer init=0;
static TrMCClusterNtuple TrMCClusterN;
int i;
if(AMSTrMCCluster::Out( IOPA.WriteAll)){
if(init++==0){
  //book the ntuple block
  HBNAME(IOPA.ntuple,"TrMCClus",TrMCClusterN.getaddress(),
  "TrMCCluster:I*4,IdsoftMC:I*4, Itra:I*4,Left(2):I*4, Center(2):I*4, Right(2):I*4, SS(5,2):R*4, Xca(3):R*4, Xcb(3):R*4, Xgl(3):R*4, SumMC:R*4");
}
  TrMCClusterN.Event()=AMSEvent::gethead()->getid();
  TrMCClusterN.Idsoft=_idsoft;
  TrMCClusterN.TrackNo=_itra;
  for(i=0;i<2;i++)TrMCClusterN.Left[i]=_left[i];
  for(i=0;i<2;i++)TrMCClusterN.Center[i]=_center[i];
  for(i=0;i<2;i++)TrMCClusterN.Right[i]=_right[i];
  for(i=0;i<2;i++)TrMCClusterN.SS[i][0]=_ss[i][0];
  for(i=0;i<2;i++)TrMCClusterN.SS[i][1]=_ss[i][1];
  for(i=0;i<2;i++)TrMCClusterN.SS[i][2]=_ss[i][2];
  for(i=0;i<2;i++)TrMCClusterN.SS[i][3]=_ss[i][3];
  for(i=0;i<2;i++)TrMCClusterN.SS[i][4]=_ss[i][4];
  for(i=0;i<3;i++)TrMCClusterN.Xca[i]=_xca[i];
  for(i=0;i<3;i++)TrMCClusterN.Xcb[i]=_xcb[i];
  for(i=0;i<3;i++)TrMCClusterN.Xgl[i]=_xgl[i];
  TrMCClusterN.Sum=_sum;
  HFNTB(IOPA.ntuple,"TrMCClus");
}

}

number AMSTrMCCluster::sitknoiseprob(const AMSTrIdSoft & id, number threshold){
number z;
number sigma=id.getsig();
if(sigma >0)z=threshold/sigma/sqrt(2.);
else z=FLT_MAX;
return DERFC(z)/2;  

}

void  AMSTrMCCluster::sitknoisespectrum(
const AMSTrIdSoft & id,integer nch,number ss[]){
  for(int i=0;i<nch;i++){
   ss[i]=id.getsig()*rnormx();
   if(i==nch/2) ss[i]=fabs(ss[i])+id.getsig()*TRMCFFKEY.thr1R[id.getside()];
  }
 
}


void AMSTrMCCluster::sitkhits(
integer idsoft , geant vect[],geant edep, geant step, integer itra ){
  AMSgObj::BookTimer.start("SITKHITS");
 AMSPoint pa(vect[0],vect[1],vect[2]);
 AMSPoint dirg(vect[3],vect[4],vect[5]);
 AMSPoint pb=pa-dirg*step;
 AMSPoint pgl=pa-dirg*step/2;
 AMSgSen *p=(AMSgSen*)AMSJob::gethead()->getgeomvolume(
  AMSTrIdGeom(idsoft).crgid());
 if(p){
  integer ilay=AMSTrIdGeom(p->getid()).getlayer();
#ifdef __AMSDEBUG__
  //  cout <<"Sitkhits - "<<ilay<<" "<<pgl<<endl;
#endif
 
 if(p->getsenstrip(pa) && p->getsenstrip(pb)){
      AMSEvent::gethead()->addnext(AMSID("AMSTrMCCluster",0),
      new AMSTrMCCluster(idsoft,pa,pb,pgl,edep,itra));
 }
 }
 else{
   cerr << "sitkhits Error" << idsoft <<" "
        <<AMSTrIdGeom(idsoft).crgid()<<endl;
 }
 AMSgObj::BookTimer.stop("SITKHITS");
}




void AMSTrMCCluster::sitknoise(){
  // Only add noise when not in raw mode
  AMSgObj::BookTimer.start("SITKNOISE");
   geant dummy;
   number noise,oldone=0;
   integer itra=555;
   number ss[5];
   number const probthr=0.05;
   float totn[2]={0,0};
   for(int l=0;l<2;l++){
     for (int i=0;i<AMSDBc::nlay();i++){
       for (int j=0;j<AMSDBc::nlad(i+1);j++){
         for (int s=0;s<2;s++){
           if(strstr(AMSJob::gethead()->getsetup(),"AMSSTATION") ||
            AMSDBc::activeladdshuttle(i+1,j+1,s)){
            AMSTrIdSoft id(i+1,j+1,s,l,0);
             int icmpt=id.gettdr();
             if(TRMCFFKEY.RawModeOn[id.gethalf()][id.getside()][icmpt])continue;
            id.upd(id.getmaxstrips()-1);
            geant r=RNDM(dummy);
            if(r<id.getindnoise()){
              //bsearch
             id.upd(0);
             while(id.getprob(r) <id.getmaxstrips()){
              r=RNDM(dummy);

              totn[l]=totn[l]+1;
              AMSTrIdSoft idx,idy;
              if(l==0){
                  idx=id;
                  idy=AMSTrIdSoft(i+1,j+1,s,1,0);
              }
              else {
                  idy=id;
                  idx=AMSTrIdSoft(i+1,j+1,s,0,0);
              }
              integer nambig;
              AMSTrIdGeom *pid=idx.ambig(idy,nambig);
              if(pid){
                 sitknoisespectrum(id,5,ss);
#ifdef __AMSDEBUG__
                 //                  static integer i14(0);
                 //                 cout << i14++<<" "<<ss[2]<<endl;
                 //                 cout <<id<<endl;    
                 HF1(200103+id.getside(),ss[0],1.);
                 HF1(200103+id.getside(),ss[1],1.);
                 HF1(200103+id.getside(),ss[3],1.);
                 HF1(200103+id.getside(),ss[4],1.);
                 HF1(200105+id.getside(),ss[2],1.);
#endif
                  AMSEvent::gethead()->addnext(AMSID("AMSTrMCCluster",0),
                  new AMSTrMCCluster(*pid,id.getside(),5,ss,itra));
              }
             }
            }
           }
         }
       }

     }
       HF1(200101+l,totn[l],1.);

   }

   AMSgObj::BookTimer.stop("SITKNOISE");

}

void  AMSTrMCCluster::sitkcrosstalk(){
}





void AMSTOFMCCluster::sitofhits(integer idsoft , geant vect[],geant edep, 
geant tofg){
  //        Very Temporary
  AMSPoint pnt(vect[0],vect[1],vect[2]);
   AMSEvent::gethead()->addnext(AMSID("AMSTOFMCCluster",0),
   new AMSTOFMCCluster(idsoft,pnt,edep,tofg));

 




}

void AMSAntiMCCluster::siantihits(integer idsoft , geant vect[],geant edep, 
geant tofg){
  //        Very Temporary
  AMSPoint pnt(vect[0],vect[1],vect[2]);
   AMSEvent::gethead()->addnext(AMSID("AMSAntiMCCluster",0),
   new AMSAntiMCCluster(idsoft,pnt,edep,tofg));

}




void AMSTOFMCCluster::_writeEl(){

if(AMSTOFMCCluster::Out( IOPA.WriteAll)){
static integer init=0;
static TOFMCClusterNtuple TOFMCClusterN;
int i;
if(init++==0){
  //book the ntuple block
  HBNAME(IOPA.ntuple,"TOFMCClu",TOFMCClusterN.getaddress(),
  "TOFMCEvent:I*4,TOFMCIdsoft:I*4, TOFMCXcoo(3):R*4, TOFMCtof:R*4, TOFMCedep:R*4");
}
  TOFMCClusterN.Event()=AMSEvent::gethead()->getid();
  TOFMCClusterN.Idsoft=idsoft;
  for(i=0;i<3;i++)TOFMCClusterN.Coo[i]=xcoo[i];
  TOFMCClusterN.TOF=tof;
  TOFMCClusterN.Edep=edep;
  HFNTB(IOPA.ntuple,"TOFMCClu");
}

}

void AMSAntiMCCluster::_writeEl(){

if(AMSAntiMCCluster::Out( IOPA.WriteAll)){
static integer init=0;
static TOFMCClusterNtuple AntiMCClusterN;
int i;
if(init++==0){
  //book the ntuple block
  HBNAME(IOPA.ntuple,"AntiMCCl",AntiMCClusterN.getaddress(),
  "AntiMCEvent:I*4,AntiMCIdsoft:I*4, AntiMCXcoo(3):R*4, AntiMCtof:R*4, AntiMCedep:R*4");
}
  AntiMCClusterN.Event()=AMSEvent::gethead()->getid();
  AntiMCClusterN.Idsoft=_idsoft;
  for(i=0;i<3;i++)AntiMCClusterN.Coo[i]=_xcoo[i];
  AntiMCClusterN.TOF=_tof;
  AntiMCClusterN.Edep=_edep;
  HFNTB(IOPA.ntuple,"AntiMCCl");
}

}

integer AMSCTCMCCluster::getdetno(){
   integer dt=(_idsoft/10)%10;
   switch(dt){
   case 1:
     // PTF
     return 1;
   case 2:
     //AGL
     return 0;
   default:
     //PMT
     return 2;
   }
}

void AMSCTCMCCluster::sictchits(integer idsoft , geant vect[],geant charge, 
geant stepc, geant getot, geant edep, geant time){
   AMSPoint pnt(vect[0],vect[1],vect[2]);
   AMSDir dir(vect[3],vect[4],vect[5]);
   number beta=getot != 0 ? vect[6]/getot : 0;
   if(beta>0){
    AMSCTCMCCluster *p=
    new AMSCTCMCCluster(idsoft,pnt,dir,charge,stepc,beta,edep, time);
    AMSEvent::gethead()->addnext(AMSID("AMSCTCMCCluster",p->getlayno()-1),p);
   }

}

void AMSCTCMCCluster::_writeEl(){

if(AMSCTCMCCluster::Out( IOPA.WriteAll)){
static integer init=0;
static CTCMCClusterNtuple CTCMCClusterN;
int i;
if(init++==0){
  //book the ntuple block
  HBNAME(IOPA.ntuple,"CTCMCClu",CTCMCClusterN.getaddress(),
  "CTCMCEvent:I*4,CTCMCIdsoft:I*4, CTCMCXcoo(3):R*4,CTCMCXdir(3):R*4,CTCstep:R*4, CTCCharge:R*4, CTCbeta:R*4, CTCEdep:R*4");
}
  CTCMCClusterN.Event()=AMSEvent::gethead()->getid();
  CTCMCClusterN.Idsoft=_idsoft;
  for(i=0;i<3;i++)CTCMCClusterN.Coo[i]=_xcoo[i];
  for(i=0;i<3;i++)CTCMCClusterN.Dir[i]=_xdir[i];
  CTCMCClusterN.Step=_step;
  CTCMCClusterN.Charge=_charge;
  CTCMCClusterN.Beta=_beta;
  CTCMCClusterN.Edep=_edep;
  HFNTB(IOPA.ntuple,"CTCMCClu");
}

}


integer AMSTrMCCluster::Out(integer status){
static integer init=0;
static integer WriteAll=0;
if(init == 0){
 init=1;
 integer ntrig=AMSJob::gethead()->gettriggerN();
 for(int n=0;n<ntrig;n++){
   if(strcmp("AMSTrMCCluster",AMSJob::gethead()->gettriggerC(n))==0){
     WriteAll=1;
     break;
   }
 }
}
return (WriteAll || status);
}

integer AMSTOFMCCluster::Out(integer status){
static integer init=0;
static integer WriteAll=0;
if(init == 0){
 init=1;
 integer ntrig=AMSJob::gethead()->gettriggerN();
 for(int n=0;n<ntrig;n++){
   if(strcmp("AMSTOFMCCluster",AMSJob::gethead()->gettriggerC(n))==0){
     WriteAll=1;
     break;
   }
 }
}
return (WriteAll || status);
}

integer AMSCTCMCCluster::Out(integer status){
static integer init=0;
static integer WriteAll=0;
if(init == 0){
 init=1;
 integer ntrig=AMSJob::gethead()->gettriggerN();
 for(int n=0;n<ntrig;n++){
   if(strcmp("AMSCTCMCCluster",AMSJob::gethead()->gettriggerC(n))==0){
     WriteAll=1;
     break;
   }
 }
}
return (WriteAll || status);
}



integer AMSAntiMCCluster::Out(integer status){
static integer init=0;
static integer WriteAll=0;
if(init == 0){
 init=1;
 integer ntrig=AMSJob::gethead()->gettriggerN();
 for(int n=0;n<ntrig;n++){
   if(strcmp("AMSAntiMCCluster",AMSJob::gethead()->gettriggerC(n))==0){
     WriteAll=1;
     break;
   }
 }
}
return (WriteAll || status);
}
