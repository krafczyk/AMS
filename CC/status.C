//  $Id$
// Author V.Choutko.
#include "status.h"
#include "snode.h"
#include "amsgobj.h"
#include "job.h"
#include "amsstl.h"
#include "event.h"
#include "timeid.h"
#include "daqevt.h"
integer AMSStatus::_Mode=0;
void AMSStatus::_init(){
     _Nelem=0;
}
   
ostream & AMSStatus::print(ostream & stream)const{
  return(AMSID::print(stream)  << endl<<" Errors : "<<_Errors<<endl << "Accepted : "<<_Accepted<<endl<< " Rejected : "<<_Rejected<<endl);
}

AMSStatus* AMSStatus::create(int version){
 if( version==0){ 
   static AMSStatus tstatus("EventStatusTable",version);
   return &tstatus;
 }
else{
cout <<"  statussize "<<sizeof(AMSStatus)<<endl;
if(sizeof(AMSStatus)<8000000){
   static AMSStatus tstatus("EventStatusTable02",version);

   return &tstatus;
}
else if(sizeof(AMSStatus)<10000000){
   static AMSStatus tstatus("EventStatusTable03",version);
   cout<<"AMSStatus::create-I-EventStatusTable03 Created "<<endl;
   return &tstatus;
}
else{
   static AMSStatus tstatus("EventStatusTable04",version);
   cout<<"AMSStatus::create-I-EventStatusTable04 Created "<<endl;
   return &tstatus;
}
}
}

integer AMSStatus::isFull(uinteger run, uinteger evt, time_t time,DAQEvent*pdaq,bool force){
  const int howmany=20;
  static time_t oldtime=0;
  integer timechanged= time!=oldtime?1:0;
  if(AMSEvent::get_num_threads()==1 && run==_Run && _Nelem>0 && evt<_Status[_Nelem-1].event){
    cerr <<"AMSStatus::isFull-E-EventSequenceBroken "<<_Nelem<<" "<<run<<" "<<evt<<" "<<_Status[_Nelem-1].event<<endl;
#pragma omp critical (st1)
   _Errors++;
   if(_Errors>howmany){
       cerr<<"AMSStatus::isFull-F-EventSequenceBroken-TooManyTimesExiting"<< " "<<_Errors<<endl; 
       exit(1);
   }
   return 2;
  }
  if(_Nelem>=MAXDAQRATE+STATUSSIZE){
#pragma omp critical (printlost)
        cerr <<"AMSSTatus::isFull-E-MaxDAQRateExceeds "<<MAXDAQRATE<<
        " some of the events will be lost "<<AMSEvent::get_thread_num()<<" "<<evt<<" "<<pdaq<<endl;
#pragma omp critical (st1)
        _Errors++;

        return 2;
} 
  bool ret= ((_Nelem>=STATUSSIZE || force) && timechanged ) || (run!=_Run && _Nelem>0) || (_Nelem>0 && pdaq && pdaq->getoffset()-_Offset>4294967294-MAXDAQRATE*10000 && timechanged);
    if(ret){
       cout <<  "  StatusTableFull "<<_Offset<<" "<<_Nelem<<" "<<evt<<endl;
       return 1;
    }
    else{
#pragma omp critical (st1)
      oldtime=time;
      return 0;
   }
}


