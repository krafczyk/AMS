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
  if (nmax-pos < 2+_nelem || _nelem > 255 || _nelem==0) return pos;
  adc[pos+1]=mkaddress(strip,va,half,icmpt);
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
  if (i==0)return (1 | 2<<6 | 5 <<9);
  else if(i==1)return (1 | 5<<6 | 5 <<9);
else return 0x0;
}

integer AMSTrRawCluster::checkdaqid(int16u id){
if(id==getdaqid(0))return 1;
else if(id==getdaqid(1))return 2 ;
else return 0;
}


void AMSTrRawCluster::builddaq(integer i, integer n, int16u *p){

  AMSTrRawCluster *ptr=(AMSTrRawCluster*)AMSEvent::gethead()->
  getheadC("AMSTrRawCluster",i);
  integer ltr=0;
  *p=getdaqid(i);
  int16 * p16=(int16*)p;
  while (ptr){
   ltr+=ptr->lvl3format(p16+1+ltr,n-1-ltr);
   ptr=ptr->next();
  }


}


integer AMSTrRawCluster::calcdaqlength(integer i){
  AMSTrRawCluster *ptr=(AMSTrRawCluster*)AMSEvent::gethead()->
  getheadC("AMSTrRawCluster",i);
  integer l=1;
  while (ptr){
   l+=ptr->_nelem+2;
   ptr=ptr->next();
  }
  return l;
}


void AMSTrRawCluster::buildraw(integer n, int16u *p){
  integer ic=checkdaqid(*p)-1;
{
  AMSContainer *ptr=AMSEvent::gethead()->getC("AMSTrRawCluster",ic);
  if(ptr)ptr->eraseC();
  else cerr << "AMSTrRawCluster::buildraw-S-No container "<<ic;
}
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
