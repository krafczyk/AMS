// Author V. Choutko 24-may-1996
 
#include <trid.h>
#include <trrawcluster.h>
#include <extC.h>
#include <commons.h>
#include <math.h>
#include <amsgobj.h>
#include <mccluster.h>
#include <timeid.h>

integer AMSTrRawCluster::TestRawMode(){
  AMSTrIdSoft id(_address);
  int icmpt=AMSDBc::compactshuttle(id.getlayer(),id.getdrp());
  return TRMCFFKEY.RawModeOn[id.gethalf()][id.getside()][icmpt];   

}

integer AMSTrRawCluster::lvl3format(int16 * adc, integer nmax, integer pedantic){
  //
  // convert my stupid format to lvl3 one for shuttle flight (mb also stupid)
  //
   AMSTrIdSoft id(_address);
  int16 pos =0;
  int16 icmpt=(int16)AMSDBc::compactshuttle(id.getlayer(),id.getdrp());
#ifdef __AMSDEBUG__
  assert (icmpt < 32 && icmpt >=0);
#endif
  int16 half=(int16)id.gethalf();
  id.upd(_strip);
  int16 va=(int16)id.getva();
  int16 strip=(int16)id.getstripa();
  if (nmax-pos < 2+_nelem || _nelem > 255 || _nelem==0) return pos;
  adc[pos+1]=mkaddress(strip,va,half,icmpt,pedantic);
  integer imax=0;
  geant rmax=-1000000;
  for (int i=0;i<_nelem;i++){
   id.upd(_strip+i);
   if(_array[i] > rmax){
     rmax=_array[i];
     imax=i;
   }
   adc[pos+i+2]=int16(_array[i]);
  }
    adc[pos]=(_nelem-1) | (imax<<8); 
    pos+=2+_nelem;
 return pos; 
}





void AMSTrRawCluster::expand(number *adc)const {
AMSTrIdSoft id(_address);
  for (int i=0;i<_nelem;i++){
   id.upd(_strip+i);
   adc[id.getstrip()]=_array[i]/id.getgain()*ADC2KeV();
  }


}



AMSTrRawCluster::~AMSTrRawCluster(){
      UPool.udelete(_array);
}


AMSTrRawCluster::AMSTrRawCluster(integer ad, integer left, integer right, 
                                 geant *p):_address(ad),_strip(left),
                                 _nelem(right-left+1){
    _array=(integer*)UPool.insert(sizeof(_array[0])*_nelem);
    for(int k=0;k<_nelem;k++)_array[k]=integer(*(p+k));  
}

AMSTrRawCluster::AMSTrRawCluster(integer ad, integer left, integer right, 
                                 int16 *p):_address(ad),_strip(left),
                                 _nelem(right-left+1){
    _array=(integer*)UPool.insert(sizeof(_array[0])*_nelem);
    for(int k=0;k<_nelem;k++)_array[k]=*(p+k);  
}




