#include <trid.h>
#include <trrawcluster.h>
#include <extC.h>
#include <commons.h>
#include <math.h>
#include <amsgobj.h>
#include <mccluster.h>
#include <timeid.h>
#include <trid.h>
#include <ntuple.h>
integer AMSTrRawCluster::TestRawMode(){
  AMSTrIdSoft id(_address);
  int icmpt=id.gettdr();
  return TRMCFFKEY.RawModeOn[id.gethalf()][id.getside()][icmpt];   

}

integer AMSTrRawCluster::lvl3format(int16 * adc, integer nmax, integer pedantic){
  //
  // convert my stupid format to lvl3 one for shuttle flight (mb also stupid)
  //
   AMSTrIdSoft id(_address);
  int16 pos =0;
  id.upd(_strip);
  if (nmax-pos < 2+_nelem || _nelem > 255 || _nelem==0) return pos;
  adc[pos+1]=id.gethaddr(pedantic);
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
   if(TRCALIB.LaserRun)adc[id.getstrip()]=_array[i]/id.getgain()/id.getlaser();
   else adc[id.getstrip()]=_array[i]/id.getgain();
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
            //            cout <<"si- "<<half<<" "<<i<<" "<<nleft<<" "<<
            //              nright-nleft<<" "<<integer(*(ida[i]+nleft))<<endl;
            j=nright+1;           
        }
      }
      UPool.udelete(ida[i]);
  }
}

AMSgObj::BookTimer.stop("SITKDIGIc");

}

void AMSTrRawCluster::_writeEl(){

  // fill the ntuple 
static integer init=0;
static TrRawClusterNtuple TrN;
if(AMSTrRawCluster::Out( IOPA.WriteAll  )){
if(init++==0){
  //book the ntuple block
  HBNAME(IOPA.ntuple,"TrRawCl",TrN.getaddress(),
  "TrRawCl:I*4,RawAddress:I*4,RawLength:I*4");

}
  TrN.Event()=AMSEvent::gethead()->getid();
  TrN.address=_address+_strip*10000;
  TrN.nelem=_nelem;
  HFNTB(IOPA.ntuple,"TrRawCl");
}


}


void AMSTrRawCluster::_copyEl(){
}


void AMSTrRawCluster::_printEl(ostream & stream){
  stream <<_address<<" "<<_strip<<" "<<_nelem<<endl;
}


