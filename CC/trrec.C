// Author V. Choutko 24-may-1996
//
// Mar 20, 1997. ak. check if Pthit != NULL in AMSTrTrack::Fit
//
// Last Edit : Mar 20, 1997. ak
// 
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
#include <cont.h>
#include <tkdbc.h>
#include <trigger3.h>







integer AMSTrTrack::_RefitIsNeeded=0;

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
                                      1,4,5,6,0,0,   // 1456   16
                                      2,3,4,5,0,0,   // 2345   17
                                      2,3,4,6,0,0,   // 2346   18
                                      2,3,5,6,0,0,   // 2356   19
                                      2,4,5,6,0,0,   // 2456   20
                                      3,4,5,6,0,0,   // 3456   21
                                      1,2,3,0,0,0,   // 123    22
                                      1,2,4,0,0,0,   // 124    23
                                      1,2,5,0,0,0,   // 125    24
                                      1,2,6,0,0,0,   // 126    25
                                      1,3,4,0,0,0,   // 134    26
                                      1,3,5,0,0,0,   // 135    27
                                      1,3,6,0,0,0,   // 136    28
                                      1,4,5,0,0,0,   // 145    29
                                      1,4,6,0,0,0,   // 146    30
                                      1,5,6,0,0,0,   // 156    31
                                      2,3,4,0,0,0,   // 234    32
                                      2,3,5,0,0,0,   // 235    33
                                      2,3,6,0,0,0,   // 236    34
                                      2,4,5,0,0,0,   // 245    35
                                      2,4,6,0,0,0,   // 246    36
                                      2,5,6,0,0,0,   // 256    37
                                      3,4,5,0,0,0,   // 345    38
                                      3,4,6,0,0,0,   // 346    39
                                      3,5,6,0,0,0,   // 356    40
                                      4,5,6,0,0,0    // 456    41
                                      };  
integer AMSTrTrack::patmiss[npat][6]={0,0,0,0,0,0,   // 123456  0
                                      0,0,0,0,0,5,   // 12346   1
                                      0,0,0,0,0,4,   // 12356   2
                                      0,0,0,0,0,3,   // 12456   3
                                      0,0,0,0,0,2,   // 13456   4
                                      0,0,0,0,0,6,   // 12345   5
                                      0,0,0,0,0,1,   // 23456   6
                                      0,0,0,0,5,6,   // 1234    7
                                      0,0,0,0,4,6,   // 1235    8
                                      0,0,0,0,4,5,   // 1236    9
                                      0,0,0,0,3,6,   // 1245   10
                                      0,0,0,0,3,5,   // 1246   11
                                      0,0,0,0,3,4,   // 1256   12
                                      0,0,0,0,2,6,   // 1345   13 
                                      0,0,0,0,2,5,   // 1346   14
                                      0,0,0,0,2,4,   // 1356   15
                                      0,0,0,0,2,3,   // 1456   16
                                      0,0,0,0,1,6,   // 2345   17
                                      0,0,0,0,1,5,   // 2346   18
                                      0,0,0,0,1,4,   // 2356   19
                                      0,0,0,0,1,3,   // 2456   20
                                      0,0,0,0,1,2,   // 3456   21
                                      0,0,0,4,5,6,   // 123    22
                                      0,0,0,3,5,6,   // 124    23
                                      0,0,0,3,4,6,   // 125    24
                                      0,0,0,3,4,5,   // 126    25
                                      0,0,0,2,5,6,   // 134    26
                                      0,0,0,2,4,6,   // 135    27
                                      0,0,0,2,4,5,   // 136    28
                                      0,0,0,2,3,6,   // 145    29
                                      0,0,0,2,3,5,   // 146    30
                                      0,0,0,2,3,4,   // 156    31
                                      0,0,0,1,5,6,   // 234    32
                                      0,0,0,1,4,6,   // 235    33
                                      0,0,0,1,4,5,   // 236    34
                                      0,0,0,1,3,6,   // 245    35
                                      0,0,0,1,3,5,   // 246    36
                                      0,0,0,1,3,4,   // 256    37
                                      0,0,0,1,2,6,   // 345    38
                                      0,0,0,1,2,5,   // 346    39
                                      0,0,0,1,2,4,   // 356    40
                                      0,0,0,1,2,3    // 456    41
                                      };  
integer AMSTrTrack::patpoints[npat]=
              {6,5,5,5,5,5,5,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,3,3,3,3,3,3,
               3,3,3,3,3,3,3,3,3,3,3,3,3,3};


integer AMSTrCluster::build(integer refit){
  AMSlink * OriginalLast[2];
  integer OriginalNumber[2];
  AMSContainer * pct[2];
  geant Thr2R[2];
  integer ThrClNel[2];
  if(refit){
    AMSgObj::BookTimer.start("TrClusterRefit");
    // prepare some elements
    int i;
    for (i=0;i<2;i++){
      pct[i]=AMSEvent::gethead()->getC("AMSTrCluster",i);
      #ifdef __AMSDEBUG__
        assert(pct[i]!=NULL);
      #endif
      OriginalNumber[i]=pct[i]->getnelem();
      OriginalLast[i]=pct[i]->getlast();
      Thr2R[i]=TRCLFFKEY.Thr2R[i];
      ThrClNel[i]=TRCLFFKEY.ThrClNEl[i];
      TRCLFFKEY.Thr2R[i]=min((number)3.,Thr2R[i]*3.);
      TRCLFFKEY.ThrClNEl[i]=min(5,ThrClNel[i]+2);
    }
  }
 
  integer size=(AMSDBc::maxstripsdrp()+1+
  2*max(TRCLFFKEY.ThrClNEl[0],TRCLFFKEY.ThrClNEl[1]))*sizeof(number);
  number *  adc  = (number*)UPool.insert(size); 
  AMSTrIdSoft id;
  for(int icll=0;icll<2;icll++){
   AMSTrRawCluster *p=(AMSTrRawCluster*)AMSEvent::gethead()->
   getheadC("AMSTrRawCluster",icll,1);
   VZERO(adc,size/4);
   while(p){
    // Unpack cluster into tmp space
    // find clusters
     id=AMSTrIdSoft(p->getid());
     integer ilay=id.getlayer();
     integer side=id.getside();
     // Very debug
     //          if(ilay==3 && side==0){p=p->next();continue;}
     p->expand(adc+TRCLFFKEY.ThrClNEl[side]/2);
    if(p->testlast()){
              
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
      status=AMSDBc::WIDE;
      left=max(side==1?TRCLFFKEY.ThrClNEl[side]/2:0,
      i-TRCLFFKEY.ThrClNEl[side]/2);
      center=i;
      right=min(i+TRCLFFKEY.ThrClNEl[side]/2,
            id.getmaxstrips()+(side==1?1:2)*(TRCLFFKEY.ThrClNEl[side]/2)-1);  
      } 
      else if(adc[i+1]<adc[i+2] && adc[i+2]>TRCLFFKEY.Thr1A[side]){
       // two clusters near each other; take care about rightmost strip;
      status=AMSDBc::NEAR;
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
       if(!id.checkstatus(AMSDBc::BAD) && 
          adc[j]/id.getsig()>TRCLFFKEY.Thr3R[side]){
       }
       else adc[j]=0;
      }  

      for (j=left;j<center;j++){
       id.upd(j-TRCLFFKEY.ThrClNEl[side]/2);
        if(j-center< -1 && j-left==0 &&  
           (id.checkstatus(AMSDBc::BAD) ||adc[j]/id.getsig()<TRCLFFKEY.Thr2R[side])) {
          left++;
          continue;
        }
        if(j-center< 0 && j-left==0 &&
           (id.checkstatus(AMSDBc::BAD) ||adc[j]/id.getsig()<max(1.,TRCLFFKEY.Thr2R[side]/3.))){
          left++;
          continue;
        }
      }
      integer ro=right;
      for (j=right;j>center;j--){
       id.upd(j-TRCLFFKEY.ThrClNEl[side]/2);
        if(j-center> 1 && j-right==0 &&  
           (id.checkstatus(AMSDBc::BAD) ||adc[j]/id.getsig()<TRCLFFKEY.Thr2R[side])) {
          right--;
          continue;
        }
        if(j-center> 0 && j-right==0 &&
           (id.checkstatus(AMSDBc::BAD) ||adc[j]/id.getsig()<max(1.,TRCLFFKEY.Thr2R[side]/3.))){
          right--;
          continue;
        }
      }

      for (j=left;j<right+1;j++){
       id.upd(j-TRCLFFKEY.ThrClNEl[side]/2);
       if(adc[j]>TRCLFFKEY.Thr2A[side])above++;
        if(j==ro+1 && status==AMSDBc::NEAR)sum+=adc[j]/2;
        else sum+=adc[j];
//        ssum=ssum+pow(id.getsig(),2.);
        ssum=ssum+id.getsig()*id.getsig();
        pos=pos+1*(j-center)*adc[j];
//        rms=rms+pow(1*(j-center),2)*adc[j];
        rms=rms+(j-center)*(j-center)*adc[j];
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
         if(right-left+1 > TRCLFFKEY.ThrClNEl[side]){
           if(adc[left]>adc[right])right--;
           else left++;
         }
         _addnext(
         id,status,left-center,right-center+1,sum,ssum,pos,rms,adc+left);
           for (int j=left;j<right+1;j++){
             if(j==right+1 && status==AMSDBc::NEAR)adc[j]=adc[j]/2;
             else adc[j]=0;
           }
      }                      
     }
     else{
      if(adc[i] > TRCLFFKEY.Thr1A[side]){
       // susp bump found
       id.upd(i-TRCLFFKEY.ThrClNEl[side]/2);
       if(id.checkstatus(AMSDBc::BAD)==0 && 
          id.getsig() < TRCLFFKEY.Thr1S[side] && 
          adc[i]/id.getsig() > TRCLFFKEY.Thr1R[side] )ref=adc[i];
      }
     }
     } 
     VZERO(adc,size/4);
    } 
     p=p->next();           
  }
  }
  UPool.udelete(adc);

  if(refit){
     int i;
     for (i=0;i<2;i++){
       pct[i]=AMSEvent::gethead()->getC("AMSTrCluster",i);
      #ifdef __AMSDEBUG__
        assert(pct[i]!=NULL);
      #endif

       // Restore thresholds

       TRCLFFKEY.Thr2R[i]=Thr2R[i];
       TRCLFFKEY.ThrClNEl[i]=ThrClNel[i];

       // Mark all new clusters to delete

       AMSTrCluster * pclnew=((AMSTrCluster*)OriginalLast[i])->next();
       while(pclnew){
         pclnew->setstatus(AMSDBc::DELETED);
         pclnew=pclnew->next();
       }

       // Find & mark corresponding elements

       AMSTrCluster *pcl=(AMSTrCluster*)pct[i]->gethead();
       while(pcl){
         if(pcl->checkstatus(AMSDBc::REFITTED)){
          pclnew=((AMSTrCluster*)OriginalLast[i])->next();
          AMSTrIdSoft pclid=pcl->getid();
          while(pclnew){
            if(pclnew->getid() == pclid){
              // mark
              pcl->setstatus(AMSDBc::DELETED);
              pclnew->clearstatus(AMSDBc::DELETED);
              pclnew->setstatus(AMSDBc::REFITTED);
            }     
            pclnew=pclnew->next();
          }
         }
         pcl=pcl->next();
       } 
       // Delete marked clusters
      pcl=(AMSTrCluster*)pct[i]->gethead();
      while(pcl && pcl->checkstatus(AMSDBc::DELETED)){
        pct[i]->removeEl(0,0);
        pcl=(AMSTrCluster*)pct[i]->gethead(); 
      }     
      while(pcl){
        while(pcl->next() && (pcl->next())->checkstatus(AMSDBc::DELETED))
        pct[i]->removeEl(pcl,0);
        pcl=pcl->next();
      }
      // Restore positions
     AMSlink * ptmp=pct[i]->gethead();
     integer ip=1;
     while(ptmp){
      ptmp->setpos(ip++);
      ptmp=ptmp->_next;
     }
     }
    AMSgObj::BookTimer.stop("TrClusterRefit");
  }
  return 1;
}



