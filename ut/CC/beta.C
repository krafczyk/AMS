// Author V. Choutko 4-june-1996
 
#include <beta.h>
#include <commons.h>
#include <math.h>
#include <limits.h>
#include <amsgobj.h>
#include <extC.h>
#include <upool.h>
#include <ntuple.h>


integer AMSBeta::patconf[npatb][4]={  1,2,3,4,        // 1234  0
                                      1,2,3,0,        // 123   1
                                      1,2,4,0,        // 124   2
                                      1,3,4,0,        // 134   3
                                      2,3,4,0,        // 234   4
                                      1,3,0,0,        // 13    5
                                      1,4,0,0,        // 14    6
                                      2,3,0,0,        // 23    7
                                      2,4,0,0};       // 24    8
integer AMSBeta::patpoints[npatb]={4,3,3,3,3,2,2,2,2};



integer AMSBeta::build(integer refit){
  // pattern recognition + fit
  AMSPoint SearchReg(BETAFITFFKEY.SearchReg[0],BETAFITFFKEY.SearchReg[1],
  BETAFITFFKEY.SearchReg[2]);
  int patb;
  for ( patb=0;patb<npatb;patb++){
   int patt;
   number theta=0;
   number td;
   for( patt=0;patt<npat;patt++){
    AMSTrTrack *ptrack=(AMSTrTrack*)AMSEvent::gethead()->
    getheadC("AMSTrTrack",patt);
    while(ptrack ){   
    
      if(ptrack->checkstatus(AMSDBc::USED)==0){        
   // Now TOF hits 

     AMSTOFCluster * phit[4]={0,0,0,0};
     number sleng[4];
     if(BETAFITFFKEY.pattern[patb]){
      phit[0]=AMSTOFCluster::gethead(AMSBeta::patconf[patb][0]-1);
      while( phit[0]){
       number chi2space=0;
       if(BETAFITFFKEY.FullReco || phit[0]->checkstatus(AMSDBc::USED)==0 ){
           AMSPoint dst=AMSBeta::Distance(phit[0]->getcoo(),phit[0]->getecoo(),
           ptrack,sleng[0],td);
        if(dst<SearchReg){
         chi2space+=sqrt(dst[0]*dst[0]+dst[1]*dst[1]);
         phit[1]=AMSTOFCluster::gethead(AMSBeta::patconf[patb][1]-1);
         while( phit[1]){
          if(BETAFITFFKEY.FullReco || phit[1]->checkstatus(AMSDBc::USED)==0 ){
           AMSPoint dst=AMSBeta::Distance(phit[1]->getcoo(),phit[1]->getecoo(),
           ptrack,sleng[1],td);
           if(dst<SearchReg){
            chi2space+=sqrt(dst[0]*dst[0]+dst[1]*dst[1]);
            if(AMSBeta::patpoints[patb] >2){
             phit[2]=AMSTOFCluster::gethead(AMSBeta::patconf[patb][2]-1);
             while( phit[2]){
              if(BETAFITFFKEY.FullReco || phit[2]->checkstatus(AMSDBc::USED)==0 ){
               AMSPoint dst=AMSBeta::Distance(phit[2]->getcoo(),phit[2]->
               getecoo(),ptrack,sleng[2],td);
               if(dst < SearchReg){
                chi2space+=sqrt(dst[0]*dst[0]+dst[1]*dst[1]);
                if(AMSBeta::patpoints[patb] >3){
                phit[3]=AMSTOFCluster::gethead(AMSBeta::patconf[patb][3]-1);
                while( phit[3]){
                if(BETAFITFFKEY.FullReco || phit[3]->checkstatus(AMSDBc::USED)==0 ){
                 AMSPoint dst=AMSBeta::Distance(phit[3]->getcoo(),phit[3]->
                 getecoo(),ptrack,sleng[3],td);
                 if(dst < SearchReg){
                 chi2space+=sqrt(dst[0]*dst[0]+dst[1]*dst[1]);
                   //4  point combination found
                  if(AMSBeta::_addnext(patb,4,sleng,phit,ptrack,theta,chi2space)){
                   goto out;
                  }
                 
                 }        
                }
                phit[3]=phit[3]->next();
                }
                }
                else{
                   //3  point combination found
                  if(AMSBeta::_addnext(patb,3,sleng,phit,ptrack,theta,chi2space))goto out;

                }     
               }
              }
              phit[2]=phit[2]->next();
             }
            }
            else{
                   //2  point combination found
                  if(AMSBeta::_addnext(patb,2,sleng,phit,ptrack,theta,chi2space))goto out;
            }  
           }
          }
          phit[1]=phit[1]->next();
         }  
        }
       }
       phit[0]=phit[0]->next();
      }
      
     }
      }
out:
     ptrack=ptrack->next();
    }
   }
  }
return 1;
}

AMSPoint AMSBeta::Distance(AMSPoint coo, AMSPoint ecoo, AMSTrTrack *ptr,
                            number & sleng, number & theta){
AMSDir dir(0,0,1.);
AMSPoint outp;
number phi;
ptr->interpolate(coo,dir,outp,theta,phi,sleng);
outp=((outp-coo)/ecoo).abs();
return outp;
}