void AMSTrRawCluster::sitkdigi(){
  AMSgObj::BookTimer.start("SITKDIGIa");
integer const ms=4000;
integer const maxva=64;
static geant* ida[ms];
geant idlocal[maxva];
VZERO(ida, sizeof(ida[0])*ms/4);
  AMSTrMCCluster* ptr=(AMSTrMCCluster*)
  AMSEvent::gethead()->getheadC("AMSTrMCCluster",0);
while(ptr){
   ptr->addcontent('x',ida);
   ptr->addcontent('y',ida);
   ptr=ptr->next();
}

AMSgObj::BookTimer.stop("SITKDIGIa");

int i,j,k,l;
if(TRMCFFKEY.CalcCmnNoise[0]){
  //
  // add sigmas & calculate properly cmnnoise
  //
 AMSgObj::BookTimer.start("SITKDIGIb");
 for(i=0;i<ms;i++){
  if(ida[i]){
    AMSTrIdSoft idd(i);
    integer ilay=idd.getlayer();
    k=idd.getside();
      for (j=0;j<AMSDBc::NStripsDrp(ilay,k);j++){
        if(*(ida[i]+j)){
         idd.upd(j);
         integer vamin=j-idd.getstripa();
         integer vamax=j+maxva-idd.getstripa();
         geant ecmn=idd.getcmnnoise()*rnormx();
         for (l=vamin;l<vamax;l++){
           idd.upd(l);
           *(ida[i]+l)+=idd.getsig()*rnormx()+ecmn;   
           idlocal[l-vamin]=*(ida[i]+l);
         }
         AMSsortNAGa(idlocal,maxva);
         geant cmn=0;
         for(l=TRMCFFKEY.CalcCmnNoise[1];l<maxva-TRMCFFKEY.CalcCmnNoise[1];l++)
         cmn+=idlocal[l];
         cmn=cmn/(maxva-2*TRMCFFKEY.CalcCmnNoise[1]);
         for(l=vamin;l<vamax;l++)
          *(ida[i]+l)=*(ida[i]+l)-cmn;
         j=vamax;
        }
      }

  }
 }

AMSgObj::BookTimer.stop("SITKDIGIb");

}
AMSgObj::BookTimer.start("SITKDIGIc");
for ( i=0;i<ms;i++){
  if(ida[i]){
     AMSTrIdSoft idd(i);
     integer ilay=idd.getlayer();
     integer half=idd.gethalf();
     AMSTrRawCluster *pcl;
     k=idd.getside();
      pcl=0;
      integer nlmin;
      integer nleft=0;
      integer nright=0;
      for (j=0;j<AMSDBc::NStripsDrp(ilay,k);j++){
        idd.upd(j);
        if(*(ida[i]+j)> TRMCFFKEY.thr1R[k]*idd.getsig()){
          nlmin = nright==0?0:nright+1; 
          nleft=max(j-TRMCFFKEY.neib[k],nlmin);
          idd.upd(nleft);
          while(nleft >nlmin && 
          *(ida[i]+nleft)> TRMCFFKEY.thr2R[k]*idd.getsig())idd.upd(--nleft);
          nright=min(j+TRMCFFKEY.neib[k],AMSDBc::NStripsDrp(ilay,k)-1);
          idd.upd(nright);
          while(nright < AMSDBc::NStripsDrp(ilay,k)-1 && 
          *(ida[i]+nright)> TRMCFFKEY.thr2R[k]*idd.getsig())idd.upd(++nright);
          for (int k=nleft;k<=nright;k++){
            *(ida[i]+k)=idd.getgain()*(
            *(ida[i]+k)+(TRMCFFKEY.CalcCmnNoise[0]==0?idd.getsig()*rnormx():0));
            if(*(ida[i]+k) > TRMCFFKEY.adcoverflow)*(ida[i]+k)=TRMCFFKEY.adcoverflow;
            if(*(ida[i]+k) < -TRMCFFKEY.adcoverflow)*(ida[i]+k)=-TRMCFFKEY.adcoverflow;
          }
           pcl= new
           AMSTrRawCluster(i,nleft,nright,ida[i]+nleft);
            AMSEvent::gethead()->addnext(AMSID("AMSTrRawCluster",half),pcl);
            j=nright+1;           
        }
      }
      UPool.udelete(ida[i]);
  }
}

AMSgObj::BookTimer.stop("SITKDIGIc");

}

void AMSTrRawCluster::_writeEl(){
}


void AMSTrRawCluster::_copyEl(){
}


void AMSTrRawCluster::_printEl(ostream & stream){
  stream <<_address<<" "<<_strip<<" "<<_nelem<<endl;
}


int16u AMSTrRawCluster::getdaqid(int i){
  if (i==0)return (1 | 2<<6 | 11 <<9);
  else if(i==1)return (1 | 5<<6 | 11 <<9);
else return 0x0;
}
int16u AMSTrRawCluster::getdaqidRaw(int i){
  if (i==0)return (2<<6 | 11 <<9);
  else if(i==1)return (5<<6 | 11 <<9);
else return 0x0;
}

integer AMSTrRawCluster::checkdaqid(int16u id){
if(id==getdaqid(0))return 1;
else if(id==getdaqid(1))return 2 ;
else return 0;
}

integer AMSTrRawCluster::checkdaqidRaw(int16u id){
if(id==getdaqidRaw(0))return 1;
else if(id==getdaqidRaw(1))return 2 ;
else return 0;
}


