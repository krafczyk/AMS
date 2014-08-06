//  $Id$
#include "trdsim.h"
#include "event.h"
#include "extC.h"
#include "tofsim02.h"
#include "ntuple.h"
#include "mceventg.h"
#include "timeid.h"
#include "trdcalib.h"
using namespace trdsim;
void AMSTRDRawHit::lvl3CompatibilityAddress(int16u ptr,uinteger& udr, uinteger & ufe,uinteger& ute,uinteger& chan){
 chan = ptr & 15;
 ute = (ptr>>4) & 3;
 ufe = (ptr>>6) & 7;
 udr = (ptr>>9) & 7;
}

number AMSTRDRawHit::delay_sf_table[numvawf];
number AMSTRDRawHit::va_waveform[numvawf]={0.,-420.,-740.,-950.,-1080.,-1150.,-1180.,-1200.,-1180.,-1160., -1132., -1085., -1040., -1000., -960., -920., -878., -830., -790., -750., -700.,-650., -615., -583., -552., -510., -480., -440., -420., -400., -380., -360.};


void AMSTRDRawHit::init_delay_table(){

  // Initialize delay table based on VA waveform

  number sumall=0;
  for (int i=0;i<numvawf;i++){
      sumall+=va_waveform[i];
  }
  for (int i=0;i<numvawf;i++){
      number sum=0;
      for (int j=i;j<numvawf;j++){
	  sum+=va_waveform[j];
	}
      if (fabs(sumall)>0.){
	delay_sf_table[i]=sum/sumall;
      }
    }
}

number AMSTRDRawHit::delay_sf(number usec){
  // Returns scale factor for delay in microsecs
  // Very simple table

  if (usec>0. && usec <10.){
    int index=(int)(31.*usec/10.);
    return delay_sf_table[index];
  }
  else{
    return 1.;
  }
}

bool AMSTRDRawHit::_HardWareCompatibilityMode=false;
void AMSTRDRawHit::sitrddigi(){

    AMSTRDMCCluster * ptr=
    (AMSTRDMCCluster*)AMSEvent::gethead()->getheadC("AMSTRDMCCluster",0,1);    

    geant itrack=0;
    geant edep=0;
    while(ptr){

      int i=0;
      number delay=0.;
      itrack=ptr->getitra();
      // Get delay info for the track corresponding to this cluster
      AMSmceventg *pmcg=(AMSmceventg*)AMSEvent::gethead()->getheadC("AMSmceventg",0);
      while(pmcg && i<itrack) {
	// Get mcg for this track (at the beginning of the list)
	if (pmcg){
	  delay = pmcg->getdelay();
	}
        pmcg=pmcg->next();
	i++;
      }
    
     edep+=ptr->getedep()*TRDMCFFKEY.GeV2ADC;
     //Put in scale factor for delayed event
     if (delay>0) edep *= AMSTRDRawHit::delay_sf(delay);

     if(ptr->testlast()){
           AMSTRDIdSoft idsoft(ptr->getid());
           number amp=(edep*idsoft.getmcgain()+idsoft.getped()+idsoft.getsig()*rnormx());
           if (amp>idsoft.overflow())amp=idsoft.overflow();
          
           float sigmaDR = std::max(TRDMCFFKEY.MinSigma, idsoft.getsig()); // at least MinSigma
           sigmaDR       = std::min(TRDMCFFKEY.MaxSigma, sigmaDR);         // at most MaxSigma
           if(amp-idsoft.getped()>fabs(TRDMCFFKEY.Thr1R*sigmaDR)){
        AMSEvent::gethead()->addnext(AMSID("AMSTRDRawHit",idsoft.getcrate()),
        new AMSTRDRawHit(idsoft,(amp-idsoft.getped())*TRDMCFFKEY.f2i));
//         cout <<"  raw cluster-0 "<<idsoft.getsig()<<" "<<idsoft.getcrate()<<" "<<idsoft.getlayer() <<" "<<idsoft.getladder()<<" "<<" "<<" "<<idsoft.gettube()<<" "<<(amp-idsoft.getped())*TRDMCFFKEY.f2i<<endl;
        
        edep=0;       
        }
     }

     ptr=ptr->next();
    }
	AMSEvent::gethead()->getC("AMSTRDRawHit",0)->getnelem();

  if(TRDMCFFKEY.NoiseOn && TOF2RawSide::GlobFasTrigOK())AMSTRDRawHit::sitrdnoise();


}


