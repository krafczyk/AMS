// 
// Feb 7, 1998. ak. do not write if DB is on
//
#include <timeid.h>
#include <job.h>
#include <event.h>
#include <astring.h>
#include <fstream.h>
#include <commons.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <trcalib.h>
#include <ctype.h>
uinteger * AMSTimeID::_Table=0;
const uinteger AMSTimeID::CRC32=0x04c11db7;
AMSTimeID::AMSTimeID(AMSID  id, tm   begin, tm  end, integer nbytes=0, 
                     void *pdata=0):
           AMSNode(id),_pData((uinteger*)pdata),_UpdateMe(0){
      _Nbytes=nbytes;
      _fillDB(AMSDATADIR.amsdatabase);
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
         if(_Begin >= _End){
           cout <<" But validity period is zero. Request rejected. "<<endl;
           _UpdateMe=0;
         }
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

void AMSTimeID::UpdCRC(){
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
    _UpdateMe=0;
    return _Nbytes;
  }
  else return 0;
}



integer AMSTimeID::validate(time_t & Time, integer reenter){
if (Time >= _Begin && Time <= _End){
  if(_CRC == _CalcCRC())return 1;
  else {
      cerr<<"AMSTimeID::validate-S-CRC Error "<<getname()<<" Old CRC "
      <<_CRC<<" New CRC "   <<_CalcCRC()<<endl;
  }
  return 0;
}
else if(reenter<2){
  // try to read it from file ....
  read(AMSDATADIR.amsdatabase,reenter);
  return validate(Time,reenter+1);
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
    _Table=new uinteger[256];
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

integer AMSTimeID::write(char * dir){

#ifndef __DB__
  enum open_mode{binary=0x80};
    fstream fbin;
    AString fnam(dir);
    fnam+=getname();
    fnam+= getid()==0?".0":".1";
    if(AMSJob::gethead()->isCalibration()  & AMSJob::CTracker){
     char name[255];
     ostrstream ost(name,sizeof(name));
     ost << "."<<AMSTrIdCalib::getrun()<<ends;
     fnam+=name;     
    }
    fbin.open((const char *)fnam,ios::out|binary|ios::trunc);
    if(fbin){
     uinteger * pdata;
     integer ns=_Nbytes/sizeof(pdata[0])+3;
     pdata =new uinteger[ns];
     if(pdata){
      CopyOut(pdata);
      pdata[_Nbytes/sizeof(pdata[0])]=uinteger(_Insert);
      pdata[_Nbytes/sizeof(pdata[0])+1]=uinteger(_Begin);
      pdata[_Nbytes/sizeof(pdata[0])+2]=uinteger(_End);
      _convert(pdata,ns);
      fbin.write((char*)pdata,ns*sizeof(pdata[0]));
      fbin.close();
      delete [] pdata;
      return fbin.good();
    }
     else cerr<<"AMSTimeID::write-E-Failed to allocate memory "<<_Nbytes<<endl;
    }
    else {
      cerr<<"AMSTimeID::write-E-CouldNot open file "<<fnam;
    }
#endif

    return 0;

}


integer AMSTimeID::read(char * dir, integer reenter){

  // first get a run no from dbase
  uinteger run=_getRun(AMSEvent::gethead()->getrun());

  enum open_mode{binary=0x80};
    fstream fbin;
    AString fnam(dir);
    fnam+=getname();
    fnam+= getid()==0?".0":".1";
    if(run && !reenter){
     char name[255];
     ostrstream ost(name,sizeof(name));
     ost << "."<<run<<ends;
     fnam+=name;     
    }

    else {
      cout <<"AMSTimeID::read-W-Default value for TDV "<<getname()<<" will be used."<<endl;
    }
    fbin.open((const char *)fnam,ios::in|binary);
    if(fbin){
#ifdef __AMSDEBUG__
      cout <<"AMSTimeID::read-I-Open file "<<fnam<<endl;
#endif

     uinteger * pdata;
     integer ns=_Nbytes/sizeof(pdata[0])+3;
     pdata =new uinteger[ns];
     if(pdata){
      fbin.read((char*)pdata,ns*sizeof(pdata[0]));
      if(fbin.good()){
       _convert(pdata,ns);
       CopyIn(pdata);
      _Insert=time_t(pdata[_Nbytes/sizeof(pdata[0])]);
      _Begin=time_t(pdata[_Nbytes/sizeof(pdata[0])+1]);
      _End=time_t(pdata[_Nbytes/sizeof(pdata[0])+2]);
      }
      else {
        cout<<"AMSTimeID::read-W-Problems to Read File "<<fnam<<endl;
      }
      fbin.close();
      delete [] pdata;




      return fbin.good();
     }
     else cerr<<"AMSTimeID::read-E-Failed to allocate memory "<<_Nbytes<<endl;
    }
    else {
      cerr<<"AMSTimeID::read-W-CouldNot open file "<<fnam<<endl;
    }
    return 0;

}

void AMSTimeID::_convert(uinteger *pdata, integer n){

  if(AMSDBc::BigEndian){
    // Let's convert   to little endian...
   unsigned char tmp;
   unsigned char *pc = (unsigned char*)pdata;
   int i;
   for(i=0;i<n;i++){
     tmp=*pc;
     *pc=*(pc+3);
     *(pc+3)=tmp;
     tmp=*(pc+1);
     *(pc+1)=*(pc+2);
     *(pc+2)=tmp;
     pc=pc+sizeof(pdata[0]);
   }
  }


}
uinteger AMSTimeID::_getRun(uinteger run){

integer index=AMSbiel(_pDataBaseEntries,run,_DataBaseSize);

if(index <0)return 0;
else if (index>=_DataBaseSize)return _pDataBaseEntries[_DataBaseSize-1];
else{
if(_pDataBaseEntries[index] ==run)return run;
else return index>0?_pDataBaseEntries[index-1]:0;
}




}

AString * AMSTimeID::_selectEntry=0;

integer AMSTimeID::_select(dirent *entry){
return strstr(entry->d_name,(char*)*_selectEntry)!=NULL;    
}

void AMSTimeID::_fillDB(const char *dir){
  //typedef integer (*pselect)(dirent * entry);
_pDataBaseEntries=0;
_DataBaseSize=0;
    AString fnam(getname());
    fnam+= getid()==0?".0":".1";
    _selectEntry=&fnam;
    dirent ** namelist;
    int nptr=scandir(dir,&namelist,&_select,NULL);     
    if(nptr){
     _pDataBaseEntries=new uinteger[nptr];
     for(int i=0;i<nptr;i++) {
      int valid=0;
      int kvalid=0;
      for(int k=strlen((char*)fnam);k<namelist[i]->d_namlen-1;k++){
       if((namelist[i]->d_name)[k]=='.' )valid++;
       if((namelist[i]->d_name)[k]=='.')kvalid=k;
      }
      if(valid==1 && isdigit(namelist[i]->d_name[kvalid+1]))sscanf((namelist[i]->d_name)+kvalid+1,"%d",
                         _pDataBaseEntries+_DataBaseSize++);
      free(namelist[i]);
     }
    AMSsortNAGa(_pDataBaseEntries,_DataBaseSize);
    free(namelist);

    }
#ifdef __AMSDEBUG__
    cout <<"AMSTimeID::_fillDB-I-"<<_DataBaseSize<<" entries found for TDV "<<fnam<<endl; 
#endif

}