void AMSTrRawCluster::builddaq(integer i, integer n, int16u *p){

  AMSTrRawCluster *ptr=(AMSTrRawCluster*)AMSEvent::gethead()->
  getheadC("AMSTrRawCluster",i);
  integer ltr=0;
  *p=getdaqid(i);
  int16 * p16=(int16*)p;
  while (ptr){
   if(!(ptr->TestRawMode()))ltr+=ptr->lvl3format(p16+1+ltr,n-1-ltr,1);
   ptr=ptr->next();
  }


}
void AMSTrRawCluster::builddaqRaw(integer i, integer n, int16u *p){
int j,k,l;
integer lay,lad,half,side;
integer cleng=0;
if(n){
  geant *  adc  = (geant*)UPool.insert(sizeof(adc[0])*AMSDBc::maxstrips());
 int16u *paux=p;
 *paux=getdaqidRaw(i);
  paux++;
 for(j=0;j<2;j++){
  for(k=0;k<31;k++){
    if(TRMCFFKEY.RawModeOn[i][j][k]){
      //make address
     AMSDBc::expandshuttle(k,lay,lad);
     AMSTrIdSoft id(lay,lad,i,j);
     integer va=id.getva();
     geant ecmn=id.getcmnnoise()*rnormx();
     *(paux)=mkaddress(0,va,i,k,1);
     // check on the event hits
     VZERO(adc,sizeof(adc[0])/sizeof(integer)*AMSDBc::NStripsDrp(lay,j));
     AMSTrMCCluster* ptr=(AMSTrMCCluster*)
     AMSEvent::gethead()->getheadC("AMSTrMCCluster",0);
     while(ptr){
       ptr->addcontent(id,adc);
      ptr=ptr->next();
     }
     for(l=0;l<AMSDBc::NStripsDrp(lay,j);l++){
      id.upd(l);
      if(id.getva()!=va){
       va=id.getva();
       ecmn=id.getcmnnoise()*rnormx();
      }
      adc[l]=adc[l]+id.getsig()*rnormx()+id.getped()+ecmn+0.5;
      if(adc[l]<0)adc[l]=0;
      if(adc[l]>4095)adc[l]=4095;
      *(paux+1+l)=int16u(adc[l]) | (1<<15);
     }
     paux+=1+AMSDBc::NStripsDrp(lay,j);
#ifdef __AMSDEBUG__
     cleng+=1+AMSDBc::NStripsDrp(lay,j);
#endif     
    }
  }
 }

#ifdef _AMSDEBUG__
assert(cleng==n-1);
#endif
UPool.udelete(adc);

}
}


integer AMSTrRawCluster::calcdaqlength(integer i){
  AMSTrRawCluster *ptr=(AMSTrRawCluster*)AMSEvent::gethead()->
  getheadC("AMSTrRawCluster",i);
  integer l=0;
  if(ptr)l=1;
  while (ptr){
   if(!(ptr->TestRawMode()))l+=ptr->_nelem+2;
   ptr=ptr->next();
  }
  return l;
}

integer AMSTrRawCluster::calcdaqlengthRaw(integer i){
int j,k;
integer l=0;
for(j=0;j<2;j++){
  for(k=0;k<31;k++){
    if(TRMCFFKEY.RawModeOn[i][j][k])l+=AMSDBc::NStripsDrp(1,j)+1;
  }
}
if(l)l++;
return l;
}


void AMSTrRawCluster::buildraw(integer n, int16u *p){
  integer ic=checkdaqid(*p)-1;
  int leng=0;
  int16u * ptr;
  for(ptr=p+1;ptr<p+n;ptr+=leng+3){
     leng=(*ptr)&255;
     integer va,strip,half,drp,lay,lad,side;
     getaddress(int16u(*(ptr+1)),strip,va,side,half,drp);
     AMSDBc::expandshuttle(drp,lay,lad);
     half=ic;
     AMSTrIdSoft id(lay,lad,half,side);
     
     AMSEvent::gethead()->addnext(AMSID("AMSTrRawCluster",ic), new
     AMSTrRawCluster(id.cmpt(),strip,strip+leng,(int16*)ptr+2));
  }
  


#ifdef __AMSDEBUG__
{
  AMSContainer * ptrc =AMSEvent::gethead()->getC("AMSTrRawCluster",ic);
  if(ptrc && AMSEvent::debug>1)ptrc->printC(cout);
}
#endif


}

