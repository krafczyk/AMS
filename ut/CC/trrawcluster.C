// Author V. Choutko 24-may-1996
 
#include <trid.h>
#include <trrawcluster.h>
#include <extC.h>
#include <commons.h>
#include <math.h>
#include <amsgobj.h>
#include <mccluster.h>

integer AMSTrRawCluster::lvl3format(int16 * adc, integer nmax){
  //
  // convert my stupid format to lvl3 one for shuttle flight (mb also stupid)
  // the address is now 16 bit and is the format :
  //  5             1                4                       6              = 16
  // 0-22 layer  0-1  half  0-9 (ben) 10-15 (nonb) VA   0-63 strip no (ambig R/L here)
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
  if (nmax-pos < 2+_nelem || 2+_nelem > 255) return pos;
  adc[pos+1]=mkaddress(strip,va,half,icmpt);
  integer imax=0;
  geant rmax=-1000000;
  for (int i=0;i<_nelem;i++){
   id.upd(_strip+i);
   if(_array[i] > rmax){
     rmax=_array[i];
     imax=i-1;
   }
   adc[pos+i+2]=int16(_array[i]);
  }
    adc[pos]=(_nelem+2) | (imax<<8); 
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
    for(int k=0;k<_nelem;k++)_array[k]=*(p+k);  
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
 for(int i=0;i<ms;i++){
  if(ida[i]){
    AMSTrIdSoft idd(i);
    integer ilay=idd.getlayer();
    k=idd.getside();
      for (j=0;j<AMSDBc::NStripsDrp(ilay,k);j++){
        if(*(ida[i]+j)){
         idd.upd(j);
         integer vamin=j-idd.getstripa();
         integer vamax=j+maxva-idd.getstripa();
         for (l=vamin;l<vamax;l++){
           idd.upd(l);
           idlocal[l-vamin]=*(ida[i]+l)+idd.getsig()*rnormx()+
           idd.getcmnnoise();   
         }
         AMSsortNAGa(idlocal,maxva);
         geant cmn=0;
         for(l=TRMCFFKEY.CalcCmnNoise[1];l<maxva-TRMCFFKEY.CalcCmnNoise[1];l++)
         cmn+=idlocal[l];
         cmn=cmn/(maxva-2*TRMCFFKEY.CalcCmnNoise[1]);
         for(l=vamin;l<vamax;l++)
          *(ida[i]+l)=*(ida[i]+l)+idd.getcmnnoise()-cmn;
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
     AMSTrRawCluster *pcl;
     integer k=idd.getside();
      pcl=0;
      integer nlmin;
      integer nleft=0;
      integer nright=0;
      for (int j=0;j<AMSDBc::NStripsDrp(ilay,k);j++){
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
            if(*(ida[i]+k)>TRMCFFKEY.adcoverflow)
             *(ida[i]+k)=TRMCFFKEY.adcoverflow;
          }
           pcl= new
           AMSTrRawCluster(i,nleft,nright,ida[i]+nleft);
            AMSEvent::gethead()->addnext(AMSID("AMSTrRawCluster",0),pcl);
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




void AMSTrRawCluster::builddaq(integer n, uinteger *p){

  AMSTrRawCluster *ptr=(AMSTrRawCluster*)AMSEvent::gethead()->
  getheadC("AMSTrRawCluster",0);
  integer ltr=0;
  int16 *p16= (int16*)p;
  while (ptr){
   ltr+=ptr->lvl3format(p16+ltr,2*n-ltr);
   ptr=ptr->next();
  }
  if(ltr%2 )*(p16+ltr)=0;
#ifdef __AMSDEBUG__
    assert(ltr <=2*n);
#endif
  if(AMSDBc::BigEndian){
   // convert to little endian if big endian
   int16u *p16u= (int16u*)p;
   for (int i=0;i<n;i++){
    int16u  tmp1=*(p16u+2*i);
    int16u  tmp2=*(p16u+2*i+1);
    *(p+i)=tmp1+(tmp2<<16);
   }
  }


}


integer AMSTrRawCluster::calcdaqlength(){
  AMSTrRawCluster *ptr=(AMSTrRawCluster*)AMSEvent::gethead()->
  getheadC("AMSTrRawCluster",0);
  integer l=0;
  while (ptr){
   l+=ptr->_nelem+2;
   ptr=ptr->next();
  }
  return (l+1)/2;
}


void AMSTrRawCluster::buildraw(integer n, uinteger *p){
{
  AMSContainer *ptr=AMSEvent::gethead()->getC("AMSTrRawCluster",0);
  ptr->eraseC();
}
  int16u* ptru= (int16u*)p;
  if(AMSDBc::BigEndian){
  // convert back to big endian
   for (int i=0;i<n;i++){
    int16u  tmp1=int16u(*(p+i)&65535);
    int16u  tmp2=int16u(*(p+i)>>16);
     *(ptru+2*i)=tmp1;
    *(ptru+2*i+1)=tmp2;
   }
  }
  int16* ptr;
  int leng=0;
  for(ptr=(int16*)p;ptr<((int16*)p)+2*n;ptr+=leng){
     leng=(*ptr)&255;
     if(leng==0)break;
     else if(leng<3){
       cerr <<"AMSTrRawCluster::buildraw-S-invalidLength "<<*(ptr)<<endl;
     }
     integer va,strip,half,drp,lay,lad,side;
     getaddress(int16u(*(ptr+1)),strip,va,side,half,drp);
     AMSDBc::expandshuttle(drp,lay,lad);
     AMSTrIdSoft id(lay,lad,half,side);
     
     AMSEvent::gethead()->addnext(AMSID("AMSTrRawCluster",0), new
     AMSTrRawCluster(id.cmpt(),strip,strip+leng-3,ptr+2));
  }
  

  if(AMSDBc::BigEndian){
   // convert to little endian if big endian
   ptru= (int16u*)p;
   for (int i=0;i<n;i++){
    int16u  tmp1=*(ptru+2*i);
    int16u  tmp2=*(ptru+2*i+1);
    *(p+i)=tmp1+(tmp2<<16);
   }
  }

#ifdef __AMSDEBUG__
{
  AMSContainer * ptrc =AMSEvent::gethead()->getC("AMSTrRawCluster",0);
  if(ptrc && AMSEvent::debug>1)ptrc->printC(cout);
}
#endif


}