void AMSStatus::Sort(){
//  sort statuses before db writing in case of multiple threads
if(_Nelem<=1)return;
//for(int k=0;k<_Nelem;k++){
// cout <<"  k "<<k<<" "<<_Status[k].event<<endl;
//}
AMSsortNAGa(_Status,_Nelem);
//for(int k=0;k<_Nelem;k++){
// cout <<"sorted  k "<<k<<" "<<_Status[k].event<<endl;
//}
}
void AMSStatus::adds(uinteger run, uinteger evt, uinteger* status, time_t time){
  if(_Nelem==0  || (isFull(run,evt,time,NULL)>1 && !AMSFFKEY.Update )){


AMSEvent::ResetThreadWait(1);
AMSEvent::Barrier()=true;
cout <<"  in barrier AMSStatus::adds "<< AMSEvent::get_thread_num()<<endl;
#pragma omp barrier
_Offset=9223372036854775807LL;
#pragma omp critical (st1)
{
    uint64 offset=((DAQEvent*)AMSEvent::gethead()->getheadC("DAQEvent",0))->getoffset();
     
    if(offset<_Offset)_Offset=offset;
    if(AMSEvent::get_thread_num()==0){
     AMSEvent::Barrier()=false;
    _Nelem=0;
    _Run=run;
    _Begin=time;
    _End=time;
    if(_Begin==0){
      cerr <<"AMSStatus::adds-E-BeginTimeIsZeroForRun"<<" "<<_Run<<endl;
      _Errors++;
    }
   }
 }
cout <<"  out barrier AMSStatus::adds "<< AMSEvent::get_thread_num()<<endl;
#pragma omp barrier

}

#pragma omp critical (st1)
{
  if(_End<time)_End=time;
  if(_Begin>time)_Begin=time;
  _Status[_Nelem].event=evt;
  _Status[_Nelem].st[0]=status[0];
   uinteger offset=uinteger(((DAQEvent*)AMSEvent::gethead()->getheadC("DAQEvent",0))->getoffset()-_Offset);
  _Status[_Nelem].off=offset;
  _Status[_Nelem].st[1]=status[1];
//  cout <<" evt "<<evt<<" "<<offset<<" "<<_Offset<<endl;
  if(_Nelem<=MAXDAQRATE+STATUSSIZE-1)_Nelem++;
  else {
        cerr <<"AMSSTatus::adds-E-MaxDAQRateExceeds "<<MAXDAQRATE<<
        " some of the events will be lost"<<endl;
}
}
}

void AMSStatus::updates(uinteger run, uinteger evt, uinteger* status, time_t time){
 stm event(evt);
 int out= AMSbins(_Status,event,_Nelem);
  if(out>0){
    _Status[out-1].st[0]=status[0];
    _Status[out-1].st[1]=status[1];
  }
  else {
      cerr<<"AMSStatus::updates-E--NoMatchFoundRun "<<run<<" " <<evt<<endl;
#pragma omp critical (st1)
      _Errors++;
  }
  if(out==_Nelem && AMSFFKEY.Update && isDBUpdateR()){
   UpdateStatusTableDB();
  }
}

AMSStatus::statusI AMSStatus::getstatus(uinteger evt, uinteger run){
  uinteger one=1;
  if(_Run && run != _Run){
   cerr<<"AMSStatus::getstatus-E-WrongRun "<<run<<" Expected "<<_Run<<endl;
#pragma omp critical (st1)
   _Errors++;
   return statusI((one<<31),0);
  }
  // try hint +
  //cout <<" nelem "<<_Nelem<<endl;
  int out;
  stm event(evt);
  if( _Hint>=_Nelem || evt!=_Status[_Hint].event)out= AMSbins(_Status,event,_Nelem);
  else out=_Hint+1;
  static int repeat=0;
 if (out>0){
   _Hint=out;
   repeat=0;
   return statusI(_Status[out-1].st[0],_Status[out-1].st[1])  ;
 }
 else if(repeat<10  ){
   cerr<<"AMSStatus::getstatus-E-NoMatchFoundRun "<<run<<" "<<out<<" "<<evt<<" "<<_Nelem<<" "<<_Status[-out].event<<" "<<_Status[-out-1].event<<endl;
#pragma omp critical (st1)
   _Errors++;
   repeat++;
   return statusI((one<<31),0);
 }
 else if(repeat==10 ){
   cerr<<"AMSStatus::getstatus-E-NoMatchFoundLastMessage"<<out<<" "<<evt<<endl;
#pragma omp critical (st1)
   _Errors++;
   repeat++;
   return statusI((one<<31),0);
 }
 else {
   return statusI((one<<31),0);
}
}


bool  AMSStatus::geteventpos(uinteger run, uinteger evt, uinteger curevent){
  if(_Run && run != _Run){
   cerr<<"AMSStatus::geteventpos-E-WrongRun "<<run<<" Expected "<<_Run<<endl;
   return false;
  }
  // try hint +
  int out;
 stm event(evt);
  if(_Hint>=_Nelem || evt!=_Status[_Hint].event)out= AMSbins(_Status,event,_Nelem);
  else out=_Hint+1;
 if (out>0){
   _Hint=out;
   //event found;
#pragma omp critical (g4)
 ((DAQEvent*)AMSEvent::gethead()->getheadC("DAQEvent",0))->setoffset(_Offset+_Status[out-1].off);
  return true;   
 }
 else {
   // No Match Found
   if(evt>_Status[_Nelem-1].event && curevent<_Status[_Nelem-1].event){
#pragma omp critical (g4)
      ((DAQEvent*)AMSEvent::gethead()->getheadC("DAQEvent",0))->setoffset(_Offset+_Status[_Nelem-1].off);
   }
   else if(curevent>_Status[_Nelem-1].event){ 
    static int npr=0;
    if(npr++<100)cerr<<"AMSStatus::geteventpos-E-NoMatchfound "<<run<<" "<<curevent<<" "<<_Status[_Nelem-1].event<<" "<<endl;
   }
   else if(evt<_Status[_Nelem-1].event){ 
    cerr<<"AMSStatus::geteventpos-E-NoMatchFoundRun "<<run<<" "<<out<<" "<<evt<<" "<<_Nelem<<" "<<_Status[-out].event<<" "<<_Status[-out-1].event<<endl;
#pragma omp critical (g4)
      ((DAQEvent*)AMSEvent::gethead()->getheadC("DAQEvent",0))->setoffset(_Offset+_Status[-out].off);
   }
   return false;
}
}