integer AMSBeta::_addnext(integer pat, integer nhit, number sleng[],
        AMSTOFCluster* pthit[4],AMSTrTrack * ptrack, number theta, number c2s){
        c2s=c2s/nhit;
#ifdef __UPOOL__
    AMSBeta beta(pat,  pthit, ptrack, c2s);
    AMSBeta *pbeta=   &beta;
#else
    AMSBeta *pbeta=new AMSBeta(pat,  pthit, ptrack,c2s);
#endif
    pbeta->SimpleFit(nhit, sleng);
    if(pbeta->getchi2()< BETAFITFFKEY.Chi2 && fabs(pbeta->getbeta())<1.41){
      // Mark Track as used
         ptrack->setstatus(AMSDBc::USED);
         // Mark hits as USED
         int i;
         for( i=0;i<nhit;i++){
           if(pthit[i]->checkstatus(AMSDBc::USED))
           pthit[i]->setstatus(AMSDBc::AMBIG);
           pthit[i]->setstatus(AMSDBc::USED);
         }
         if(fabs(pbeta->getbeta()) < BETAFITFFKEY.LowBetaThr && pat !=7){
          // release hits if pat # 7 and low beta
          for( i=0;i<nhit;i++){
           if(pthit[i]->getntof() ==2)pthit[i]->clearstatus(AMSDBc::USED);
           if(pthit[i]->getntof() ==3)pthit[i]->clearstatus(AMSDBc::USED);
          }
          // set AMBIG flag on beta here if pat = 0,1 or 4
          if(pat==0 || pat==1 || pat==4)pbeta->setstatus(AMSDBc::AMBIG);
         }                  
         // permanently add;
         
#ifdef __UPOOL__
          pbeta=new AMSBeta(beta);
#endif
          AMSEvent::gethead()->addnext(AMSID("AMSBeta",pat),pbeta);
          return 1;
    }   
#ifndef __UPOOL__
       delete pbeta;
#endif
    return 0;
}


void AMSBeta::SimpleFit(integer nhit, number x[]){
 number y[4];
 number ey[4];
 number xy=0;
 number x2=0;
 number xa=0;
 number ya=0;
 number e2=0;
 number a,b;   //y=ax+b
 const number c=2.99792e10; //speed of light
 int i;
 for( i=0;i<nhit;i++){
  y[i]=_pcluster[i]->gettime()*c;
  ey[i]=_pcluster[i]->getetime()*c;
 }
 for(i=0;i<nhit;i++){
  xy+=x[i]*y[i]/ey[i]/ey[i];
  x2+=x[i]*x[i]/ey[i]/ey[i];
  xa+=x[i]/ey[i]/ey[i];
  ya+=y[i]/ey[i]/ey[i];
  e2+=1/ey[i]/ey[i];
 }
 xy=xy/e2;
 x2=x2/e2;
 xa=xa/e2;
 ya=ya/e2;
 a=(xy-xa*ya)/(x2-xa*xa);
 b=ya-a*xa;
 _Beta=1/a;
 _Chi2=0;
 _InvErrBeta=0;
 for(i=0;i<nhit;i++)_Chi2+=(y[i]-a*x[i]-b)/ey[i]*(y[i]-a*x[i]-b)/ey[i];
 _Chi2=_Chi2/(nhit-1);
 for(i=0;i<nhit;i++)_InvErrBeta+=ey[i]*ey[i];
 _InvErrBeta=sqrt(_InvErrBeta)/nhit/sqrt(x2-xa*xa);
}




void AMSBeta::_writeEl(){
  // fill the ntuple 
static BetaNtuple BN;
static integer init=0;
if(init++==0){
  //book the ntuple block
  //
  HBNAME(IOPA.ntuple,"Beta",BN.getaddress(),
  "BetaEvent:I*4,BetaPattern:I*4, Beta:R*4, BetaError:R*4, BetaChi2:R*4,BetaChi2S:R*4");

}
  BN.Event()=AMSEvent::gethead()->getid();
  BN.Pattern=_Pattern;  
  BN.Beta=_Beta;
  BN.Error=_InvErrBeta;
  BN.Chi2=_Chi2;
  BN.Chi2S=_Chi2Space;
  HFNTB(IOPA.ntuple,"Beta");
}

void AMSBeta::_copyEl(){
}


void AMSBeta::print(){
int i;
for( i=0;i<npatb;i++){
 AMSContainer *p =AMSEvent::gethead()->getC("AMSBeta",i);
 if(p && BETAFITFFKEY.pattern[i])p->printC(cout);
}
}


void AMSBeta::_printEl(ostream & stream){ stream << " Pattern " << _Pattern << " Beta "<<_Beta<<" ErrBeta" <<_InvErrBeta<<" Chi2 "<<_Chi2<<" Chi2S "<<_Chi2Space<<endl;}
