#include <timeid.h>
AMSTimeID::AMSTimeID(AMSID  id, tm & begin, tm &end, 
                     integer nbytes=0, void *pdata=0):AMSNode(id),_pData(pdata){
      _Nbytes=nbytes;
#ifdef __AMSDEBUG__
      if(_Nbytes%sizeof(integer)!=0){
        cerr <<"AMSTimeID-ctor-F-Nbytes not aligned "<<_Nbytes<<endl;
        exit(1);
      }
#endif
      time(&_Insert);
      _Begin=mktime(&begin); 
      _End=mktime(&end); 
      _Nbytes+=sizeof(integer);
      _CRC=_CalcCRC();
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
