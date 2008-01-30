//  $Id: trrawcluster.C,v 1.77 2008/01/30 18:18:02 choutko Exp $
#include "trid.h"
#include "trrawcluster.h"
#include "extC.h"
#include "commons.h"
#include <math.h>
#include "amsgobj.h"
#include "mccluster.h"
#include "timeid.h"
#include "trid.h"
#include "ntuple.h"
#include "event.h"
class TrCalib_def{
public:
integer Layer;
integer Ladder;
integer Half;
integer Side;
integer Strip;
geant Ped;
geant Sigma;
geant RawSigma;
integer BadCh;
geant CmnNoise;
integer Crate;
integer Haddr;
};
using trid::ntdr;
const integer AMSTrRawCluster::MATCHED=1;
integer AMSTrRawCluster::TestRawMode(){
  AMSTrIdSoft id(_address);
  int icmpt=id.gettdr();
  return TRMCFFKEY.RawModeOn[id.getcrate()][id.getside()][icmpt];   

}

integer AMSTrRawCluster::lvl3format(int16 * adc, integer nmax,  integer matchedonly){
  //
  // convert my stupid format to lvl3 one for shuttle flight (mb also stupid)
  //


   AMSTrIdSoft id(_address);
  int16 pos =0;
  id.upd(_strip);
//  if(id.getside()==0 && matchedonly && !checkstatus(MATCHED)){
//   cout << "notmatched K found "<<endl;
//  }
  if (nmax-pos < 2+_nelem || _nelem > 63 || _nelem==0 ||
  (id.getside()==0 && matchedonly && !checkstatus(MATCHED))) return pos;
  adc[pos+1]=id.gethaddr();
  //if(id.getside())cout <<" haddr "<<id.gethaddr()<<" "<<id.gettdr()<<" "<<id.getcrate()<<" "<<id.getdrp()<<endl;
  integer imax=0;
  geant rmax=-1000000;
  int16 sn;
  for (int i=0;i<_nelem;i++){
   id.upd(_strip+i);
   if(id.getsig() && _array[i]/id.getsig() > rmax ){
     rmax=_array[i]/id.getsig();
     sn=(rmax+0.5);
     if(sn>63)sn=63;
     if(sn<0)sn=0;
     imax=i;
   }
   adc[pos+i+2]=int16(_array[i]);
  }
  //  if(id.getside()==1)cout <<"sn "<<sn<<endl;
  if(LVL3FFKEY.SeedThr>0)adc[pos]=(_nelem-1) | (int16u(_s2n*8)<<6); 
  else if(LVL3FFKEY.SeedThr<0)adc[pos]=(_nelem-1) | (sn<<6); 
  else  adc[pos]=(_nelem-1) | (imax<<6); 
    pos+=2+_nelem;
 return pos; 
}





void AMSTrRawCluster::expand(number *adc)const {
AMSTrIdSoft id(_address);
        if(TRCALIB.LaserRun){
        number sum=0;
        number max=-100000;
        int imax=-100000;
        for (int i=0;i<_nelem;i++){
          sum+=_array[i];
          if(_array[i]>max){
             max=_array[i];
             imax=i;
          }
         }
   
        for (int i=0;i<_nelem;i++){
          if(imax>=0)_array[i]+=-max/2;
         }
      } 


  for (int i=0;i<_nelem;i++){
   id.upd(_strip+i);
   if(TRCALIB.LaserRun){
     adc[id.getstrip()]=_array[i]/id.getgain()/id.getlaser();
   }
   else adc[id.getstrip()]=_array[i]/id.getgain();
  }


}



AMSTrRawCluster::~AMSTrRawCluster(){
      UPool.udelete(_array);
}


AMSTrRawCluster::AMSTrRawCluster(integer ad, integer left, integer right, 
                                 geant *p, geant s2n):_address(ad),
                                 _strip(left),_nelem(right-left+1),_s2n(s2n){
    _array=(integer*)UPool.insert(sizeof(_array[0])*_nelem);
    for(int k=0;k<_nelem;k++)_array[k]=integer(*(p+k));  
}

AMSTrRawCluster::AMSTrRawCluster(integer ad, integer left, integer right, 
                                 int16 *p, geant s2n):_address(ad),
                                 _strip(left),_nelem(right-left+1),_s2n(s2n){
    _array=(integer*)UPool.insert(sizeof(_array[0])*_nelem);
    for(int k=0;k<_nelem;k++)_array[k]=*(p+k);  
}




