//  $Id: trrawcluster.C,v 1.116 2011/09/28 07:50:04 choutko Exp $
#include "trid.h"
#include "trrawcluster.h"
#include "extC.h"
#include "commons.h"
#include <math.h>
#include "amsgobj.h"
#include "trmccluster.h"
#include "timeid.h"
#include "trid.h"
#include "ntuple.h"
#include "event.h"
#include <map>
#include <vector>
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
//     sn=(rmax+0.5);
//     if(sn>63)sn=63;
//     if(sn<0)sn=0;
     imax=i;
   }
   if(rmax<-0.5)rmax=-0.5;
   sn=(rmax+0.5)>63?63:(rmax+0.5);
   adc[pos+i+2]=int16(_array[i]);
  }
  //  if(id.getside()==1)cout <<"sn "<<sn<<endl;
  if(LVL3FFKEY.SeedThr>0)adc[pos]=(_nelem-1) | (int16u(_s2n*8)<<6); 
  else if(LVL3FFKEY.SeedThr<0)adc[pos]=(_nelem-1) | (sn<<6); 
  else  adc[pos]=(_nelem-1) | (imax<<6); 
    pos+=2+_nelem;
 return pos; 
}





void AMSTrRawCluster::expand(number *adc) {
AMSTrIdSoft id(_address);
        if(0&& TRCALIB.LaserRun>0){
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
        if(imax>=0)_sub=max/2;
      }


  for (int i=0;i<_nelem;i++){
   id.upd(_strip+i);
   if(TRCALIB.LaserRun>0){
     adc[id.getstrip()]=_array[i]/id.getgain()/id.getlaser();
   }
   else adc[id.getstrip()]=_array[i]/id.getgain();
  }


}



AMSTrRawCluster::~AMSTrRawCluster(){
      UPool.udelete(_array);
}

geant AMSTrRawCluster::getamp(int k){
    return k>=0 && k<_nelem?_array[k]+_sub:0;
}


AMSTrRawCluster::AMSTrRawCluster(integer ad, integer left, integer right, 
                                 geant *p, geant s2n):_address(ad),
                                 _strip(left),_nelem(right-left+1),_s2n(s2n),_sub(0){
    _array=(integer*)UPool.insert(sizeof(_array[0])*_nelem);
    for(int k=0;k<_nelem;k++)_array[k]=integer(*(p+k));  
}

AMSTrRawCluster::AMSTrRawCluster(integer ad, integer left, integer right, 
                                 int16 *p, geant s2n):_address(ad),
                                 _strip(left),_nelem(right-left+1),_s2n(s2n),_sub(0){
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
//  TrRawClusterNtuple* TrN = AMSJob::gethead()->getntuple()->Get_trraw();
  if(AMSTrRawCluster::Out( IOPA.WriteAll%10==1  )){

// Do not write K clusters if not matched
  AMSTrIdSoft id(_address);
  if(id.getside()==0 && !checkstatus(AMSTrRawCluster::MATCHED)) return;
#ifdef __WRITEROOT__
      AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif
/*
  if (TrN->Ntrraw>=MAXTRRAW) return;

// Fill the ntuple 
    TrN->address[TrN->Ntrraw]=_address+_strip*10000;
    TrN->s2n[TrN->Ntrraw]=_s2n;
    TrN->nelem[TrN->Ntrraw]=_nelem;
    TrN->Ntrraw++;
*/
  }
}


void AMSTrRawCluster::_copyEl(){
}


void AMSTrRawCluster::_printEl(ostream & stream){
  stream <<_address<<" "<<_strip<<" "<<_nelem<<endl;
}