void AMSTrRawCluster::buildrawRaw(integer n, int16u *p){
  integer const maxva=64;
  integer const ms=640;
  static geant id[ms];
  static geant idlocal[maxva];
  VZERO(id,ms*sizeof(id[0])/sizeof(integer));
  int i,j,k,l;
  integer ic=checkdaqidRaw(*p)-1;
  int16u * ptr=p+1;
  // Main loop
  while (ptr<p+n){
     integer va,strip,half,drp,lay,lad,side;
     getaddress(int16u(*(ptr)),strip,va,side,half,drp);
     AMSDBc::expandshuttle(drp,lay,lad);
     half=ic;
     AMSTrIdSoft idd(lay,lad,half,side);
     //aux loop thanks to data format to calculate corr length
     int16u * paux;
     int len=0;
     int16u bit15=1<<15;
     for(paux=ptr+1;paux<p+n;paux++){
      if( !(bit15 & *paux))break;
      else len++;  
     }    
     if(len > ms ){
      cerr <<" AMSTrRawClusterbuildrawRaw-S-LengthError Max is "<<ms <<" Current is "<<len<<endl;
      len=ms;
     }
     // copy to local buffer and subtract peds
     for(j=0;j<len;j++){
      idd.upd(j);
      id[j]=float((*(ptr+1+j)) & 4095)-idd.getped(); 
     }
     // calc cmn noise
      integer vamin,vamax,l;
      for (j=0;j<len;j+=vamax-vamin+1){
         idd.upd(j);
         vamin=j-idd.getstripa();
         vamax=j+maxva-idd.getstripa();
         for (l=vamin;l<vamax;l++){
           idlocal[l-vamin]=id[l];
         }
         AMSsortNAGa(idlocal,maxva);
         geant cmn=0;
         for(l=TRMCFFKEY.CalcCmnNoise[1];l<maxva-TRMCFFKEY.CalcCmnNoise[1];l++)cmn+=idlocal[l];
         cmn=cmn/(maxva-2*TRMCFFKEY.CalcCmnNoise[1]);
         for(l=vamin;l<vamax;l++)id[l]=id[l]-cmn;
      }
      // find "preclusters"  
         idd.upd(0);
         integer ilay=idd.getlayer();
         k=idd.getside();
         AMSTrRawCluster *pcl;
         pcl=0;
         integer nlmin;
         integer nleft=0;
         integer nright=0;
         for (j=0;j<AMSDBc::NStripsDrp(ilay,k);j++){
         idd.upd(j);
         if(id[j]> TRMCFFKEY.thr1R[k]*idd.getsig()){
          nlmin = nright==0?0:nright+1; 
          nleft=max(j-TRMCFFKEY.neib[k],nlmin);
          idd.upd(nleft);
          while(nleft >nlmin && id[nleft]> TRMCFFKEY.thr2R[k]*idd.getsig())idd.upd(--nleft);
          nright=min(j+TRMCFFKEY.neib[k],AMSDBc::NStripsDrp(ilay,k)-1);
          idd.upd(nright);
          while(nright < AMSDBc::NStripsDrp(ilay,k)-1 && 
          id[nright]> TRMCFFKEY.thr2R[k]*idd.getsig())idd.upd(++nright);
          for (int k=nleft;k<=nright;k++){
            id[k]=idd.getgain()*id[k];
            if(id[k] > TRMCFFKEY.adcoverflow)id[k]=TRMCFFKEY.adcoverflow;
            if(id[k] < -TRMCFFKEY.adcoverflow)id[k]=-TRMCFFKEY.adcoverflow;
          }
           pcl= new
           AMSTrRawCluster(idd.getaddr(),nleft,nright,id+nleft);
            AMSEvent::gethead()->addnext(AMSID("AMSTrRawCluster",half),pcl);
            j=nright+1;           
         }
         }


     ptr=ptr+len+1;
  }     




#ifdef __AMSDEBUG__
{
  AMSContainer * ptrc =AMSEvent::gethead()->getC("AMSTrRawCluster",ic);
  if(ptrc && AMSEvent::debug>1)ptrc->printC(cout);
}
#endif


}

