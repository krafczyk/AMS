// Author V. Choutko 24-may-1996
 
#include <trrec.h>
#include <commons.h>
#include <math.h>
#include <limits.h>
#include <job.h>
#include <amsstl.h>
#include <trrawcluster.h>
#include <extC.h>
#include <upool.h>
#include <string.h>
#include <tofrec.h>
#include <ntuple.h>

const integer AMSTrCluster::WIDE=1;
const integer AMSTrCluster::NEAR=2;

integer AMSTrTrack::patconf[npat][6]={1,2,3,4,5,6,   // 123456  0
                                      1,2,3,4,6,0,   // 12346   1
                                      1,2,3,5,6,0,   // 12356   2
                                      1,2,4,5,6,0,   // 12456   3
                                      1,3,4,5,6,0,   // 13456   4
                                      1,2,3,4,5,0,   // 12345   5
                                      2,3,4,5,6,0,   // 23456   6
                                      1,2,3,4,0,0,   // 1234    7
                                      1,2,3,5,0,0,   // 1235    8
                                      1,2,3,6,0,0,   // 1236    9
                                      1,2,4,5,0,0,   // 1245   10
                                      1,2,4,6,0,0,   // 1246   11
                                      1,2,5,6,0,0,   // 1256   12
                                      1,3,4,5,0,0,   // 1345   13 
                                      1,3,4,6,0,0,   // 1346   14
                                      1,3,5,6,0,0,   // 1356   15
                                      2,3,4,5,0,0,   // 2345   16
                                      2,3,4,6,0,0,   // 2346   17
                                      3,4,5,6,0,0};  // 3456   18
integer AMSTrTrack::patpoints[npat]={6,5,5,5,5,5,5,4,4,4,4,4,4,4,4,4,4,4,4};

