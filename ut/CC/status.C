// Author V.Choutko.
// modified by E.Choumilov 20.06.96. - add some TOF materials.
#include <status.h>
#include <snode.h>
#include <amsgobj.h>
#include <job.h>
#include <amsstl.h>
#include <iostream.h>
#include <event.h>
integer AMSStatus::_Mode=0;
void AMSStatus::_init(){
     _Nelem=0;
}
   
ostream & AMSStatus::print(ostream & stream)const{
  return(AMSID::print(stream)  <<  " Status" << endl);
}

void AMSStatus::create(){
  static AMSStatus tstatus("EventStatusTable");
  AMSJob::gethead()->addup(&tstatus);
}

integer AMSStatus::isFull(uinteger run, uinteger evt){
  if(_Nelem>0 && evt<_Status[0][_Nelem-1]){
    cerr <<"AMSStatus::isFull-E-EventSequenceBroken "<<_Nelem<<" "<<run<<" "<<evt<<" "<<_Status[0][_Nelem-1]<<endl;
   return 1;
  }
  return _Nelem>=50000 || (run!=_Run && _Nelem>0) ;
}

void AMSStatus::update(uinteger run, uinteger evt, uinteger status, time_t time){
  if(_Nelem==0  || isFull(run,evt)){
    _Nelem=0;
    _Run=run;
    _Begin=time;
    if(_Begin==0){
      cerr <<"AMSStatus::update-E-BeginTimeIsZeroForRun"<<" "<<_Run<<endl;
    }
  }
  _End=time;
  _Status[0][_Nelem]=evt;
  _Status[1][_Nelem]=status;
  _Nelem++;
}

uinteger AMSStatus::getstatus(uinteger evt){
  AMSgObj::BookTimer.start("EventStatus");
  int out= AMSbins(_Status[0],evt,_Nelem);
  static int repeat=0;
 if(repeat<10 && out<=0){
   cerr<<"AMSStatus::getstatus-E-NoMatchFound "<<out<<" "<<evt<<" "<<_Nelem<<endl;
   repeat++;
   AMSgObj::BookTimer.stop("EventStatus");
   return (1<<32);
 }
 else if(repeat==10 && out<=0){
   cerr<<"AMSStatus::getstatus-E-NoMatchFoundLastMessage"<<out<<" "<<evt<<endl;
   repeat++;
   AMSgObj::BookTimer.stop("EventStatus");
   return (1<<32);
 }
 else if(out>0){
   repeat=0;
   AMSgObj::BookTimer.stop("EventStatus");
   return _Status[1][out-1]  ;
 }
}

void AMSStatus::init(){
   AMSJob::map(1);
  _Mode=1;
  if(AMSFFKEY.Update){
    AMSTimeID *ptdv=AMSJob::gethead()->gettimestructure(AMSEvent::gethead()->getTDVStatus());
    for(int i=0;i<AMSJob::gethead()->gettdvn();i++){
      if( strcmp(AMSJob::gethead()->gettdvc(i),ptdv->getname())==0 ){
        _Mode=2;
        cout <<"AMSStatus::init-I-UpdataStatusDBRequested"<<endl;
      }
    }
  }
}