// H/K Static


integer AMSTrRawCluster::checkstatusSid(int16u id){
if(id==getstatusSid(0))return 1;
else if(id==getstatusSid(1))return 2 ;
else return 0;
}
 
integer AMSTrRawCluster::checkpedSid(int16u id){
if(id==getpedSid(0))return 1;
else if(id==getpedSid(1))return 2 ;
else return 0;
}

integer AMSTrRawCluster::checksigmaSid(int16u id){
if(id==getsigmaSid(0))return 1;
else if(id==getsigmaSid(1))return 2 ;
else return 0;
}


int16u AMSTrRawCluster::getstatusSid(int i){
  if (i==0)return (0 | 2<<6 | 11 <<9 | 4<<13);
  else if(i==1)return (0 | 5<<6 | 11 <<9 | 4 <<13);
else return 0x0;
}

int16u AMSTrRawCluster::getpedSid(int i){
  if (i==0)return (1 | 2<<6 | 11 <<9 | 4<<13);
  else if(i==1)return (1 | 5<<6 | 11 <<9 | 4 <<13);
else return 0x0;
}

int16u AMSTrRawCluster::getsigmaSid(int i){
  if (i==0)return (2 | 2<<6 | 11 <<9 | 4<<13);
  else if(i==1)return (2 | 5<<6 | 11 <<9 | 4 <<13);
else return 0x0;
}


integer AMSTrRawCluster::calcstatusSl(integer i){
static integer init =0;
integer j,k,lay,lad,l;
integer lg=0;
if(!TRMCFFKEY.WriteHK || ++init != 2*TRMCFFKEY.WriteHK-1+i)return 0;
for(j=0;j<2;j++){
  for(k=0;k<31;k++){
    AMSDBc::expandshuttle(k,lay,lad);
     AMSTrIdSoft id(lay,lad,i,j);
     for(l=0;l<AMSDBc::NStripsDrp(lay,j);l++){
      id.upd(l);
      geant d;
      if(RNDM(d) < 0.001)id.setstatus(AMSDBc::BAD);
      if(id.checkstatus(AMSDBc::BAD)){
       lg++;
      }
     }
  }
}

cout <<"AMSTrRawCluster::calcstatusSl-I-"<<lg<<" bad strips have been written"<<endl;
if(lg)lg++;
return lg;

}

integer AMSTrRawCluster::calcpedSl(integer i){
static integer init =0;
if(!TRMCFFKEY.WriteHK || ++init != 2*TRMCFFKEY.WriteHK+1+i)return 0;
int j,k;
integer l=0;
for(j=0;j<2;j++){
  for(k=0;k<31;k++){
    l+=AMSDBc::NStripsDrp(1,j)+1;
  }
}
if(l)l++;
return l;
}

integer AMSTrRawCluster::calcsigmaSl(integer i){
static integer init =0;
if(!TRMCFFKEY.WriteHK || ++init != 2*TRMCFFKEY.WriteHK+3+i)return 0;
int j,k;
integer l=0;
for(j=0;j<2;j++){
  for(k=0;k<31;k++){
    l+=AMSDBc::NStripsDrp(1,j)+1;
  }
}
if(l)l++;
return l;
}


void AMSTrRawCluster::updpedS(integer n, int16u* p){

  integer const ms=640;
  int i,j,k,l;
  integer ic=checkpedSid(*p)-1;
  int16u * ptr=p+1;
  // Main loop
  while (ptr<p+n){
     integer va,strip,half,drp,lay,lad,side;
     getaddress(int16u(*(ptr)),strip,va,side,half,drp);
     AMSDBc::expandshuttle(drp,lay,lad);
     half=ic;
     AMSTrIdSoft idd(lay,lad,half,side);
     //aux loop thanks to data format to calculate corr length
     int16u * paux;
     int len=0;
     int16u bit15=1<<15;
     for(paux=ptr+1;paux<p+n;paux++){
      if( !(bit15 & *paux))break;
      else len++;
     }
     if(len > ms ){
       cerr <<" AMSTrRawCluster::updpedS-S-LengthError Max is "<<ms <<" Curre\
nt is "<<len<<endl;
      len=ms;
     }
     // Here put peds into memory
     
     for(i=0;i<len;i++){
      idd.upd(i);
      idd.setped()=geant((*(ptr+i+1)) & ~32768)/idd.getgain();
     }  
      ptr=ptr+len+1;
  }
  // Set UpdateMe for corr TDV

  AMSTimeID * ptdv = AMSJob::gethead()->gettimestructure(getTDVped(ic));
   ptdv->UpdateMe()=1;
   ptdv->UpdCRC();
   time_t begin,end,insert;
   time(&insert);
   ptdv->SetTime(insert,AMSEvent::gethead()->gettime(),AMSEvent::gethead()->gettime()+86400);
   cout <<" Tracker H/K  info has been read for "<<*ptdv;
   ptdv->gettime(insert,begin,end);
   cout <<" Time Insert "<<ctime(&insert);
   cout <<" Time Begin "<<ctime(&begin);
   cout <<" Time End "<<ctime(&end);
}