void AMSStatus::init(){
  _Mode=1;
  if(  AMSFFKEY.Update){
    AMSTimeID *ptdv=AMSJob::gethead()->gettimestructure(AMSEvent::gethead()->getTDVStatus());
    for(int i=0;i<AMSJob::gethead()->gettdvn();i++){
      if( strcmp(AMSJob::gethead()->gettdvc(i),ptdv->getname())==0 ){
       AMSJob::gethead()->getstatustable()->_init();
        const int size=sizeof(STATUSFFKEY.status)/sizeof(STATUSFFKEY.status[0]);
       if(!STATUSFFKEY.status[size-2]){   
         _Mode=2;
         
         cout <<"AMSStatus::init-I-WriteStatusDBRequested"<<endl;
         // set begin,end to max
           time_t begin,insert,end;
           ptdv->gettime(insert,begin,end);
           begin=0;
           end=INT_MAX-1;
           ptdv->SetTime(insert,begin,end);
          
       }
       else {
         _Mode=3;
         cout <<"AMSStatus::init-I-UpdateStatusDBRequested"<<endl;
        // set begin>end
           time_t begin,insert,end;
           ptdv->gettime(insert,begin,end);
           end=begin-1;
           ptdv->SetTime(insert,begin,end);
       
       }
      }
    }
  }
}


integer AMSStatus::statusok(uinteger event, uinteger run){
    statusI status=getstatus(event,run);
    return _statusok(status);
}

integer AMSStatus::_statusok(statusI status){
    uinteger one=1;
    if(!(status[0] & (one<<31))){    // Status exists
    const int nsta=sizeof(STATUSFFKEY.status)/sizeof(STATUSFFKEY.status[0])-2;
      uinteger Status[nsta];
      Status[0]=((status[0] & ((1<<2)-1)));
      Status[1]=((status[0]>>2) & ((1<<1)-1));
      Status[2]=((status[0]>>3) & ((1<<1)-1));
      Status[3]=((status[0]>>4) & ((1<<1)-1));
      Status[4]=((status[0]>>5) & ((1<<1)-1));
      Status[5]=((status[0]>>6) & ((1<<1)-1));
      Status[6]=((status[0]>>7) & ((1<<1)-1));
      Status[7]=((status[0]>>8) & ((1<<2)-1));
      Status[8]=((status[0]>>10) & ((1<<3)-1));
      Status[9]=((status[0]>>13) & ((1<<2)-1));
      Status[10]=((status[0]>>15) & ((1<<2)-1));
      Status[11]=((status[0]>>17) & ((1<<2)-1));
      Status[12]=((status[0]>>19) & ((1<<2)-1));
      Status[13]=((status[0]>>21) & ((1<<2)-1));
      Status[14]=((status[0]>>23) & ((1<<3)-1));
      Status[15]=((status[0]>>26) & ((1<<2)-1));
      Status[16]=((status[0]>>28) & ((1<<2)-1));
      Status[17]=((status[0]>>30) & ((1<<1)-1));
      Status[18]=((status[0]>>31) & ((1<<1)-1));
      Status[19]=((status[1]) & ((1<<1)-1));
      Status[20]=((status[1]>>1) & ((1<<1)-1));
      Status[21]=((status[1]>>2) & ((1<<1)-1));
      Status[22]=((status[1]>>3) & ((1<<2)-1));
      Status[23]=((status[1]>>5) & ((1<<2)-1));
      Status[24]=((status[1]>>7) & ((1<<2)-1));
      Status[25]=((status[1]>>9) & ((1<<2)-1));
      Status[26]=((status[1]>>11) & ((1<<2)-1));
      Status[27]=((status[1]>>13) & ((1<<2)-1));
      Status[28]=((status[1]>>15) & ((1<<5)-1));
      Status[29]=((status[1]>>20) & ((1<<1)-1));
      Status[30]=((status[1]>>21) & ((1<<2)-1));
      Status[31]=((status[1]>>23) & ((1<<2)-1));
      Status[32]=((status[1]>>25) & ((1<<3)-1));
      Status[33]=((status[1]>>28) & ((1<<4)-1));
        uinteger local=0;
      for(int i=0;i<nsta;i++){
        local=0;
        if(STATUSFFKEY.status[i]==0)continue;
        else {
          uinteger st=STATUSFFKEY.status[i];
          uinteger nbit=32.*log(2.)/log(number(STATUSFFKEY.status[nsta+1]))+0.5;
          for (unsigned int j=0;j<nbit;j++){
            uinteger stbit=(st%STATUSFFKEY.status[nsta+1])>0?1:0;
            if((stbit<<j) & (1<<Status[i])){
          //    if(i==15){
          //     cout <<" got it "<<j<<" "<<stbit<<" "<<Status[i]<<endl;
          //    }
              local=1;
              break; 
            }
            st=st/STATUSFFKEY.status[nsta+1];
          }
        }
        if(!local){
#pragma omp critical (statusrej)
          _Rejected++;
          return 0;
        }
      }
    }
    else if(isDBUpdateR()){
#pragma omp critical (statusrej)
     _Rejected++;
     return 0;
    }   
#pragma omp critical (statusacc)
    _Accepted++;
    return 1;

}


