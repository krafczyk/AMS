// Author V. Choutko 24-may-1996
 
#include <trid.h>
#include <trrawcluster.h>
#include <extC.h>
#include <commons.h>
#include <math.h>
#include <amsgobj.h>
#include <mccluster.h>
void AMSTrRawCluster::expand(number *adc)const {
AMSTrIdSoft id(_amsid.getid());
integer ilay=id.getlayer();
integer side=id.getside();
integer left;
rc *lst=_first;
while(lst){
 id.upd(lst->array[0]);
 for (int i=1;i<lst->nelem;i++)
  adc[id.getstrip()+i-1]=
 (lst->array[i]-id.getped()-id.getcmnnoise())/id.getgain()*ADC2KeV();
 lst=lst->next;

}

}



AMSTrRawCluster::~AMSTrRawCluster(){
  if(_last){
    rc *p;
    do{
      p=_last->prev;
      UPool.udelete(_last->array);
      UPool.udelete(_last);
      _last=p;
    }while(p);
    _last=0;
  }
}
void AMSTrRawCluster::extend(integer nleft,integer nright,geant *p){
  if(_last){
   _last->next = new(UPool.insert(sizeof(rc))) rc;  
   if(_last->next)_last->next->prev=_last;
   _last=_last->next;
  }
  else{
   _first = new(UPool.insert(sizeof(rc))) rc;  
   _last=_first;
   if(_last)_last->prev=0;
  }
   if(_last){
   
   _last->next=0;
    _nelem++;
    _last->nelem=nright-nleft+2;
    _last->array=(integer*)UPool.insert(sizeof(integer)*_last->nelem);
     if(_last->array){
     *(_last->array)=nleft;
     AMSTrIdSoft id(_amsid.getid());
     for (int i=nleft;i<nright+1;i++){
       id.upd(i);
       //       cout << " construction of array "<<id.getlayer()<<" "<<id.getside()<<
       //       " "<<i<<" "<<*(p+i)<<endl;
       *(_last->array+i+1-nleft)=(integer)(id.getgain()*(*(p+i)+id.getsig()*rnormx())+
        id.getcmnnoise()+id.getped()+0.5);
      if(*(_last->array+i+1-nleft)> TRMCFFKEY.adcoverflow)
      *(_last->array+i+1-nleft)=TRMCFFKEY.adcoverflow;
      if(*(_last->array+i-nleft)<0)*(_last->array+i-nleft)=0;
     }
     }
   }
   else cerr <<" AMSTrRawCluster ctor failed"<<endl; 
}




void AMSTrRawCluster::sitkdigi(){
  AMSgObj::BookTimer.start("SITKDIGIa");
integer const ms=4000;
static geant* ida[ms];
VZERO(ida, sizeof(geant*)*ms/4);
  AMSTrMCCluster* ptr=(AMSTrMCCluster*)
  AMSEvent::gethead()->getheadC("AMSTrMCCluster",0);
while(ptr){
   ptr->addcontent('x',ida);
   ptr->addcontent('y',ida);
   ptr=ptr->next();
}

AMSgObj::BookTimer.stop("SITKDIGIa");

int i;
// add sigmas
AMSgObj::BookTimer.start("SITKDIGIb");
//for(int i=0;i<ms;i++){
//  if(ida[i]){
//     AMSTrIdSoft idd(i);
//     integer ilay=idd.getlayer();
//     integer k=idd.getside();
//      for (int j=0;j<AMSDBc::NStripsDrp(ilay,k);j++){
//      if*(ida[i]+j)=*(ida[i]+j)+idd.getsig()*rnormx();
//      }
//  }
//}
AMSgObj::BookTimer.stop("SITKDIGIb");
AMSgObj::BookTimer.start("SITKDIGIc");
for ( i=0;i<ms;i++){
  if(ida[i]){
     AMSTrIdSoft idd(i);
     integer ilay=idd.getlayer();
     AMSTrRawCluster *pcl;
     integer k=idd.getside();
      pcl=0;
      integer nleft=0;
      integer nright=0;
      for (int j=0;j<AMSDBc::NStripsDrp(ilay,k);j++){
        if(*(ida[i]+j)> TRMCFFKEY.thr[k]){
          if(nright ==0){
           nleft=max(j-TRMCFFKEY.neib[k],0);
           nright=min(j+TRMCFFKEY.neib[k],AMSDBc::NStripsDrp(ilay,k)-1);
          }
          else if (nright+TRMCFFKEY.neib[k]+1<j){
           if(pcl)pcl->extend(nleft,nright,ida[i]);
           else pcl= new
           AMSTrRawCluster(AMSID("TrRawCluster",i),1,nleft,nright,ida[i]);
           nleft=max(j-TRMCFFKEY.neib[k],0);
           nright=min(j+TRMCFFKEY.neib[k],AMSDBc::NStripsDrp(ilay,k)-1);
          }
          else {
            nright=min(j+TRMCFFKEY.neib[k],AMSDBc::NStripsDrp(ilay,k)-1);
          }

        }
      }
      if(nright !=0){
           if(pcl)pcl->extend(nleft,nright,ida[i]);

           else pcl= new
          AMSTrRawCluster(AMSID("TrRawCluster",i),1,nleft,nright,ida[i]);
      }
      if(pcl)AMSEvent::gethead()->addnext(AMSID("AMSTrRawCluster",0),pcl);
      UPool.udelete(ida[i]);
  }
}
AMSgObj::BookTimer.stop("SITKDIGIc");
//cout <<" ** end sitkdigi"<<endl;

}

void AMSTrRawCluster::_writeEl(){
}


void AMSTrRawCluster::_copyEl(){
}


void AMSTrRawCluster::_printEl(ostream & stream){
  stream <<_amsid<<endl;
}