void AMSTrCluster::build(){
  integer size=(AMSDBc::maxstrips()+1+
  2*max(TRCLFFKEY.ThrClNEl[0],TRCLFFKEY.ThrClNEl[1]))*sizeof(number);
  number *  adc  = (number*)UPool.insert(size); 
  AMSTrIdSoft id;
  AMSTrRawCluster *p=(AMSTrRawCluster*)AMSEvent::gethead()->
  getheadC("AMSTrRawCluster",0);
  while(p){
    // Unpack cluster into tmp space
    VZERO(adc,size/4);
    // find clusters
     id=AMSTrIdSoft(p->getid());
     integer ilay=id.getlayer(); 
     integer side=id.getside();
     p->expand(adc+TRCLFFKEY.ThrClNEl[side]/2);
     // Circle buffer for x layers 1 && 6;
     // 
     if(side==0 && (ilay==1 || ilay==6)){
       for (int iloc=0;iloc<TRCLFFKEY.ThrClNEl[side]/2;iloc++){
        adc[iloc]=adc[id.getmaxstrips()+iloc];
        adc[iloc+id.getmaxstrips()+TRCLFFKEY.ThrClNEl[side]/2]=
        adc[iloc+TRCLFFKEY.ThrClNEl[side]/2];
       }  
     }
     number ref;
     ref=-FLT_MAX;
     number sum;
     number ssum;
     number pos;
     number rms;
     integer left,right,status,above,center;
     for (int i=TRCLFFKEY.ThrClNEl[side]/2;
     i<id.getmaxstrips()+TRCLFFKEY.ThrClNEl[side]/2+1;i++){
     if(adc[i]<ref){
      // cluster cand found
      if( adc[i]< adc[i+1] && adc[i+1]> TRCLFFKEY.Thr1A[side]){
      // "wide" cluster
      status=AMSTrCluster::WIDE;
      left=max(side==1?TRCLFFKEY.ThrClNEl[side]/2:0,
      i-TRCLFFKEY.ThrClNEl[side]/2);
      center=i;
      right=min(i+TRCLFFKEY.ThrClNEl[side]/2,
            id.getmaxstrips()+(side==1?1:2)*(TRCLFFKEY.ThrClNEl[side]/2)-1);  
      } 
      else if(adc[i+2]>TRCLFFKEY.Thr1A[side]){
       // two clusters near each other; take care about rightmost strip;
      status=AMSTrCluster::NEAR;
      left=max(side==1?TRCLFFKEY.ThrClNEl[side]/2:0,
      i-1-TRCLFFKEY.ThrClNEl[side]/2);
      center=i-1;
      right=min(i+TRCLFFKEY.ThrClNEl[side]/2-1,
      id.getmaxstrips()+(side==1?1:2)*(TRCLFFKEY.ThrClNEl[side]/2)-1);    
      }
      else{
       status=0;
       left=max(side==1?TRCLFFKEY.ThrClNEl[side]/2:0,
       i-1-TRCLFFKEY.ThrClNEl[side]/2);
       center=i-1;
       right=min(i-1+TRCLFFKEY.ThrClNEl[side]/2,
       id.getmaxstrips()+(side==1?1:2)*(TRCLFFKEY.ThrClNEl[side]/2)-1);   
      }
      sum=0;
      ssum=0;
      pos=0;
      rms=0;
      above=0;
      //
      // we don't know here the strip size (unfortunately...)
      // so put 1 instead ...
      // 
      for (int j=left;j<right+1;j++){
       id.upd(j-TRCLFFKEY.ThrClNEl[side]/2);
       if(adc[j]/id.getsig()>TRCLFFKEY.Thr3R[side]){
        if(adc[j]>TRCLFFKEY.Thr2A[side])above++;
        if(j==right+1 && status==AMSTrCluster::NEAR)sum+=adc[j]/2;
        else sum+=adc[j];
        ssum=ssum+pow(id.getsig(),2.);
        pos=pos+1*(j-center)*adc[j];
        rms=rms+pow(1*(j-center),2)*adc[j];
       }
      }
       if(sum !=0){
        rms=sqrt(fabs(rms*sum-pos*pos))/sum; 
        pos=pos/sum+1*0.5;
        ssum=sqrt(ssum);
       }
      ref=-FLT_MAX;
      if(above >= TRCLFFKEY.ThrClNMin[side] && 
         sum> TRCLFFKEY.ThrClA[side] && ssum > 0 && 
         ssum < TRCLFFKEY.ThrClS[side] && sum/ssum > TRCLFFKEY.ThrClR[side]){
         id.upd(center-TRCLFFKEY.ThrClNEl[side]/2);
         _addnext(
         id,status,left-center,right-center+1,sum,ssum,pos,rms,adc+left);
           for (int j=left;j<right+1;j++){
             if(j==right+1 && status==AMSTrCluster::NEAR)adc[j]=adc[j]/2;
             else adc[j]=0;
           }
      }                      
     }
     else{
      if(adc[i] > TRCLFFKEY.Thr1A[side]){
       // susp bump found
       id.upd(i-TRCLFFKEY.ThrClNEl[side]/2);
       if(id.getsig() < TRCLFFKEY.Thr1S[side] && 
       adc[i]/id.getsig() > TRCLFFKEY.Thr1R[side] && 
       id.getstatus(AMSDBc::BAD)==0)ref=adc[i];
      }
     }
     }   
  p=p->next();           
  }  
  UPool.udelete(adc);
} 
number AMSTrCluster::getcofg(integer side, AMSTrIdGeom * pid){
  //
  // Here we are able to recalculate the center of gravity...
  //
#ifdef __AMSDEBUG__
 assert(side>=0 && side <2);
#endif
number cofg=0;
number eval=0;
number smax=0;
number smt=0;
if(_pValues)smax=_pValues[-_NelemL];
integer i,error;
for(i=_NelemL;i<_NelemR;i++){
 cofg+=_pValues[i-_NelemL]*pid->getcofg(side,i,error);
 if(error==0)eval+=_pValues[i-_NelemL];
}
if(eval > 0)cofg=cofg/eval;
if(eval>0)smt=smax/eval;
return cofg-cfgCorFun(smt,pid); 
}
number AMSTrCluster::cfgCorFun(number s, AMSTrIdGeom * pid){
integer side=_Id.getside();
integer strip=pid->getstrip(side);
integer l=pid->getlayer()-1;
if(strip == 0  ){
 if(s<TRCLFFKEY.CorFunParB[side][l]) return 0;
 else return TRCLFFKEY.CorFunParA[side][l]*
      atan(s-TRCLFFKEY.CorFunParB[side][l]);
}
else if(strip== pid->getmaxstrips(side)-1){
 if(s<TRCLFFKEY.CorFunParB[side][l]) return 0;
 else return -TRCLFFKEY.CorFunParA[side][l]*
      atan(s-TRCLFFKEY.CorFunParB[side][l]);
}
else return 0;
}
void AMSTrCluster::_ErrorCalc(){
#if 1  //Very Temporary
integer side=_Id.getside();
if(side==1)_ErrorMean =10.e-4;
else  _ErrorMean =30.e-4;

#else
// Here is the right code

#endif
}


  void AMSTrCluster::_addnext(const AMSTrIdSoft & id, 
                          integer status, integer nelemL,integer nelemR, 
                         number sum, number ssum, number pos, number rms,
                         number val[]) {
    AMSEvent::gethead()->addnext(AMSID("AMSTrCluster",id.getside()),
    new AMSTrCluster(id,status,nelemL,nelemR,sum,ssum,pos,rms,val));
}

AMSTrCluster::AMSTrCluster(const AMSTrIdSoft& id, integer status, 
                           integer nelemL, integer nelemR, number sum,
                           number ssum, number pos,number rms,  number val[]):
    AMSlink(0),_Id(id),_Status(status),_NelemL(nelemL), _NelemR(nelemR),_Sum(sum), 
    _Sigma(ssum), _Mean(pos), _Rms(rms){
 _ErrorCalc();
 if(getnelem()>0){
  _pValues=(number*)UPool.insert(getnelem()*sizeof(number));
  int i;
  for(i=0;i<getnelem();i++)_pValues[i]=val[i];
 }
}

