#include <timeid.h>
#include <job.h>
uinteger * AMSTimeID::_Table=0;
const uinteger AMSTimeID::CRC32=0x04c11db7;
AMSTimeID::AMSTimeID(AMSID  id, tm   begin, tm  end, integer nbytes=0, 
                     void *pdata=0):
           AMSNode(id),_pData((uinteger*)pdata),_UpdateMe(0){
      _Nbytes=nbytes;
#ifdef __AMSDEBUG__
      if(_Nbytes%sizeof(uinteger)!=0){
        cerr <<"AMSTimeID-ctor-F-Nbytes not aligned "<<_Nbytes<<endl;
        exit(1);
      }
#endif
      //  Zero light saving hour
      begin.tm_isdst=0;
      end.tm_isdst=0;
      _Begin=mktime(&begin); 
      _End=mktime(&end); 
      _Insert=_Begin;
      _Nbytes+=sizeof(uinteger);
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
         cout <<" Begin "<<ctime(&_Begin)<<" End "<<ctime(&_End)<<
           " Insert "<<ctime(&_Insert)<<endl;
         break;
        }
      }
#ifdef __AMSDEBUG__
        if(getid() != AMSJob::gethead()->isRealData()){
          cerr << "AMSTimeID-ctor-F-numerical id mismatch "<<
          getid()<<AMSJob::gethead()->isRealData()<<endl;
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
    integer n=_Nbytes/sizeof(uinteger)-1;
    integer i;
    for(i=0;i<n;i++){
     *(_pData+i)=*((uinteger*)pdata+i);
    }
    _CRC=*((uinteger*)pdata+n);
    return _Nbytes;
  }
  else return 0;
}

integer AMSTimeID::CopyOut(void *pdata){
  if(pdata && _pData){
    integer n=_Nbytes/sizeof(uinteger)-1;
    integer i;
    for(i=0;i<n;i++){
     *((uinteger*)pdata+i)=*(_pData+i);
    }
    *((uinteger*)pdata+n)=_CRC;
    return _Nbytes;
  }
  else return 0;
}



integer AMSTimeID::validate(time_t & Time){
if (Time >= _Begin && Time <= _End){
  if(_CRC == _CalcCRC())return 1;
  else {
      cerr<<"AMSTimeID::CopyIn-S-CRC Error "<<getname()<<" Old CRC "
      <<_CRC<<" New CRC "   <<_CalcCRC()<<endl;
  }
 return 0;
}
else return 0;
}

uinteger AMSTimeID::_CalcCRC(){
 _InitTable();
  int i,j,k;
  integer n=_Nbytes/sizeof(uinteger)-1;
  uinteger crc;
  if( n < 1) return 0;
  uinteger *pu=(uinteger *)_pData; 
    crc=~pu[0];
    for(i=1;i<n;i++){
      for(j=0;j<3;j++)crc=_Table[crc>>24]^(crc<<8);
     crc=crc^pu[i];  
    }
  return ~crc;
}

void AMSTimeID::_InitTable(){
  if(!_Table){
    _Table=new uinteger[255];
    assert(_Table!=NULL);
    integer i,j;
    uinteger crc;
    for(i=0;i<256;i++){
      crc=i<<24;
      for(j=0;j<8;j++)crc=crc&0x80000000 ? (crc<<1)^CRC32 : crc<<1;
      _Table[i]=crc;
      //cout << i<<" "<<_Table[i]<<endl;
    }  
  }
}