integer AMSTrRawCluster::getdaqid(int16u crate){
 for(int i=0;i<31;i++){
   if(checkdaqid(i)-1==crate)return i;
 }
 return -1;
}
integer AMSTrRawCluster::checkdaqid(int16u id){
  char sstr[128];
 for(int i=0;i<getmaxblocks();i++){
  sprintf(sstr,"JINFT%X",i);
  if(DAQEvent::ismynode(id,sstr))return i+1; 
 }
 for(int i=0;i<getmaxblocks();i++){
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




void AMSTrRawCluster::builddaq_new(integer i, integer n, int16u *p){
  int index=0;
  int crate2JinJnum[8]={3,9,0,1,16,17,22,23};

  AMSTrRawCluster *ptr=(AMSTrRawCluster*)AMSEvent::gethead()->
    getheadC("AMSTrRawCluster",i);


  map<int,vector<AMSTrRawCluster*> > mymap;
  for(int tdr=0;tdr<trid::ntdr;tdr++) mymap[tdr].push_back(0); 
  while (ptr){
    AMSTrIdSoft id(ptr->_address);
    int tdrnum=id.gettdr();
    mymap[tdrnum].push_back(ptr);
    ptr=ptr->next();
  }

  int pindex=0;
  for(int tdr=0;tdr<trid::ntdr;tdr++){
    int ncl=mymap[tdr].size();
    if(ncl>1){
     //  printf("Crate %d JINJ#: %02d TDR: %02d numclus: %3d\n",
// 	     i,crate2JinJnum[i],tdr,ncl);
      int tdr_length_index=pindex++;
      for(int icl=1;icl<ncl;icl++){
	ptr=mymap[tdr].at(icl);
	AMSTrIdSoft id(ptr->_address);
	p[pindex++] = ptr->_nelem-1;
	p[pindex++] = id.getside()?ptr->_strip:ptr->_strip+640;
	for(int ii=0;ii<ptr->_nelem;ii++)
	  p[pindex++]=ptr->getamp(ii);	  
	//	p[pindex++]=ptr->getamp(ii)*8.;
      }
      p[pindex++]=1<<7|1<<15|1<<5|tdr;
      p[tdr_length_index]=pindex-tdr_length_index-1;
    }
    if(pindex>=n){
      cerr<<"AMSTrRawCluster::builddaq-E-indext too big "<<index<< " "<<n<<endl;       break; 
    }
  }
  p[pindex++]=1<<5| 1<<15| crate2JinJnum[i];
  if(pindex!=n){
    cerr<<"AMSTrRawCluster::builddaq-E-indext wrong length "<<pindex<< " "<<n<<endl;
  }  
  
}



void AMSTrRawCluster::builddaq(integer i, integer n, int16u *p){
   int index=0;
   for(int itdr=0;itdr<trid::ntdr;itdr++){

  AMSTrRawCluster *ptr=(AMSTrRawCluster*)AMSEvent::gethead()->
  getheadC("AMSTrRawCluster",i);
     int indexp=0;
     int len=0;
     bool first=true;
     while(ptr){
       AMSTrIdSoft id(ptr->_address);
       if(id.gettdr()!=itdr){
         ptr=ptr->next();
         continue;
       }
       if(first){
         first=false;
         indexp=index;
         index++;
       }
       p[index++]=ptr->_nelem-1;
       len+=ptr->_nelem+2;
       p[index++]=id.getside()?ptr->_strip:ptr->_strip+640;
       for(int k=0;k<ptr->_nelem;k++)p[index++]=ptr->getamp(k);
       ptr=ptr->next();
      }
      if(!first){
       p[indexp]=len+1;  
       p[index++]=128 | (itdr); 
       if(index>=n){
        cerr<<"AMSTrRawCluster::builddaq-E-indext too big "<<index<< " "<<n<<endl;       break; 
       }
      }
     }
      p[index++]=getdaqid(i);
      if(index!=n){
        cerr<<"AMSTrRawCluster::builddaq-E-indext wrong length "<<index<< " "<<n<<endl;      
      }
}

integer AMSTrRawCluster::calcdaqlength(integer i){
  AMSTrRawCluster *ptr=(AMSTrRawCluster*)AMSEvent::gethead()->
  getheadC("AMSTrRawCluster",i);
  int tdra[trid::ntdr]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  integer l=0;
  while (ptr){
   AMSTrIdSoft id(ptr->_address);
     if(id.gettdr()<trid::ntdr){
        tdra[id.gettdr()]+=ptr->_nelem+2;
     }
   ptr=ptr->next();
  }
  for(int k=0;k<trid::ntdr;k++){
     if(tdra[k]){
       l+=tdra[k]+2;
     }
   }
   if(l)l++;
  return (l<32768?-l:0);
}



void AMSTrRawCluster::buildraw(integer n, int16u *pbeg){
  //  have to split integer n; add crate number on the upper part...
  unsigned int leng=n&65535;
  uinteger ic=(n>>16);
  if(ic>7){  
    cerr<<" AMSTrRawCluster::buildraw-E-crateOutOfRange "<<ic<<endl;
    return;
  }
  int cmn=0;
  if(TRCALIB.Version==0)cmn=16;
  if(DAQCFFKEY.DAQVersion==1)cmn=2;
  
  
  int add2=0;
  if(DAQCFFKEY.DAQVersion==1)add2=2;
  for (int16u* p=pbeg;p<pbeg+leng-1-add2;p+=*p+1){
    int16u tdr=(*(p+*p))&31;
    if(tdr>=trid::ntdr){
      static int nerr=0;
      if(nerr++<100)cerr<<" AMSTrRawCluster::buildraw-E-tdrOutOfRange "<<tdr<<endl;
      continue;
    }
    if(DAQEvent::isRawMode(*(p+*p))){
#ifdef __AMSDEBUG__
      cerr<<" AMSTrRawCluster::buildraw-E-RawModeNotSupportedYet "<<endl;
#endif
      return;
    }
    if(DAQEvent::isError(*(p+*p))){
      cerr<<" AMSTrRawCluster::buildraw-E-ErrorForTDR "<<tdr<<endl;
      break;
      return;
    }
    for(int16u* paux=p+DAQCFFKEY.Mode;paux<p+*p-1-cmn;paux+=*paux+2+1){
      geant s2n=*(paux)>>7;
      *(paux)=*(paux)&127;
      int16u haddr=*(paux+1)&1023;
      //
      // get rid of junk (????)
      //
      
      if(haddr+*paux==1023 && *(paux+*paux+2)>1000 && *(paux+*paux+2)<32769){
	static int junkerr=0;
	if(junkerr++<100)cerr<<"AMSTrRawCluster::buildraw-E-Junk1002detected "<< *(paux+*paux+2)<<endl;
	continue;
      }
      
      if(haddr+*(paux)>=1024){
	static int nerr=0;
	if(nerr++<100)cerr<<"  AMSTrRawCluster::buildraw-E-HaddrExtOutOfRange "<<haddr<<" "<< haddr+*(paux)<<endl;
	continue;
      }
      int va=haddr/64;
      geant cn=cmn>15?*(p+*p-cmn+va)/8.:s2n;
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
          int b=id1.getstrip();
          for(int k=b;k<b+cl+1;k++){
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
	//        cout <<"  id "<<id<<" "<<id.getstrip()<<" "<<id.getstrip()+*paux<<" "<<id.getgain()<<endl;
	int b=id.getstrip();
	for(int k=b;k<(b+*paux>=id.getmaxstrips()?id.getmaxstrips()-1:b+*paux);k++){
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
/*
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
*/
  bool update=DAQEvent::CalibDone(0);
  int nc=0;
  int ncp=0;
/*
  for(int i=0;i<getmaxblocks();i++){
   for (int j=0;j<trid::ntdr;j++){
    if(!AMSTrIdSoft::_Calib[i][j] && (pdaq && pdaq->CalibRequested(getdaqid(i),j))){
     update=false;
    }
    else if(AMSTrIdSoft::_Calib[i][j])nc++;
    if( (pdaq && pdaq->CalibRequested(getdaqid(i),j)))ncp++;
  }
  }
  cout <<" nc "<<nc<<" "<<ncp<<endl;
*/
  if(update || nc>=TRCALIB.EventsPerCheck){
     update=true;
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
   if(CALIB.InsertTimeProc)insert=AMSEvent::gethead()->getrun();
   ptdv->SetTime(insert,AMSEvent::gethead()->getrun()-1,AMSEvent::gethead()->getrun()+864000);
   cout <<" Tracker H/K  info has been read for "<<*ptdv;
   ptdv->gettime(insert,begin,end);
   cout <<" Time Insert "<<ctime(&insert);
   cout <<" Time Begin "<<ctime(&begin);
   cout <<" Time End "<<ctime(&end);
   }
  }
/*
    char hfile[161];
    UHTOC(IOPA.hfile,40,hfile,160);  
    char filename[256];
    strcpy(filename,hfile);
    sprintf(filename,"%s_tra.%d",hfile,AMSEvent::gethead()->getrun());
    integer iostat;
    integer rsize=1024;
    char event[80];  
    HROPEN(IOPA.hlun+1,"trcalibration",filename,"NP",rsize,iostat);
    if(iostat){
     cerr << "Error opening trcalib ntuple file "<<filename<<endl;
     exit(1);
    }
    else cout <<"trcalib ntuple file "<<filename<<" opened."<<endl;
*/
   if(IOPA.hlun){
   static TrCalib_def TRCALIB;
   CALIB.Ntuple++;
   HBNT(CALIB.Ntuple,"Tracker Calibaration"," ");
   HBNAME(CALIB.Ntuple,"TrCalib",(int*)(&TRCALIB),"PSLayer:I,PSLadder:I,PSHalf:I,PSSide:I, PSStrip:I,Ped:R,Sigma:R,RawSigma:R,BadCh:I,CmnNoise:R,Crate:I,Haddr:I");
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
          if(AMSTrIdSoft::_Calib[id.getcrate()][id.gettdr()]){
          TRCALIB.Ped=id.getped();
          TRCALIB.Sigma=id.getsig();
          TRCALIB.RawSigma=id.getsigraw();
          TRCALIB.BadCh=id.checkstatus(AMSDBc::BAD);
          TRCALIB.CmnNoise=id.getcmnnoise();
          }
          else{
          TRCALIB.Ped=0;
          TRCALIB.Sigma=0;
          TRCALIB.RawSigma=0;
          TRCALIB.BadCh=0;
          TRCALIB.CmnNoise=id.getcmnnoise();
          }
          TRCALIB.Crate=id.getcrate();
          TRCALIB.Haddr=id.gethaddr();
          HFNT(CALIB.Ntuple);
         }
        }
       }
     }
    }
}
   for(int i=0;i<getmaxblocks();i++){
    for (int j=0;j<trid::ntdr;j++){
     AMSTrIdSoft::_Calib[i][j]=0;
    }
   }

/*
  char hpawc[256]="//PAWC";
  HCDIR (hpawc, " ");
  char houtput[]="//trcalibration";
  HCDIR (houtput, " ");
  integer ICYCL=0;
  HROUT (CALIB.Ntuple, ICYCL, " ");
  HREND ("trcalibration");
  CLOSEF(IOPA.hlun+1);
*/
  }
 }