void AMSTrCluster::_writeEl(){
  // fill the ntuple 
static integer init=0;
static TrClusterNtuple TrN;
if(AMSTrCluster::Out( IOPA.WriteAll ||  getstatus(AMSDBc::USED ))){
if(init++==0){
  //book the ntuple block
  HBNAME(IOPA.ntuple,"TrCluste",TrN.getaddress(),
  "TrCluster:I*4,Idsoft:I*4,Status:I*4,NelemL:I*4,NelemR:I*4,Sum:R*4, Sigma:R*4, Mean:R*4, RMS:R*4, ErrorMean:R*4,Amplitude(5):R*4");

}
  TrN.Event()=AMSEvent::gethead()->getid();
  TrN.Idsoft=_Id.cmpt();
  TrN.Status=_Status;
  TrN.NelemL=_NelemL;
  TrN.NelemR=_NelemR;
  TrN.Sum=_Sum;
  TrN.Sigma=_Sigma;
  TrN.Mean=_Mean;
  TrN.RMS=_Rms;
  TrN.ErrorMean=_ErrorMean;
  for(int i=0;i<min(5,getnelem());i++)TrN.Amplitude[i]=_pValues[i]; 
  HFNTB(IOPA.ntuple,"TrCluste");
}
}

void AMSTrCluster::_copyEl(){
}

void AMSTrCluster::print(){
for(int i=0;i<2;i++){
 AMSContainer *p =AMSEvent::gethead()->getC("AMSTrCluster",i);
 if(p)p->printC(cout);
}
}

AMSTrRecHit * AMSTrRecHit::_Head[6]={0,0,0,0,0,0};
void AMSTrRecHit::build(){
 AMSTrCluster *x;
 AMSTrCluster *y;
 AMSTrIdSoft idx;
 AMSTrIdSoft idy;
 integer ia,ib,ilay,nambig;
 y=(AMSTrCluster*)AMSEvent::gethead()->getheadC("AMSTrCluster",1,0); 
 while (y){
   idy=y->getid();
   ilay=idy.getlayer();
   if(y->getstatus(AMSDBc::BAD)==0){
   x=(AMSTrCluster*)AMSEvent::gethead()->getheadC("AMSTrCluster",0);
   while(x){
      idx=x->getid();  
      if(x->getstatus(AMSDBc::BAD) ==0 ){
      if(idx.getlayer() == idy.getlayer() && 
         idx.getdrp() == idy.getdrp() && idx.gethalf() == idy.gethalf()
         && fabs(y->getVal()-x->getVal())/(y->getVal()+x->getVal()) < 
         TRCLFFKEY.ThrDSide){
        // make MANY points.... Unfortunately...
         AMSTrIdGeom *pid = idx.ambig(idy, nambig);
         for(int i=0;i<nambig;i++){
          static char name[5];
         AMSgSen *p;
         p=(AMSgSen*)AMSJob::gethead()->getgeomvolume((pid+i)->crgid());
         if(!p){
          cerr << "AMSTrRecHitBuild-S-Sensor-Error "<<
          AMSID(name,(pid+i)->cmpt());          
         }
         else{
          _addnext(p,0,ilay,x,y,
          p->str2pnt(x->getcofg(0,pid+i),y->getcofg(1,pid+i)),
          AMSPoint(x->getecofg(),y->getecofg(),(number)TRCLFFKEY.ErrZ));
         }
         } 
      } 
      }
    x=x->next();
   }
   }
   y=y->next();
 }  
}
 void AMSTrRecHit::_addnext(AMSgSen * pSen, integer status, integer layer, AMSTrCluster *x,
                            AMSTrCluster * y, const AMSPoint & hit,
                            const AMSPoint & ehit){
    number s1=x->getVal();
    number s2=y->getVal();
    x->setstatus(AMSDBc::USED);
    y->setstatus(AMSDBc::USED);
    AMSEvent::gethead()->addnext(AMSID("AMSTrRecHit",layer-1),
    new     AMSTrRecHit(pSen, status,layer,x,y,hit,ehit,s1+s2,(s1-s2)/(s1+s2)));
      
}


void AMSTrRecHit::_writeEl(){
  // fill the ntuple 
static integer init=0;
static TrRecHitNtuple THN;
int i;
if(AMSTrRecHit::Out( IOPA.WriteAll || getstatus(AMSDBc::USED ))){
if(init++==0){
  //book the ntuple block
  HBNAME(IOPA.ntuple,"TrRecHit",THN.getaddress(),
  "TrRecHit:I*4,pX:I*4,pY:I*4,StatusR:I*4,Layer:I*4,HitR(3):R*4, EhitR(3):R*4, SumR:R*4, DifoSum:R*4");

}
  THN.Event()=AMSEvent::gethead()->getid();
  THN.pX=_Xcl->getpos();
  THN.pY=_Ycl->getpos();
  if(((_Xcl->getid()).getlayer() != _Layer) || 
     ((_Ycl->getid()).getlayer() != _Layer)){
    cerr << "AMSTrRecHit-S-Logic Error "<<(_Xcl->getid()).getlayer()<<" "<<
      (_Ycl->getid()).getlayer()<<" "<<_Layer<<endl;
  }
  THN.Status=_Status;
  THN.Layer=_Layer;
  for(i=0;i<3;i++)THN.Hit[i]=_Hit[i];
  for(i=0;i<3;i++)THN.EHit[i]=_EHit[i];
  THN.Sum=_Sum;
  THN.DifoSum=_DifoSum;
  HFNTB(IOPA.ntuple,"TrRecHit");
}
}