void AMSTRDRawHit::sitrdnoise(){
//   brute force now
//   if necessary should be changed a-la sitknoise

for ( int i=0;i<TRDDBc::TRDOctagonNo();i++){
      for (int j=0;j<TRDDBc::LayersNo(i);j++){
	  for(int k=0;k<TRDDBc::LaddersNo(i,j);k++){
            for (int l=0;l<TRDDBc::TubesNo(i,j,k);l++){
             float amp=rnormx();
             if(amp>TRDMCFFKEY.Thr1R){
              AMSTRDIdGeom id(j,k,l,i);
              AMSTRDIdSoft idsoft(id);
              AMSTRDRawHit *p =new AMSTRDRawHit(idsoft,amp*idsoft.getsig()*TRDMCFFKEY.f2i);
              AMSTRDRawHit *ph= (AMSTRDRawHit *)AMSEvent::gethead()->getheadC(AMSID("AMSTRDRawHit",idsoft.getcrate()));
              if(!ph){
                  AMSEvent::gethead()->addnext(AMSID("AMSTRDRawHit",idsoft.getcrate()),p);
              }
              else{
              while(ph){
                if(ph<p){
                  AMSEvent::gethead()->addnext(AMSID("AMSTRDRawHit",idsoft.getcrate()),p);
         //cout <<"  raw cluster0 "<<idsoft.getcrate()<<" "<<idsoft.getlayer() <<" "<<idsoft.getladder()<<" "<<" "<<" "<<idsoft.gettube()<<" "<<amp*idsoft.getsig()*TRDMCFFKEY.f2i<<endl;
                  p=0;
                  break;
                }
                else if(!(p<ph)){
                  delete p;
                  p=0;
                  break;
                }
                ph=ph->next();
              }
              delete p;
              }
             }
            }
           }
          }
}

}

integer AMSTRDRawHit::getdaqid(int16u crate){
 for(int i=0;i<31;i++){
   if(checkdaqid(i)-1 == crate)return i;
 }
 return -1;
}

integer AMSTRDRawHit::checkdaqidS(int16u id){
 
 for(int i=0;i<getmaxblocks();i++){
 for(int j=0;j<trdid::nudr;j++){
  char sstr[128];
  sprintf(sstr,"UDR%X%X",i,j);
  if(DAQEvent::ismynode(id,sstr)){
       return j+i*trdid::nudr+1; 
  }
 }
 }
 return 0;
}



integer AMSTRDRawHit::checkdaqid(int16u id){

  char sstr[128];
 for(int i=0;i<getmaxblocks();i++){
  sprintf(sstr,"JINFU%X",i);
  if(DAQEvent::ismynode(id,sstr))return i+1; 
 }
 return 0;


}

integer AMSTRDRawHit::checkdaqidJ(int16u id){

  char sstr[128];
 for(int i=0;i<getmaxblocks();i++){
  sprintf(sstr,"JF-U%d",i);
  if(DAQEvent::ismynode(id,sstr))return i+1; 
 }
 return 0;


}


void AMSTRDRawHit::updtrdcalibSCI(){
  AMSTimeID *ptdv=0;
  time_t begin,end,insert;

  ptdv = AMSJob::gethead()->gettimestructure(AMSID("TRDGains",AMSJob::gethead()->isRealData()));
  ptdv->UpdateMe()=1;
  ptdv->UpdCRC();
  time(&insert);
  if(CALIB.InsertTimeProc)insert=AMSEvent::getSRun();

  // valid for 10 days
  ptdv->SetTime(insert,AMSEvent::getSRun()-1,AMSEvent::getSRun()-1+864000);
  cout <<" TRD SCI  info has been updated for "<<*ptdv;
  ptdv->gettime(insert,begin,end);
  cout <<" Time Insert "<<ctime(&insert);
  cout <<" Time Begin "<<ctime(&begin);
  cout <<" Time End "<<ctime(&end);
}