void AMSTrRawCluster::updtrcalib2009S(integer n, int16u* p){
  uinteger leng=(n&65535);
  uinteger in=(n>>16);
  uinteger ic=in/trid::ntdr;
  uinteger tdr=in%trid::ntdr;
  if(DAQEvent::CalibInit(0)){
   cout<<"AMSTrRawCluster::ipdtrcalib2009S-I-InitCalib "<<endl;
   for(int i=0;i<getmaxblocks();i++){
    for (int j=0;j<trid::ntdr;j++){
     AMSTrIdSoft::_Calib[i][j]=0;
    }
   }

  }

  cout <<"  Crate TDR "<<ic<<"  "<<tdr<<endl;
  if(AMSTrIdSoft::_Calib[ic][tdr]==1){
   cerr<<" AMSTrRawCluster::updtrcalibS-W-ICTDRAlreadySet "<<ic<<" "<<tdr<<endl;
  }
AMSTrIdSoft::_Calib[ic][tdr]=-1;
const int lt[8]={1024,1024,1024,1024,1024,1024,16,16};
int16u *plt[8]={0,0,0,0,0,0,0,0};
int16u *plast=p+1;
int table[8]={0,0,0,0,0,0,0,0};  //ped,flags,slow,sraw,shigh,double,cmns,cmn
for(int k=0;k<8;k++){
if( (*p>>k)&1){
 plt[k]=plast;
 plast+=lt[k];
}
}
if(plast-p>=leng-9){
 cerr<<"AMSTrRawCluater::updtrrcalib2009S-E-LengthProblem "<<plast-p<<" "<<leng<<endl;
 return;
}
if(!plt[0] || !plt[1] || !plt[2] || !plt[3]){
 cerr<<"AMSTrRawCluster::updtrrcalib2009S-E-NotAllCalibPresent "<<plt[0]<<" "<<plt[1]<<" "<<" "<<plt[2]<<" "<<plt[3]<<endl;
 return;
}
  int nerr=0;
  cout <<"Version "<<*plast<<endl;
  for(int k=1;k<8;k++)cout<<" Parameters "<<k<<" "<<*(plast+k)<<endl;
  int version=*(plast);
  geant rawsig_d=*(plast+7);
 for(int i=0;i<lt[0];i++){
  int16u haddr=(tdr<<10) | i;
  AMSTrIdSoft id(ic,haddr);
  if(!id.dead()){
   if(id.getgain()==0){
    id.setgain()=TRMCFFKEY.gain[id.getside()];
  }
    id.setped()=geant(*(plt[0]+i))/id.getgain();
    if(*(plt[1]+i)!=0)id.setstatus(AMSDBc::BAD);
    else id.clearstatus(AMSDBc::BAD);
    geant sig_d;
    if(id.getside()==0)sig_d=*(plast+6);
    else if(id.getstrip()<320)sig_d=*(plast+2);
    else sig_d=*(plast+4);
    id.setsig()=geant(*(plt[2]+i))/sig_d;
    id.setsigraw()=geant(*(plt[3]+i))/id.getgain()/rawsig_d;
   if(id.setsig()>id.setsigraw()){
    nerr++;
//     cerr <<"  AMSTrRawCluster::updtrcalibS-E-SigmaProblems "<<id.getsigraw()<<" "<<id.getsig() <<" "<<id<<endl;
//     cerr <<"  AMSTrRawCluster::updtrcalibS-I-Version "<<version<<" "<<sig_d<<" "<<rawsig_d<<endl;
   }
   if(i%64==0){
    if(plt[7]){
      id.setcmnnoise()=*(plt[7]+i/64)/id.getgain();
    }
   }
}
}
  AMSTrIdSoft::_Calib[ic][tdr]=1;


if(nerr>0){
     cerr <<"  AMSTrRawCluster::updtrcalibS-E-RawSigmaProblems "<<nerr<<endl;
}
  bool update=DAQEvent::CalibDone(0);
  int nc=0;
  int ncp=0;

  if(update ){
   for(int i=0;i<getmaxblocks();i++){
    for (int j=0;j<trid::ntdr;j++){
     if(AMSTrIdSoft::_Calib[i][j]==-1){
      update=false;
      cerr<<"AMSTrRawCluster::updtrcal2009S-E-TRDFailed "<<i<<" "<<j<<endl;
    
    }
   }
   }
   if(update){
   int maxi=2;
   if(!strcmp(AMSJob::gethead()->getsetup(),"AMS02P"))maxi=1;
   for (int i=0;i<maxi;i++){
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
   if(CALIB.InsertTimeProc)insert=AMSEvent::gethead()->getrun();
   ptdv->SetTime(insert,AMSEvent::gethead()->getrun()-1,AMSEvent::gethead()->getrun()+864000);
   cout <<" Tracker H/K  info has been read for "<<*ptdv;
   ptdv->gettime(insert,begin,end);
   cout <<" Time Insert "<<ctime(&insert);
   cout <<" Time Begin "<<ctime(&begin);
   cout <<" Time End "<<ctime(&end);
   }
  }
 }
/*
    char hfile[161];
    UHTOC(IOPA.hfile,40,hfile,160);  
    char filename[256];
    strcpy(filename,hfile);
    sprintf(filename,"%s_tra.%d",hfile,AMSEvent::gethead()->getrun());
    integer iostat;
    integer rsize=1024;
    char event[80];  
    HROPEN(IOPA.hlun+1,"trcalibration",filename,"NP",rsize,iostat);
    if(iostat){
     cerr << "Error opening trcalib ntuple file "<<filename<<endl;
     exit(1);
    }
    else cout <<"trcalib ntuple file "<<filename<<" opened."<<endl;
*/
   if(IOPA.hlun){
   static TrCalib_def TRCALIB;
   CALIB.Ntuple++;
   HBNT(CALIB.Ntuple,"Tracker Calibaration"," ");
   HBNAME(CALIB.Ntuple,"TrCalib",(int*)(&TRCALIB),"PSLayer:I,PSLadder:I,PSHalf:I,PSSide:I, PSStrip:I,Ped:R,Sigma:R,RawSigma:R,BadCh:I,CmnNoise:R,Crate:I,Haddr:I");
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
          if(AMSTrIdSoft::_Calib[id.getcrate()][id.gettdr()]){
          TRCALIB.Ped=id.getped();
          TRCALIB.Sigma=id.getsig();
          TRCALIB.RawSigma=id.getsigraw();
          TRCALIB.BadCh=id.checkstatus(AMSDBc::BAD);
          TRCALIB.CmnNoise=id.getcmnnoise();
          }
          else{
          TRCALIB.Ped=0;
          TRCALIB.Sigma=0;
          TRCALIB.RawSigma=0;
          TRCALIB.BadCh=0;
          TRCALIB.CmnNoise=id.getcmnnoise();
          }
          TRCALIB.Crate=id.getcrate();
          TRCALIB.Haddr=id.gethaddr();
          HFNT(CALIB.Ntuple);
         }
        }
       }
     }
    }
}
   for(int i=0;i<getmaxblocks();i++){
    for (int j=0;j<trid::ntdr;j++){
     AMSTrIdSoft::_Calib[i][j]=0;
    }
   }
  }
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