void AMSTrRecHit::_copyEl(){
}


void AMSTrRecHit::print(){
for(int i=0;i<6;i++){
 AMSContainer *p =AMSEvent::gethead()->getC("AMSTrRecHit",i);
 if(p)p->printC(cout);
}
}

geant AMSTrTrack::_Time=0;
void AMSTrTrack::build(){
  // pattern recognition + fit
  _Start();
  for (int pat=0;pat<npat;pat++){
    AMSTrRecHit * phit[6]={0,0,0,0,0,0};
    if(TRFITFFKEY.pattern[pat]){
      // Try to make StrLine Fit
      integer first=AMSTrTrack::patconf[pat][0]-1;
      integer second=AMSTrTrack::patconf[pat][1]-1;
      phit[0]=AMSTrRecHit::gethead(first);
      number x[2];
      number y[2];
      number par[2];
      number epar[2];
      while( phit[0]){
           if(_CheckTime()>AMSFFKEY.CpuLimit){
            throw AMSTrTrackError(" Cpulimit Exceeded ");
           }
       if(phit[0]->getstatus(AMSDBc::USED)==0){
       phit[1]=AMSTrRecHit::gethead(second);
       while( phit[1]){
        if(phit[1]->getstatus(AMSDBc::USED)==0){
        x[0]=phit[0]-> getHit()[2];
        x[1]=phit[1]-> getHit()[2];
        y[0]=phit[0]-> getHit()[0];
        y[1]=phit[1]-> getHit()[0];
        number axy=(x[0]*y[0]+x[1]*y[1])/2;
        number ax=(x[0]+x[1])/2;
        number ay=(y[0]+y[1])/2;
        number axx=(x[0]*x[0]+x[1]*x[1])/2;
        par[1]=(axy-ax*ay)/(axx-ax*ax);
        par[0]=ay-ax*par[1];
        // Search for others
        phit[2]=AMSTrRecHit::gethead(AMSTrTrack::patconf[pat][2]-1);
        while(phit[2]){
         if(phit[2]->getstatus(AMSDBc::USED)==0){
          // Check if the point lies near the str line
           if(AMSTrTrack::Distance(par,phit[2])>= TRFITFFKEY.SearchRegStrLine)
           {phit[2]=phit[2]->next();continue;}
         phit[3]=AMSTrRecHit::gethead(AMSTrTrack::patconf[pat][3]-1);
         while(phit[3]){
          // Check if the point lies near the str line
          if(phit[3]->getstatus(AMSDBc::USED)==0){
          if(AMSTrTrack::Distance(par,phit[3])>= TRFITFFKEY.SearchRegStrLine)
          {phit[3]=phit[3]->next();continue;}
          if(AMSTrTrack::patpoints[pat] >4){         
          phit[4]=AMSTrRecHit::gethead(AMSTrTrack::patconf[pat][4]-1);
          while(phit[4]){
           if(phit[4]->getstatus(AMSDBc::USED)==0){
           if(AMSTrTrack::Distance(par,phit[4])>= TRFITFFKEY.SearchRegStrLine)
           {phit[4]=phit[4]->next();continue;}
           if(AMSTrTrack::patpoints[pat]>5){
           phit[5]=AMSTrRecHit::gethead(AMSTrTrack::patconf[pat][5]-1);
           while(phit[5]){
             if(phit[5]->getstatus(AMSDBc::USED)==0){
              if(AMSTrTrack::Distance(par,phit[5])>= 
              TRFITFFKEY.SearchRegStrLine){phit[5]=phit[5]->next();continue;}
                // 6 point combination found
                if(AMSTrTrack::_addnext(pat,6,phit))goto out;
                
             }
            phit[5]=phit[5]->next();
           }
           }
           else{  // 5 points only
                // 5 point combination found
                if(AMSTrTrack::_addnext(pat,5,phit))goto out;
                
           }
           }
           phit[4]=phit[4]->next();
          }
          }
          else{       // 4 points only
                // 4 point combination found
                if(AMSTrTrack::_addnext(pat,4,phit))goto out;
                
          }
          }
          phit[3]=phit[3]->next();
         }
         }
         phit[2]=phit[2]->next();
        }
        }         
        phit[1]=phit[1]->next();
       }
       }  
out:
       phit[0]=phit[0]->next();
      }
      
    }
  }


}

number AMSTrTrack::Distance(number par[2], AMSTrRecHit *ptr){
   return fabs(par[0]+par[1]*ptr->getHit()[2]-ptr->getHit()[0])/
            sqrt(1+par[1]*par[1]);
}