void AMSTRDRawHit::updtrdcalibJ(int n, int16u* pbeg){
 unsigned int length=n&65535;
 unsigned int ic=(n>>16);
 for (int16u* p=pbeg;p<pbeg+length-1;p+=*p+1){
  int udr=((*(p+*p))&31)/4;
  uinteger leng= ((*p)+4) | ( (udr+ic*trdid::nudr)<<16);
  updtrdcalib(leng,p+1);
 }
}




void AMSTRDRawHit::updtrdcalib(int n, int16u* p){
  integer leng=(n&65535);
  leng-=9;
  uinteger in=(n>>16);
  uinteger ic=in/trdid::nudr;
  uinteger udr=in%trdid::nudr;
  int leng2=leng/2;
  const int span=448;
  if(leng2!=span && leng2!=span*2){
   cerr<<"AMSTRDRaw::updtrcalib-WrongLength "<<leng2<<" "<<endl;
   return;
  }
  int status=*(p+leng+4);
  if(DAQEvent::isError(status)){
   cerr<<"AMSTRDRaw::updtrcalib-ErrorBitSet "<<endl;
   return;
  }
  AMSTRDIdSoft::_Calib[ic][udr]=1;
  for (int i=0;i<span;i++){
        int ufe=i%7;
        int cha=i/7;
        int roch=cha%16;
        int ute=cha/16;
        AMSTRDIdSoft id(ic,udr,ufe,ute,roch);
       if(!id.dead()){
	 //         if(id.getgain()==0)id.setgain()=1;
         id.setped()=*(p+i)/TRDMCFFKEY.f2i;
         id.setsig()=*(p+span+i)/TRDMCFFKEY.f2i;
         if(id.getsig()>TRDCALIB.BadChanThr){
             id.setstatus(AMSDBc::BAD);
          }
         else id.clearstatus(AMSDBc::BAD);
       }
  }
   DAQEvent * pdaq = (DAQEvent*)AMSEvent::gethead()->getheadC("DAQEvent",6);
  bool update=true;
  int nc=0;
  int ncp=0;
  for(int i=0;i<getmaxblocks();i++){
   for (int j=0;j<trdid::nudr;j++){
    bool creq=false;
    for(int k=0;k<4;k++){
     creq=creq || (pdaq && pdaq->CalibRequested(getdaqid(i),j*4+k));
    }
    if(!AMSTRDIdSoft::_Calib[i][j] && creq){
     update=false;
    }
    else if(AMSTRDIdSoft::_Calib[i][j])nc++;
    if(creq)ncp++;
  }
 }
 cout <<"  nc TRD "<<nc<<" "<<ncp<<endl;
  if(update){
    AMSTRDIdCalib::ntuple(AMSEvent::gethead()->getrun());
   for(int i=0;i<getmaxblocks();i++){
    for (int j=0;j<trdid::nudr;j++){
     AMSTRDIdSoft::_Calib[i][j]=0;
    }
   }
   AMSTimeID *ptdv;
     time_t begin,end,insert;
      const int ntrd=2;
//      const char* TDV2Update[ntrd]={"TRDPedestals","TRDSigmas","TRDStatus"};
      const char* TDV2Update[ntrd]={"TRDPedestals","TRDSigmas"};
      for (int i=0;i<ntrd;i++){
      ptdv = AMSJob::gethead()->gettimestructure(AMSID(TDV2Update[i],AMSJob::gethead()->isRealData()));
      ptdv->UpdateMe()=1;
      ptdv->UpdCRC();
      time(&insert);
      if(CALIB.InsertTimeProc)insert=AMSEvent::gethead()->getrun();
      ptdv->SetTime(insert,AMSEvent::gethead()->getrun()-1,AMSEvent::gethead()->getrun()-1+864000);
      cout <<" TRD H/K  info has been updated for "<<*ptdv;
      ptdv->gettime(insert,begin,end);
      cout <<" Time Insert "<<ctime(&insert);
      cout <<" Time Begin "<<ctime(&begin);
      cout <<" Time End "<<ctime(&end);

 }

}
}
void AMSTRDRawHit::updtrdcalib2009(int n, int16u* p){
  integer leng=(n&65535);
  p++;
  leng-=10;
  uinteger in=(n>>16);
  uinteger ic=in/trdid::nudr;
  uinteger udr=in%trdid::nudr;
  int leng2=leng/3;
  const int span=448;
  AMSTRDIdSoft::_Calib[ic][udr]=-1;
  if(DAQEvent::CalibInit(1)){
   cout<<"AMSTrRRDRawHit::updtrdcalib2009S-I-InitCalib "<<endl;
   for(int i=0;i<getmaxblocks();i++){
    for (int j=0;j<trdid::nudr;j++){
     AMSTRDIdSoft::_Calib[i][j]=0;
    }
   }
  }

  if(leng2!=span && leng2!=span*3/2){
   cerr<<"AMSTRDRaw::updtrcalib-WrongLength "<<leng2<<" "<<endl;
   return;
  }
  int status=*(p+leng+4);
  if(DAQEvent::isError(status)){
   cerr<<"AMSTRDRaw::updtrcalib-ErrorBitSet "<<endl;
   return;
  }
  int nbad=0;
  for (int i=0;i<span;i++){
        int ufe=i%7;
        int cha=i/7;
        int roch=cha%16;
        int ute=cha/16;
        AMSTRDIdSoft id(ic,udr,ufe,ute,roch);
       if(!id.dead()){
	 //         if(id.getgain()==0)id.setgain()=1;
         id.setped()=*(p+i)/TRDMCFFKEY.f2i;
         id.setsig()=*(p+span+i)/TRDMCFFKEY.f2i/4;
         if(id.getsig()>TRDCALIB.BadChanThr){
             id.setstatus(AMSDBc::BAD);
             nbad++;
          }
         else id.clearstatus(AMSDBc::BAD);
       }
  }
  if(nbad>100){
   cerr<<"AMSTRDRawHit::updtrdcalib2009-E-TooManyBadCh "<<nbad<<endl;
  }
  else   AMSTRDIdSoft::_Calib[ic][udr]=1;
  bool update=DAQEvent::CalibDone(1);
  if(update){
    AMSTRDIdCalib::ntuple(AMSEvent::gethead()->getrun());
   for(int i=0;i<getmaxblocks();i++){
    for (int j=0;j<trdid::nudr;j++){
     if(AMSTRDIdSoft::_Calib[i][j]==-1){
       update=false;
     }
     AMSTRDIdSoft::_Calib[i][j]=0;
    }
   }
   if(update){
   AMSTimeID *ptdv;
     time_t begin,end,insert;
      const int ntrd=2;
//      AMSTRDIdSoft::ResetGains();
//      const char* TDV2Update[ntrd]={"TRDPedestals","TRDSigmas","TRDStatus"};
      const char* TDV2Update[ntrd]={"TRDPedestals","TRDSigmas"};
      for (int i=0;i<ntrd;i++){
      ptdv = AMSJob::gethead()->gettimestructure(AMSID(TDV2Update[i],AMSJob::gethead()->isRealData()));
      ptdv->UpdateMe()=1;
      ptdv->UpdCRC();
      time(&insert);
      if(CALIB.InsertTimeProc)insert=AMSEvent::gethead()->getrun();
      ptdv->SetTime(insert,AMSEvent::gethead()->getrun()-1,AMSEvent::gethead()->getrun()-1+864000);
      cout <<" TRD H/K  info has been updated for "<<*ptdv;
      ptdv->gettime(insert,begin,end);
      cout <<" Time Insert "<<ctime(&insert);
      cout <<" Time Begin "<<ctime(&begin);
      cout <<" Time End "<<ctime(&end);
    }
 }

}
}


