#include <timeid.h>
#include <job.h>
AMSTimeID::AMSTimeID(AMSID  id, tm & begin, tm &end, integer nbytes=0, 
                     void *pdata=0):
           AMSNode(id),_pData(pdata),_UpdateMe(0){
      _Nbytes=nbytes;
#ifdef __AMSDEBUG__
      if(_Nbytes%sizeof(integer)!=0){
        cerr <<"AMSTimeID-ctor-F-Nbytes not aligned "<<_Nbytes<<endl;
        exit(1);
      }
#endif
      _Begin=mktime(&begin); 
      _End=mktime(&end); 
      _Insert=_Begin;
      _Nbytes+=sizeof(integer);
      _CRC=_CalcCRC();
      int i;
      for(i=0;i<AMSJob::gethead()->gettdvn();i++){
        if( (
           strcmp(AMSJob::gethead()->gettdvc(i),getname())==0 ||
           strcmp(AMSJob::gethead()->gettdvc(i),"UpdateAllTDV")==0) ){
         _UpdateMe=1;
         time(&_Insert);
         cout <<"AMSTimeID-ctor-I-Update for "<<getname()<<" "<<getid()<<
           " requested. "<<endl;
         break;
        }
      }
#ifdef __AMSDEBUG__
        if(getid() != AMSJob::gethead()->getjobtype()){
          cerr << "AMSTimeID-ctor-F-numerical id mismatch "<<
          getid()<<AMSJob::gethead()->getjobtype()<<endl;
          exit(1);
        }
#endif
}

void AMSTimeID::gettime(time_t & insert, time_t & begin, time_t & end) const{
insert=_Insert;
begin=_Begin;
end=_End;
}

void AMSTimeID::SetTime(time_t insert, time_t begin, time_t end) {
_Insert=insert;
_Begin=begin;
_End=end;
}


integer AMSTimeID::CopyIn(void *pdata){
  if(pdata && _pData){
    integer n=_Nbytes/sizeof(integer)-1;
    integer i;
    for(i=0;i<n;i++){
     *((integer*)_pData+i)=*((integer*)pdata+i);
    }
    _CRC=*((integer*)pdata+n);
    if(_CRC != _CalcCRC()){
      cerr<<"AMSTimeID::CopyIn-E-CRC Error "<<getname()<<" "<<_CRC<<" "
      <<_CalcCRC()<<endl;
      exit(1);
    }
    else return _Nbytes;
  }
  else return 0;
}

integer AMSTimeID::CopyOut(void *pdata){
  if(pdata && _pData){
    integer n=_Nbytes/sizeof(integer)-1;
    integer i;
    for(i=0;i<n;i++){
     *((integer*)pdata+i)=*((integer*)_pData+i);
    }
    *((integer*)pdata+n)=_CRC;
    return _Nbytes;
  }
  else return 0;
}



integer AMSTimeID::validate(time_t & Time){
if (Time >= _Begin && Time <= _End)return 1;
else return 0;
}

integer AMSTimeID::_CalcCRC(){
 integer crc=0;
 int i;
    integer n=_Nbytes/sizeof(integer)-1;
    for(i=0;i<n;i++){
     crc=crc ^ *((integer*)_pData+i);
    }
  return crc;
}