integer AMSTrTrack::_addnext(integer pat, integer nhit, AMSTrRecHit* pthit[6]){
#ifdef __UPOOL__
    AMSTrTrack track(pat, nhit ,pthit);
    AMSTrTrack *ptrack=   &track;
#else
    AMSTrTrack *ptrack=   new AMSTrTrack(pat, nhit ,pthit);
#endif
    
    number gers=0.03;
    ptrack->SimpleFit(AMSPoint(gers,gers,gers));
    if(ptrack->_Chi2StrLine< TRFITFFKEY.Chi2StrLine){
     if(ptrack->_Chi2Circle< TRFITFFKEY.Chi2Circle && 
      fabs(ptrack->_CircleRidgidity)>TRFITFFKEY.RidgidityMin ){
          
       if(  ptrack->Fit(0) < TRFITFFKEY.Chi2FastFit && ptrack->TOFOK()){
         ptrack->AdvancedFit();
         // Mark hits as USED
         for(int i=0;i<nhit;i++)pthit[i]->setstatus(AMSDBc::USED);
         // permanently add;
#ifdef __UPOOL__
          ptrack=new AMSTrTrack(track);
#endif
          AMSEvent::gethead()->addnext(AMSID("AMSTrTrack",pat),ptrack);
          return 1;
       }
     }
    }
#ifndef __UPOOL__
    delete ptrack;  
#endif   
    return 0;
}

void AMSTrTrack::AdvancedFit(){
  if(_Ridgidity < 0){
   Fit(1);
   Fit(2);
   Fit(3);
   Fit(4);
   Fit(5);
  }

}

integer AMSTrTrack::TOFOK(){
  if (TRFITFFKEY.UseTOF && _Pattern == 16){
   // Cycle thru all TOF clusters;
   // at least UseTOF of them should be matched with the track
   integer i;
   integer matched=0;
   // Take cut from beta defs
   AMSPoint SearchReg(BETAFITFFKEY.SearchReg[0],BETAFITFFKEY.SearchReg[1],
   BETAFITFFKEY.SearchReg[2]);

   for(i=0;i<4;i++){
    AMSTOFCluster * phit = AMSTOFCluster::gethead(i);
    AMSPoint Res;
    number theta,phi,sleng;
    if(phit){
     interpolate(phit->getcoo() ,AMSPoint(0,0,1), Res, theta, phi, sleng);
    }
    while (phit){
     if( ((phit->getcoo()-Res)/phit->getecoo()).abs()< SearchReg)matched++;
     phit=phit->next();
    }  
   }
   if(matched < TRFITFFKEY.UseTOF)return 0;
   else return 1;
  }
  else return 1;
}

void AMSTrTrack::SimpleFit(AMSPoint ehit){

integer ifit=0;
integer npt=_NHits;
const integer maxhits=10;
assert(npt < maxhits);
geant x[maxhits];
geant y[maxhits];
geant wxy[maxhits];
geant z[maxhits];
geant ssz[maxhits];
geant ressz[maxhits];
geant resxy[2*maxhits];
geant spcor[maxhits];
number work[maxhits];
geant chixy;
geant chiz;
geant xmom,dip,phis;
integer iflag=0;
for (int i=0;i<npt;i++){
 z[npt-i-1]=_Pthit[i]->getHit()[0];
 x[npt-i-1]=_Pthit[i]->getHit()[1];
 y[npt-i-1]=_Pthit[i]->getHit()[2];
 wxy[npt-i-1]=(ehit[0]*ehit[0]+ehit[1]*ehit[1]);
 if(wxy[npt-i-1]==0)wxy[npt-i-1]=
 (_Pthit[i]->getEHit()[1] * _Pthit[i]->getEHit()[1]+
  _Pthit[i]->getEHit()[2] * _Pthit[i]->getEHit()[2]);
  wxy[npt-i-1]=1/wxy[npt-i-1];
 ssz[npt-1-i]=ehit[0];
 if(ssz[npt-1-i]==0)ssz[npt-1-i]=_Pthit[i]->getEHit()[0];
 ssz[npt-1-i]=1/ssz[npt-1-i]; 
}

TRAFIT(ifit,x,y,wxy,z,ssz,npt,resxy,ressz,iflag,spcor,work,chixy,chiz,xmom,
       dip,phis);
if(iflag/1000 == 0)_Chi2Circle=chixy;
else _Chi2Circle=FLT_MAX;
if(iflag%1000 ==0)_Chi2StrLine=chiz;
else _Chi2StrLine=FLT_MAX;
_CircleRidgidity=xmom;
}