void AMSTRDRawHit::buildrawJ(int n, int16u* pbeg){
 buildraw(n-1,pbeg+1);
}
void AMSTRDRawHit::buildraw(int n, int16u* pbeg){
unsigned int length=n&65535;
unsigned int ic=(n>>16);
int add2=0;
if(DAQCFFKEY.DAQVersion==1)add2=2;
for (int16u* p=pbeg;p<pbeg+length-1-add2;p+=*p+1){
 bool raw=DAQEvent::isRawMode(*(p+*p));
 bool compressed=DAQEvent::isCompMode(*(p+*p));
 int udr=((*(p+*p))&31)/4;
 if(udr>=trdid::nudr){
   cerr<<"AMSTRDRawHit::buildraw-E-udrOutOfRange "<<udr<<endl;
   continue;
 }
 int len=*p;
 int rawl=0;
 if(raw ){
  rawl=448;
  if(len!=rawl+1 && !compressed){
#ifdef __AMSDEBUG__
   cerr<<"AMSTRDRawHit::buildraw-E-WrongSubBlockLengthInRoawMode "<<*p<<endl;
   cerr <<"  UDR No "<<udr<<endl;
#endif
   return;
  }
  else {
  for (int i=0;i<rawl;i++){
        int ufe=i%7;
        int cha=i/7;
        int roch=cha%16;
        int ute=cha/16;
        AMSTRDIdSoft id(ic,udr,ufe,ute,roch);
        if(!id.dead()){
         AMSTRDRawHit *ph=new AMSTRDRawHit(id,(((*(p+i+1))&4095)-id.getped())*TRDMCFFKEY.f2i);
         int icn=ic;
         if(compressed){
          ph->setstatus(AMSDBc::RECOVERED);
          if(!(AMSJob::gethead()->isCalibration() & AMSJob::CTRD))icn+=2;      
         }
         AMSEvent::gethead()->addnext(AMSID("AMSTRDRawHit",icn), ph);
         //cout <<id<<" "<<((*(p+i))&32767)-id.getped()<<" "<<id.getsig()<<endl;
       }
       else{
         //cerr<<"AMSTRDRawHit::buildraw-E-IDDead"<<id<<endl;
       }
  }
 }
 }
 if(compressed && !(AMSJob::gethead()->isCalibration() & AMSJob::CTRD)){
  // compressed mode detected
//wild guess about data format
DAQCFFKEY.Mode=len%2==0?2:1;
  for (int j=DAQCFFKEY.Mode+rawl;j<len;j+=2){
        uint16 adr=(*(p+j))%512;
        int ufe=adr/64;
        int cha=adr%64;
        int roch=cha%16;
        int ute=cha/16;
        if(ufe>=trdid::nufe){
         static int nmsg=0;
         if(nmsg++<100)cerr<<"AMSTRDRawHit::buildraw-E-ufeOutOfRange "<<ufe<<endl;
         continue;
        }
        AMSTRDIdSoft id(ic,udr,ufe,ute,roch);
        if(!id.dead()){
         int16 amp=*(p+j+1);
#ifdef __AMSDEBUG__
            if(amp<0)cerr<<"AMSTRDRawHit::buildraw-W-AmpProblem "<<amp<<endl;
#endif
         //cout <<"  raw cluster2 "<<ic<<" "<<id.getlayer() <<" "<<id.getladder()<<" "<<udr<<" "<<" "<<id.gettube()<<" "<<amp<<endl;
         AMSEvent::gethead()->addnext(AMSID("AMSTRDRawHit",ic), new
         AMSTRDRawHit(id,amp));
       }
       else{
//         cerr<<"AMSTRDRawHit::buildraw-E-IDDead"<<id<<endl;
       }
  }
   
 }
 }


}

