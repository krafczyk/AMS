// Author V. Choutko 4-june-1996
// 31.07.98 E.Choumilov. Cluster Time recovering(for 1-sided counters) added.
//
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

// Pattern recognition + fit
 AMSPoint SearchReg(BETAFITFFKEY.SearchReg[0],BETAFITFFKEY.SearchReg[1],
 BETAFITFFKEY.SearchReg[2]);

// Loop on TOF patterns
 for ( int patb=0; patb<npatb; patb++){
   if(!BETAFITFFKEY.pattern[patb]) continue;
   number theta=0;
   number td;

// Loop first on tracks with K-hits, second on FalseTOFX
   for( int Sonly=0; Sonly<2; Sonly++){

// Loop on track patterns
    for( int patt=0; patt<npat; patt++){

// Loop on tracks
     AMSTrTrack *ptrack=(AMSTrTrack*)AMSEvent::gethead()->getheadC("AMSTrTrack",patt);
     for ( ; ptrack ; ptrack=ptrack->next()) {

       if (Sonly==0 && ptrack->checkstatus(AMSDBc::FalseTOFX)) continue;
       if (Sonly==1 && !ptrack->checkstatus(AMSDBc::FalseTOFX)) continue;
       if(!BETAFITFFKEY.FullReco && ptrack->checkstatus(AMSDBc::USED)) continue;
       AMSTOFCluster * phit[4]={0,0,0,0};
       number sleng[4];

// Loop on first TOF plane
       phit[0]=AMSTOFCluster::gethead(AMSBeta::patconf[patb][0]-1);
       for ( ; phit[0]; phit[0]=phit[0]->next()) {
         number chi2space=0;
         if(phit[0]->checkstatus(AMSDBc::BAD)) continue;
         if (!BETAFITFFKEY.FullReco && !ptrack->checkstatus(AMSDBc::FalseTOFX)) {
           if (phit[0]->checkstatus(AMSDBc::USED)) continue;
         }
         AMSPoint dst=AMSBeta::Distance(phit[0]->getcoo(),phit[0]->getecoo(),
         ptrack,sleng[0],td);
         if (dst>=SearchReg) continue;
         chi2space+=sqrt(dst[0]*dst[0]+dst[1]*dst[1]);

// Loop on second TOF plane
         phit[1]=AMSTOFCluster::gethead(AMSBeta::patconf[patb][1]-1);
         for ( ; phit[1]; phit[1]=phit[1]->next()) {
           if(phit[1]->checkstatus(AMSDBc::BAD)) continue;
           if (!BETAFITFFKEY.FullReco && !ptrack->checkstatus(AMSDBc::FalseTOFX)) {
             if (phit[1]->checkstatus(AMSDBc::USED)) continue;
           }
           AMSPoint dst=AMSBeta::Distance(phit[1]->getcoo(),phit[1]->getecoo(),
           ptrack,sleng[1],td);
           if (dst>=SearchReg) continue;
           chi2space+=sqrt(dst[0]*dst[0]+dst[1]*dst[1]);
// 2-point combination found
           if(AMSBeta::patpoints[patb]==2){
             if(AMSBeta::_addnext(patb,2,sleng,phit,ptrack,theta,chi2space))
               goto nexttrack;
             continue;
           }

// Loop on third TOF plane
           phit[2]=AMSTOFCluster::gethead(AMSBeta::patconf[patb][2]-1);
           for ( ; phit[2]; phit[2]=phit[2]->next()) {
             if(phit[2]->checkstatus(AMSDBc::BAD)) continue;
             if (!BETAFITFFKEY.FullReco && !ptrack->checkstatus(AMSDBc::FalseTOFX)) {
             if (phit[2]->checkstatus(AMSDBc::USED)) continue;
               }
             AMSPoint dst=AMSBeta::Distance(phit[2]->getcoo(),phit[2]->
             getecoo(),ptrack,sleng[2],td);
             if(dst>=SearchReg) continue;
             chi2space+=sqrt(dst[0]*dst[0]+dst[1]*dst[1]);
// 3-point combination found
             if(AMSBeta::patpoints[patb]==3){
               if(AMSBeta::_addnext(patb,3,sleng,phit,ptrack,theta,chi2space))
                 goto nexttrack;
               continue;
             }

// Loop on fourth TOF plane
             phit[3]=AMSTOFCluster::gethead(AMSBeta::patconf[patb][3]-1);
             for ( ; phit[3]; phit[3]=phit[3]->next()) {
               if(phit[3]->checkstatus(AMSDBc::BAD)) continue;
               if (!BETAFITFFKEY.FullReco && !ptrack->checkstatus(AMSDBc::FalseTOFX)){
                 if (phit[3]->checkstatus(AMSDBc::USED)) continue;
               }
               AMSPoint dst=AMSBeta::Distance(phit[3]->getcoo(),phit[3]->
               getecoo(),ptrack,sleng[3],td);
               if(dst>=SearchReg) continue;
               chi2space+=sqrt(dst[0]*dst[0]+dst[1]*dst[1]);
// 4-point combination found
               if(AMSBeta::_addnext(patb,4,sleng,phit,ptrack,theta,chi2space))
                 goto nexttrack;
               continue;
             }
           }
         }
       }
nexttrack: 
       continue;
     }
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
//----> recover 1-sided TOFRawCluster/TOFClusters using track info 
    int nh;
    integer status;
    if(!ptrack->checkstatus(AMSDBc::FalseTOFX)){ 
     for(nh=0;nh<nhit;nh++){
      status=pthit[nh]->getstatus();
      if((status&SCBADB2)!=0 && (status&SCBADB5)!=0){//tempor  use now only TOF-recovered
        pbeta->setstatus(AMSDBc::RECOVERED);
        pthit[nh]->recovers(ptrack);
      }
     }
    }
//---->
    pbeta->SimpleFit(nhit, sleng);
    if(pbeta->getchi2()< BETAFITFFKEY.Chi2 && fabs(pbeta->getbeta())<1.41){
      // Mark Track as used
         if(ptrack->checkstatus(AMSDBc::USED)) 
           ptrack->setstatus(AMSDBc::AMBIG);
         if(ptrack->checkstatus(AMSDBc::RELEASED)) 
           ptrack->setstatus(AMSDBc::AMBIG);
         ptrack->setstatus(AMSDBc::USED);
      // Mark TOF hits as USED
         int i;
         for( i=0;i<nhit;i++){
           if(pthit[i]->checkstatus(AMSDBc::USED)) 
             pthit[i]->setstatus(AMSDBc::AMBIG);
           if(pthit[i]->checkstatus(AMSDBc::RELEASED))
             pthit[i]->setstatus(AMSDBc::AMBIG);
           pthit[i]->setstatus(AMSDBc::USED);
         }

/* RELEASE hits for 1234, 123, 234 patters if beta is too low */
/* and wait for use with pattern 23 */
         if(fabs(pbeta->getbeta()) < BETAFITFFKEY.LowBetaThr && pat !=7){
           //release track
           ptrack->clearstatus(AMSDBc::USED);
           ptrack->setstatus(AMSDBc::RELEASED);
           // release hits if pat # 7 and low beta
           for( i=0;i<nhit;i++){
            if(pthit[i]->getntof() ==2)pthit[i]->clearstatus(AMSDBc::USED);
            if(pthit[i]->getntof() ==3)pthit[i]->clearstatus(AMSDBc::USED);
            if(pthit[i]->getntof() ==2)pthit[i]->setstatus(AMSDBc::RELEASED);
            if(pthit[i]->getntof() ==3)pthit[i]->setstatus(AMSDBc::RELEASED);
           }
          // set AMBIG flag on beta here if pat = 0,1 or 4
           if(pat==0 || pat==1 || pat==4)pbeta->setstatus(AMSDBc::AMBIG);
         }                  

/* Set the ambiguity bit for tracks sharing the same TOF hits */
/* This is necessary for FalseTOFX tracks */
         for (i=0;i<nhit;i++){
           if (pthit[i]->checkstatus(AMSDBc::AMBIG)) {
             pbeta->setstatus(AMSDBc::AMBIG);
             break;  
           }
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
 if(nhit>2)_Chi2=_Chi2/(nhit-2);
 else _Chi2=0;
 _InvErrBeta=sqrt(1./e2)/sqrt(x2-xa*xa);   
 //if(_Chi2>0)_InvErrBeta*=sqrt(_Chi2);
}




void AMSBeta::_writeEl(){
  BetaNtuple* BN = AMSJob::gethead()->getntuple()->Get_beta();

  if (BN->Nbeta>=MAXBETA) return;

// fill the ntuple 
  BN->Status[BN->Nbeta]=_status;
  BN->Pattern[BN->Nbeta]=_Pattern;  
  BN->Beta[BN->Nbeta]=_Beta;
  BN->Error[BN->Nbeta]=_InvErrBeta;
  BN->Chi2[BN->Nbeta]=_Chi2;
  BN->Chi2S[BN->Nbeta]=_Chi2Space;
  if(_Pattern ==0)BN->NTOF[BN->Nbeta]=4;
  else if(_Pattern < 5)BN->NTOF[BN->Nbeta]=3;
  else BN->NTOF[BN->Nbeta]=2;
  for(int k=BN->NTOF[BN->Nbeta];k<4;k++)BN->pTOF[BN->Nbeta][k]=0;
  for(k=0;k<BN->NTOF[BN->Nbeta];k++){
    BN->pTOF[BN->Nbeta][k]=_pcluster[k]->getpos();
    int i,pat;
    pat=_pcluster[k]->getntof()-1;
    if(AMSTOFCluster::Out(IOPA.WriteAll%10==1)){
      // Writeall
      for(i=0;i<pat;i++){
        AMSContainer *pc=AMSEvent::gethead()->getC("AMSTOFCluster",i);
         #ifdef __AMSDEBUG__
          assert(pc != NULL);
         #endif
         BN->pTOF[BN->Nbeta][k]+=pc->getnelem();
      }
    }                                                        
    else {
    //WriteUsedOnly
      for(i=0;i<pat;i++){
        AMSTOFCluster *ptr=(AMSTOFCluster*)AMSEvent::gethead()->getheadC("AMSTOFCluster",i);
          while(ptr && ptr->checkstatus(AMSDBc::USED)){
            BN->pTOF[BN->Nbeta][k]++;
            ptr=ptr->next();
          }
      }
    }



    BN->pTr[BN->Nbeta]=_ptrack->getpos();
    pat=_ptrack->getpattern();
    if(AMSTrTrack::Out(IOPA.WriteAll%10==1)){
      // Writeall
    
       for(i=0;i<pat;i++){
        AMSContainer *pc=AMSEvent::gethead()->getC("AMSTrTrack",i);
        #ifdef __AMSDEBUG__
         assert(pc != NULL);
        #endif
        BN->pTr[BN->Nbeta]+=pc->getnelem();
       }
    }
    else {
      //WriteUsedOnly
       for(i=0;i<pat;i++){
        AMSTrTrack *ptr=(AMSTrTrack*)AMSEvent::gethead()->getheadC("AMSTrTrack",i);
         while(ptr && ptr->checkstatus(AMSDBc::USED)){ 
           BN->pTr[BN->Nbeta]++;
           ptr=ptr->next();
         }
       }
    }


  }

  BN->Nbeta++;
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