number AMSTrTrack::Fit(integer fit, integer ipart){

  // fit =0  fit pattern
  // fit =1  fit 1st part if pat=0,1,2,3, ... etc + interpolate to end of 2nd 
  // fit =2  fit 2nd half if pat=0,1,2,3  ... etc 
  // fit =3  Geanefit pattern
  // fit =4  fast fit with ims=0
  // fit =5  geane fit with ims=0
  integer npt=_NHits;
  const integer maxhits=10;
  assert(npt < maxhits);
  static geant hits[maxhits][3];
  static geant sigma[maxhits][3];
  static geant normal[maxhits][3];
  integer ialgo=1;
  geant out[9];
  integer i;
    if(fit==3 || fit==5)ialgo=3;      
    for(i=0;i<_NHits;i++){
     normal[i][0]=0;
     normal[i][1]=0;
     normal[i][2]=-1;
    }
  if(fit == 0 || fit==3 || fit==4 || fit==5){
    for(i=0;i<_NHits;i++){
     for(int j=0;j<3;j++){
      hits[_NHits-i-1][j]=_Pthit[i]->getHit()[j];
      sigma[_NHits-i-1][j]=_Pthit[i]->getEHit()[j];
     }
    }
  }
  else if(fit ==1){
    if(_Pattern==0){
      //fit 124
      npt=3;
     for(int j=0;j<3;j++){
      hits[2][j]=_Pthit[0]->getHit()[j];
      sigma[2][j]=_Pthit[0]->getEHit()[j];
      hits[1][j]=_Pthit[1]->getHit()[j];
      sigma[1][j]=_Pthit[1]->getEHit()[j];
      hits[0][j]=_Pthit[3]->getHit()[j];
      sigma[0][j]=_Pthit[3]->getEHit()[j];
     }
      
    }
    else if(_Pattern<19){
      //fit 123
      npt=3;
     for(int j=0;j<3;j++){
      hits[2][j]=_Pthit[0]->getHit()[j];
      sigma[2][j]=_Pthit[0]->getEHit()[j];
      hits[1][j]=_Pthit[1]->getHit()[j];
      sigma[1][j]=_Pthit[1]->getEHit()[j];
      hits[0][j]=_Pthit[2]->getHit()[j];
      sigma[0][j]=_Pthit[2]->getEHit()[j];
     }
    }
    else{
     _HChi2[0]=FLT_MAX;
     return _HChi2[0];       
    }    
  }  
  else if(fit ==2){
    if(_Pattern == 0 ){
      // fit 346
      npt=3;
     for(int j=0;j<3;j++){
      hits[2][j]=_Pthit[2]->getHit()[j];
      sigma[2][j]=_Pthit[2]->getEHit()[j];
      hits[1][j]=_Pthit[3]->getHit()[j];
      sigma[1][j]=_Pthit[3]->getEHit()[j];
      hits[0][j]=_Pthit[5]->getHit()[j];
      sigma[0][j]=_Pthit[5]->getEHit()[j];
     }
    }
    else if(_Pattern <7){
      // fit 345
      npt=3;
     for(int j=0;j<3;j++){
      hits[2][j]=_Pthit[2]->getHit()[j];
      sigma[2][j]=_Pthit[2]->getEHit()[j];
      hits[1][j]=_Pthit[3]->getHit()[j];
      sigma[1][j]=_Pthit[3]->getEHit()[j];
      hits[0][j]=_Pthit[4]->getHit()[j];
      sigma[0][j]=_Pthit[4]->getEHit()[j];
     }
    }
    else if(_Pattern <19){
      // fit 234
      npt=3;
     for(int j=0;j<3;j++){
      hits[2][j]=_Pthit[1]->getHit()[j];
      sigma[2][j]=_Pthit[1]->getEHit()[j];
      hits[1][j]=_Pthit[2]->getHit()[j];
      sigma[1][j]=_Pthit[2]->getEHit()[j];
      hits[0][j]=_Pthit[3]->getHit()[j];
      sigma[0][j]=_Pthit[3]->getEHit()[j];
     }
    }
    else{
     _HChi2[1]=FLT_MAX;
     return _HChi2[1];       
    }    
  }
  else{
     return FLT_MAX;       
  }   
integer ims; 
if(fit<4)ims=1;
else ims=0;
TKFITG(npt,hits,sigma,normal,ipart,ialgo,ims,out);
if(fit==0){
_Chi2FastFit=out[6];
if(out[7] != 0)_Chi2FastFit=FLT_MAX;
_Ridgidity=out[5];
_ErrRidgidity=out[8];
_Theta=out[3];
_Phi=out[4];
_P0=AMSPoint(out[0],out[1],out[2]);
 
}
else if(fit==1){
_HChi2[0]=out[6];
if(out[7] != 0)_HChi2[0]=FLT_MAX;
_HRidgidity[0]=out[5];
_HErrRidgidity[0]=out[8];
_HTheta[0]=out[3];
_HPhi[0]=out[4];
_HP0[0]=AMSPoint(out[0],out[1],out[2]);

// interpolate if fit == 1;
AMSTrRecHit *phit;
if(_Pattern == 0)phit= _Pthit[5];
else if(_Pattern <7 )phit=_Pthit[4];
else phit=_Pthit[3];
#ifdef __AMSDEBUG__
  assert(phit!= NULL);
#endif

 AMSDir SenDir((phit->getpsen())->getinrm(2,0),
        (phit->getpsen())->getinrm(2,1),(phit->getpsen())->getinrm(2,2) );
 AMSPoint SenPnt=phit->getHit();
 number sleng;
 interpolate(SenPnt, SenDir, _HP0[0], _HTheta[0], _HPhi[0], sleng);
 



}
else if(fit==2){
_AdvancedFitDone=1;
_HChi2[1]=out[6];
if(out[7] != 0)_HChi2[1]=FLT_MAX;
_HRidgidity[1]=out[5];
_HErrRidgidity[1]=out[8];
_HTheta[1]=out[3];
_HPhi[1]=out[4];
_HP0[1]=AMSPoint(out[0],out[1],out[2]);



}
else if(fit==3){
_GeaneFitDone=ipart;
if(out[7] !=0)_GeaneFitDone=0;
_GChi2=out[6];
if(out[7] != 0)_GChi2=FLT_MAX;
_GRidgidity=out[5];
_GErrRidgidity=out[8];
_GTheta=out[3];
_GPhi=out[4];
_GP0=AMSPoint(out[0],out[1],out[2]);
}

else if(fit==4){
_Chi2MS=out[6];
if(out[7] != 0)_Chi2MS=FLT_MAX;
_RidgidityMS=out[5];
}
else if(fit==5){
_GChi2MS=out[6];
if(out[7] != 0)_GChi2MS=FLT_MAX;
_GRidgidityMS=out[5];
}

return _Chi2FastFit;
}