void AMSTRDRawHit::builddaq(int i, int length,int16u*p){
   int index=0;
   for(int iudr=0;iudr<trdid::nudr;iudr++){
     int indexp=0;
     int len=0;
     bool first=true;
     AMSTRDRawHit * ptr=
     (AMSTRDRawHit*)AMSEvent::gethead()->getheadC("AMSTRDRawHit",i,2);  
     int16u amp=0;
     while(ptr){
       AMSTRDIdSoft id(ptr->getidsoft());
       integer udr=id.getudr();
        if(udr!=iudr){
          ptr=ptr->next();
          continue;
        }        
 //       if(!id.checkstatus(AMSDBc::BAD)){
         amp+=ptr->getamp();
         //cout <<"  raw cluster "<<i<<" "<<ilay <<" "<<ilad<<" "<<udr<<" "<<" "<<id.gettube()<<" "<<ptr->getamp()<<endl;
//        }
       if(1 || ptr->testlast()){
        if(amp>0 ){
         if(first){
           first=false;
           indexp=index;
           index++;
         }
          p[index++]=(id.gethaddr())%512;
          p[index++]=amp;         
         //cout <<"  written "<<i<<" "<<ilay <<" "<<ilad<<" "<<udr<<" "<<" "<<id.gettube()<<" "<<amp<<endl;
         len+=2;
        }
        amp=0;
       }
       ptr=ptr->next();
     }
     if(len){
      p[indexp]=len+1;
      p[index++]=(iudr*4) | 128 ;
     }
     if(index>=length){
      cerr<<"AMSTRDRawHit-E-builddaq-IndexOutOfrange "<<length<<" "<<index<<endl;
      break; 
    }
    }
    p[index++]=(getdaqid(i)) | (1<<5) | (1<<15); 
    if(index!=length){
      cerr<<"AMSTRDRawHit-E-builddaq-WrongLength "<<length<<" "<<index<<endl;
    }
   }