integer AMSTrCluster::buildWeak(integer refit){
  AMSlink * OriginalLast[2];
  integer OriginalNumber[2];
  AMSContainer * pct[2];
  geant Thr2R[2];
  integer ThrClNel[2];
  if(refit){
   return -1;
  }
 
  integer size=(AMSDBc::maxstripsdrp()+1+
  2*max(TRCLFFKEY.ThrClNEl[0],TRCLFFKEY.ThrClNEl[1]))*sizeof(number);
  number *  adc  = (number*)UPool.insert(size); 
  AMSTrIdSoft id;
  // only x clusters can be weak
  for(int icll=0;icll<2;icll++){
   AMSTrRawCluster *p=(AMSTrRawCluster*)AMSEvent::gethead()->
   getheadC("AMSTrRawCluster",icll,1);
   VZERO(adc,size/4);
   while(p){
    // Unpack cluster into tmp space
    // find clusters
     id=AMSTrIdSoft(p->getid());
     integer ilay=id.getlayer(); 
     integer side=id.getside();
     if(side==0){
     // Very debug
     //         if(ilay==3 && side==0){p=p->next();continue;}
     p->expand(adc+TRCLFFKEY.ThrClNEl[side]/2);
    if(p->testlast()){

              
     // Circle buffer for x layers 1 && 6;
     // 
     if(side==0 && (ilay==1 || ilay==6)){
       for (int iloc=0;iloc<TRCLFFKEY.ThrClNEl[side]/2;iloc++){
        adc[iloc]=adc[id.getmaxstrips()+iloc];
        adc[iloc+id.getmaxstrips()+TRCLFFKEY.ThrClNEl[side]/2]=
        adc[iloc+TRCLFFKEY.ThrClNEl[side]/2];
       }  
     }

           // first (Unfortunately) find out and destroy all " normal clusters"
     {
     

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
      status=AMSDBc::WIDE;
      left=max(side==1?TRCLFFKEY.ThrClNEl[side]/2:0,
      i-TRCLFFKEY.ThrClNEl[side]/2);
      center=i;
      right=min(i+TRCLFFKEY.ThrClNEl[side]/2,
            id.getmaxstrips()+(side==1?1:2)*(TRCLFFKEY.ThrClNEl[side]/2)-1);  
      } 
      else if(adc[i+1]<adc[i+2] && adc[i+2]>TRCLFFKEY.Thr1A[side]){
       // two clusters near each other; take care about rightmost strip;
      status=AMSDBc::NEAR;
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
       if(!id.checkstatus(AMSDBc::BAD) && 
          adc[j]/id.getsig()>TRCLFFKEY.Thr3R[side]){
       }
       else adc[j]=0;
      }  

      for (j=left;j<center;j++){
       id.upd(j-TRCLFFKEY.ThrClNEl[side]/2);
        if(j-center< -1 && j-left==0   ) {
          left++;
          continue;
        }
        if(j-center< 0 && j-left==0 &&
           (id.checkstatus(AMSDBc::BAD) ||adc[j]/id.getsig()<max(1.,TRCLFFKEY.Thr2R[side]/3.))){
          left++;
          continue;
        }
      }
      integer ro=right;
      for (j=right;j>center;j--){
       id.upd(j-TRCLFFKEY.ThrClNEl[side]/2);
        if(j-center> 1 && j-right==0 ) {
          right--;
          continue;
        }
        if(j-center> 0 && j-right==0 &&
           (id.checkstatus(AMSDBc::BAD) ||adc[j]/id.getsig()<max(1.,TRCLFFKEY.Thr2R[side]/3.))){
          right--;
          continue;
        }
      }

      for (j=left;j<right+1;j++){
       id.upd(j-TRCLFFKEY.ThrClNEl[side]/2);
       if(adc[j]>TRCLFFKEY.Thr2A[side])above++;
        if(j==ro+1 && status==AMSDBc::NEAR)sum+=adc[j]/2;
        else sum+=adc[j];
//        ssum=ssum+pow(id.getsig(),2.);
        ssum=ssum+id.getsig()*id.getsig();
        pos=pos+1*(j-center)*adc[j];
//        rms=rms+pow(1*(j-center),2)*adc[j];
        rms=rms+(j-center)*(j-center)*adc[j];
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
         if(right-left+1 > TRCLFFKEY.ThrClNEl[side]){
           if(adc[left]>adc[right])right--;
           else left++;
         }
           for (int j=left;j<right+1;j++){
             if(j==right+1 && status==AMSDBc::NEAR)adc[j]=adc[j]/2;
             else adc[j]=0;
           }
      }                      
     }
     else{
      if(adc[i] > TRCLFFKEY.Thr1A[side]){
       // susp bump found
       id.upd(i-TRCLFFKEY.ThrClNEl[side]/2);
       if(id.checkstatus(AMSDBc::BAD)==0 && 
          id.getsig() < TRCLFFKEY.Thr1S[side] && 
          adc[i]/id.getsig() > TRCLFFKEY.Thr1R[side] )ref=adc[i];
      }
     }
     } 




     }
     // Now find "weak clusters"
     {




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
      status= AMSDBc::WIDE;
      left=max(side==1?TRCLFFKEY.ThrClNEl[side]/2:0,
      i-TRCLFFKEY.ThrClNEl[side]/2);
      center=i;
      right=min(i+TRCLFFKEY.ThrClNEl[side]/2,
            id.getmaxstrips()+(side==1?1:2)*(TRCLFFKEY.ThrClNEl[side]/2)-1);  
      } 
      else if(adc[i+1]<adc[i+2] && adc[i+2]>TRCLFFKEY.Thr1A[side]){
       // two clusters near each other; take care about rightmost strip;
      status=AMSDBc::NEAR;
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
       if(!id.checkstatus(AMSDBc::BAD) && 
          adc[j]/id.getsig()>TRCLFFKEY.Thr3R[side]){
        if(j-center<= -TRCLFFKEY.ThrClNEl[side]/2 && 
           adc[j]/id.getsig()<TRCLFFKEY.Thr2R[side]){
           left++;
           j++;
           id.upd(j-TRCLFFKEY.ThrClNEl[side]/2);
           if(j-center< 0 && 
           adc[j]/id.getsig()<max(1.,TRCLFFKEY.Thr2R[side]/3.)){
            left++;
            continue;
           }
        }
        if(j-center>= TRCLFFKEY.ThrClNEl[side]/2 && 
           adc[j]/id.getsig()<TRCLFFKEY.Thr2R[side]){
           right--;
           id.upd(j-1-TRCLFFKEY.ThrClNEl[side]/2);
           if(j-1-center> 0 && 
           adc[j-1]/id.getsig()<max(1.,TRCLFFKEY.Thr2R[side]/3.)){
            right--;
           }
           
           continue;
        }
       if(adc[j]>TRCLFFKEY.Thr2A[side])above++;
        if(j==right+1 && status==AMSDBc::NEAR)sum+=adc[j]/2;
        else sum+=adc[j];
//        ssum=ssum+pow(id.getsig(),2.);
        ssum=ssum+id.getsig()*id.getsig();
        pos=pos+1*(j-center)*adc[j];
//        rms=rms+pow(1*(j-center),2)*adc[j];
        rms=rms+(j-center)*(j-center)*adc[j];
       }
       else adc[j]=0;

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
         if(right-left+1 > TRCLFFKEY.ThrClNEl[side]){
           if(adc[left]>adc[right])right--;
           else left++;
         }
         status=status | AMSDBc::WEAK;
         if(id.getsig()!=0 )_addnext(
         id,status,left-center,right-center+1,sum,ssum,pos,rms,adc+left);
         status=status &  (~AMSDBc::WEAK);
           for (int j=left;j<right+1;j++){
             if(j==right+1 && status==AMSDBc::NEAR)adc[j]=adc[j]/2;
             else adc[j]=0;
           }
      }                      
     }
     else{
      if(adc[i] > TRCLFFKEY.Thr1A[side]){
       // susp bump found
       id.upd(i-TRCLFFKEY.ThrClNEl[side]/2);
       if(id.checkstatus(AMSDBc::BAD)==0 && 
          id.getsig() < TRCLFFKEY.Thr1S[side] && 
          adc[i]/id.getsig() > TRMCFFKEY.thr1R[side] )ref=adc[i];
      }
     }
     }







     }


 
     VZERO(adc,size/4);
    }
     } 
     p=p->next();           
   }
  }
  UPool.udelete(adc);


    return 1;
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
if(eval > 0){
cofg=cofg/eval;
smt=smax/eval;
}
else{
#ifdef __AMSDEBUG__
  cerr <<"AMSTrCluster::getcofg-E-NegativeClusterAmplitudeFound "<<eval<<endl;
#endif
cofg=0;
smt=0;
}
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
#if 1
  // Temporary
integer side=_Id.getside();
if(side==1)_ErrorMean =TRCLFFKEY.ErrY;
else  _ErrorMean =TRCLFFKEY.ErrX;

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
    AMSlink(status,0),_Id(id),_NelemL(nelemL), _NelemR(nelemR),_Sum(sum), 
    _Sigma(ssum), _Mean(pos), _Rms(rms){
 _ErrorCalc();
 if(getnelem()>0){
  _pValues=(number*)UPool.insert(getnelem()*sizeof(number));
  int i;
  for(i=0;i<getnelem();i++)_pValues[i]=val[i];
 }
}