void AMSTrTrack::getParFastFit(number&  Chi2, number& Rid, number&  Err,
number&  Theta, number&  Phi, AMSPoint&  X0)const
{Chi2=_Chi2FastFit;Rid=_Ridgidity;Theta=_Theta;Phi=_Phi;X0=_P0;Err=_ErrRidgidity;}

void AMSTrTrack::getParSimpleFit(number & Chi2xy, number &Chi2sz, number & Rid)const
{Chi2xy=_Chi2Circle;Chi2sz=_Chi2StrLine;Rid=_CircleRidgidity;}

void AMSTrTrack::getParAdvancedFit(number&   GChi2, number&  GRid, number&  GErr,
number&  GTheta, number&  GPhi, AMSPoint&  GP0,
number HChi2[2], number HRid[2], number HErr[2], number HTheta[2],
                              number HPhi[2], AMSPoint  HP0[2] ) const {
 GChi2=_GChi2; GRid=_GRidgidity; GErr=_GErrRidgidity; GTheta=_GTheta;
GPhi=_GPhi;GP0=_GP0;
for(int i=0;i<2;i++){
 HChi2[i]=_HChi2[i];
 HRid[i]=_HRidgidity[i];
 HErr[i]=_HErrRidgidity[i];
 HTheta[i]=_HTheta[i];
 HPhi[i]=_HPhi[i];
 HP0[i]=_HP0[i];
}
}

void AMSTrTrack::_writeEl(){
  // fill the ntuple 
static integer init=0;
static TrTrackNtuple TrTN;
int i;
if(AMSTrTrack::Out(IOPA.WriteAll || getstatus(AMSDBc::USED))){
if(init++==0){
  //book the ntuple block
  HBNAME(IOPA.ntuple,"TrTrack",TrTN.getaddress(),
  "TrTrack:I*4,TrStatus:I*4,Pattern:I*4,  NHits:I*4 , pHits(6):I*4 , GeaneFitDone:I*4,AdvancedFitDone:I*4,Chi2StrLine:R*4,Chi2Circle:R*4,CircleRidgidity:R*4,Chi2FastFit:R*4,Ridgidity:R*4,ErrRidgidity:R*4,Theta:R*4,Phi:R*4,P0(3):R*4,GChi2:R*4,GRidgidity:R*4,GErrRidgidity:R*4,GTheta:R*4,GPhi:R*4,GP0(3):R*4,HChi2(2):R*4,HRidgidity(2):R*4,HErrRidgidity(2):R*4,  HTheta(2):R*4,HPhi(2):R*4,HP0(3,2):R*4,FChi2MS:R*4,GChi2MS:R*4,RidgidityMS:R*4,GRidgidityMS:R*4");

}
  TrTN.Event()=AMSEvent::gethead()->getid();
  TrTN.Status=_Status;
  TrTN.Pattern=_Pattern;
  TrTN.NHits=_NHits;
  for(int k=0;k<_NHits;k++){
   TrTN.pHits[k]=_Pthit[k]->getpos();
  }
  TrTN.GeaneFitDone=_GeaneFitDone;
  TrTN.AdvancedFitDone=_AdvancedFitDone;
  TrTN.Chi2StrLine=(geant)_Chi2StrLine;
  TrTN.Chi2Circle=(geant)_Chi2Circle;
  TrTN.CircleRidgidity=(geant)_CircleRidgidity;
  TrTN.Chi2FastFit=(geant)_Chi2FastFit;
  TrTN.Ridgidity=(geant)_Ridgidity;
  TrTN.ErrRidgidity=(geant)_ErrRidgidity;
  TrTN.Theta=(geant)_Theta;
  TrTN.Phi=(geant)_Phi;
  for(i=0;i<3;i++)TrTN.P0[i]=(geant)_P0[i];
  if(_GeaneFitDone){ 
   TrTN.GChi2=(geant)_GChi2;
   TrTN.GRidgidity=(geant)_GRidgidity;
   TrTN.GErrRidgidity=(geant)_GErrRidgidity;
   TrTN.GTheta=(geant)_GTheta;
   TrTN.GPhi=(geant)_GPhi;
   for(i=0;i<3;i++)TrTN.GP0[i]=(geant)_GP0[i];
  }
  else{
   TrTN.GChi2=-1;
   TrTN.GRidgidity=0;
   TrTN.GErrRidgidity=0;
   TrTN.GTheta=0;
   TrTN.GPhi=0;
   for(i=0;i<3;i++)TrTN.GP0[i]=0;
  } 
  if(_AdvancedFitDone){
   for(i=0;i<2;i++)TrTN.HChi2[i]=(geant) _HChi2[i];
   for(i=0;i<2;i++)TrTN.HRidgidity[i]=(geant)_HRidgidity[i];
   for(i=0;i<2;i++)TrTN.HErrRidgidity[i]=(geant)_HErrRidgidity[i];
   for(i=0;i<2;i++)TrTN.HTheta[i]=(geant)_HTheta[i];
   for(i=0;i<2;i++)TrTN.HPhi[i]=(geant)_HPhi[i];
   for(i=0;i<3;i++)TrTN.HP0_1[i]=(geant)_HP0[0][i];
   for(i=0;i<3;i++)TrTN.HP0_2[i]=(geant)_HP0[1][i];
  }
  else{
   for(i=0;i<2;i++)TrTN.HChi2[i]=-1;
   for(i=0;i<2;i++)TrTN.HRidgidity[i]=0;
   for(i=0;i<2;i++)TrTN.HErrRidgidity[i]=0;
   for(i=0;i<2;i++)TrTN.HTheta[i]=0;
   for(i=0;i<2;i++)TrTN.HPhi[i]=0;
   for(i=0;i<3;i++)TrTN.HP0_1[i]=0;
   for(i=0;i<3;i++)TrTN.HP0_2[i]=0;
  }
  TrTN.FChi2MS=(geant)_Chi2MS;
  TrTN.GChi2MS=(geant)_GChi2MS;
  TrTN.RidgidityMS=(geant)_RidgidityMS;
  TrTN.GRidgidityMS=(geant)_GRidgidityMS;
  HFNTB(IOPA.ntuple,"TrTrack");
}
}