integer AMSStatus::getnextok(){
 int skipped=0;
 for(int i=_Hint;i<_Nelem;i++){
   if(_statusok(statusI(_Status[i].st[0],_Status[i].st[1]))){
//  protection in mthreaded mode
//
  uint64 offset=((DAQEvent*)AMSEvent::gethead()->getheadC("DAQEvent",0))->getsoffset();
// a bit tricky in mt mode
const int maxthr=256;
const long long max32=4294967296LL;
  uint64 off64=_Offset+_Status[i].off;
if(i<maxthr && _Status[i].off>_Status[maxthr].off && off64>=uint64(max32)){
   cerr<<"AMSSetatus::getnextok-W-32bitProblemfound "<<i<<" "<<_Status[i].off<<" "<<_Offset<<" "<<off64%max32<<endl;
     off64=off64%max32;
}
     if(offset<off64){
         ((DAQEvent*)AMSEvent::gethead()->getheadC("DAQEvent",0))->setoffset(off64);
     _Hint=i;
     return skipped;
     }
     else return 0;
   }
   skipped++;
 }
 if(_Hint<_Nelem-1){
  uint64 offset=((DAQEvent*)AMSEvent::gethead()->getheadC("DAQEvent",0))->getsoffset();
     if(offset<_Offset+_Status[_Nelem-1].off){
       ((DAQEvent*)AMSEvent::gethead()->getheadC("DAQEvent",0))->setoffset(_Offset+_Status[_Nelem-1].off);
       _Hint=_Nelem-1;
       return skipped;
     } 
}
 else  if(AMSFFKEY.Update && isDBUpdateR()){
   UpdateStatusTableDB();
 }
 return 0;
}

void AMSStatus::UpdateStatusTableDB(){
      AMSTimeID *ptdv=AMSJob::gethead()->gettimestructure(AMSEvent::getTDVStatus());
      ptdv->SetNbytes(AMSJob::gethead()->getstatustable()->getsizeV());
      AMSJob::gethead()->getstatustable()->Sort();
      uinteger crcold=ptdv->getCRC();
      ptdv->UpdCRC();
      if(crcold!=ptdv->getCRC()){
       ptdv->UpdateMe()=1;
       time_t begin,end,insert;
       ptdv->gettime(insert,begin,end);
       time(&insert);
       ptdv->SetTime(insert,begin,end);
       cout <<" Event Status info  status has been updated for "<<*ptdv;
       cout <<" Time Insert "<<ctime(&insert);
       cout <<" Time Begin "<<ctime(&begin);
       cout <<" Time End "<<ctime(&end);
       cout << " Starting to update "<<*ptdv; 
       if(  !ptdv->write(AMSDATADIR.amsdatabase))
        cerr <<"AMSStatus::UpdateStatusTableDB-S-ProblemtoUpdate "<<*ptdv;
      }
  }