void AMSTrRawCluster::updsigmaS(integer n, int16u* p){

  integer const ms=640;
  int i,j,k,l;
  integer ic=checksigmaSid(*p)-1;
  int16u * ptr=p+1;
  // Main loop
  while (ptr<p+n){
     integer va,strip,half,drp,lay,lad,side;
     getaddress(int16u(*(ptr)),strip,va,side,half,drp);
     AMSDBc::expandshuttle(drp,lay,lad);
     half=ic;
     AMSTrIdSoft idd(lay,lad,half,side);
     //aux loop thanks to data format to calculate corr length
     int16u * paux;
     int len=0;
     int16u bit15=1<<15;
     for(paux=ptr+1;paux<p+n;paux++){
      if( !(bit15 & *paux))break;
      else len++;
     }
     if(len > ms ){
       cerr <<" AMSTrRawCluster::updsigmaS-S-LengthError Max is "<<ms <<" Curre\
nt is "<<len<<endl;
      len=ms;
     }
     // Here put peds into memory
     
     for(i=0;i<len;i++){
      idd.upd(i);
      idd.setsig()=geant((*(ptr+i+1)) & ~32768)/idd.getgain();
     }  
      ptr=ptr+len+1;
  }

  // Set UpdateMe for corr TDV

  AMSTimeID * ptdv = AMSJob::gethead()->gettimestructure(getTDVsigma(ic));
   ptdv->UpdateMe()=1;
   ptdv->UpdCRC();
   time_t begin,end,insert;
   time(&insert);
   ptdv->SetTime(insert,AMSEvent::gethead()->gettime(),AMSEvent::gethead()->gettime()+86400);
   ptdv->gettime(insert,begin,end);
   cout <<" Tracker H/K  info has been read for "<<*ptdv;
   cout <<" Time Insert "<<ctime(&insert);
   cout <<" Time Begin "<<ctime(&begin);
   cout <<" Time End "<<ctime(&end);
}




void AMSTrRawCluster::updstatusS(integer n, int16u* p){

  integer const ms=640;
  int i,j,k,l;
  integer ic=checkstatusSid(*p)-1;
  // Zero Status 

    for ( i=0;i<AMSDBc::nlay();i++){
      for ( j=0;j<AMSDBc::nlad(i+1);j++){
        for ( l=0;l<2;l++){
            AMSTrIdSoft id(i+1,j+1,ic,l);
            for( k=0;k<AMSDBc::NStripsDrp(i+1,l);k++){
             id.upd(k);
             id.setstatus(0);
            }
        }
      }
    }




  int16u * ptr=p+1;
  // Main loop
  integer nbad=0;
  while (ptr<p+n){
     integer va,strip,half,drp,lay,lad,side;
     getaddress(int16u(*(ptr)),strip,va,side,half,drp);
     AMSDBc::expandshuttle(drp,lay,lad);
     half=ic;
     AMSTrIdSoft idd(lay,lad,half,side,strip);
     // Here put status into memory
      idd.setstatus(1);     
      nbad++;
      ptr=ptr+1;
  }
  cout <<"AMSTrRawCluster::updstatusS-I-"<<nbad<<" bad strips have been read"<<endl;
  // Set UpdateMe for corr TDV

  AMSTimeID * ptdv = AMSJob::gethead()->gettimestructure(getTDVstatus(ic));
   ptdv->UpdateMe()=1;
   ptdv->UpdCRC();
   time_t begin,end,insert;
   time(&insert);
   ptdv->SetTime(insert,AMSEvent::gethead()->gettime(),AMSEvent::gethead()->gettime()+86400);
   cout <<" Tracker H/K  info has been read for "<<*ptdv;
   ptdv->gettime(insert,begin,end);
   cout <<" Time Insert "<<ctime(&insert);
   cout <<" Time Begin "<<ctime(&begin);
   cout <<" Time End "<<ctime(&end);
}


