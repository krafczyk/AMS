//  $Id: status.C,v 1.33 2009/01/06 10:07:30 choutko Exp $
// Author V.Choutko.
#include "status.h"
#include "snode.h"
#include "amsgobj.h"
#include "job.h"
#include "amsstl.h"
#include <iostream.h>
#include "event.h"
#include "timeid.h"
integer AMSStatus::_Mode=0;
void AMSStatus::_init(){
     _Nelem=0;
}
   
ostream & AMSStatus::print(ostream & stream)const{
  return(AMSID::print(stream)  <<  " Status" << endl);
}

AMSStatus* AMSStatus::create(int version){
 if( version==0){ 
   static AMSStatus tstatus("EventStatusTable",version);
   return &tstatus;
 }
else{
   static AMSStatus tstatus("EventStatusTable02",version);
   return &tstatus;
}

}

integer AMSStatus::isFull(uinteger run, uinteger evt, time_t time,bool force){
  static time_t oldtime=0;
  integer timechanged= time!=oldtime?1:0;
  if(AMSEvent::get_num_threads()==1 && run==_Run && _Nelem>0 && evt<_Status[0][_Nelem-1]){
    cerr <<"AMSStatus::isFull-E-EventSequenceBroken "<<_Nelem<<" "<<run<<" "<<evt<<" "<<_Status[0][_Nelem-1]<<endl;
#pragma omp critical (st1)
   _Errors++;
   return 2;
  }
  if(_Nelem>=MAXDAQRATE+STATUSSIZE){
        cerr <<"AMSSTatus::isFull-E-MaxDAQRateExceeds "<<MAXDAQRATE<<
        " some of the events will be lost"<<endl;
#pragma omp critical (st1)
        _Errors++;

        return 2;
}
  bool ret= ((_Nelem>=STATUSSIZE || force) && timechanged ) || (run!=_Run && _Nelem>0) || (_Nelem>0 && (AMSEvent::gethead()->getC("DAQEvent",0)) && ((DAQEvent*)AMSEvent::gethead()->getheadC("DAQEvent",0))->getoffset()-_Offset>INT_MAX);
    if(ret){
       cout <<  "  StatusTableFull "<<_Offset<<" "<<_Nelem<<endl;
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
uinteger *tmp =new uinteger[_Nelem];
uinteger **padd=new uinteger*[_Nelem];
for(int i=0;i<_Nelem;i++){
 tmp[i]=_Status[0][i];
 padd[i]=tmp+i;
}
AMSsortNAG(padd,_Nelem);
for(int i=0;i<4;i++){
 for(int k=0;k<_Nelem;k++)tmp[k]=_Status[i][k];
 for(int k=0;k<_Nelem;k++)_Status[i][k]=*(padd[k]);
}
delete[] padd;
delete[] tmp;
// for(int k=0;k<_Nelem;k++)cout <<_Status[0][k]<<" "<<_Status[1][k]<<" "<<_Status[2][k]<<" "<<_Status[3][k]<<endl;
}
void AMSStatus::adds(uinteger run, uinteger evt, uinteger* status, time_t time){
  if(_Nelem==0  || isFull(run,evt,time)>1){


#pragma omp barrier
_Offset=9223372036854775807LL;
#pragma omp critical (st1)
{
    uint64 offset=((DAQEvent*)AMSEvent::gethead()->getheadC("DAQEvent",0))->getoffset();
     
    if(offset<_Offset)_Offset=offset;
    if(AMSEvent::get_thread_num()==0){
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
#pragma omp barrier
}

#pragma omp critical (st1)
{
  if(_End<time)_End=time;
  _Status[0][_Nelem]=evt;
  _Status[1][_Nelem]=status[0];
   uinteger offset=uinteger(((DAQEvent*)AMSEvent::gethead()->getheadC("DAQEvent",0))->getoffset()-_Offset);
  _Status[2][_Nelem]=offset;
  _Status[3][_Nelem]=status[1];
  if(_Nelem<=MAXDAQRATE+STATUSSIZE-1)_Nelem++;
  else {
        cerr <<"AMSSTatus::adds-E-MaxDAQRateExceeds "<<MAXDAQRATE<<
        " some of the events will be lost"<<endl;
}
}
}

void AMSStatus::updates(uinteger run, uinteger evt, uinteger* status, time_t time){
 int out= AMSbins(_Status[0],evt,_Nelem);
  if(out>0){
    _Status[1][out-1]=status[0];
    _Status[3][out-1]=status[1];
  }
  else {
      cerr<<"AMSStatus::updates-E--NoMatchFoundRun "<<run<<" " <<evt<<endl;
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
   _Errors++;
   return statusI((one<<31),0);
  }
  // try hint +
  int out;
  if(_Hint>=_Nelem || evt!=_Status[0][_Hint])out= AMSbins(_Status[0],evt,_Nelem);
  else out=_Hint+1;
  static int repeat=0;
 if (out>0){
   _Hint=out;
   repeat=0;
   return statusI(_Status[1][out-1],_Status[3][out-1])  ;
 }
 else if(repeat<10  ){
   cerr<<"AMSStatus::getstatus-E-NoMatchFoundRun "<<run<<" "<<out<<" "<<evt<<" "<<_Nelem<<" "<<_Status[0][-out]<<" "<<_Status[0][-out-1]<<endl;
   _Errors++;
   repeat++;
   return statusI((one<<31),0);
 }
 else if(repeat==10 ){
   cerr<<"AMSStatus::getstatus-E-NoMatchFoundLastMessage"<<out<<" "<<evt<<endl;
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
  if(_Hint>=_Nelem || evt!=_Status[0][_Hint])out= AMSbins(_Status[0],evt,_Nelem);
  else out=_Hint+1;
 if (out>0){
   _Hint=out;
   //event found;
 ((DAQEvent*)AMSEvent::gethead()->getheadC("DAQEvent",0))->setoffset(_Offset+_Status[2][out-1]);
  return true;   
 }
 else {
   // No Match Found
   if(evt>_Status[0][_Nelem-1] && curevent<_Status[0][_Nelem-1]){
      ((DAQEvent*)AMSEvent::gethead()->getheadC("DAQEvent",0))->setoffset(_Offset+_Status[2][_Nelem-1]);
   }
   else if(curevent>_Status[0][_Nelem-1]){ 
    static int npr=0;
    if(npr++<100)cerr<<"AMSStatus::geteventpos-E-NoMatchfound "<<run<<" "<<curevent<<" "<<_Status[0][_Nelem-1]<<" "<<endl;
   }
   else if(evt<_Status[0][_Nelem-1]){ 
    cerr<<"AMSStatus::geteventpos-E-NoMatchFoundRun "<<run<<" "<<out<<" "<<evt<<" "<<_Nelem<<" "<<_Status[0][-out]<<" "<<_Status[0][-out-1]<<endl;
      ((DAQEvent*)AMSEvent::gethead()->getheadC("DAQEvent",0))->setoffset(_Offset+_Status[2][-out]);
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
       if(!STATUSFFKEY.status[32]){   
         _Mode=2;
         
         cout <<"AMSStatus::init-I-WriteStatusDBRequested"<<endl;
         // set begin,end to max
           time_t begin,insert,end;
           ptdv->gettime(insert,begin,end);
           begin=begin-3e8;
           end=end+3e8;
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
      const int nsta=18;
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
        uinteger local=0;
      for(int i=0;i<nsta;i++){
        local=0;
        if(STATUSFFKEY.status[i]==0)continue;
        else {
          uinteger st=STATUSFFKEY.status[i];
          uinteger nbit=32.*log(2.)/log(number(STATUSFFKEY.status[33]))+0.5;
          for (int j=0;j<nbit;j++){
            uinteger stbit=(st%STATUSFFKEY.status[33])>0?1:0;
            if((stbit<<j) & (1<<Status[i])){
          //    if(i==15){
          //     cout <<" got it "<<j<<" "<<stbit<<" "<<Status[i]<<endl;
          //    }
              local=1;
              break; 
            }
            st=st/10;
          }
        }
        if(!local){
          _Rejected++;
          return 0;
        }
      }
    }
    else if(isDBUpdateR()){
     _Rejected++;
     return 0;
    }   
    _Accepted++;
    return 1;

}


integer AMSStatus::getnextok(){
 int skipped=0;
 for(int i=_Hint;i<_Nelem;i++){
   if(_statusok(statusI(_Status[1][i],_Status[3][i]))){
     ((DAQEvent*)AMSEvent::gethead()->getheadC("DAQEvent",0))->setoffset(_Offset+_Status[2][i]);
     _Hint=i;
     return skipped;
   }
   skipped++;
 }
 if(_Hint<_Nelem)((DAQEvent*)AMSEvent::gethead()->getheadC("DAQEvent",0))->setoffset(_Offset+_Status[2][_Nelem-1]);
 else  if(AMSFFKEY.Update && isDBUpdateR()){
   UpdateStatusTableDB();
 }
 return skipped;
}

void AMSStatus::UpdateStatusTableDB(){
      AMSTimeID *ptdv=AMSJob::gethead()->gettimestructure(AMSEvent::getTDVStatus());
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


