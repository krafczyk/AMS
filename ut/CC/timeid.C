#include <timeid.h>
AMSTimeID::AMSTimeID(char * id, tm & begin, tm &end, 
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
    integer n=_Nbytes/sizeof(integer);
    integer i;
    for(i=0;i<n;i++){
     *((integer*)_pData+i)=*((integer*)pdata+i);
    }
    return _Nbytes;
  }
  else return 0;
}

integer AMSTimeID::CopyOut(void *pdata){
  if(pdata && _pData){
    integer n=_Nbytes/sizeof(integer);
    integer i;
    for(i=0;i<n;i++){
     *((integer*)pdata+i)=*((integer*)_pData+i);
    }
    return _Nbytes;
  }
  else return 0;
}



integer AMSTimeID::validate(time_t & Time){
if (Time >= _Begin && Time <= _End)return 1;
else return 0;
}