AMSID AMSTrRawCluster::getTDVstatus(int i){
  if(i ==0)return AMSID("TrackerStatus.l",AMSJob::gethead()->isRealData());
  else if(i ==1)return AMSID("TrackerStatus.r",AMSJob::gethead()->isRealData());
  else {
    cerr <<"AMSTrRawCluster::getstatusTDV-F-illegal crate "<<i<<endl;
    exit(1);
  }
}
AMSID AMSTrRawCluster::getTDVsigma(int i){
  if(i ==0)return AMSID("TrackerSigmas.l",AMSJob::gethead()->isRealData());
  else if(i ==1)return AMSID("TrackerSigmas.r",AMSJob::gethead()->isRealData());
  else {
    cerr <<"AMSTrRawCluster::getsigmaTDV-F-illegal crate "<<i<<endl;
    exit(1);
  }
}
AMSID AMSTrRawCluster::getTDVped(int i){
  if(i ==0)return AMSID("TrackerPedestals.l",AMSJob::gethead()->isRealData());
  else if(i ==1)return AMSID("TrackerPedestals.r",AMSJob::gethead()->isRealData());
  else {
    cerr <<"AMSTrRawCluster::getpedTDV-F-illegal crate "<<i<<endl;
    exit(1);
  }
}







void AMSTrRawCluster::writestatusS(integer i, integer length, int16u* ptr){
 int j,k,l;
 integer lay,lad;
  if(length){
   int16u *paux=ptr;
   *paux=getstatusSid(i);
   for(j=0;j<2;j++){
    for(k=0;k<31;k++){
     AMSDBc::expandshuttle(k,lay,lad);
     AMSTrIdSoft id(lay,lad,i,j);
     for(l=0;l<AMSDBc::NStripsDrp(lay,j);l++){
      id.upd(l);
      integer va=id.getva();
      if(id.checkstatus(AMSDBc::BAD)){
       *(++paux)=mkaddress(0,va,i,k,1);
      }       
     }
    }
   }
  }
}

void AMSTrRawCluster::writepedS(integer i, integer length, int16u* ptr){
int j,k,l;
integer lay,lad;
integer cleng=0;
if(length){
 int16u *paux=ptr;
 *paux=getpedSid(i);
 for(j=0;j<2;j++){
  for(k=0;k<31;k++){
     AMSDBc::expandshuttle(k,lay,lad);
     AMSTrIdSoft id(lay,lad,i,j);
      integer va=id.getva();
      *(++paux)=mkaddress(0,va,i,k,1);
     for(l=0;l<AMSDBc::NStripsDrp(lay,j);l++){
      id.upd(l);
       *(paux+1+l)=int16u(id.getped()*id.getgain()) | (1<<15);
     }
     paux+=AMSDBc::NStripsDrp(lay,j);
  }
 }
}

}



void AMSTrRawCluster::writesigmaS(integer i, integer length, int16u* ptr){

int j,k,l;
integer lay,lad;
integer cleng=0;
if(length){
 int16u *paux=ptr;
 *paux=getsigmaSid(i);
 for(j=0;j<2;j++){
  for(k=0;k<31;k++){
     AMSDBc::expandshuttle(k,lay,lad);
     AMSTrIdSoft id(lay,lad,i,j);
     integer va=id.getva();
     *(++paux)=mkaddress(0,va,i,k,1);
     for(l=0;l<AMSDBc::NStripsDrp(lay,j);l++){
      id.upd(l);
      *(paux+1+l)=int16u(id.getsig()*id.getgain()) | (1<<15);
     }
     paux+=AMSDBc::NStripsDrp(lay,j);
  }
 }
}



}