void AMSTrCluster::_writeEl(){
TrClusterNtuple* TrN = AMSJob::gethead()->getntuple()->Get_trcl();

  if (TrN->Ntrcl>=MAXTRCL) return;

// Fill the ntuple : check on IOPA.WriteAll%10 
  integer flag =    (IOPA.WriteAll%10==1)
                 || (IOPA.WriteAll%10==0 && checkstatus(AMSDBc::USED))
                 || (IOPA.WriteAll%10==2 && !checkstatus(AMSDBc::AwayTOF));

  if(AMSTrCluster::Out(flag) ){
    TrN->Idsoft[TrN->Ntrcl]=_Id.cmpt();
    TrN->Status[TrN->Ntrcl]=_status;
    TrN->NelemL[TrN->Ntrcl]=_NelemL;
    TrN->NelemR[TrN->Ntrcl]=_NelemR;
    TrN->Sum[TrN->Ntrcl]=_Sum;
    TrN->Sigma[TrN->Ntrcl]=_Sigma;
    TrN->Mean[TrN->Ntrcl]=_Mean;
    TrN->RMS[TrN->Ntrcl]=_Rms;
    TrN->ErrorMean[TrN->Ntrcl]=_ErrorMean;
    for(int i=0;i<min(5,getnelem());i++)TrN->Amplitude[TrN->Ntrcl][i]=_pValues[i]; 
    for(i=min(5,getnelem());i<5;i++)TrN->Amplitude[TrN->Ntrcl][i]=0;
    TrN->Ntrcl++;
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


integer AMSTrRecHit::build(integer refit){
  if(refit){
    // Cleanup all  containers
    int i;
    for(i=0;;i++){
      AMSContainer *pctr=AMSEvent::gethead()->getC("AMSTrRecHit",i);
      if(pctr)pctr->eraseC();
      else break ;
    }
  } 
 AMSTrCluster *x;
 AMSTrCluster *y;
 AMSTrIdSoft idx;
 AMSTrIdSoft idy;
 integer ia,ib,ilay,nambig;
 y=(AMSTrCluster*)AMSEvent::gethead()->getheadC("AMSTrCluster",1,0); 
 while (y){
   idy=y->getid();
   ilay=idy.getlayer();
   if(y->checkstatus(AMSDBc::BAD)==0){
   x=(AMSTrCluster*)AMSEvent::gethead()->getheadC("AMSTrCluster",0);
   while(x){
      idx=x->getid();  
      if(x->checkstatus(AMSDBc::BAD) ==0 ){
      if(idx.getlayer() == idy.getlayer() && 
         idx.getdrp() == idy.getdrp() && idx.gethalf() == idy.gethalf()
         && fabs(y->getVal()-x->getVal())/(y->getVal()+x->getVal()) < 
         TRCLFFKEY.ThrDSide){
        // make MANY points.... Unfortunately...
         AMSTrIdGeom *pid = idx.ambig(idy, nambig);
         for(int i=0;i<nambig;i++){
         AMSgSen *p;
         p=(AMSgSen*)AMSJob::gethead()->getgeomvolume((pid+i)->crgid());
         if(!p){
           if(TKDBc::GetStatus((pid+i)->getlayer()-1,(pid+i)->getladder()-1,
           (pid+i)->getsensor()-1))
           cerr << "AMSTrRecHitBuild-S-Sensor-Error "<<
             (pid+i)->crgid()<<endl;          
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

// Mark hits away from TOF predictions
 if (TRFITFFKEY.TOFTracking!=0) markAwayTOFHits();

 return 1;

}



integer AMSTrRecHit::buildWeak(integer refit){
  if(refit){
   return -1;
  } 
 AMSTrCluster *x;
 AMSTrCluster *y;
 AMSTrIdSoft idx;
 AMSTrIdSoft idy;
 integer ia,ib,ilay,nambig;
 y=(AMSTrCluster*)AMSEvent::gethead()->getheadC("AMSTrCluster",1,0); 
 while (y){
   idy=y->getid();
   ilay=idy.getlayer();
   if(y->checkstatus(AMSDBc::BAD)==0 && 
      y->checkstatus(AMSDBc::WEAK)==0){
   x=(AMSTrCluster*)AMSEvent::gethead()->getheadC("AMSTrCluster",0);
   while(x){
      idx=x->getid();  
      if(x->checkstatus(AMSDBc::BAD) ==0 &&
       x->checkstatus(AMSDBc::WEAK)){
      if(idx.getlayer() == idy.getlayer() && 
         idx.getdrp() == idy.getdrp() && idx.gethalf() == idy.gethalf()
         && fabs(y->getVal()-x->getVal())/(y->getVal()+x->getVal()) < 
         TRCLFFKEY.ThrDSide){
        // make MANY points.... Unfortunately...
         AMSTrIdGeom *pid = idx.ambig(idy, nambig);
         for(int i=0;i<nambig;i++){
         AMSgSen *p;
         p=(AMSgSen*)AMSJob::gethead()->getgeomvolume((pid+i)->crgid());
         if(!p){
           if(TKDBc::GetStatus((pid+i)->getlayer()-1,(pid+i)->getladder()-1,
           (pid+i)->getsensor()-1))
           cerr << "AMSTrRecHitBuild-S-Sensor-Error "<<
             (pid+i)->crgid()<<endl;          
         }
         else{
           //           cout <<" rec hit weak added "<<endl;
          _addnext(p,AMSDBc::WEAK,ilay,x,y,
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

// Mark hits away from TOF predictions
 if (TRFITFFKEY.TOFTracking!=0) markAwayTOFHits();

 return 1;

}


/* This function checks if AMSTrRecHits are consistent with TOF information */
/* and marks them if it is not the case                                     */
integer AMSTrRecHit::markAwayTOFHits(){

    int i;

    TriggerLVL3 *plvl3;
    plvl3 = (TriggerLVL3*)AMSEvent::gethead()->getheadC("TriggerLVL3",0);
// LVL3 required if existing
    if (plvl3==NULL) {
// #ifdef __AMSDEBUG__
//       cout << "markAwayTOFHits: No Level3 Trigger existing" << endl;
// #endif
      return 1;
    }
    if ( plvl3->skip() ) return 1;

    AMSTOFCluster *phit[4], *ploop;

// There should be one and only one AMSTOFCluster on planes 1, 4
// according to LVL3 trigger
    phit[0] = AMSTOFCluster::gethead(0);
    if ( (phit[0] == NULL) || (phit[0]->next()) ) return 1;
    phit[3] = AMSTOFCluster::gethead(3);
    if ( (phit[3] == NULL) || (phit[3]->next()) ) return 1;

// Initial straight line from planes 1 and 4 for ZX projection
    number slope_x= (phit[3]->getcoo()[0] - phit[0]->getcoo()[0]) /
               (phit[3]->getcoo()[2] - phit[0]->getcoo()[2]);
    number intercept_x= phit[0]->getcoo()[0] - slope_x*phit[0]->getcoo()[2];

// Look for the best AMSTOFCluster on plane 2 within errors
// to improve the X prediction on the tracker
    number resmax2=999.;
    phit[1]=NULL;
    for (ploop = AMSTOFCluster::gethead(1); ploop ; ploop=ploop->next() ){
      number resx2 = fabs(ploop->getcoo()[0] 
                          - slope_x*ploop->getcoo()[2] - intercept_x);
      if (resx2<resmax2) {
        resmax2 = resx2;
        phit[1] = ploop;
      }
    }

// Look for the best AMSTOFCluster on plane 3 within errors
// to improve the X prediction
    number resmax3=999.;
    phit[2]=NULL;
    for (ploop = AMSTOFCluster::gethead(2); ploop ; ploop=ploop->next() ){
      number resx3 = fabs(ploop->getcoo()[0] 
                          - slope_x*ploop->getcoo()[2] - intercept_x);
      if (resx3<resmax3) {
        resmax3 = resx3;
        phit[2] = ploop;
      }
    }

// We require at least 3 AMSTOFClusters
    if ( (phit[1]==NULL) && (phit[2]==NULL) ) return 1;

// Straight line parameters for the ZX plane 
// Use only Pad information (planes2,3: TOF calib. independent)
// unless they are missing
    number sw=0, sz=0, sx=0, sxz=0, szz=0;
    for (i=0; i<4; i++){
      if (phit[i]==NULL) continue;
      if (i==0 && phit[1]!=NULL) continue;
      if (i==3 && phit[2]!=NULL) continue;
      number w = phit[i]->getecoo()[0]; if (w<=0.) continue; w = 1./w/w;
      number x = phit[i]->getcoo()[0];
      number z = phit[i]->getcoo()[2];
      sw += w;
      sx += x*w;
      sz += z*w;
      sxz += x*z*w;
      szz += z*z*w;
    }
    number determinant = szz*sw-sz*sz;
    slope_x = (sxz*sw-sx*sz)/determinant;
    intercept_x = (szz*sx-sz*sxz)/determinant;
    number covss = sw/determinant;
    number covsi = -sz/determinant;
    number covii = szz/determinant;

// Straight line parameters for the ZY plane 
// (Only planes 1,4 => TOFCalib independent, but robust and enough)
    number slope_y= (phit[3]->getcoo()[1] - phit[0]->getcoo()[1]) /
               (phit[3]->getcoo()[2] - phit[0]->getcoo()[2]);
    number intercept_y= phit[0]->getcoo()[1] - slope_y*phit[0]->getcoo()[2];

// First mark all TRClusters as "away from TOF" by default
    AMSTrCluster* pclus;
    for (i=0;i<1;i++) {
      pclus = (AMSTrCluster*)AMSEvent::gethead()->getheadC("AMSTrCluster",i);
      for (; pclus!=NULL; pclus=pclus->next()) {
        pclus->setstatus(AMSDBc::AwayTOF);
      }
    }

// Mark AMSTrRecHits which are outside the TOF path
    AMSTrRecHit * ptrhit;
    AMSPoint hit;
    geant searchregtof = TOFDBc::plnstr(5)+2.*TOFDBc::plnstr(13);
    for (i=0;i<6;i++) {
      for (ptrhit=AMSTrRecHit::gethead(i); ptrhit!=NULL; ptrhit=ptrhit->next()){
        hit = ptrhit->getHit();
        number xres = fabs(hit[0]-intercept_x - slope_x*hit[2]);
        number yres = fabs(hit[1]-intercept_y - slope_y*hit[2]);
        if (    xres > searchregtof
             || yres > searchregtof    ) {
          ptrhit->setstatus(AMSDBc::AwayTOF);
        }
        else {
          pclus = ptrhit->getClusterP(0);
          if (pclus) pclus->clearstatus(AMSDBc::AwayTOF);
          pclus = ptrhit->getClusterP(1);
          if (pclus) pclus->clearstatus(AMSDBc::AwayTOF);
        }
      }
    }
      return 0;

}






 void AMSTrRecHit::_addnext(AMSgSen * pSen, integer status, integer layer, AMSTrCluster *x,
                            AMSTrCluster * y, const AMSPoint & hit,
                            const AMSPoint & ehit){
    number s1=0,s2=0;
    if(x)s1=x->getVal();
    if(y)s2=y->getVal();
    if(x)x->setstatus(AMSDBc::USED);
    if(y)y->setstatus(AMSDBc::USED);
    AMSEvent::gethead()->addnext(AMSID("AMSTrRecHit",layer-1),
    new     AMSTrRecHit(pSen, status,layer,x,y,hit,ehit,s1+s2,(s1-s2)/(s1+s2)));
      
}


void AMSTrRecHit::_writeEl(){
 TrRecHitNtuple* THN = AMSJob::gethead()->getntuple()->Get_trrh();

  if (THN->Ntrrh>=MAXTRRH) return;

// Fill the ntuple 
  integer flag =    (IOPA.WriteAll%10==1)
                 || (IOPA.WriteAll%10==0 && checkstatus(AMSDBc::USED))
                 || (IOPA.WriteAll%10==2 && !checkstatus(AMSDBc::AwayTOF));

  if(AMSTrRecHit::Out(flag) ){
    if(_Xcl)THN->pX[THN->Ntrrh]=_Xcl->getpos();
    else THN->pX[THN->Ntrrh]=-1;
    THN->pY[THN->Ntrrh]=_Ycl->getpos();
    int pat;
    pat=1;
    if(AMSTrCluster::Out(IOPA.WriteAll%10==1)){
      // Writeall
      for(int i=0;i<pat;i++){
          AMSContainer *pc=AMSEvent::gethead()->getC("AMSTrCluster",i);
           #ifdef __AMSDEBUG__
            assert(pc != NULL);
           #endif
           THN->pY[THN->Ntrrh]+=pc->getnelem();
      }
    }                                                        
    else if (AMSTrCluster::Out(IOPA.WriteAll%10==0)){
      //Write only USED hits
      for(int i=0;i<pat;i++){
        AMSTrCluster *ptr=(AMSTrCluster*)AMSEvent::gethead()->getheadC("AMSTrCluster",i);
        while(ptr && ptr->checkstatus(AMSDBc::USED) ){
          THN->pY[THN->Ntrrh]++;
          ptr=ptr->next();
        }
      }
    }
    else if (AMSTrCluster::Out(IOPA.WriteAll%10==2)){
      //Write only hits consistent with TOF
      for(int i=0;i<pat;i++){
        AMSTrCluster *ptr=(AMSTrCluster*)AMSEvent::gethead()->getheadC("AMSTrCluster",i);
        while(ptr && !(ptr->checkstatus(AMSDBc::AwayTOF)) ){
          THN->pY[THN->Ntrrh]++;
          ptr=ptr->next();
        }
      }
    }
    else return;
  
    if(!checkstatus(AMSDBc::FalseX) && !checkstatus(AMSDBc::FalseTOFX) &&
        ((_Xcl->getid()).getlayer() != _Layer) || 
       ((_Ycl->getid()).getlayer() != _Layer) ){
      cerr << "AMSTrRecHit-S-Logic Error "<<(_Xcl->getid()).getlayer()<<" "<<
        (_Ycl->getid()).getlayer()<<" "<<_Layer<<endl;
    }
    THN->Status[THN->Ntrrh]=_status;
    THN->Layer[THN->Ntrrh]=_Layer;
    for(int i=0;i<3;i++)THN->Hit[THN->Ntrrh][i]=_Hit[i];
    for(i=0;i<3;i++)THN->EHit[THN->Ntrrh][i]=_EHit[i];
    THN->Sum[THN->Ntrrh]=_Sum;
    THN->DifoSum[THN->Ntrrh]=_DifoSum;
    THN->Ntrrh++;
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


integer AMSTrTrack::build(integer refit){
  integer NTrackFound=-1;
  // pattern recognition + fit
  if(refit){
    // Cleanup all track containers
    int i;
    for(i=0;;i++){
      AMSContainer *pctr=AMSEvent::gethead()->getC("AMSTrTrack",i);
      if(pctr)pctr->eraseC();
      else break ;
    }
  } 
  _RefitIsNeeded=0;
  _Start();
  // Add test here
   
  { 
    int xs=0; 
    for (int kk=0;kk<6;kk++){
    AMSTrRecHit * phit=AMSTrRecHit::gethead(kk);
    if(phit)xs++;
  }
    if(xs>3)AMSEvent::gethead()->addnext(AMSID("Test",0),new Test());
  }

  for (int pat=0;pat<npat;pat++){
    AMSTrRecHit * phit[6]={0,0,0,0,0,0};
    if(TRFITFFKEY.pattern[pat]){
      int fp=patpoints[pat]-1;    
      // Try to make StrLine Fit
      integer first=AMSTrTrack::patconf[pat][0]-1;
      integer second=AMSTrTrack::patconf[pat][fp]-1;
      phit[0]=AMSTrRecHit::gethead(first);
      number par[2][2];
      while( phit[0]){
       if(phit[0]->Good()){
       phit[fp]=AMSTrRecHit::gethead(second);
       while( phit[fp]){
        if(phit[fp]->Good()){
        par[0][0]=(phit[fp]-> getHit()[0]-phit[0]-> getHit()[0])/
               (phit[fp]-> getHit()[2]-phit[0]-> getHit()[2]);
        par[0][1]=phit[0]-> getHit()[0]-par[0][0]*phit[0]-> getHit()[2];
        par[1][0]=(phit[fp]-> getHit()[1]-phit[0]-> getHit()[1])/
               (phit[fp]-> getHit()[2]-phit[0]-> getHit()[2]);
        par[1][1]=phit[0]-> getHit()[1]-par[1][0]*phit[0]-> getHit()[2];
        if(NTrackFound<0)NTrackFound=0;
        // Search for others
        phit[1]=AMSTrRecHit::gethead(AMSTrTrack::patconf[pat][1]-1);
        while(phit[1]){
         if(phit[1]->Good()){
          // Check if the point lies near the str line
           if(AMSTrTrack::Distance(par,phit[1]))
           {phit[1]=phit[1]->next();continue;}
          if(AMSTrTrack::patpoints[pat] >3){         
         phit[2]=AMSTrRecHit::gethead(AMSTrTrack::patconf[pat][2]-1);
         while(phit[2]){
          // Check if the point lies near the str line
          if(phit[2]->Good()){
          if(AMSTrTrack::Distance(par,phit[2]))
          {phit[2]=phit[2]->next();continue;}
          if(AMSTrTrack::patpoints[pat] >4){         
          phit[3]=AMSTrRecHit::gethead(AMSTrTrack::patconf[pat][3]-1);
          while(phit[3]){
           if(phit[3]->Good()){
           if(AMSTrTrack::Distance(par,phit[3]))
           {phit[3]=phit[3]->next();continue;}
           if(AMSTrTrack::patpoints[pat]>5){
           phit[4]=AMSTrRecHit::gethead(AMSTrTrack::patconf[pat][4]-1);
           while(phit[4]){
             if(phit[4]->Good()){
              if(AMSTrTrack::Distance(par,phit[4]))
              {phit[4]=phit[4]->next();continue;}
                // 6 point combination found
              if(AMSTrTrack::_addnext(pat,6,phit)){
                  NTrackFound++;
                  goto out;
              }                
                
             }
            phit[4]=phit[4]->next();
           }
           }
           else{  // 5 points only
                // 5 point combination found
             if(AMSTrTrack::_addnext(pat,5,phit)){
                  NTrackFound++;
                  goto out;
             }
                
           }
           }
           phit[3]=phit[3]->next();
          }
          }
          else{       // 4 points only
                // 4 point combination found
                
                if(AMSTrTrack::_addnext(pat,4,phit)){
                  NTrackFound++;
                  goto out;
                }                
          }
          }
          phit[2]=phit[2]->next();
          }
          }
          else{       // 3 points only
                // 3 point combination found
                
                if(AMSTrTrack::_addnext(pat,3,phit)){
                  NTrackFound++;
                  goto out;
                }                

          }
         }
         phit[1]=phit[1]->next();
        }
        }         
        phit[fp]=phit[fp]->next();
       }
       }  
out:
       phit[0]=phit[0]->next();
      }
      
    }
  }
return NTrackFound;
}




integer AMSTrTrack::buildWeak(integer refit){
  integer NTrackFound=-1;
  // pattern recognition + fit
  if(refit){
   return NTrackFound;
  } 
  _RefitIsNeeded=0;
  _Start();
   

  for (int pat=7;pat<22;pat++){
    // Only 4 points patterns used
    AMSTrRecHit * phit[6]={0,0,0,0,0,0};
    if(TRFITFFKEY.pattern[pat]){
      int fp=patpoints[pat]-1;    
#ifdef __AMSDEBUG__
     assert (fp==3);
#endif 
      // Try to make StrLine Fit
      integer first=AMSTrTrack::patconf[pat][0]-1;
      integer second=AMSTrTrack::patconf[pat][fp]-1;
      phit[0]=AMSTrRecHit::gethead(first);
      number par[2][2];
      while( phit[0]){
       if(phit[0]->Good() && phit[0]->checkstatus(AMSDBc::WEAK)==0){
       phit[fp]=AMSTrRecHit::gethead(second);
       while( phit[fp]){
        if(phit[fp]->Good() && phit[fp]->checkstatus(AMSDBc::WEAK)==0){
        par[0][0]=(phit[fp]-> getHit()[0]-phit[0]-> getHit()[0])/
               (phit[fp]-> getHit()[2]-phit[0]-> getHit()[2]);
        par[0][1]=phit[0]-> getHit()[0]-par[0][0]*phit[0]-> getHit()[2];
        par[1][0]=(phit[fp]-> getHit()[1]-phit[0]-> getHit()[1])/
               (phit[fp]-> getHit()[2]-phit[0]-> getHit()[2]);
        par[1][1]=phit[0]-> getHit()[1]-par[1][0]*phit[0]-> getHit()[2];
        // Search for others
        if(NTrackFound<0)NTrackFound=0;
        phit[1]=AMSTrRecHit::gethead(AMSTrTrack::patconf[pat][1]-1);
        while(phit[1]){
         if(phit[1]->Good()){
          // Check if the point lies near the str line
           if(AMSTrTrack::Distance(par,phit[1]))
           {phit[1]=phit[1]->next();continue;}
         phit[2]=AMSTrRecHit::gethead(AMSTrTrack::patconf[pat][2]-1);
         while(phit[2]){
          // Check if the point lies near the str line
          if(phit[2]->Good()){
          if(AMSTrTrack::Distance(par,phit[2]))
          {phit[2]=phit[2]->next();continue;}
                // 4 point combination found
                
                if(AMSTrTrack::_addnext(pat,4,phit)){
                  NTrackFound++;
                  goto out;
                }                
          }
          phit[2]=phit[2]->next();
         }
         }
         phit[1]=phit[1]->next();
        }
        }         
        phit[fp]=phit[fp]->next();
       }
       }  
out:
       phit[0]=phit[0]->next();
      }
      
    }
  }
return NTrackFound;
}







integer AMSTrTrack::buildFalseX(integer patstart){
  integer NTrackFound=-1;
  // pattern recognition + fit
  _RefitIsNeeded=0;
  _Start();

  for (int pat=patstart;pat<npat;pat++){
    AMSTrRecHit * phit[6]={0,0,0,0,0,0};
    if(1  || TRFITFFKEY.pattern[pat]){
      int fp=patpoints[pat]-1;    
      // Try to make StrLine Fit
      integer first=AMSTrTrack::patconf[pat][0]-1;
      integer second=AMSTrTrack::patconf[pat][fp]-1;
      phit[0]=AMSTrRecHit::gethead(first);
      number par[2][2];
      while( phit[0]){
       if(phit[0]->Good() && phit[0]->checkstatus(AMSDBc::WEAK)==0){
       phit[fp]=AMSTrRecHit::gethead(second);
       while( phit[fp]){
        if(phit[fp]->Good() && phit[fp]->checkstatus(AMSDBc::WEAK)==0){
        par[0][0]=(phit[fp]-> getHit()[0]-phit[0]-> getHit()[0])/
               (phit[fp]-> getHit()[2]-phit[0]-> getHit()[2]);
        par[0][1]=phit[0]-> getHit()[0]-par[0][0]*phit[0]-> getHit()[2];
        par[1][0]=(phit[fp]-> getHit()[1]-phit[0]-> getHit()[1])/
               (phit[fp]-> getHit()[2]-phit[0]-> getHit()[2]);
        par[1][1]=phit[0]-> getHit()[1]-par[1][0]*phit[0]-> getHit()[2];
        // Search for others
        if(NTrackFound<0)NTrackFound=0;
        phit[1]=AMSTrRecHit::gethead(AMSTrTrack::patconf[pat][1]-1);
        while(phit[1]){
         if(phit[1]->Good()){
          // Check if the point lies near the str line
           if(AMSTrTrack::Distance(par,phit[1]))
           {phit[1]=phit[1]->next();continue;}
          // Check if the point lies near the str line
           if(AMSTrTrack::Distance(par,phit[1]))
           {phit[1]=phit[1]->next();continue;}
          if(AMSTrTrack::patpoints[pat] >3){         
         phit[2]=AMSTrRecHit::gethead(AMSTrTrack::patconf[pat][2]-1);
         while(phit[2]){
          // Check if the point lies near the str line
          if(phit[2]->Good()){
          if(AMSTrTrack::Distance(par,phit[2]))
          {phit[2]=phit[2]->next();continue;}
          if(AMSTrTrack::patpoints[pat] >4){         
          phit[3]=AMSTrRecHit::gethead(AMSTrTrack::patconf[pat][3]-1);
          while(phit[3]){
           if(phit[3]->Good()){
           if(AMSTrTrack::Distance(par,phit[3]))
           {phit[3]=phit[3]->next();continue;}
           if(AMSTrTrack::patpoints[pat]>5){
           phit[4]=AMSTrRecHit::gethead(AMSTrTrack::patconf[pat][4]-1);
           while(phit[4]){
             if(phit[4]->Good()){
              if(AMSTrTrack::Distance(par,phit[4]))
              {phit[4]=phit[4]->next();continue;}
                // 6 point combination found
              if(AMSTrTrack::_addnext(pat,6,phit)){
                  goto out;
              }                
                
             }
            phit[4]=phit[4]->next();
           }
           }
           else{  // 5 points only
                // 5 point combination found
             int f=AMSTrTrack::_addnextFalseX(pat,5,phit);
             if(f){
                  if(f>0)NTrackFound++;
                  goto out;
             }
                
           }
           }
           phit[3]=phit[3]->next();
          }
          }
          else{       // 4 points only
                // 4 point combination found
            int f=AMSTrTrack::_addnextFalseX(pat,4,phit);
                if(f){
                  if(f>0)NTrackFound++;
                  goto out;
                }                
          }
          }
          phit[2]=phit[2]->next();
          }
          }
          else{       // 3 points only
                // 3 point combination found
            int f= AMSTrTrack::_addnextFalseX(pat,3,phit);
                if(f){
                  if(f>0)NTrackFound++;
                  goto out;
                }                

          }
         }
         phit[1]=phit[1]->next();
        }
        }         
        phit[fp]=phit[fp]->next();
       }
       }  
out:
       phit[0]=phit[0]->next();
      }
      
    }
  }
return NTrackFound;
}









integer AMSTrTrack::Distance(number par[2][2], AMSTrRecHit *ptr){
const integer freq=10;
static integer trig=0;
trig=(trig+1)%freq;
           if(trig==0 && _CheckTime()>AMSFFKEY.CpuLimit){
            throw AMSTrTrackError(" Cpulimit Exceeded ");
           }

   return fabs(par[0][1]+par[0][0]*ptr->getHit()[2]-ptr->getHit()[0])/
            sqrt(1+par[0][0]*par[0][0]) > TRFITFFKEY.SearchRegStrLine ||
          fabs(par[1][1]+par[1][0]*ptr->getHit()[2]-ptr->getHit()[1])/
            sqrt(1+par[1][0]*par[1][0]) > TRFITFFKEY.SearchRegCircle;
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
          
       if( (  (ptrack->Fit(0) < 
            TRFITFFKEY.Chi2FastFit)) && ptrack->TOFOK()){
             ptrack->AdvancedFit();
         // permanently add;
#ifdef __UPOOL__
          ptrack=new AMSTrTrack(track);
#endif
          _addnextR(ptrack, pat, nhit, pthit);
          return 1;
       }
     }
    }
#ifndef __UPOOL__
    delete ptrack;  
#endif   
    return 0;
}




void AMSTrTrack::_addnextR(AMSTrTrack *ptrack, integer pat, integer nhit, AMSTrRecHit* pthit[6]){

         int i;
         // Mark hits as USED
         for( i=0;i<nhit;i++){
           if(pthit[i]->checkstatus(AMSDBc::USED))
            pthit[i]->setstatus(AMSDBc::AMBIG);
           else pthit[i]->setstatus(AMSDBc::USED);
           if(pthit[i]->checkstatus(AMSDBc::FalseX))
            ptrack->setstatus(AMSDBc::FalseX);
           if(pthit[i]->checkstatus(AMSDBc::WEAK))
            ptrack->setstatus(AMSDBc::WEAK);
           if(pthit[i]->checkstatus(AMSDBc::FalseTOFX))
            ptrack->setstatus(AMSDBc::FalseTOFX);
         }
          number dc[2];
          dc[0]=fabs(sin(ptrack->gettheta())*cos(ptrack->getphi()));
          dc[1]=fabs(sin(ptrack->gettheta())*sin(ptrack->getphi()));
          int n;
          for(n=0;n<2;n++){
           if(dc[n] > TRFITFFKEY.MinRefitCos[n]){
             for( i=0;i<nhit;i++){
              AMSTrCluster *pcl= pthit[i]->getClusterP(n);
              if(pcl)pcl->setstatus(AMSDBc::REFITTED);
              _RefitIsNeeded++;
             }
           }
          }
         // permanently add;
          AMSEvent::gethead()->addnext(AMSID("AMSTrTrack",pat),ptrack);
}





integer AMSTrTrack::_addnextFalseX(integer pat, integer nhit, AMSTrRecHit* pthit[6]){
AMSgObj::BookTimer.start("TrFalseX");
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
      if( (  (ptrack->Fit(0) < TRFITFFKEY.Chi2FalseX))
           && ptrack->TOFOK()){
        // Here we should add at least one point and fit 
        // First determine which planes are missed and interpolate to them,
        // find corresponding sensor ID
        // Then make additional  hit(s) and include them into fitting 
        // finally change pattern according to new track and
        // add "FalseX" bit into track status word

        {
          integer pointfound=0;
          for(int i=nhit;i<6;i++){
            integer ladder=8;
            integer sensor=5;
             AMSTrIdGeom id(patmiss[pat][i],ladder,sensor,0,0);
             AMSgvolume* p= AMSJob::gethead()->getgeomvolume(id.crgid());
            if(p){
              AMSPoint  P1;
                { 
                  AMSDir pntdir(0,0,1.);
                  AMSPoint pntplane(p->getcooA(0),p->getcooA(1),p->getcooA(2));
                  number theta,phi,length;
                  ptrack->interpolate(pntplane,pntdir,P1,theta,phi,length); 
                }
              
              AMSTrCluster * py=
                (AMSTrCluster*)AMSEvent::gethead()->getheadC("AMSTrCluster",1,0); 
              integer yfound=0;  
              while(py){
                AMSTrIdSoft idy=py->getid();
                if(idy.getlayer()==id.getlayer()){
                  if((P1[0]<0 && idy.gethalf()==0) || (P1[0]>0 && idy.gethalf()==1))yfound++;
                }
                py=py->next();
              }
              
              if(yfound){
                // Now 2nd pass : find corresponding ladder & sensor
                AMSgSen * pls=0;
                AMSPoint PS(p->getpar(0),p->getpar(1),p->getpar(2));
                if(id.FindAtt(P1,PS))pls=(AMSgSen*) AMSJob::gethead()->getgeomvolume(id.crgid());
                if(pls){
                  // Here Search for corresponding y-hit in the container 
                  AMSDir pntdir(pls->getnrmA(0,2),pls->getnrmA(1,2),pls->getnrmA(2,2));
                  AMSPoint pntplane(pls->getcooA(0),pls->getcooA(1),pls->getcooA(2));
                  number theta,phi,length;
                  ptrack->interpolate(pntplane,pntdir,P1,theta,phi,length);
                  // Now check for the corr y-hit
                  
                  
                  py=
                    (AMSTrCluster*)AMSEvent::gethead()->getheadC("AMSTrCluster",1,0); 
                  while(py){
                    AMSTrIdSoft idy=py->getid();
                    if(idy.getlayer()==id.getlayer() && abs(idy.getdrp()-id.getladder())<1){
                      if((P1[0]<0 && idy.gethalf()==0) || (P1[0]>0 && idy.gethalf()==1)){
                        //  Create False RawHit and put it in the corr container 
                        AMSPoint loc=pls->gl2loc(P1);
                        id.R2Gy(idy.getstrip());
                        AMSPoint hit=pls->str2pnt(loc[0]+PS[0],py->getcofg(1,&id)); 
                        AMSPoint Err(TRFITFFKEY.SearchRegStrLine,
                                     TRFITFFKEY.SearchRegStrLine,TRFITFFKEY.SearchRegStrLine);
                        if((hit-P1).abs() < Err){
                          AMSTrRecHit::_addnext(pls,AMSDBc::FalseX,id.getlayer(),0,py,hit,
                                                AMSPoint((number)TRCLFFKEY.ErrZ*2,py->getecofg(),(number)TRCLFFKEY.ErrZ));
                          pointfound++;
                        } 
                        
                      }
                    }
                    py=py->next();
                  }
                  if(1){
                    if(pointfound>0){
#ifndef __UPOOL__
                      delete ptrack;  
#endif   
                      AMSgObj::BookTimer.stop("TrFalseX");
                      return 1;
                    }
                    else if(TRFITFFKEY.pattern[pat]){
#ifdef __UPOOL__
                      ptrack=new AMSTrTrack(track);
#endif
                      _addnextR(ptrack, pat, nhit, pthit);
                      AMSgObj::BookTimer.stop("TrFalseX");
                      return -1;
                    }
                    else {
#ifndef __UPOOL__
                      delete ptrack;  
#endif   
                      AMSgObj::BookTimer.stop("TrFalseX");
                      return 0;
                    }
                  }
                }
              }
            }
#ifdef __AMSDEBUG__
            else  cerr <<" AMSTrTrack::_addnextFalseX-E-NoSensorsForPlaneFound "<<
                    patmiss[pat][i]<<endl;
#endif
            
          }
          if(0){
            if(pointfound>0){
#ifndef __UPOOL__
              delete ptrack;  
#endif   
              AMSgObj::BookTimer.stop("TrFalseX");
              return 1;
            }
            else if(TRFITFFKEY.pattern[pat]){
#ifdef __UPOOL__
              ptrack=new AMSTrTrack(track);
#endif
              _addnextR(ptrack, pat, nhit, pthit);
              AMSgObj::BookTimer.stop("TrFalseX");
              return -1;
            }
            else {
#ifndef __UPOOL__
              delete ptrack;  
#endif   
              AMSgObj::BookTimer.stop("TrFalseX");
              return 0;
            }
          }
        }
      }
     }
    }
#ifndef __UPOOL__
    delete ptrack;  
#endif   
    AMSgObj::BookTimer.stop("TrFalseX");
    return 0;
}





void AMSTrTrack::AdvancedFit(int forced){
   if( forced || _Ridgidity < 0){
    if(_Pattern <22){
      Fit(1);
      Fit(2);
    }
    Fit(3);
    Fit(4);
    Fit(5);
   }

}

integer AMSTrTrack::TOFOK(){
    if (TRFITFFKEY.UseTOF && (_Pattern > 6 || 
                              checkstatus(AMSDBc::FalseX) ||
                              checkstatus(AMSDBc::FalseTOFX))){
  //  if (TRFITFFKEY.UseTOF && (_Pattern == 17 || _Pattern > 21)){
  //   if (TRFITFFKEY.UseTOF ){
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
     if( ((phit->getcoo()-Res)/phit->getecoo()).abs()< SearchReg){
     if(phit->getntof() < 3)matched+=1;  
     else matched+=10;
     }
     phit=phit->next();
    }  
   }
   switch (TRFITFFKEY.UseTOF){
   case 0:
    return 1;
   case 1:
    if(matched/10 <1 && matched%10 <1)return 0;
    break;
   case 2:
    if(matched/10 <1 || matched%10<1)return 0;
    break;
   case 3:
    if(matched/10 <2 && matched%10<2)return 0;
    break;
   case 4:
    if(matched/10 <2 || matched%10<2)return 0;
    break;
   }
   return 1;  
    }
  else return 1;
}

void AMSTrTrack::SimpleFit(AMSPoint ehit){

integer ifit=0;
integer npt=_NHits;
const integer maxhits=10;
assert(npt < maxhits && npt > 2);
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
geant xmom,dip,phis,exmom;
integer iflag=0;
geant p0[3];
for (int i=0;i<npt;i++){
 z[i]=_Pthit[i]->getHit()[0];
 x[i]=_Pthit[i]->getHit()[1];
 y[i]=_Pthit[i]->getHit()[2];
 wxy[i]=(ehit[0]*ehit[0]+ehit[1]*ehit[1]);
 if(wxy[i]==0)wxy[i]=
 (_Pthit[i]->getEHit()[1] * _Pthit[i]->getEHit()[1]+
  _Pthit[i]->getEHit()[2] * _Pthit[i]->getEHit()[2]);
  wxy[i]=1/wxy[i];
 ssz[i]=ehit[0];
 if(ssz[i]==0)ssz[i]=_Pthit[i]->getEHit()[0];
 ssz[i]=1/ssz[i]; 
}

TRAFIT(ifit,x,y,wxy,z,ssz,npt,resxy,ressz,iflag,spcor,work,chixy,chiz,xmom,
       exmom,p0,dip,phis);
if(iflag/1000 == 0)_Chi2Circle=chixy;
else _Chi2Circle=FLT_MAX;
if(iflag%1000 ==0)_Chi2StrLine=chiz;
else _Chi2StrLine=FLT_MAX;
_CircleRidgidity=xmom;
//if(TRFITFFKEY.FastTracking){
//  // Fill fastfit here
//  _Ridgidity=_CircleRidgidity; 
//  _ErrRidgidity=exmom;
//  //  _P0[0]=p0[0];
//  //  _P0[1]=p0[1];
//  //  _P0[2]=p0[2];
//  _P0[0]=_Pthit[1]->getHit()[0];
//  _P0[1]=_Pthit[1]->getHit()[1];
//  _P0[2]=_Pthit[1]->getHit()[2];
//
//  _Theta=dip;
//  // _Phi=phis;
//  _Phi=atan2(_Pthit[2]->getHit()[1]-_Pthit[0]->getHit()[1],
//             _Pthit[2]->getHit()[0]-_Pthit[0]->getHit()[0]);
//
//}
}

void AMSTrTrack::TOFFit(integer ntof, AMSPoint tofhit, AMSPoint etofhit){

integer ifit=0;
integer npt=_NHits+ntof;
const integer maxhits=14;
assert (npt<maxhits && _NHits>2 && ntof>2);
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
geant xmom,dip,phis,exmom;
integer iflag=0;
geant p0[3];
for (int i=0;i<_NHits;i++){
 z[i]=_Pthit[i]->getHit()[0];
 x[i]=_Pthit[i]->getHit()[1];
 y[i]=_Pthit[i]->getHit()[2];
 wxy[i]= (_Pthit[i]->getEHit()[1] * _Pthit[i]->getEHit()[1]+
          _Pthit[i]->getEHit()[2] * _Pthit[i]->getEHit()[2]);
 wxy[i]= 1/wxy[i];
 ssz[i]= _Pthit[i]->getEHit()[0];
 ssz[i]= 1/ssz[i]; 
}
for (i=0;i<ntof;i++){
 z[_NHits+i] = tofhit[0];
 x[_NHits+i] = tofhit[1];
 y[_NHits+i] = tofhit[2];
 wxy[_NHits+i] = (etofhit[1]*etofhit[1] + etofhit[2]*etofhit[2]);
 wxy[_NHits+i] = 1/wxy[_NHits+i];
 ssz[_NHits+i] = etofhit[0];
 ssz[_NHits+i] = 1/ssz[_NHits+i]; 
}

TRAFIT(ifit,x,y,wxy,z,ssz,npt,resxy,ressz,iflag,spcor,work,chixy,chiz,xmom,
       exmom,p0,dip,phis);
if(iflag/1000 == 0)_Chi2Circle=chixy;
else _Chi2Circle=FLT_MAX;
if(iflag%1000 ==0)_Chi2StrLine=chiz;
else _Chi2StrLine=FLT_MAX;
_CircleRidgidity=xmom;
}


number AMSTrTrack::Fit(integer fit, integer ipart){

  // fit =0  fit pattern
  // fit =1  fit 1st part if pat=0,1,2,3, ... etc  
  // fit =2  fit 2nd half if pat=0,1,2,3  ... etc + interpolate to beg of 1st
  // fit =3  Geanefit pattern
  // fit =4  fast fit with ims=0
  // fit =5  geane fit with ims=0

  // Protection from too low mass
    if(ipart==2)ipart=5;
    if(ipart==3)ipart=6;
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
      if (_Pthit[i]) {
       hits[i][j]=_Pthit[i]->getHit()[j];
       sigma[i][j]=_Pthit[i]->getEHit()[j];
      } else {
        cout <<"AMSTrTrack::Fit -W- _Pthit["<<i<<"] = NULL, j"<<j<<endl;
      }
     }
    }
  }
  else if(fit ==1){
    if(_Pattern==0){
      //fit 124
      npt=3;
     for(int j=0;j<3;j++){
      hits[0][j]=_Pthit[0]->getHit()[j];
      sigma[0][j]=_Pthit[0]->getEHit()[j];
      hits[1][j]=_Pthit[1]->getHit()[j];
      sigma[1][j]=_Pthit[1]->getEHit()[j];
      hits[2][j]=_Pthit[3]->getHit()[j];
      sigma[2][j]=_Pthit[3]->getEHit()[j];
     }
      
    }
    else if(_Pattern<npat){
      //fit 123
      npt=3;
     for(int j=0;j<3;j++){
      hits[0][j]=_Pthit[0]->getHit()[j];
      sigma[0][j]=_Pthit[0]->getEHit()[j];
      hits[1][j]=_Pthit[1]->getHit()[j];
      sigma[1][j]=_Pthit[1]->getEHit()[j];
      hits[2][j]=_Pthit[2]->getHit()[j];
      sigma[2][j]=_Pthit[2]->getEHit()[j];
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
      hits[0][j]=_Pthit[2]->getHit()[j];
      sigma[0][j]=_Pthit[2]->getEHit()[j];
      hits[1][j]=_Pthit[3]->getHit()[j];
      sigma[1][j]=_Pthit[3]->getEHit()[j];
      hits[2][j]=_Pthit[5]->getHit()[j];
      sigma[2][j]=_Pthit[5]->getEHit()[j];
     }
    }
    else if(_Pattern <7){
      // fit 345
      npt=3;
     for(int j=0;j<3;j++){
      hits[0][j]=_Pthit[2]->getHit()[j];
      sigma[0][j]=_Pthit[2]->getEHit()[j];
      hits[1][j]=_Pthit[3]->getHit()[j];
      sigma[1][j]=_Pthit[3]->getEHit()[j];
      hits[2][j]=_Pthit[4]->getHit()[j];
      sigma[2][j]=_Pthit[4]->getEHit()[j];
     }
    }
    else if(_Pattern <npat){
      // fit 234
      npt=3;
     for(int j=0;j<3;j++){
      hits[0][j]=_Pthit[1]->getHit()[j];
      sigma[0][j]=_Pthit[1]->getEHit()[j];
      hits[1][j]=_Pthit[2]->getHit()[j];
      sigma[1][j]=_Pthit[2]->getEHit()[j];
      hits[2][j]=_Pthit[3]->getHit()[j];
      sigma[2][j]=_Pthit[3]->getEHit()[j];
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
_FastFitDone=1;
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


// interpolate if fit == 2;

AMSTrRecHit *phit=_Pthit[0];
#ifdef __AMSDEBUG__
  assert(phit!= NULL);
#endif

 AMSDir SenDir((phit->getpsen())->getnrmA(2,0),
        (phit->getpsen())->getnrmA(2,1),(phit->getpsen())->getnrmA(2,2) );
 AMSPoint SenPnt=phit->getHit();
 number sleng;
 interpolate(SenPnt, SenDir, _HP0[1], _HTheta[1], _HPhi[1], sleng);
 if(_HTheta[1]<AMSDBc::pi/2)_HTheta[1]=AMSDBc::pi-_HTheta[1];
  
}
else if(fit==3){
#ifdef __AMSDEBUG__
int i,j;
for(i=0;i<npt;i++){
  for(j=0;j<3;j++){
    if(sigma[i][j]==0){
      cerr<<"AMSTrTrack::Fit-F-Some Errors are zero "<<i<<" "<<j<<endl;
      exit(1);
    }
  }
}
#endif     
_GeaneFitDone=ipart;
_GChi2=out[6];
_GRidgidity=out[5];
_GErrRidgidity=out[8];
_GTheta=out[3];
_GPhi=out[4];
_GP0=AMSPoint(out[0],out[1],out[2]);
if(out[7] !=0){
  _GeaneFitDone=0;
  _GChi2=FLT_MAX;
  _GErrRidgidity=FLT_MAX;
  _GRidgidity=FLT_MAX;
}  
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
  TrTrackNtuple* TrTN = AMSJob::gethead()->getntuple()->Get_trtr();

  if (TrTN->Ntrtr>=MAXTRTR) return;

// Fill the ntuple 
  if(AMSTrTrack::Out(1)){
    int i;
    TrTN->Status[TrTN->Ntrtr]=_status;
    TrTN->Pattern[TrTN->Ntrtr]=_Pattern;
    TrTN->NHits[TrTN->Ntrtr]=_NHits;
    for(int k=_NHits;k<AMSDBc::nlay();k++)TrTN->pHits[TrTN->Ntrtr][k]=0;
    for(k=0;k<_NHits;k++){
     TrTN->pHits[TrTN->Ntrtr][k]=_Pthit[k]->getpos();
      int pat;
      pat=_Pthit[k]->getLayer()-1;
      if (AMSTrRecHit::Out(IOPA.WriteAll%10==1)){
        // Writeall
        for(i=0;i<pat;i++){
          AMSContainer *pc=AMSEvent::gethead()->getC("AMSTrRecHit",i);
           #ifdef __AMSDEBUG__
            assert(pc != NULL);
           #endif
           TrTN->pHits[TrTN->Ntrtr][k]+=pc->getnelem();
        }
      }                                                        
      else if (AMSTrRecHit::Out(IOPA.WriteAll%10==0)){
      //WriteUsedOnly
        for(i=0;i<pat;i++){
          AMSTrRecHit *ptr=(AMSTrRecHit*)AMSEvent::gethead()->getheadC("AMSTrRecHit",i);
          while(ptr && ptr->checkstatus(AMSDBc::USED)){
            TrTN->pHits[TrTN->Ntrtr][k]++;
            ptr=ptr->next();
          }
        }
      }
      else if (AMSTrRecHit::Out(IOPA.WriteAll%10==2)){
      //WriteUsedOnly
        for(i=0;i<pat;i++){
          AMSTrRecHit *ptr=(AMSTrRecHit*)AMSEvent::gethead()->getheadC("AMSTrRecHit",i);
          while(ptr && !(ptr->checkstatus(AMSDBc::AwayTOF)) ){
            TrTN->pHits[TrTN->Ntrtr][k]++;
            ptr=ptr->next();
          }
        }
      }
      else return;
    }
  
  
    TrTN->FastFitDone[TrTN->Ntrtr]=_FastFitDone;
    TrTN->GeaneFitDone[TrTN->Ntrtr]=_GeaneFitDone;
    TrTN->AdvancedFitDone[TrTN->Ntrtr]=_AdvancedFitDone;
    TrTN->Chi2StrLine[TrTN->Ntrtr]=geant(_Chi2StrLine);
    TrTN->Chi2Circle[TrTN->Ntrtr]=geant(_Chi2Circle);
    TrTN->CircleRidgidity[TrTN->Ntrtr]=(geant)_CircleRidgidity;
    TrTN->Chi2FastFit[TrTN->Ntrtr]=(geant)_Chi2FastFit;
    TrTN->Ridgidity[TrTN->Ntrtr]=(geant)_Ridgidity;
    TrTN->ErrRidgidity[TrTN->Ntrtr]=(geant)_ErrRidgidity;
    TrTN->Theta[TrTN->Ntrtr]=(geant)_Theta;
    TrTN->Phi[TrTN->Ntrtr]=(geant)_Phi;
    for(i=0;i<3;i++)TrTN->P0[TrTN->Ntrtr][i]=(geant)_P0[i];
    if(_GeaneFitDone){ 
     TrTN->GChi2[TrTN->Ntrtr]=(geant)_GChi2;
     TrTN->GRidgidity[TrTN->Ntrtr]=(geant)_GRidgidity;
     TrTN->GErrRidgidity[TrTN->Ntrtr]=(geant)_GErrRidgidity;
     TrTN->GTheta[TrTN->Ntrtr]=(geant)_GTheta;
     TrTN->GPhi[TrTN->Ntrtr]=(geant)_GPhi;
     for(i=0;i<3;i++)TrTN->GP0[TrTN->Ntrtr][i]=(geant)_GP0[i];
    }
    else{
     TrTN->GChi2[TrTN->Ntrtr]=-1;
     TrTN->GRidgidity[TrTN->Ntrtr]=0;
     TrTN->GErrRidgidity[TrTN->Ntrtr]=0;
     TrTN->GTheta[TrTN->Ntrtr]=0;
     TrTN->GPhi[TrTN->Ntrtr]=0;
     for(i=0;i<3;i++)TrTN->GP0[TrTN->Ntrtr][i]=0;
    } 
    if(_AdvancedFitDone){
     for(i=0;i<2;i++)TrTN->HChi2[TrTN->Ntrtr][i]=(geant) _HChi2[i];
     for(i=0;i<2;i++)TrTN->HRidgidity[TrTN->Ntrtr][i]=(geant)_HRidgidity[i];
     for(i=0;i<2;i++)TrTN->HErrRidgidity[TrTN->Ntrtr][i]=(geant)_HErrRidgidity[i];
     for(i=0;i<2;i++)TrTN->HTheta[TrTN->Ntrtr][i]=(geant)_HTheta[i];
     for(i=0;i<2;i++)TrTN->HPhi[TrTN->Ntrtr][i]=(geant)_HPhi[i];
     for(i=0;i<3;i++)TrTN->HP0[TrTN->Ntrtr][0][i]=(geant)_HP0[0][i];
     for(i=0;i<3;i++)TrTN->HP0[TrTN->Ntrtr][1][i]=(geant)_HP0[1][i];
    }
    else{
     for(i=0;i<2;i++)TrTN->HChi2[TrTN->Ntrtr][i]=-1;
     for(i=0;i<2;i++)TrTN->HRidgidity[TrTN->Ntrtr][i]=0;
     for(i=0;i<2;i++)TrTN->HErrRidgidity[TrTN->Ntrtr][i]=0;
     for(i=0;i<2;i++)TrTN->HTheta[TrTN->Ntrtr][i]=0;
     for(i=0;i<2;i++)TrTN->HPhi[TrTN->Ntrtr][i]=0;
     for(i=0;i<3;i++)TrTN->HP0[TrTN->Ntrtr][0][i]=0;
     for(i=0;i<3;i++)TrTN->HP0[TrTN->Ntrtr][1][i]=0;
    }
    TrTN->FChi2MS[TrTN->Ntrtr]=(geant)_Chi2MS;
    TrTN->GChi2MS[TrTN->Ntrtr]=(geant)_GChi2MS;
    TrTN->RidgidityMS[TrTN->Ntrtr]=(geant)_RidgidityMS;
    TrTN->GRidgidityMS[TrTN->Ntrtr]=(geant)_GRidgidityMS;
    TrTN->Ntrtr++;

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

void AMSTrTrack::interpolateCyl(AMSPoint CylCenter, AMSDir CylAxis,
                                number CylRadius, number idir, AMSPoint & P1,
                                number & theta, number & phi, number & length){

  // interpolates track to cylinder (Cylpar = x_cen,y_cen, radius)
  //in the direction idir  
  // and calculates the track parameters(P1,theta,phi) and total track length

  geant out[7];
  static number m55[5][5];
  geant init[7];
  geant point[7];
  geant charge=1;
  point[0]=CylCenter[0];
  point[1]=CylCenter[1];
  point[2]=CylCenter[2];
  point[3]=CylAxis[0];
  point[4]=CylAxis[1];
  point[5]=CylAxis[2];
  point[6]=CylRadius;
  geant slength;
  number s2=(CylCenter-_P0).prod(CylCenter-_P0);
  number s1=(CylCenter-_P0).prod(CylAxis);
  number sdist=CylRadius-sqrt(s2-s1*s1);
  if(sdist<0){
    // interpolate to 2nd ladder first;
   integer ok=intercept(P1,2,theta,phi);
   AMSDir dir(idir*sin(theta)*cos(phi),
             idir*sin(theta)*sin(phi),
             idir*cos(theta));
   init[0]=P1[0];
   init[1]=P1[1];
   init[2]=P1[2];
   init[3]=dir[0];
   init[4]=dir[1];
   init[5]=dir[2];
  }
  else {
   AMSDir dir(idir*sin(_Theta)*cos(_Phi),
             idir*sin(_Theta)*sin(_Phi),
             idir*cos(_Theta));
   init[0]=_P0[0];
   init[1]=_P0[1];
   init[2]=_P0[2];
   init[3]=dir[0];
   init[4]=dir[1];
   init[5]=dir[2];
  }
   init[6]=_Ridgidity;
   TKFITPARCYL(init, charge,  point,  out,  m55, slength);
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

/* This function creates a false AMSTrRecHit for each S-AMSTrCluster */
/* consistent with TOF information. The X coordinate is imposed from */
/* the straight line fit to >=3 TOF planes in the XZ projection      */

integer AMSTrTrack::makeFalseTOFXHits(){

    int i;

    TriggerLVL3 *plvl3;
    plvl3 = (TriggerLVL3*)AMSEvent::gethead()->getheadC("TriggerLVL3",0);
// LVL3 required if existing
    if ( plvl3==NULL) {
// #ifdef __AMSDEBUG__
//    cout << "makeFalseTOFXHits: No Level3 Trigger existing" << endl;
// #endif
      return 1;
    }
    if ( plvl3->skip() ) return 1;

    AMSTOFCluster *phit[4], *ploop;

// There should be one and only one AMSTOFCluster on planes 1, 4
// according to LVL3 trigger
    phit[0] = AMSTOFCluster::gethead(0);
    if ( (phit[0] == NULL) || (phit[0]->next()) ) return 1;
    phit[3] = AMSTOFCluster::gethead(3);
    if ( (phit[3] == NULL) || (phit[3]->next()) ) return 1;

// Initial straight line from planes 1 and 4 for ZX projection
    number slope_x= (phit[3]->getcoo()[0] - phit[0]->getcoo()[0]) /
               (phit[3]->getcoo()[2] - phit[0]->getcoo()[2]);
    number intercept_x= phit[0]->getcoo()[0] - slope_x*phit[0]->getcoo()[2];

// Look for the closest AMSTOFCluster on plane 2
    number resmax2=999;
    phit[1]=NULL;
    for (ploop = AMSTOFCluster::gethead(1); ploop ; ploop=ploop->next() ){
      number resx2 = fabs(ploop->getcoo()[0] 
                          - slope_x*ploop->getcoo()[2] - intercept_x);
      if (resx2<resmax2) {
        resmax2 = resx2;
        phit[1] = ploop;
      }
    }

// Look for the closest AMSTOFCluster on plane 3
// to improve the X prediction
    number resmax3=999;
    phit[2]=NULL;
    for (ploop = AMSTOFCluster::gethead(2); ploop ; ploop=ploop->next() ){
      number resx3 = fabs(ploop->getcoo()[0] 
                          - slope_x*ploop->getcoo()[2] - intercept_x);
      if (resx3<resmax3) {
        resmax3 = resx3;
        phit[2] = ploop;
      }
    }

// We require at least 3 AMSTOFClusters
    if ( (phit[1]==NULL) && (phit[2]==NULL) ) return 1;

// Straight line parameters for the ZX plane 
// Use only Pad information -planes 2 or 3- if not consistent with planes 
// 1 or 4, respectively (info from planes 1,4 is TOF-calibration dependent)
    number sw=0, sz=0, sx=0, sxz=0, szz=0;
    geant searchregtof = TOFDBc::plnstr(5)+2.*TOFDBc::plnstr(13);
    for (i=0; i<4; i++){
      if (phit[i]==NULL) continue;
      if (i==0 && phit[1]!=NULL && resmax2>searchregtof) continue;
      if (i==3 && phit[2]!=NULL && resmax3>searchregtof) continue;
      number w = phit[i]->getecoo()[0]; if (w<=0.) continue; w = 1./w/w;
      number x = phit[i]->getcoo()[0];
      number z = phit[i]->getcoo()[2];
      sw += w;
      sx += x*w;
      sz += z*w;
      sxz += x*z*w;
      szz += z*z*w;
    }
    number determinant = szz*sw-sz*sz;
    slope_x = (sxz*sw-sx*sz)/determinant;
    intercept_x = (szz*sx-sz*sxz)/determinant;
    number covss = sw/determinant;
    number covsi = -sz/determinant;
    number covii = szz/determinant;

// Straight line parameters for the ZY plane 
// (Only planes 1,4 => TOFCalib independent, but robust and enough)
    number slope_y= (phit[3]->getcoo()[1] - phit[0]->getcoo()[1]) /
               (phit[3]->getcoo()[2] - phit[0]->getcoo()[2]);
    number intercept_y= phit[0]->getcoo()[1] - slope_y*phit[0]->getcoo()[2];

// Create Fake AMSTrRecHits on all tracker planes containing sensible Y clusters
    AMSTrCluster *py;
    for (py=(AMSTrCluster*)AMSEvent::gethead()->getheadC("AMSTrCluster",1,0);
         py != NULL ; py=py->next()) {
// Do nothing for bad clusters
      if (py->checkstatus(AMSDBc::BAD) != 0) continue;
// SoftId, approximate IdGeom and approximate gSensor
      AMSTrIdSoft idsoft = py->getid();
      integer layer = idsoft.getlayer();
      integer ladder = idsoft.getdrp();
      integer half = idsoft.gethalf();
      integer sensor;
      if (half==0) 
                   sensor = 5; 
      else 
                   sensor = AMSDBc::nsen(layer,ladder) - 5;
      AMSTrIdGeom idgeom(layer,ladder,sensor,0,0);
      AMSgSen *psensor=(AMSgSen*)AMSJob::gethead()->getgeomvolume(idgeom.crgid());
      idgeom.R2G(idsoft);
// Get the approximate global y coordinate
      AMSPoint glopos = psensor->str2pnt(0.,py->getcofg(1,&idgeom));
// Impose the X global coordinate from TOF
      glopos[0] = intercept_x + slope_x*glopos[2];
// Do not use if it is far away from TOF prediction
      geant searchregtof = TOFDBc::plnstr(5)+2.*TOFDBc::plnstr(13);
      if (fabs(glopos[1] - intercept_y - slope_y*glopos[2])
               > searchregtof) continue; // 1/2 SC bar
// Find now the right sensor number on the ladder
// And do not use it if TOF prediction is far away from the ladder
      AMSPoint sensor_size(   psensor->getpar(0)
                              , psensor->getpar(1)
                              , psensor->getpar(2));
      if(idgeom.FindAtt(glopos,sensor_size)==0) continue;
// Go to local coordinates to get a better global position
      AMSPoint locpos = psensor->gl2loc(glopos);
      glopos = psensor->str2pnt(locpos[0]+sensor_size[0],py->getcofg(1,&idgeom));
// Error
      AMSPoint gloerr;
      gloerr[0] = sqrt(covii+2*glopos[2]*covsi+glopos[2]*glopos[2]*covss);
      gloerr[1] = py->getecofg();
      gloerr[2] = (number)TRCLFFKEY.ErrZ;

// Create a new Fake hit with TOF on X
      AMSTrRecHit::_addnext(
        psensor,
        AMSDBc::FalseTOFX,
        idsoft.getlayer(),
        0,
        py,
        glopos,
        gloerr );
        // cout << "makeFalseTOFXHits: New Hit done at plane " << 
        //           idsoft.getlayer() << glopos << gloerr << endl;
    }
    return 0;
}

integer AMSTrTrack::buildFalseTOFX(integer refit){
  integer NTrackFound=-1;
  // pattern recognition + fit
  if(refit){
    // Cleanup all track containers
    int i;
    for(i=0;;i++){
      AMSContainer *pctr=AMSEvent::gethead()->getC("AMSTrTrack",i);
      if(pctr)pctr->eraseC();
      else break ;
    }
  } 
  _RefitIsNeeded=0;
  _Start();
  // Add test here

// Build Fake Clusters with TOF
  integer hitstatus = makeFalseTOFXHits();
  if (hitstatus!=0) return -1;
   
  { 
    int xs=0; 
    for (int kk=0;kk<6;kk++){
      AMSTrRecHit * phit;
      for (phit=AMSTrRecHit::gethead(kk); phit!=NULL; phit=phit->next()){
        if (phit->Good() && phit->getstatus()==AMSDBc::FalseTOFX) {
          xs++; 
          break;
        }
      }
    }
    if(xs>3)AMSEvent::gethead()->addnext(AMSID("Test",0),new Test());
    else return -1;
  }

  for (int pat=0;pat<npat;pat++){
    AMSTrRecHit * phit[6]={0,0,0,0,0,0};
    if(TRFITFFKEY.pattern[pat]){
      int fp=patpoints[pat]-1;    
      // Try to make StrLine Fit
      integer first=AMSTrTrack::patconf[pat][0]-1;
      integer second=AMSTrTrack::patconf[pat][fp]-1;
      phit[0]=AMSTrRecHit::gethead(first);
      number par[2][2];
      while( phit[0]){
       if(phit[0]->Good() && phit[0]->checkstatus(AMSDBc::FalseTOFX)!=0){
       phit[fp]=AMSTrRecHit::gethead(second);
       while( phit[fp]){
        if(phit[fp]->Good() && phit[fp]->checkstatus(AMSDBc::FalseTOFX)!=0){
        par[0][0]=(phit[fp]-> getHit()[0]-phit[0]-> getHit()[0])/
               (phit[fp]-> getHit()[2]-phit[0]-> getHit()[2]);
        par[0][1]=phit[0]-> getHit()[0]-par[0][0]*phit[0]-> getHit()[2];
        par[1][0]=(phit[fp]-> getHit()[1]-phit[0]-> getHit()[1])/
               (phit[fp]-> getHit()[2]-phit[0]-> getHit()[2]);
        par[1][1]=phit[0]-> getHit()[1]-par[1][0]*phit[0]-> getHit()[2];
        if(NTrackFound<0)NTrackFound=0;
        // Search for others
        phit[1]=AMSTrRecHit::gethead(AMSTrTrack::patconf[pat][1]-1);
        while(phit[1]){
         if(phit[1]->Good() && phit[1]->checkstatus(AMSDBc::FalseTOFX)!=0){
          // Check if the point lies near the str line
           if(AMSTrTrack::DistanceTOF(par,phit[1]))
           {phit[1]=phit[1]->next();continue;}
          if(AMSTrTrack::patpoints[pat] >3){         
         phit[2]=AMSTrRecHit::gethead(AMSTrTrack::patconf[pat][2]-1);
         while(phit[2]){
          // Check if the point lies near the str line
          if(phit[2]->Good() && phit[2]->checkstatus(AMSDBc::FalseTOFX)!=0){
          if(AMSTrTrack::DistanceTOF(par,phit[2]))
          {phit[2]=phit[2]->next();continue;}
          if(AMSTrTrack::patpoints[pat] >4){         
          phit[3]=AMSTrRecHit::gethead(AMSTrTrack::patconf[pat][3]-1);
          while(phit[3]){
           if(phit[3]->Good() && phit[3]->checkstatus(AMSDBc::FalseTOFX)!=0){
           if(AMSTrTrack::DistanceTOF(par,phit[3]))
           {phit[3]=phit[3]->next();continue;}
           if(AMSTrTrack::patpoints[pat]>5){
           phit[4]=AMSTrRecHit::gethead(AMSTrTrack::patconf[pat][4]-1);
           while(phit[4]){
             if(phit[4]->Good() && phit[4]->checkstatus(AMSDBc::FalseTOFX)!=0){
              if(AMSTrTrack::DistanceTOF(par,phit[4]))
              {phit[4]=phit[4]->next();continue;}
                // 6 point combination found
              if(AMSTrTrack::_addnext(pat,6,phit)){
                  // cout << "FalseTOFX track found with 5 hits" << endl;
                  NTrackFound++;
                  goto out;
              }                
                
             }
            phit[4]=phit[4]->next();
           }
           }
           else{  // 5 points only
                // 5 point combination found
             if(AMSTrTrack::_addnext(pat,5,phit)){
                  // cout << "FalseTOFX track found with 5 hits" << endl;
                  NTrackFound++;
                  goto out;
             }
                
           }
           }
           phit[3]=phit[3]->next();
          }
          }
          else{       // 4 points only
                // 4 point combination found
                
                if(AMSTrTrack::_addnext(pat,4,phit)){
                  // cout << "FalseTOFX track found with 4 hits" << endl;
                  NTrackFound++;
                  goto out;
                }                
          }
          }
          phit[2]=phit[2]->next();
          }
          }
          }
         phit[1]=phit[1]->next();
        }
        }         
        phit[fp]=phit[fp]->next();
       }
       }  
out:
       phit[0]=phit[0]->next();
      }
      
    }
  }
return NTrackFound;
}

integer AMSTrTrack::DistanceTOF(number par[2][2], AMSTrRecHit *ptr){
const integer freq=10;
static integer trig=0;
trig=(trig+1)%freq;
           if(trig==0 && _CheckTime()>AMSFFKEY.CpuLimit){
            throw AMSTrTrackError(" Cpulimit Exceeded ");
           }

   geant searchregtof = TOFDBc::plnstr(5)+2.*TOFDBc::plnstr(13);
   
   return fabs(par[0][1]+par[0][0]*ptr->getHit()[2]-ptr->getHit()[0])/
            sqrt(1+par[0][0]*par[0][0]) > searchregtof ||
          fabs(par[1][1]+par[1][0]*ptr->getHit()[2]-ptr->getHit()[1])/
            sqrt(1+par[1][0]*par[1][0]) > TRFITFFKEY.SearchRegCircle;
}


integer AMSTrTrack::intercept(AMSPoint &P1,integer layer, number &theta, number &phi){
  AMSTrRecHit * phit=AMSTrRecHit::gethead(layer);
  if(phit){
    AMSgSen *pls=phit->getpsen();
    AMSDir pntdir(pls->getnrmA(0,2),pls->getnrmA(1,2),pls->getnrmA(2,2));
    AMSPoint pntplane(pls->getcooA(0),pls->getcooA(1),pls->getcooA(2));
    number length;
    interpolate(pntplane,pntdir,P1,theta,phi,length);
  }    
  else{
    // no hits but still track extrap needed
    AMSTrIdGeom g(layer+1,8,4,0,0);    // reference
    AMSgvolume *pls =AMSJob::gethead()->getgeomvolume(g.crgid());
    if(pls){
      AMSDir pntdir(pls->getnrmA(0,2),pls->getnrmA(1,2),pls->getnrmA(2,2));
      AMSPoint pntplane(pls->getcooA(0),pls->getcooA(1),pls->getcooA(2));
      number length;
      interpolate(pntplane,pntdir,P1,theta,phi,length);
    }
    else {
      cerr <<"AMSTrTrack::intercept-E-NoGeomVolumeFound"<<g.crgid()<<endl;
      P1=0;
      theta=0;
      phi=0;
      return 0;
    }
  }
  return 1;
}