void AMSTrRawCluster::sitkdigi(){

// set defaults
{
    AMSTrMCCluster * ptrhit=
    (AMSTrMCCluster*)AMSEvent::gethead()->getheadC("AMSTrMCCluster",0);    
      while(ptrhit){
           if(ptrhit->IsNoise())ptrhit->setstatus(AMSDBc::AwayTOF);
        ptrhit=ptrhit->next();
      }
}


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
//if(TRMCFFKEY.CalcCmnNoise[0]){
if(1){
  //
  // add sigmas & calculate properly cmnnoise
  //
 AMSgObj::BookTimer.start("SITKDIGIb");
 for(i=0;i<ms;i++){
  if(ida[i]){
    AMSTrIdSoft idd(i);
    integer ilay=idd.getlayer();
    k=idd.getside();
      for (j=0;j<TKDBc::NStripsDrp(ilay,k);j++){
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
         //AMSsortNAGa(idlocal,maxva);
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

// Add noise now
  ptr=(AMSTrMCCluster*)AMSEvent::gethead()->getheadC("AMSTrMCCluster",0);
while(ptr){
   ptr->addcontent('x',ida,1);
   ptr->addcontent('y',ida,1);
   ptr=ptr->next();
}



AMSgObj::BookTimer.stop("SITKDIGIb");

}
AMSgObj::BookTimer.start("SITKDIGIc");
for ( i=0;i<ms;i++){
  if(ida[i]){
     AMSTrIdSoft idd(i);
     integer ilay=idd.getlayer();
     integer crate=idd.getcrate();
     AMSTrRawCluster *pcl;
     int side=idd.getside();
      pcl=0;
      integer nlmin,nlf,nrt;
      integer nleft=0;
      integer nright=0;
      for (j=0;j<TKDBc::NStripsDrp(ilay,side);j++){
      geant s2n=0;
        idd.upd(j);
        if(idd.getsig()>0 && *(ida[i]+j)> TRMCFFKEY.thr1R[side]*idd.getsig()){
          s2n= *(ida[i]+j)/idd.getsig();
          nlmin = nright==0?0:nright+1;
          nleft=max(j-TRMCFFKEY.neib[side],nlmin); 
          for (nlf=nleft;nlf>nlmin;--nlf){
            idd.upd(nlf);
            if(*(ida[i]+nlf)<= TRMCFFKEY.thr2R[side]*idd.getsig())break;
            else nleft=nlf;
          }
          nright=min(j+TRMCFFKEY.neib[side],TKDBc::NStripsDrp(ilay,side)-1);
          for (nrt=nright;nrt<TKDBc::NStripsDrp(ilay,side)-1;++nrt){
            idd.upd(nrt);
            if(*(ida[i]+nrt)<= TRMCFFKEY.thr2R[side]*idd.getsig())break;
            else nright=nrt;
          }
          number sum=0;
          int k;
          for (k=nleft;k<=nright;k++){
           sum+=*(ida[i]+k);
          }
          for ( k=nleft;k<=nright;k++){
            idd.upd(k);
            (*(ida[i]+k))*=idd.getgain();
             number addon=(idd.getped()+idd.getcmnnoise()+sum/maxva)*idd.getavgain();
            if(*(ida[i]+k) +addon> TRMCFFKEY.adcoverflow){
                 *(ida[i]+k)=TRMCFFKEY.adcoverflow-addon;
            }
            else if(*(ida[i]+k) +addon<0){
                 *(ida[i]+k)=0-addon;
            }            
           if(*(ida[i]+k)>32767)*(ida[i]+k)=32767;
           if(*(ida[i]+k)<-32767)*(ida[i]+k)=-32767;
          }
           pcl= new
           AMSTrRawCluster(i,nleft,nright,ida[i]+nleft,s2n);
            AMSEvent::gethead()->addnext(AMSID("AMSTrRawCluster",crate),pcl);
            //            cout <<"si- "<<side<<" "<<i<<" "<<nleft<<" "<<
            //              nright-nleft<<" "<<integer(*(ida[i]+nleft))<<endl;
            j=nright+1;           
        }
      }
      UPool.udelete(ida[i]);
  }
}

  matchKS();
AMSgObj::BookTimer.stop("SITKDIGIc");

}
integer AMSTrRawCluster::_matched(){
  AMSTrIdSoft id(_address);
  if(id.getside()==1 || checkstatus(MATCHED)) return 1;
  else return 0;
}

void AMSTrRawCluster::_writeEl(){
  TrRawClusterNtuple* TrN = AMSJob::gethead()->getntuple()->Get_trraw();
  if(AMSTrRawCluster::Out( IOPA.WriteAll%10==1  )){

// Do not write K clusters if not matched
  AMSTrIdSoft id(_address);
  if(id.getside()==0 && !checkstatus(AMSTrRawCluster::MATCHED)) return;
#ifdef __WRITEROOT__
      AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif

  if (TrN->Ntrraw>=MAXTRRAW) return;

// Fill the ntuple 
    TrN->address[TrN->Ntrraw]=_address+_strip*10000;
    TrN->s2n[TrN->Ntrraw]=_s2n;
    TrN->nelem[TrN->Ntrraw]=_nelem;
    TrN->Ntrraw++;
  }
}


void AMSTrRawCluster::_copyEl(){
}


void AMSTrRawCluster::_printEl(ostream & stream){
  stream <<_address<<" "<<_strip<<" "<<_nelem<<endl;
}


int16 AMSTrRawCluster::getdaqid(int i){
 switch(i){
   case 2:
      return 0;
   case 3:
      return 1;
   case 0:
      return 3;
   case 1:
      return 9;
   case 4:
      return 16;
   case 5:
      return 17;
   case 6:
      return 22;
   case 7:
      return 23;
   default:
      return -1;
}
}


 
int16u AMSTrRawCluster::getdaqidRaw(int i){
  if (i<getmaxblocks())return ( 1 | (AMSTrIdSoft::CrateNo(i))<<6 | 11 <<9);
  else return 0x0;
}

int16u AMSTrRawCluster::getdaqidMixed(int i){
  if (i<getmaxblocks())return ( 12 | (AMSTrIdSoft::CrateNo(i))<<6 | 11 <<9);
  else return 0x0;
}

int16u AMSTrRawCluster::getdaqidParameters(int i){
  if (i<getmaxblocks())return ( 6 | (AMSTrIdSoft::CrateNo(i))<<6 | 11 <<9);
  else return 0x0;
}

int16u AMSTrRawCluster::getdaqidCompressed(int i){
  if (i<getmaxblocks())return ( 14 | (AMSTrIdSoft::CrateNo(i))<<6 | 11 <<9);
  else return 0x0;
}

integer AMSTrRawCluster::checkdaqid(int16u id){
 for(int i=0;i<getmaxblocks();i++){
  if((id)==getdaqid(i))return i+1;
 }
 for(int i=0;i<getmaxblocks();i++){
  char sstr[128];
  sprintf(sstr,"JF-T%d",i);
  if(DAQEvent::ismynode(id,sstr))return i+1; 
 }
 return 0;
}


integer AMSTrRawCluster::checkdaqidS(int16u id){
 
 for(int i=0;i<getmaxblocks();i++){
 for(int j=0;j<trid::ntdr;j++){
  char sstr[128];
  char ab[2];
  ab[1]='\0';
  ab[0]='A';
  if(j%2)ab[0]='B';
  sprintf(sstr,"TDR%X%X%s",i,j/2,ab);
  if(DAQEvent::ismynode(id,sstr)){
       return j+i*trid::ntdr+1; 
  }
 }
 }
 return 0;
}


integer AMSTrRawCluster::checkdaqidRaw(int16u id){
 for(int i=0;i<getmaxblocksRaw();i++){
  if(id==getdaqidRaw(i))return i+1;
 }
 return 0;
}

integer AMSTrRawCluster::checkdaqidMixed(int16u id){
 for(int i=0;i<getmaxblocks();i++){
  if(id==getdaqidMixed(i))return i+1;
 }
 return 0;
}

integer AMSTrRawCluster::checkdaqidParameters(int16u id){
 for(int i=0;i<getmaxblocks();i++){
  if(id==getdaqidParameters(i))return i+1;
 }
 return 0;
}

integer AMSTrRawCluster::checkdaqidCompressed(int16u id){
 for(int i=0;i<getmaxblocks();i++){
  if(id==getdaqidCompressed(i))return i+1;
 }
 return 0;
}

integer AMSTrRawCluster::GetTrCrate(int16u id){
  for(int i=0;i<getmaxblocks();i++){
   if( ((id>>6)&7) == AMSTrIdSoft::CrateNo(i))return i+1;
  }
  return 0;
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
  geant *  adc  = (geant*)UPool.insert(sizeof(adc[0])*TKDBc::maxstripsdrp());
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
     *(paux)=id.gethaddr();     
     // check on the event hits
     VZERO(adc,sizeof(adc[0])/sizeof(integer)*TKDBc::NStripsDrp(id.getlayer(),j));
     AMSTrMCCluster* ptr=(AMSTrMCCluster*)
     AMSEvent::gethead()->getheadC("AMSTrMCCluster",0);
     while(ptr){
       ptr->addcontent(id,adc);
      ptr=ptr->next();
     }
     for(l=0;l<TKDBc::NStripsDrp(id.getlayer(),j);l++){
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
     paux+=1+TKDBc::NStripsDrp(id.getlayer(),j);
#ifdef __AMSDEBUG__
     cleng+=1+TKDBc::NStripsDrp(id.getlayer(),j);
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
   if(!(ptr->TestRawMode()) && ptr->_matched())l+=ptr->_nelem+2;
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
      l+=TKDBc::NStripsDrp(1,j)+1;
    }
  }
}
if(l)l++;
return l;
}


void AMSTrRawCluster::buildraw(integer n, int16u *pbeg){
//  have to split integer n; add crate number on the upper part...
  unsigned int leng=n&65535;
  uinteger ic=(n>>16);
  const int cmn=16;
  integer ic1=checkdaqid(*(pbeg-1+leng))-1;
//  cout <<"  crate "<<ic<<" found" <<" "<<ic1<<endl;
for (int16u* p=pbeg;p<pbeg+leng-1;p+=*p+1){
 //cout <<" length "<<leng<<" "<<*p<<" "<<((*(p+*p))&31)<<endl;
 //if(ic==2)ic=1;
 //if(ic==3)ic=0;
 int16u tdr=(*(p+*p))&31;
 if(DAQEvent::isRawMode(*(p+*p))){
  #ifdef __AMSDEBUG__
   cerr<<" AMSTrRawCluster::buildraw-E-RawModeNotSupportedYet "<<endl;
  #endif
   return;
 }
 if(DAQEvent::isError(*(p+*p))){
  cerr<<" AMSTrRawCluster::buildraw-E-ErrorForTDR "<<tdr<<endl;
  break;
 }
 for(int16u* paux=p+1;paux<p+*p-1-cmn;paux+=*paux+2+1){
  int16u haddr=*(paux+1);
  if(haddr>1023 ){
#ifdef __AMSDEBUG__
    cerr<<"  AMSTrRawCluster::buildraw-E-HaddrOutOfRange  "<<haddr<<" "<<*paux<<endl;
#endif
    continue;
  }
//#ifdef __AMSDEBUG__
 else if(haddr+*(paux)>=1024){
    cerr<<"  AMSTrRawCluster::buildraw-E-HaddrExtOutOfRange "<<haddr<<" "<< haddr+*(paux)<<endl;
    continue;
 }
//#endif
// get common noise also instead of s2n which not exists for ams02
  int va=haddr/64;
  geant cn=*(p+*p-16+va)/8.;
  haddr=haddr| (tdr<<10);
  AMSTrIdSoft id(ic,haddr);
     if(!id.dead() ){
        if(id.getside()==1 && id.getstrip()+*paux>=id.getmaxstrips()){
// algo feature
#ifdef __AMSDEBUG__
    cerr<<"  AMSTrRawCluster::buildraw-W-HaddrExtOutOfRange "<<id.getstrip()<<" "<< id.getstrip()+*(paux)<<endl;
#endif
           int cl=id.getstrip()+*paux-id.getmaxstrips();
           int16u haddr1=id.getmaxstrips();
            haddr1=haddr1| (tdr<<10);
            AMSTrIdSoft id1(ic,haddr1);
            AMSEvent::gethead()->addnext(AMSID("AMSTrRawCluster",ic), new
        AMSTrRawCluster(id1.getaddr(),id1.getstrip(),id1.getstrip()+cl,(int16*)(paux+2+id.getmaxstrips()-id.getstrip()),cn));
          for(int k=id1.getstrip();k<id1.getstrip()+cl+1;k++){
            id1.upd(k);
            if(id1.getgain()==0){
            id1.setgain()=8;
            id1.setsig()=2.5;
            id1.clearstatus(AMSDBc::BAD);
          }
        }

        }
        AMSEvent::gethead()->addnext(AMSID("AMSTrRawCluster",ic), new
        AMSTrRawCluster(id.getaddr(),id.getstrip(),id.getstrip()+*paux>=id.getmaxstrips()?id.getmaxstrips()-1:id.getstrip()+*paux,
        (int16*)(paux+2),cn));
//        cout <<"  id "<<id<<" "<<id.getstrip()<<" "<<id.getstrip()+*paux<<endl;
          for(int k=id.getstrip();k<(id.getstrip()+*paux>=id.getmaxstrips()?id.getmaxstrips()-1:id.getstrip()+*paux);k++){
            id.upd(k);
            if(id.getgain()==0){
            id.setgain()=8;
            id.setsig()=2.5;
            id.clearstatus(AMSDBc::BAD);
          }
        }
     }
#ifdef __AMSDEBUG__
     else{
       cerr <<" AMSTrRawCluster::buildraw-E-Id.Dead "<<id.gethaddr()<<" "<<
       ic<<endl;
     }
#endif
   } 
}
  


#ifdef __AMSDEBUG__
{
  AMSContainer * ptrc =AMSEvent::gethead()->getC("AMSTrRawCluster",ic);
  if(ptrc && AMSEvent::debug>1)ptrc->printC(cout);
}
#endif

   matchKS();
}

 void AMSTrRawCluster::matchKS(){
  // Get rid of K without S 

  for(int crate=0;crate<AMSTrIdSoft::ncrates();crate++){
   AMSTrRawCluster *pk=(AMSTrRawCluster*)AMSEvent::gethead()->
    getheadC("AMSTrRawCluster",crate);
   AMSTrRawCluster *pb=pk;
    while(pk){
       AMSTrIdSoft idk(pk->_address);
       if(idk.getside()==0){
            pk->setstatus(AMSTrRawCluster::MATCHED);
            break;
        AMSTrRawCluster *ps=pb;
        while(ps){
          AMSTrIdSoft ids(ps->_address);
          if(ids.getside()==1 && ids.getdrp()==idk.getdrp()&& ids.getlayer()==idk.getlayer()){
            pk->setstatus(AMSTrRawCluster::MATCHED);
            break;
          }
          ps=ps->next();
        }          
       }
       pk=pk->next();
  }
         

}

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
         for (j=0;j<TKDBc::NStripsDrp(ilay,k);j++){
         idd.upd(j);
         if(id[j]> TRMCFFKEY.thr1R[k]*idd.getsig()){
          geant s2n=id[j]/idd.getsig();
          nlmin = nright==0?0:nright+1; 
          nleft=max(j-TRMCFFKEY.neib[k],nlmin);
          idd.upd(nleft);
          while(nleft >nlmin && id[nleft]> TRMCFFKEY.thr2R[k]*idd.getsig())idd.upd(--nleft);
          nright=min(j+TRMCFFKEY.neib[k],TKDBc::NStripsDrp(ilay,k)-1);
          idd.upd(nright);
          while(nright < TKDBc::NStripsDrp(ilay,k)-1 && 
          id[nright]> TRMCFFKEY.thr2R[k]*idd.getsig())idd.upd(++nright);
          number sum=0;
          int k;
          for (k=nleft;k<=nright;k++){
           sum+=id[k];
          }
          for (k=nleft;k<=nright;k++){
            idd.upd(k);
            id[k]*=idd.getgain();
             number addon=(idd.getped()+idd.getcmnnoise()+sum/maxva)*idd.getavgain();
            if(id[k] +addon> TRMCFFKEY.adcoverflow)id[k]=TRMCFFKEY.adcoverflow-addon;
            if(id[k] +addon<0)id[k]=0-addon;
          }
           if(id[k]>32767)id[k]=32767;
           if(id[k]<-32767)id[k]=-32767;
           pcl= new
           AMSTrRawCluster(idd.getaddr(),nleft,nright,id+nleft,s2n);
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
    uinteger subl=*ptr;
    if(subl ==0){
      cerr <<"AMSTrRawCluster::buildrawRawB-E-SubLengthZero, skipped event"<<endl;
      return;
    } 
    int16u *ptro=ptr;
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
          if(j==0)conn=0;
          else conn=1;
         }
         else {
          if(j==0)conn=2;
          else conn=3;
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
            if(j<2)conn=j;
            else conn=j;
          }
          else {
           if(j<2)conn=j;
           else conn=j;
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
      //      ptr+=3;         // add one word
      ptr=ptro+subl;
    }
  }

  // Get rid of K without S 

  matchKS();         


}


void AMSTrRawCluster::buildrawMixed(integer n, int16u *p){
}




void AMSTrRawCluster::buildrawCompressed(integer n, int16u *p){
  integer len;
  int i,j,k;
  integer ic=GetTrCrate(*p)-1;
  int16u * ptr=p+1;
  int tl=0;
  // Main loop
  while (ptr<p+n){
    // Read two tdrs
    uinteger subl=*ptr;
    if(subl ==0){
      cerr <<"AMSTrRawCluster::buildrawCompressed-E-SubLengthZero, skipped event"<<endl;
      return;
    } 
    tl+=subl;
    //cout <<"tl "<<tl<<" "<<subl<<" "<<n<<endl;
    int16u *ptro=ptr;
    integer ntdr = *(ptr+1) & 31;
    if(tl>n+1 ){
      cerr <<" AMSTrRawCluster::buildrawCompressed-E-WrongStructureSkipEvent "<<tl<<" "<<n+1<<endl;
     return;
    }
    ptr+=2;
    for(i=0;i<ntdr;i++){
     int16u tdrn=*ptr;
     
     ptr+=1;
     int rl=*ptr;
     //cout <<"tdrn "<<tdrn<<" "<<rl<<endl;
      int16u *p2=ptr;
      int leng=0;
      for(p2=ptr+2;p2<ptr+rl-1;p2+=leng+3){
        leng=(*p2)&63;
        // change here TSRA
        if(tdrn<16){
          //S
          *(p2+1)= (*(p2+1)) & (~(15<<11));
          *(p2+1)= (*(p2+1)) | (tdrn<<11);
        }
        else {
          // K;
                    *(p2+1)= (*(p2+1)) & (~(7<<12));
                    *(p2+1)= (*(p2+1)) | ((tdrn-16)<<12);
        }
        AMSTrIdSoft id(ic,int16u(*(p2+1)));
        if(!id.dead() ){
          int16 sn=(((*p2)>>6)&63);
          float s2n=float(sn)/8;
          //          cout <<id.getstrip()<<" "<<leng<<endl;
          if(id.teststrip(id.getstrip()+leng)){
            AMSEvent::gethead()->addnext(AMSID("AMSTrRawCluster",ic), new
            AMSTrRawCluster(id.getaddr(),id.getstrip(),id.getstrip()+leng,
            (int16*)p2+2,s2n));
            //cout <<" ok "<<id.getaddr()<<" "<<id.getstrip()<<" "<<leng<<endl;
          }
          else {
            // split into two clusters;
            //cout <<" split "<<id.getaddr()<<" "<<id.getstrip()<<" "<<leng<<endl;
            AMSEvent::gethead()->addnext(AMSID("AMSTrRawCluster",ic), new
            AMSTrRawCluster(id.getaddr(),id.getstrip(),id.getmaxstrips()-1,
           (int16*)p2+2,s2n));
            integer second=id.getstrip()+leng-id.getmaxstrips();
            id.upd(0);
            AMSEvent::gethead()->addnext(AMSID("AMSTrRawCluster",ic), new
            AMSTrRawCluster(id.getaddr(),id.getstrip(),second,
            (int16*)p2+2+leng-second,s2n));
          }
        }
        #ifdef __AMSDEBUG__
        else cerr <<" AMSTrRawCluster::buildrawCompressed-E-Id.Dead "<<id.gethaddr()<<" "<<rl<<" "<<leng<<endl;
        #endif
        //     cout <<"br- "<<ic<<" "<<id.getaddr()<<" "
        // <<id.getstrip()<<" "<<leng<<" "<<*((int16*)p2+2)<<endl;
      } 
     ptr+=rl;
     
    }
      ptr=ptro+subl;         // add one word
      //cout <<" go next block "<<" "<<subl<<" "<<ptr<<" "<<p+n<<endl;
  }

  // Get rid of K without S 

     matchKS();         


}





void AMSTrRawCluster::buildrawParameters(integer n, int16u *p){
  integer len;
  int i,j,k;
  integer ic=GetTrCrate(*p)-1;
  int16u * ptr=p+1;
  int tl=0;
  // Main loop
  while (ptr<p+n){
    uinteger subl=*ptr;
    if(subl ==0){
      cerr <<"AMSTrRawCluster::buildrawParameters-E-SubLengthZero, skipped event"<<endl;
      return;
    } 
    tl+=subl;
    int16u *ptro=ptr;
    integer ntdr = *(ptr+1) & 31;
    ptr+=2;
    for(i=0;i<ntdr;i++){
     int16u tdrn=*ptr;

     if(tdrn < 16){
       // S side
       for(j=0;j<2;j++){
         int16u conn,tdrs;
         tdrs=tdrn/2;
         if(tdrn%2==0){
          if(j==0)conn=0;
          else conn=1;
         }
         else {
          if(j==0)conn=2;
          else conn=3;
         }
         int16u haddr=(conn<<10) | (tdrs <<12);
         AMSTrIdSoft idd(ic,haddr);
         if(!idd.dead()){
#ifndef __AMSDEBUG__
         if(tdrn==0)
#endif
           cout <<"For id "<<idd;
         }
       }
     }
     else if(tdrn<24){
       // K Side
       len=384;
       for(j=0;j<4;j++){
          int16u conn, tdrk;
          if(tdrn%2 ==0){
            if(j<2)conn=j;
            else conn=j;
          }
          else {
           if(j<2)conn=j;
           else conn=j;
           conn+= 4;
          }
          tdrk=(tdrn-16)/2;
          int16u haddr=(10<<6) |( conn<<10) | (tdrk <<13);
          AMSTrIdSoft idd(ic,haddr);
         if(!idd.dead()){
#ifndef __AMSDEBUG__
         if(tdrn==16)
#endif
           cout <<"For id "<<idd;
         }
       }
     }
     else {
       cerr<<"trrawcluster::buildrawParameters-ETDRNOutOfRange "<<tdrn<<endl;
     }


     
     ptr+=1;
     int rl=*ptr;
      static char descr[40][128]={
        "Number of events to throw out before beginning calibration(128)",
        "Number of events for pedestal calculation(2048)",
        "Number of events for sigma raw calculation(512)",
        "Number of events for sigma Pass1 calculation(1024)",
        "Number of events for sigma Pass2 calculation(0)",
        "Number of events for non gaussien pass(2048)",
        "Step in histo to seek peak value (in 1/8)(1)",
        "Number of bin for histo (max 128)(64)",
        "f1 Cut for histo (in 1/8)(14)",
        "f2 Cut for histo pass 2 (in 1/8)(10)",
        "g Cut for non gaussien behaviour (in 1/8)(24)",
        "out_g Number of event out of bound for non gaussien(50)",
        "Seed cut (in 1/8)(24)",
        "Neigbour cut (in 1/8)(8)",
        "ADC min(48)",
        "Number of events between 2 dynamical pedestal(0 means never)(0)",
        "spare(0)",
        "spare(0)",
        "spare(0)",
        "spare(0)",
        "spare(0)",
        "spare(0)",
        "spare(0)",
        "spare(0)",
        "spare(0)",
        "spare(0)",
        "spare(0)",
        "spare(0)",
        "spare(0)",
        "spare(0)",
        "Number of strip for laser in intensity 1(0)",
        "First strip of region u1,1(0)",
        "First strip of region o1,1(0)",
        "First strip of region u1,2(0)",
        "First strip of region o1,2(0)",
        "Number of strip for laser in intensity 2(0)",
        "First strip of region u2,1(0)",
        "First strip of region o2,1(0)",
        "First strip of region u2,2(0)",
        "First strip of region o2,2(0)"
      }; 
#ifndef __AMSDEBUG__
      if(tdrn==0 || tdrn==16){
#endif 
        for(int kl=0;kl<40;kl++){
          cout <<descr[kl]<<" "<<*(ptr+2+kl)<<endl;
        }
        cout <<" "<<endl;                    
        
#ifndef __AMSDEBUG__
      }
#endif 

     ptr+=rl;
     
    }
      ptr=ptro+subl;        
  }



}










void AMSTrRawCluster::buildpreclusters(AMSTrIdSoft & idd, integer len, geant id[]){
     // calc cmn noise
  integer const maxva=64;
  static geant idlocal[maxva];
  integer ic=idd.getcrate();
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
         for (j=0;j<TKDBc::NStripsDrp(ilay,k);j++){
         idd.upd(j);
         geant s2n=0;
         if(id[j]> TRMCFFKEY.thr1R[k]*idd.getsig()){
          s2n=id[j]/idd.getsig();
          nlmin = nright==0?0:nright+1; 
          nleft=max(j-TRMCFFKEY.neib[k],nlmin);
          idd.upd(nleft);
          while(nleft >nlmin && id[nleft]> TRMCFFKEY.thr2R[k]*idd.getsig())idd.upd(--nleft);
          nright=min(j+TRMCFFKEY.neib[k],TKDBc::NStripsDrp(ilay,k)-1);
          idd.upd(nright);
          while(nright < TKDBc::NStripsDrp(ilay,k)-1 && 
          id[nright]> TRMCFFKEY.thr2R[k]*idd.getsig())idd.upd(++nright);
          number sum=0;
          int k;
          for (k=nleft;k<=nright;k++){
           sum+=id[k];
          }
          for ( k=nleft;k<=nright;k++){
            idd.upd(k);
            id[k]*=idd.getgain();
             number addon=(idd.getped()+idd.getcmnnoise()+sum/maxva)*idd.getavgain();
            if(id[k] +addon> TRMCFFKEY.adcoverflow)id[k]=TRMCFFKEY.adcoverflow-addon;
            if(id[k] +addon<0)id[k]=0-addon;
           if(id[k]>32767)id[k]=32767;
           if(id[k]<-32767)id[k]=-32767;
          }
           pcl= new
           AMSTrRawCluster(idd.getaddr(),nleft,nright,id+nleft,s2n);
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

integer AMSTrRawCluster::checkcmnSRawid(int16u id){
if(id==getcmnSRawid(0))return 1;
else if(id==getcmnSRawid(1))return 2 ;
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
  if (i==0)return (4 | 2<<6 | 11 <<9 );
  else if(i==1)return (4 | 5<<6 | 11 <<9 );
else return 0x0;
}

int16u AMSTrRawCluster::getcmnSRawid(int i){
  if (i==0)return (6 | 2<<6 | 11 <<9 );
  else if(i==1)return (6 | 5<<6 | 11 <<9 );
else return 0x0;
}

int16u AMSTrRawCluster::getsigSRawid(int i){
  if (i==0)return (3 | 2<<6 | 11 <<9 );
  else if(i==1)return (3 | 5<<6 | 11 <<9 );
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
     for(l=0;l<TKDBc::NStripsDrp(id.getlayer(),j);l++){
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
    l+=TKDBc::NStripsDrp(1,j)+1;
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
    l+=TKDBc::NStripsDrp(1,j)+1;
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
    uinteger subl=*ptr;
    if(subl ==0){
      cerr <<"updpedsRaw-E-SubLengthZero, skipped event"<<endl;
      return;
    } 
    int16u *ptro=ptr;
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
          if(j==0)conn=0;
          else conn=1;
         }
         else{
          if(j==0)conn=2;
          else conn=3;
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
            if(j<2)conn=j;
            else conn=j;
          }
          else {
           if(j<2)conn=j;
           else conn=j;
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
    ptr=ptro+subl;
  }



  // Set UpdateMe for corr TDV

  AMSTimeID * ptdv = AMSJob::gethead()->gettimestructure(getTDVped(ic));
   ptdv->UpdateMe()=1;
   ptdv->UpdCRC();
   time_t begin,end,insert;
   time(&insert);
   ptdv->SetTime(AMSEvent::gethead()->gettime(),AMSEvent::gethead()->getrun(),AMSEvent::gethead()->getrun()+86400);
   cout <<" Tracker H/K  info has been read for "<<*ptdv;
   ptdv->gettime(insert,begin,end);
   cout <<" Time Insert "<<ctime(&insert);
   cout <<" Time Begin "<<ctime(&begin);
   cout <<" Time End "<<ctime(&end);
}

void AMSTrRawCluster::updtrcalibS(integer n, int16u* p){
  uinteger leng=(n&65535);
  leng-=10;
  uinteger in=(n>>16);
  uinteger ic=in/trid::ntdr;
  uinteger tdr=in%trid::ntdr;
//  cout << "crate tdr "<<ic<<" "<<tdr<<endl;
  int leng4=leng/4;
  if(leng4!=1024){
   cerr<<"AMSTrRawCluster::updtrcalibS-E-WrongLength "<<leng4<<endl;
   return;
  }
  cout <<"  Crate TDR "<<ic<<"  "<<tdr<<endl;
  if(AMSTrIdSoft::_Calib[ic][tdr]==1){
   cerr<<" AMSTrRawCluster::updtrcalibS-W-ICTDRAlreadySet "<<ic<<" "<<tdr<<endl;
  }
  AMSTrIdSoft::_Calib[ic][tdr]=1;

  int version=*(p+leng);
  geant sig_d=*(p+leng+1);
  geant rawsig_d=*(p+leng+3);
  cout <<version<<" "<<sig_d<<" "<<rawsig_d<<endl;
 int nerr=0;
 cout <<"  AMSTrRawCluster::updtrcalibS-I-Parameters ";
 for(int i=0;i<13;i++){
  cout <<*(p+leng+i)<<" ";
 }
 cout <<endl;
 for(int i=0;i<leng4;i++){
  int16u haddr=(tdr<<10) | i;
  AMSTrIdSoft id(ic,haddr);
  if(!id.dead()){
   if(id.getgain()==0){
    id.setgain()=TRMCFFKEY.gain[id.getside()];
   }
   if(*(p+leng4+i)!=0)id.setstatus(AMSDBc::BAD);
   else id.clearstatus(AMSDBc::BAD);
   if(*(p+i)>32768)id.setstatus(AMSDBc::BAD);
   else id.setped()=geant(*(p+i))/id.getgain();
   if(*(p+i+2*leng4)>32768)id.setstatus(AMSDBc::BAD);
   else id.setsig()=geant(*(p+i+2*leng4))/sig_d;
   if(*(p+i+3*leng4)>32768)id.setstatus(AMSDBc::BAD);
   else id.setsigraw()=geant(*(p+i+3*leng4))/id.getgain()/rawsig_d;
   if(id.setsig()>id.setsigraw()){
    nerr++;
//     cerr <<"  AMSTrRawCluster::updtrcalibS-E-SigmaProblems "<<id.getsigraw()<<" "<<id.getsig() <<" "<<id<<endl;
//     cerr <<"  AMSTrRawCluster::updtrcalibS-I-Version "<<version<<" "<<sig_d<<" "<<rawsig_d<<endl;
   }
   if(i%64==0){
    number cmn=0;
    number ncmn=0;
    for (int k=i;k<i+64;k++){
      if(*(p+i+2*leng4)<32768 &&  *(p+i+3*leng4)<32768){
       ncmn++;
       cmn+=((*(p+i+3*leng4))*(*(p+i+3*leng4))/id.getgain()/rawsig_d/id.getgain()/rawsig_d-(*(p+i+2*leng4))*(*(p+i+2*leng4))/sig_d/sig_d);
      }   
    }
    if(ncmn>32){
      if(cmn>0)cmn=sqrt(cmn/ncmn);
      else cmn=0;
      id.setcmnnoise()=cmn;
    }
    else id.setstatus(AMSDBc::BAD);
   }
  }  
#ifdef __AMSDEBUG__
     else{
       cerr <<" AMSTrRawCluster::updtrcalibS-E-Id.Dead "<<id.gethaddr()<<" "<<
       ic<<endl;
     }
#endif
}
if(nerr>0){
     cerr <<"  AMSTrRawCluster::updtrcalibS-E-RawSigmaProblems "<<nerr<<endl;
}
  bool update=true;
  int nc=0;
  for(int i=0;i<getmaxblocks();i++){
   for (int j=0;j<trid::ntdr;j++){
    if(!AMSTrIdSoft::_Calib[i][j]){
     update=false;
    }
    else nc++;
  }
  }
  cout <<" nc "<<nc<<endl;
  if(update || nc>=TRCALIB.EventsPerCheck){
   for(int i=0;i<getmaxblocks();i++){
    for (int j=0;j<trid::ntdr;j++){
     AMSTrIdSoft::_Calib[i][j]=0;
    }
   }
   for (int i=0;i<2;i++){
   AMSTimeID * ptdv;
  for (int k=0;k<=TRCALIB.Method;k++){
    if(k==0)ptdv = AMSJob::gethead()->gettimestructure(getTDVped(i));
    else if(k==1)ptdv = AMSJob::gethead()->gettimestructure(getTDVsigma(i));
    else if(k==2)ptdv = AMSJob::gethead()->gettimestructure(getTDVstatus(i));
    else if(k==3)ptdv = AMSJob::gethead()->gettimestructure(getTDVrawsigma(i));
    else if(k==4)ptdv = AMSJob::gethead()->gettimestructure(getTDVgains(i));
    else if(k==5 && i==0)ptdv = AMSJob::gethead()->gettimestructure(getTDVCmnNoise());
    else break;
   if(update)ptdv->UpdateMe()=1;
   ptdv->UpdCRC();
   time_t begin,end,insert;
   time(&insert);
   ptdv->SetTime(AMSEvent::gethead()->gettime(),AMSEvent::gethead()->gettime()-1,AMSEvent::gethead()->gettime()+8640000);
   cout <<" Tracker H/K  info has been read for "<<*ptdv;
   ptdv->gettime(insert,begin,end);
   cout <<" Time Insert "<<ctime(&insert);
   cout <<" Time Begin "<<ctime(&begin);
   cout <<" Time End "<<ctime(&end);
   }
  }
    char hfile[161];
    UHTOC(IOPA.hfile,40,hfile,160);  
    char filename[256];
    strcpy(filename,hfile);
    sprintf(filename,"%s.%d",hfile,AMSEvent::gethead()->gettime());
    integer iostat;
    integer rsize=1024;
    char event[80];  
    HROPEN(IOPA.hlun+1,"trcalibration",filename,"NP",rsize,iostat);
    if(iostat){
     cerr << "Error opening trcalib ntuple file "<<filename<<endl;
     exit(1);
    }
    else cout <<"trcalib ntuple file "<<filename<<" opened."<<endl;

   TrCalib_def TRCALIB;
   HBNT(IOPA.ntuple,"Tracker Calibaration"," ");
   HBNAME(IOPA.ntuple,"TrCalib",(int*)(&TRCALIB),"PSLayer:I,PSLadder:I,PSHalf:I,PSSide:I, PSStrip:I,Ped:R,Sigma:R,RawSigma:R,BadCh:I,CmnNoise:R,Crate:I,Haddr:I");
   int i,j,k,l,m;
    for(l=0;l<2;l++){
    for(k=0;k<2;k++){
     for(i=0;i<TKDBc::nlay();i++){
       for(j=0;j<TKDBc::nlad(i+1);j++){
        AMSTrIdSoft id(i+1,j+1,k,l,0);
        if(id.dead())continue;
        for(m=0;m<TKDBc::NStripsDrp(i+1,l);m++){
          id.upd(m);
          TRCALIB.Layer=i+1;
          TRCALIB.Ladder=j+1;
          TRCALIB.Half=k;
          TRCALIB.Side=l;
          TRCALIB.Strip=m;
          TRCALIB.Ped=id.getped();
          TRCALIB.Sigma=id.getsig();
          TRCALIB.RawSigma=id.getsigraw();
          TRCALIB.BadCh=id.checkstatus(AMSDBc::BAD);
          TRCALIB.CmnNoise=id.getcmnnoise();
          TRCALIB.Crate=id.getcrate();
          TRCALIB.Haddr=id.gethaddr();
          HFNT(IOPA.ntuple);
         }
        }
       }
     }
    }
  char hpawc[256]="//PAWC";
  HCDIR (hpawc, " ");
  char houtput[]="//trcalibration";
  HCDIR (houtput, " ");
  integer ICYCL=0;
  HROUT (1, ICYCL, " ");
  HREND ("trcalibration");
  CLOSEF(IOPA.hlun+1);

  }
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
   ptdv->SetTime(AMSEvent::gethead()->gettime(),AMSEvent::gethead()->getrun(),AMSEvent::gethead()->getrun()+86400);
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
    uinteger subl=*ptr;
    if(subl ==0){
      cerr <<"AMSTrRawCluster::updsigSRaw-E-SubLengthZero, skipped event"<<endl;
      return;
    } 
    int16u *ptro=ptr;
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
          if(j==0)conn=0;
          else conn=1;
         }
         else {
          if(j==0)conn=2;
          else conn=3;
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
            if(j<2)conn=j;
            else conn=j;
          }
          else {
           if(j<2)conn=j;
           else conn=j;
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
    ptr=ptro+subl;
  }



  // Set UpdateMe for corr TDV

  AMSTimeID * ptdv = AMSJob::gethead()->gettimestructure(getTDVsigma(ic));
   ptdv->UpdateMe()=1;
   ptdv->UpdCRC();
   time_t begin,end,insert;
   time(&insert);
   ptdv->SetTime(AMSEvent::gethead()->gettime(),AMSEvent::gethead()->getrun(),AMSEvent::gethead()->getrun()+86400);
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
   ptdv->SetTime(AMSEvent::gethead()->gettime(),AMSEvent::gethead()->getrun(),AMSEvent::gethead()->getrun()+86400);
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

    for ( i=0;i<TKDBc::nlay();i++){
      for ( j=0;j<TKDBc::nlad(i+1);j++){
        for ( l=0;l<2;l++){
            AMSTrIdSoft id(i+1,j+1,ic,l,0);
            for( k=0;k<TKDBc::NStripsDrp(i+1,l);k++){
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
   ptdv->SetTime(AMSEvent::gethead()->gettime(),AMSEvent::gethead()->getrun(),AMSEvent::gethead()->getrun()+86400);
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
    uinteger subl=*ptr;
    if(subl ==0){
      cerr <<"AMSTrRawCluster::updstatusSRaw-E-SubLengthZero, skipped event"<<endl;
      return;
    } 
    int16u *ptro=ptr;
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
          if(j==0)conn=0;
          else conn=1;
         }
         else {
          if(j==0)conn=2;
          else conn=3;
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
            if(j<2)conn=j;
            else conn=j;
          }
          else {
           if(j<2)conn=j;
           else conn=j;
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
    ptr=ptro+subl;
  }



  // Set UpdateMe for corr TDV

  AMSTimeID * ptdv = AMSJob::gethead()->gettimestructure(getTDVstatus(ic));
   ptdv->UpdateMe()=1;
   ptdv->UpdCRC();
   time_t begin,end,insert;
   time(&insert);
   ptdv->SetTime(AMSEvent::gethead()->gettime(),AMSEvent::gethead()->getrun(),AMSEvent::gethead()->getrun()+86400);
   cout <<" Tracker H/K  info has been read for "<<*ptdv;
   ptdv->gettime(insert,begin,end);
   cout <<" Time Insert "<<ctime(&insert);
   cout <<" Time Begin "<<ctime(&begin);
   cout <<" Time End "<<ctime(&end);
}



void AMSTrRawCluster::updcmnSRaw(integer n, int16u* p){
  // bits from most to least and 1 means good; 

  integer const ms=640;
  integer len;
  int i,j,k;
  integer ic=checkcmnSRawid(*p)-1;
  int16u * ptr=p+1;
  // Main loop
  while (ptr<p+n){
    uinteger subl=*ptr;
    if(subl ==0){
      cerr <<"AMSTrRawCluster::updcmnSRaw-E-SubLengthZero, skipped event"<<endl;
      return;
    } 
    int16u *ptro=ptr;
    int ntdr=*(ptr+1) & 31; 
    ptr+=2;
    
    for(i=0;i<ntdr;i++){
     int16u tdrn=*ptr;
     int16u lrec=*(ptr+1);
     ptr++;
     if(tdrn < 16){
       // S side
       len=10;
       for(j=0;j<2;j++){
         int16u conn,tdrs;
         tdrs=tdrn/2;
         if(tdrn%2==0){
          if(j==0)conn=0;
          else conn=1;
         }
         else {
          if(j==0)conn=2;
          else conn=3;
         }
         int16u haddr=(conn<<10) | (tdrs <<12);
         AMSTrIdSoft idd(ic,haddr);
         if(!idd.dead()){
          for(k=0;k<len;k++){
             idd.upd(k*64);
             idd.setcmnnoise()=*(ptr+2+k+j*len)/idd.getgain();
          }
         }
       }
     }
     else {
       // K Side
       len=6;
       for(j=0;j<4;j++){
          int16u conn, tdrk;
          if(tdrn%2 ==0){
            if(j<2)conn=j;
            else conn=j;
          }
          else {
           if(j<2)conn=j;
           else conn=j;
           conn+= 4;
          }
          tdrk=(tdrn-16)/2;
          int16u haddr=(10<<6) |( conn<<10) | (tdrk <<13);
          AMSTrIdSoft idd(ic,haddr);
         if(!idd.dead()){
          for(k=0;k<len;k++){
             idd.upd(k*64);
             idd.setcmnnoise()=*(ptr+2+k+j*len)/idd.getgain();
          }
         }
       }
     }
     ptr+=lrec;
    }
    ptr=ptro+subl;
  }



  // Set UpdateMe for corr TDV

  AMSTimeID * ptdv = AMSJob::gethead()->gettimestructure(getTDVCmnNoise());
   ptdv->UpdateMe()=1;
   ptdv->UpdCRC();
   time_t begin,end,insert;
   time(&insert);
   ptdv->SetTime(AMSEvent::gethead()->gettime(),AMSEvent::gethead()->getrun(),AMSEvent::gethead()->getrun()+86400);
   cout <<" Tracker H/K  info has been read for "<<*ptdv;
   ptdv->gettime(insert,begin,end);
   cout <<" Time Insert "<<ctime(&insert);
   cout <<" Time Begin "<<ctime(&begin);
   cout <<" Time End "<<ctime(&end);
}







AMSID AMSTrRawCluster::getTDVstatus(int i){
  return AMSID(AMSTrIdSoft::TrackerStatus(i),AMSJob::gethead()->isRealData());
}

AMSID AMSTrRawCluster::getTDVCmnNoise(){
  return AMSID(AMSTrIdSoft::TrackerCmnNoise(),AMSJob::gethead()->isRealData());
}

AMSID AMSTrRawCluster::getTDVIndNoise(){
  return AMSID("TrackerIndNoise",AMSJob::gethead()->isRealData());
}



AMSID AMSTrRawCluster::getTDVsigma(int i){
  return AMSID(AMSTrIdSoft::TrackerSigmas(i),AMSJob::gethead()->isRealData());
}

AMSID AMSTrRawCluster::getTDVrawsigma(int i){
  return AMSID(AMSTrIdSoft::TrackerRawSigmas(i),AMSJob::gethead()->isRealData());
}
AMSID AMSTrRawCluster::getTDVrhomatrix(int i){
  return AMSID(AMSTrIdSoft::TrackerRhoMatrix(i),AMSJob::gethead()->isRealData());
}


AMSID AMSTrRawCluster::getTDVped(int i){
  return AMSID(AMSTrIdSoft::TrackerPedestals(i),AMSJob::gethead()->isRealData());
}

AMSID AMSTrRawCluster::getTDVgains(int i){
  return AMSID(AMSTrIdSoft::TrackerGains(i),AMSJob::gethead()->isRealData());
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
     for(l=0;l<TKDBc::NStripsDrp(id.getlayer(),j);l++){
      id.upd(l);
      if(id.checkstatus(AMSDBc::BAD)){
       *(++paux)=id.gethaddr();
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
      *(++paux)=id.gethaddr();
     for(l=0;l<TKDBc::NStripsDrp(id.getlayer(),j);l++){
      id.upd(l);
       *(paux+1+l)=int16u(id.getped()*id.getgain()) | (1<<15);
     }
     paux+=TKDBc::NStripsDrp(id.getlayer(),j);
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
     *(++paux)=id.gethaddr();
     for(l=0;l<TKDBc::NStripsDrp(id.getlayer(),j);l++){
      id.upd(l);
      *(paux+1+l)=int16u(id.getsig()*id.getgain()) | (1<<15);
     }
     paux+=TKDBc::NStripsDrp(id.getlayer(),j);
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
