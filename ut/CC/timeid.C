#include <timeid.h>
#include <job.h>
uinteger * AMSTimeID::Table=0;
const uinteger AMSTimeID::CRC32=0x04c11db7;
AMSTimeID::AMSTimeID(AMSID  id, tm   begin, tm  end, integer nbytes=0, 
                     void *pdata=0):
           AMSNode(id),_pData(pdata),_UpdateMe(0){
      _Nbytes=nbytes;
#ifdef __AMSDEBUG__
      if(_Nbytes%sizeof(integer)!=0){
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
    _CRC=*((uinteger*)pdata+n);
    return _Nbytes;
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
 uinteger crc=0xffffffff;;
 int i;
    integer n=_Nbytes-sizeof(uinteger);
    for(i=0;i<n;i++){
      crc=(crc<<8) ^ Table[crc>>24] ^ *((unsigned char*)_pData+i);
    }
  return ~crc;
}

void AMSTimeID::_InitTable(){
  if(!Table){
    Table=new uinteger[255];
    assert(Table!=NULL);
    integer i,j;
    uinteger crc;
    for(i=0;i<256;i++){
      crc=i<<24;
      for(j=0;j<8;j++)crc=crc&0x80000000 ? (crc<<1)^CRC32: crc<<1;
      Table[i]=crc;
    }  
  }
}