void AMSTrTrack::_copyEl(){
}


void AMSTrTrack::print(){
for(int i=0;i<npat;i++){
 AMSContainer *p =AMSEvent::gethead()->getC("AMSTrTrack",i);
 if(p && TRFITFFKEY.pattern[i])p->printC(cout);
}
}

void AMSTrTrack::interpolate(AMSPoint  pntplane, AMSDir dirplane,AMSPoint & P1,
                             number & theta, number & phi, number & length){

  // interpolates track to plane (pntplane, dirplane)
  // and calculates the track parameters(P1,theta,phi) and total track length

  geant out[7];
  static number m55[5][5];
  geant init[7];
  geant point[6];
  geant charge=1;
   AMSDir dir(sin(_Theta)*cos(_Phi),
             sin(_Theta)*sin(_Phi),
             cos(_Theta));
   init[0]=_P0[0];
   init[1]=_P0[1];
   init[2]=_P0[2];
   init[3]=dir[0];
   init[4]=dir[1];
   init[5]=dir[2];
   init[6]=_Ridgidity;
  point[0]=pntplane[0];
  point[1]=pntplane[1];
  point[2]=pntplane[2];
  point[3]=dirplane[0];
  point[4]=dirplane[1];
  point[5]=dirplane[2];
  geant slength;
  TKFITPAR(init, charge,  point,  out,  m55, slength);
  P1[0]=out[0];
  P1[1]=out[1];
  P1[2]=out[2];
  theta=acos(out[5]);
  phi=atan2(out[4],out[3]);
  length=slength;  
}


void AMSTrTrack::init(AMSTrRecHit * phit[] ){
int i;
for( i=0;i<6;i++)_Pthit[i]=phit[i];

 _GChi2=-1;
 _GRidgidity=0;
 _GErrRidgidity=0;
 _GTheta=0;
 _GPhi=0;
 _GP0=0;
 _Chi2MS=-1;
 _GChi2MS=-1;
 _RidgidityMS=0;
 _GRidgidityMS=0;
 for(i=0;i<2;i++){
 _HRidgidity[i]=0;
 _HErrRidgidity[i]=0;
 _HTheta[i]=0;
 _HPhi[i]=0;
 _HP0[i]=0;
 }

}

AMSTrTrackError::AMSTrTrackError(char * name){
  if(name){
    integer n=strlen(name)+1;
    if(n>255)n=255;
    strncpy(msg,name,n);
  }
}
char * AMSTrTrackError::getmessage(){return msg;}

integer AMSTrCluster::Out(integer status){
static integer init=0;
static integer WriteAll=0;
if(init == 0){
 init=1;
 integer ntrig=AMSJob::gethead()->gettriggerN();
  for(int n=0;n<ntrig;n++){
    if(strcmp("AMSTrCluster",AMSJob::gethead()->gettriggerC(n))==0){
     WriteAll=1;
     break;
    }
  }
}
return (WriteAll || status);
}



integer AMSTrRecHit::Out(integer status){
static integer init=0;
static integer WriteAll=0;
if(init == 0){
 init=1;
 integer ntrig=AMSJob::gethead()->gettriggerN();
  for(int n=0;n<ntrig;n++){
    if(strcmp("AMSTrRecHit",AMSJob::gethead()->gettriggerC(n))==0){
     WriteAll=1;
     break;
    }
  }
}
return (WriteAll || status);
}



integer AMSTrTrack::Out(integer status){
static integer init=0;
static integer WriteAll=0;
if(init == 0){
 init=1;
 integer ntrig=AMSJob::gethead()->gettriggerN();
  for(int n=0;n<ntrig;n++){
    if(strcmp("AMSTrTrack",AMSJob::gethead()->gettriggerC(n))==0){
     WriteAll=1;
     break;
    }
  }
}
return (WriteAll || status);
}