int16u AMSTrRawCluster::getdaqid(int i){
  if (i==0)return ( 2<<6 | 11 <<9);
  else if(i==1)return ( 5<<6 | 11 <<9);
else return 0x0;
}
int16u AMSTrRawCluster::getdaqidRaw(int i){
  if (i==0)return (1 | 2<<6 | 11 <<9);
  else if(i==1)return (1 | 5<<6 | 11 <<9);
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
integer cleng=0;
if(n){
  geant *  adc  = (geant*)UPool.insert(sizeof(adc[0])*AMSDBc::maxstripsdrp());
 int16u *paux=p;
 *paux=getdaqidRaw(i);
  paux++;
 for(j=0;j<2;j++){
  for(k=0;k<ntdr;k++){
    if(TRMCFFKEY.RawModeOn[i][j][k]){
      //make address
     AMSTrIdSoft id(i,k,j,0);
     if(id.dead())continue;
     int va=id.getva();
     geant ecmn=id.getcmnnoise()*rnormx();
     *(paux)=id.gethaddr(1);     
     // check on the event hits
     VZERO(adc,sizeof(adc[0])/sizeof(integer)*AMSDBc::NStripsDrp(id.getlayer(),j));
     AMSTrMCCluster* ptr=(AMSTrMCCluster*)
     AMSEvent::gethead()->getheadC("AMSTrMCCluster",0);
     while(ptr){
       ptr->addcontent(id,adc);
      ptr=ptr->next();
     }
     for(l=0;l<AMSDBc::NStripsDrp(id.getlayer(),j);l++){
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
     paux+=1+AMSDBc::NStripsDrp(id.getlayer(),j);
#ifdef __AMSDEBUG__
     cleng+=1+AMSDBc::NStripsDrp(id.getlayer(),j);
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
  for(k=0;k<ntdr;k++){
    if(TRMCFFKEY.RawModeOn[i][j][k]){
      AMSTrIdSoft id(i,k,j,0);
      if(id.dead())continue;
      l+=AMSDBc::NStripsDrp(1,j)+1;
    }
  }
}
if(l)l++;
return l;
}


void AMSTrRawCluster::buildraw(integer n, int16u *p){
  integer ic=checkdaqid(*p)-1;
  int leng=0;
  int16u * ptr;
  
  for(ptr=p+1;ptr<p+n-1;ptr+=leng+3){      // cluster length > 1 
     leng=(*ptr)&255;
     AMSTrIdSoft id(ic,int16u(*(ptr+1)));
     if(!id.dead() ){
       if(id.teststrip(id.getstrip()+leng)){
        AMSEvent::gethead()->addnext(AMSID("AMSTrRawCluster",ic), new
        AMSTrRawCluster(id.getaddr(),id.getstrip(),id.getstrip()+leng,
        (int16*)ptr+2));
        //        cout <<" ok "<<id.getaddr()<<" "<<id.getstrip()<<" "<<leng<<endl;
       }
        else {
          // split into two clusters;
          // cout <<" split "<<id.getaddr()<<" "<<id.getstrip()<<" "<<leng<<endl;
        AMSEvent::gethead()->addnext(AMSID("AMSTrRawCluster",ic), new
        AMSTrRawCluster(id.getaddr(),id.getstrip(),id.getmaxstrips(),
        (int16*)ptr+2));
        integer second=id.getstrip()+leng-id.getmaxstrips();
        id.upd(0);
         AMSEvent::gethead()->addnext(AMSID("AMSTrRawCluster",ic), new
         AMSTrRawCluster(id.getaddr(),id.getstrip(),second,
         (int16*)ptr+2+leng-second));
          
        }
     }
#ifdef __AMSDEBUG__
     else cerr <<" AMSTrRawCluster::buildraw-E-Id.Dead "<<id.gethaddr()<<endl;
#endif
     //     cout <<"br- "<<ic<<" "<<id.getaddr()<<" "
     // <<id.getstrip()<<" "<<leng<<" "<<*((int16*)ptr+2)<<endl;
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
     AMSTrIdSoft idd(ic,int16u(*(ptr)));
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
     if(!idd.dead()){
     // copy to local buffer and subtract peds
     for(j=0;j<len;j++){
      idd.upd(j);
      id[j]=float((*(ptr+1+j)) & 4095)-idd.getped(); 
     }
     // calc cmn noise
      integer vamin,vamax,l;
      for (j=0;j<len;j+=maxva){
         idd.upd(j);
         vamin=j-idd.getstripa();
         vamax=j+maxva-idd.getstripa();
         integer avsig=0;
         for (l=vamin;l<vamax;l++){
           idd.upd(l);
           if(idd.getsignsigraw())idlocal[avsig++]=id[l];
         }
         AMSsortNAGa(idlocal,avsig);
         geant cmn=0;
         if(avsig>2*TRMCFFKEY.CalcCmnNoise[1]+1){
           for(l=TRMCFFKEY.CalcCmnNoise[1];l<avsig-TRMCFFKEY.CalcCmnNoise[1];l++)cmn+=idlocal[l];
           cmn=cmn/(avsig-2*TRMCFFKEY.CalcCmnNoise[1]);
         }
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
            AMSEvent::gethead()->addnext(AMSID("AMSTrRawCluster",ic),pcl);
            j=nright+1;           
         }
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



void AMSTrRawCluster::buildrawRawA(integer n, int16u *p){
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
     AMSTrIdSoft idd(ic,int16u(*(ptr)));
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
     if(!idd.dead()){
     // copy to local buffer and subtract peds
     for(j=0;j<len;j++){
      idd.upd(j);
      id[j]=float((*(ptr+1+j)) & 4095)-idd.getped(); 
     }
     buildpreclusters(idd,len,id);

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





void AMSTrRawCluster::buildrawRawB(integer n, int16u *p){
  integer const ms=640;
  integer len;
  static geant id[ms];
  //VZERO(id,ms*sizeof(id[0])/sizeof(integer));
  int i,j,k;
  integer ic=checkdaqidRaw(*p)-1;
  int16u * ptr=p+1;
  // Main loop
  integer oldformat=1;
  while (ptr<p+n){
    // Read two tdrs
    integer subl=*ptr;
    integer ntdr = *(ptr+1) & 31;
    if(subl != 3084){
      // Probably new format
      oldformat=0; 
    }
    ptr+=2;
    for(i=0;i<ntdr;i++){
     int16u tdrn=*ptr;
     int16u lrec=*(ptr+1);
     ptr++;
     if(tdrn < 16){
       // S side
       len=640;
       for(j=0;j<2;j++){
         int16u conn,tdrs;
         tdrs=tdrn/2;
         if(tdrn%2==0){
          if(j==0)conn=1;
          else conn=0;
         }
         else {
          if(j==0)conn=3;
          else conn=2;
         }
         int16u haddr=(conn<<10) | (tdrs <<12);
         AMSTrIdSoft idd(ic,haddr);
         if(!idd.dead()){
          // copy to local buffer and subtract peds
          for(k=0;k<320;k++){
           idd.upd(k);
           id[k]=float(*(ptr+2+k+j*(640+128*oldformat)))-idd.getped(); 
          }
          for(k=320;k<640;k++){
           idd.upd(k);
           id[k]=float(*(ptr+2+k+64*oldformat+j*(640+128*oldformat)))-idd.getped(); 
          }
          buildpreclusters(idd,len,id);
         }
       }
     }
     else if(tdrn<24){
       // K Side
       len=384;
       for(j=0;j<4;j++){
          int16u conn, tdrk;
          if(tdrn%2 ==0){
            if(j<2)conn=j+2;
            else conn=j-2;
          }
          else {
           if(j<2)conn=j+2;
           else conn=j-2;
           conn+= 4;
          }
          tdrk=(tdrn-16)/2;
          int16u haddr=(10<<6) |( conn<<10) | (tdrk <<13);
          AMSTrIdSoft idd(ic,haddr);
         if(!idd.dead()){
          // copy to local buffer and subtract peds
          for(k=0;k<384;k++){
           idd.upd(k);
          id[k]=float(*(ptr+2+k+j*384))-idd.getped(); 
          }
          buildpreclusters(idd,len,id);
         }
       }
     }
     else {
       cerr<<"trrawcluster::buildrawRawB-ETDRNOutOfRange "<<tdrn<<endl;
     }
     ptr+=lrec;
    }
    if(oldformat == 0){
      ptr+=3;         // add one word
      
    }
  }

}

void AMSTrRawCluster::buildpreclusters(AMSTrIdSoft & idd, integer len, geant id[]){
     // calc cmn noise
  integer const maxva=64;
  static geant idlocal[maxva];
  integer ic=idd.gethalf();
      int i,j,k;
      integer vamin,vamax,l;
      for (j=0;j<len;j+=maxva){
         idd.upd(j);
         vamin=j-idd.getstripa();
         vamax=j+maxva-idd.getstripa();
         integer avsig=0;
         geant cmn=0;
         if(TRCALIB.Method==2){
         for (l=vamin;l<vamax;l++){
           idd.upd(l);
           idlocal[l-vamin]=id[l];
            cmn+=id[l]*idd.getsignsigraw(); 
            avsig+=idd.getsignsigraw();            
         }
         if(avsig>1)cmn=cmn/avsig;
         for(l=vamin;l<vamax;l++){
           idd.upd(l);
           if( !idd.getsignsigraw() ){
            geant cmn=0;
            geant avsig=0;
            for(int kk=0;kk<maxva;kk++){
              if(idd.getrhomatrix(kk)){
               idd.upd(kk+vamin);
               cmn+=idlocal[kk]*fabs(idd.getsigraw());
               avsig++;
              }
            }
            idd.upd(l);
            if(avsig>0 && idd.getsigraw()!=0){
               cmn=cmn/avsig/fabs(idd.getsigraw());
            }
            id[l]+=-cmn;
           }
           else  id[l]+=-cmn;
         }
         }
         else{
         for (l=vamin;l<vamax;l++){
           idd.upd(l);
           if(idd.getsignsigraw())idlocal[avsig++]=id[l];
         }
         AMSsortNAGa(idlocal,avsig);
         if(avsig>2*TRMCFFKEY.CalcCmnNoise[1]+1){
           for(l=TRMCFFKEY.CalcCmnNoise[1];l<avsig-TRMCFFKEY.CalcCmnNoise[1];l++)cmn+=idlocal[l];
           cmn=cmn/(avsig-2*TRMCFFKEY.CalcCmnNoise[1]);
         }
         for(l=vamin;l<vamax;l++)id[l]=id[l]-cmn;



         }
      }
      // Dynamically Update pedestals if needed
      if(TRCALIB.DPS){
       for(j=0;j<len;j++){
          idd.upd(j);
          if(fabs(id[j])<TRCALIB.DPS*idd.getsig()){
           idd.setped()=idd.getped()+id[j]/TRCALIB.UPDF;
         }           
       }
      }
      // Delete Bad Clusters
       for(j=0;j<len;j++){
         idd.upd(j);
         if(idd.checkstatus(AMSDBc::BAD))id[j]=0;
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
            AMSEvent::gethead()->addnext(AMSID("AMSTrRawCluster",ic),pcl);
            j=nright+1;           
         }
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

integer AMSTrRawCluster::checkpedSRawid(int16u id){
if(id==getpedSRawid(0))return 1;
else if(id==getpedSRawid(1))return 2 ;
else return 0;
}

integer AMSTrRawCluster::checksigSRawid(int16u id){
if(id==getsigSRawid(0))return 1;
else if(id==getsigSRawid(1))return 2 ;
else return 0;
}

integer AMSTrRawCluster::checkstatusSRawid(int16u id){
if(id==getstatusSRawid(0))return 1;
else if(id==getstatusSRawid(1))return 2 ;
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

int16u AMSTrRawCluster::getpedSRawid(int i){
  if (i==0)return (3 | 2<<6 | 11 <<9 );
  else if(i==1)return (3 | 5<<6 | 11 <<9 );
else return 0x0;
}

int16u AMSTrRawCluster::getsigSRawid(int i){
  if (i==0)return (4 | 2<<6 | 11 <<9 );
  else if(i==1)return (4 | 5<<6 | 11 <<9 );
else return 0x0;
}

int16u AMSTrRawCluster::getstatusSRawid(int i){
  if (i==0)return (2 | 2<<6 | 11 <<9 );
  else if(i==1)return (2 | 5<<6 | 11 <<9 );
else return 0x0;
}

int16u AMSTrRawCluster::getsigmaSid(int i){
  if (i==0)return (2 | 2<<6 | 11 <<9 | 4<<13);
  else if(i==1)return (2 | 5<<6 | 11 <<9 | 4 <<13);
else return 0x0;
}


integer AMSTrRawCluster::calcstatusSl(integer i){
static integer init =0;
integer j,k,l;
integer lg=0;
if(!TRMCFFKEY.WriteHK || ++init != 2*TRMCFFKEY.WriteHK-1+i)return 0;
for(j=0;j<2;j++){
  for(k=0;k<ntdr;k++){
     AMSTrIdSoft id(i,k,j,0);
     if(id.dead())continue;
     for(l=0;l<AMSDBc::NStripsDrp(id.getlayer(),j);l++){
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
  for(k=0;k<ntdr;k++){
     AMSTrIdSoft id(i,k,j,0);
     if(id.dead())continue;
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
  for(k=0;k<ntdr;k++){
     AMSTrIdSoft id(i,k,j,0);
     if(id.dead())continue;
    l+=AMSDBc::NStripsDrp(1,j)+1;
  }
}
if(l)l++;
return l;
}


void AMSTrRawCluster::updpedSRaw(integer n, int16u* p){


  integer const ms=640;
  integer len;
  int i,j,k;
  integer ic=checkpedSRawid(*p)-1;
  int16u * ptr=p+1;
  // Main loop
  while (ptr<p+n){
    // Read two tdrs
    //    if(*ptr != 3084){
    //  cerr <<"updpedsRaw-E-bad event length, skipped "<<*ptr<<endl; 
    //  return;
    //}
    int ntdr=*(ptr+1) & 31; 
    ptr+=2;
    
    for(i=0;i<ntdr;i++){
     int16u tdrn=*ptr;
     int16u lrec=*(ptr+1);
     ptr++;
     if(tdrn < 16){
       // S side
       len=640;
       for(j=0;j<2;j++){
         int16u conn,tdrs;
         tdrs=tdrn/2;
         if(tdrn%2==0){
          if(j==0)conn=1;
          else conn=0;
         }
         else{
          if(j==0)conn=3;
          else conn=2;
         }
         int16u haddr=(conn<<10) | (tdrs <<12);
         AMSTrIdSoft idd(ic,haddr);
         if(!idd.dead()){
          // copy to local buffer and subtract peds
          for(k=0;k<320;k++){
           idd.upd(k);
           idd.setped()=float(*(ptr+2+k+j*640))/idd.getgain(); 
          }
          for(k=320;k<640;k++){
           idd.upd(k);
           idd.setped()=float(*(ptr+2+k+j*640))/idd.getgain(); 
          }
         }
       }
     }
     else if(tdrn<24){
       // K Side
       len=384;
       for(j=0;j<4;j++){
          int16u conn, tdrk;
          if(tdrn%2 ==0){
            if(j<2)conn=j+2;
            else conn=j-2;
          }
          else {
           if(j<2)conn=j+2;
           else conn=j-2;
           conn+= 4;
          }
          tdrk=(tdrn-16)/2;
          int16u haddr=(10<<6) |( conn<<10) | (tdrk <<13);
          AMSTrIdSoft idd(ic,haddr);
         if(!idd.dead()){
          // copy to local buffer and subtract peds
          for(k=0;k<384;k++){
           idd.upd(k);
           idd.setped()=float(*(ptr+2+k+j*384))/idd.getgain(); 
          }
         }
       }
     }
     else {
       cerr <<"trrawcluster::updpedSRaw-E-TDRNOutOfRange "<<tdrn<<endl;
     }
     ptr+=lrec;
    }
    ptr+=3;
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



void AMSTrRawCluster::updpedS(integer n, int16u* p){

  integer const ms=640;
  int i,j,k,l;
  integer ic=checkpedSid(*p)-1;
  int16u * ptr=p+1;
  // Main loop
  while (ptr<p+n){
     AMSTrIdSoft idd(ic,int16u(*(ptr)));
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





void AMSTrRawCluster::updsigSRaw(integer n, int16u* p){


  integer const ms=640;
  integer len;
  int i,j,k;
  integer ic=checksigSRawid(*p)-1;
  int16u * ptr=p+1;
  // Main loop
  while (ptr<p+n){
    // Read two tdrs
    //    if(*ptr != 3084){
    //  cerr <<"updpedsRaw-E-bad event length, skipped "<<*ptr<<endl; 
    //  return;
    //}
    int ntdr=*(ptr+1) & 31; 
    ptr+=2;
    
    for(i=0;i<ntdr;i++){
     int16u tdrn=*ptr;
     int16u lrec=*(ptr+1);
     ptr++;
     if(tdrn < 16){
       // S side
       len=640;
       for(j=0;j<2;j++){
         int16u conn,tdrs;
         tdrs=tdrn/2;
         if(tdrn%2==0){
          if(j==0)conn=1;
          else conn=0;
         }
         else {
          if(j==0)conn=3;
          else conn=2;
         }
         int16u haddr=(conn<<10) | (tdrs <<12);
         AMSTrIdSoft idd(ic,haddr);
         if(!idd.dead()){
          // copy to local buffer and subtract peds
          for(k=0;k<320;k++){
           idd.upd(k);
           idd.setsig()=float(*(ptr+2+k+j*640))/idd.getgain(); 
          }
          for(k=320;k<640;k++){
           idd.upd(k);
           idd.setsig()=float(*(ptr+2+k+j*640))/idd.getgain(); 
          }
         }
       }
     }
     else if(tdrn<24){
       // K Side
       len=384;
       for(j=0;j<4;j++){
          int16u conn, tdrk;
          if(tdrn%2 ==0){
            if(j<2)conn=j+2;
            else conn=j-2;
          }
          else {
           if(j<2)conn=j+2;
           else conn=j-2;
           conn+= 4;
          }
          tdrk=(tdrn-16)/2;
          int16u haddr=(10<<6) |( conn<<10) | (tdrk <<13);
          AMSTrIdSoft idd(ic,haddr);
         if(!idd.dead()){
          // copy to local buffer and subtract peds
          for(k=0;k<384;k++){
           idd.upd(k);
           idd.setsig()=float(*(ptr+2+k+j*384))/idd.getgain(); 
          }
         }
       }
     }
     else{
       cerr<<"AMSTrRawCluster::updsigSRaw-E-TDRNOutOfRange "<<tdrn<<endl;
     }
     ptr+=lrec;
    }
    ptr+=3;
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





void AMSTrRawCluster::updsigmaS(integer n, int16u* p){

  integer const ms=640;
  int i,j,k,l;
  integer ic=checksigmaSid(*p)-1;
  int16u * ptr=p+1;
  // Main loop
  while (ptr<p+n){
     AMSTrIdSoft idd(ic,int16u(*(ptr)));
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
            AMSTrIdSoft id(i+1,j+1,ic,l,0);
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
     AMSTrIdSoft idd(ic,int16u(*(ptr)));
     // Here put status into memory
      idd.setstatus(AMSDBc::BAD);     
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



void AMSTrRawCluster::updstatusSRaw(integer n, int16u* p){
  // bits from most to least and 1 means good; 

  integer const ms=640;
  integer len;
  int i,j,k;
  integer ic=checkstatusSRawid(*p)-1;
  int16u * ptr=p+1;
  // Main loop
  while (ptr<p+n){
    // Read two tdrsn
    //    if(*ptr != 3084){
    //  cerr <<"updpedsRaw-E-bad event length, skipped "<<*ptr<<endl; 
    //  return;
    //}
    int ntdr=*(ptr+1) & 31; 
    ptr+=2;
    
    for(i=0;i<ntdr;i++){
     int16u tdrn=*ptr;
     int16u lrec=*(ptr+1);
     ptr++;
     if(tdrn < 16){
       // S side
       len=40;
       for(j=0;j<2;j++){
         int16u conn,tdrs;
         tdrs=tdrn/2;
         if(tdrn%2==0){
          if(j==0)conn=1;
          else conn=0;
         }
         else {
          if(j==0)conn=3;
          else conn=2;
         }
         int16u haddr=(conn<<10) | (tdrs <<12);
         AMSTrIdSoft idd(ic,haddr);
         if(!idd.dead()){
          // copy to local buffer and subtract peds
          for(k=0;k<len;k++){
            for(int kbit=0;kbit<16;kbit++){
             idd.upd(k*16+kbit);
             if(*(ptr+2+k+j*len) & (1<<(15-kbit)))idd.clearstatus(AMSDBc::BAD);
             else idd.setstatus(AMSDBc::BAD);
            } 
          }
         }
       }
     }
     else {
       // K Side
       len=24;
       for(j=0;j<4;j++){
          int16u conn, tdrk;
          if(tdrn%2 ==0){
            if(j<2)conn=j+2;
            else conn=j-2;
          }
          else {
           if(j<2)conn=j+2;
           else conn=j-2;
           conn+= 4;
          }
          tdrk=(tdrn-16)/2;
          int16u haddr=(10<<6) |( conn<<10) | (tdrk <<13);
          AMSTrIdSoft idd(ic,haddr);
         if(!idd.dead()){
          // copy to local buffer and subtract peds
          for(k=0;k<len;k++){
            for(int kbit=0;kbit<16;kbit++){
             idd.upd(k*16+kbit);
             if(*(ptr+2+k+j*len) & (1<<(15-kbit)))idd.clearstatus(AMSDBc::BAD);
             else idd.setstatus(AMSDBc::BAD);
            } 
          }
         }
       }
     }
     ptr+=lrec;
    }
    ptr+=3;
  }



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

AMSID AMSTrRawCluster::getTDVCmnNoise(){
  return AMSID("TrackerCmnNoise",AMSJob::gethead()->isRealData());
}

AMSID AMSTrRawCluster::getTDVIndNoise(){
  return AMSID("TrackerIndNoise",AMSJob::gethead()->isRealData());
}



AMSID AMSTrRawCluster::getTDVsigma(int i){
  if(i ==0)return AMSID("TrackerSigmas.l",AMSJob::gethead()->isRealData());
  else if(i ==1)return AMSID("TrackerSigmas.r",AMSJob::gethead()->isRealData());
  else {
    cerr <<"AMSTrRawCluster::getsigmaTDV-F-illegal crate "<<i<<endl;
    exit(1);
  }
}

AMSID AMSTrRawCluster::getTDVrawsigma(int i){
  if(i ==0)return AMSID("TrackerRawSigmas.l",AMSJob::gethead()->isRealData());
  else if(i ==1)return AMSID("TrackerRawSigmas.r",AMSJob::gethead()->isRealData());
  else {
    cerr <<"AMSTrRawCluster::getrawsigmaTDV-F-illegal crate "<<i<<endl;
    exit(1);
  }
}
AMSID AMSTrRawCluster::getTDVrhomatrix(int i){
  if(i ==0)return AMSID("TrackerRhoMatrix.l",AMSJob::gethead()->isRealData());
  else if(i ==1)return AMSID("TrackerRhoMatrix.r",AMSJob::gethead()->isRealData());
  else {
    cerr <<"AMSTrRawCluster::getrhomatrixTDV-F-illegal crate "<<i<<endl;
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
  if(length){
   int16u *paux=ptr;
   *paux=getstatusSid(i);
   for(j=0;j<2;j++){
    for(k=0;k<ntdr;k++){
     AMSTrIdSoft id(i,k,j,0);
     if(id.dead())continue;
     for(l=0;l<AMSDBc::NStripsDrp(id.getlayer(),j);l++){
      id.upd(l);
      if(id.checkstatus(AMSDBc::BAD)){
       *(++paux)=id.gethaddr(1);
      }       
     }
    }
   }
  }
}

void AMSTrRawCluster::writepedS(integer i, integer length, int16u* ptr){
int j,k,l;
integer cleng=0;
if(length){
 int16u *paux=ptr;
 *paux=getpedSid(i);
 for(j=0;j<2;j++){
  for(k=0;k<ntdr;k++){
     AMSTrIdSoft id(i,k,j,0);
     if(id.dead())continue;
      *(++paux)=id.gethaddr(1);
     for(l=0;l<AMSDBc::NStripsDrp(id.getlayer(),j);l++){
      id.upd(l);
       *(paux+1+l)=int16u(id.getped()*id.getgain()) | (1<<15);
     }
     paux+=AMSDBc::NStripsDrp(id.getlayer(),j);
  }
 }
}

}



void AMSTrRawCluster::writesigmaS(integer i, integer length, int16u* ptr){

int j,k,l;
integer cleng=0;
if(length){
 int16u *paux=ptr;
 *paux=getsigmaSid(i);
 for(j=0;j<2;j++){
  for(k=0;k<ntdr;k++){
     AMSTrIdSoft id(i,k,j,0);
     if(id.dead())continue;
     *(++paux)=id.gethaddr(1);
     for(l=0;l<AMSDBc::NStripsDrp(id.getlayer(),j);l++){
      id.upd(l);
      *(paux+1+l)=int16u(id.getsig()*id.getgain()) | (1<<15);
     }
     paux+=AMSDBc::NStripsDrp(id.getlayer(),j);
  }
 }
}



}



integer AMSTrRawCluster::Out(integer status){
static integer init=0;
static integer WriteAll=0;
if(init == 0){
 init=1;
 integer ntrig=AMSJob::gethead()->gettriggerN();
  for(int n=0;n<ntrig;n++){
    if(strcmp("AMSTrRawCluster",AMSJob::gethead()->gettriggerC(n))==0){
     WriteAll=1;
     break;
    }
  }
}
return (WriteAll || status);
}