integer AMSTRDRawHit::calcdaqlength(integer i){
     integer length=0;
     integer udra[trdid::nudr]={0,0,0,0,0,0};
     AMSTRDRawHit * ptr=
     (AMSTRDRawHit*)AMSEvent::gethead()->getheadC("AMSTRDRawHit",i,2);  
     int16u amp=0;
     while(ptr){
       AMSTRDIdSoft id(ptr->getidsoft());
       integer udr=id.getudr();
//        if(!id.checkstatus(AMSDBc::BAD)){
         amp+=ptr->getamp();
//        }
       if(1 || ptr->testlast()){
        if(amp>0 && udr<trdid::nudr){
         length+=2;
         udra[udr]=1;
        }
        amp=0;
       }
       ptr=ptr->next();
     }
     if(length>0){
       for(int i=0;i<trdid::nudr;i++){
        if(udra[i])length+=2;
     }
     length++;
    }
return -length;
}

void AMSTRDRawHit::_writeEl(){
  integer flag =    (IOPA.WriteAll%10==1)
                 || ( checkstatus(AMSDBc::USED));
  if(AMSTRDRawHit::Out( flag  )){
#ifdef __WRITEROOT__
    AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif
/*
    TRDRawHitNtuple* TrN = AMSJob::gethead()->getntuple()->Get_trdht();
    if (TrN->Ntrdht>=MAXTRDRHT) return;
    TrN->Layer[TrN->Ntrdht]=_id.getlayer();
    TrN->Ladder[TrN->Ntrdht]=_id.getladder();
    TrN->Tube[TrN->Ntrdht]=_id.gettube();
    TrN->Amp[TrN->Ntrdht]=Amp();
    TrN->Ntrdht++;
*/
  }

}

void AMSTRDRawHit::_copyEl(){
}

void AMSTRDRawHit::_printEl(ostream & o){
o<<_id<<" "<<_Amp<<endl;
}



integer AMSTRDRawHit::Out(integer status){
static integer init=0;
static integer WriteAll=0;
if(init == 0){
 init=1;
 integer ntrig=AMSJob::gethead()->gettriggerN();
  for(int n=0;n<ntrig;n++){
    if(strcmp("AMSTRDRawHit",AMSJob::gethead()->gettriggerC(n))==0){
     WriteAll=1;
     break;
    }
  }
}
return (WriteAll || status);
}






integer AMSTRDRawHit::lvl3format(int16 * adc, integer nmax){
  //
  // convert my stupid format to lvl3 one for ams02 flight 
  // (pure fantasy, not yet defined
  //  word 1:   tube no in address format
  //  word 2:   tube amp  (-ped)


  if (nmax < 2)return 0;
  adc[0]=_id.gethaddr();
  adc[1]=_Amp;
  return 2;
}



