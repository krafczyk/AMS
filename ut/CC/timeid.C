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
#include <trcalib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/file.h>
#ifndef __IBMAIX__
#include <dirent.h>
#else

#define _D_NAME_MAX 255

struct  dirent {
        ulong_t         d_offset;       /* real off after this entry */
        ino_t           d_ino;          /* inode number of entry */
                                        /* make ino_t when it's ulong */
        ushort_t        d_reclen;       /* length of this record */
        ushort_t        d_namlen;       /* length of string in d_name */
        char            d_name[_D_NAME_MAX+1];  /* name must be no longer than t
his */
                                        /* redefine w/#define when name decided
*/
};


extern "C" int scandir(		const char *, struct dirent ***, 
                                int (*)(struct dirent *),  
                                int (*)(struct dirent **, struct dirent **));
#endif

extern char *tdvNameTab[maxtdv];
extern int  tdvIdTab[maxtdv];
extern int  ntdvNames;
#ifdef __DB__

#include <dbS.h>

extern LMS* lms;

#endif

uinteger * AMSTimeID::_Table=0;
const uinteger AMSTimeID::CRC32=0x04c11db7;
AMSTimeID::AMSTimeID(AMSID  id, tm   begin, tm  end, integer nbytes=0, 
                     void *pdata=0):
           AMSNode(id),_pData((uinteger*)pdata),_UpdateMe(0){
      _Nbytes=nbytes;

#ifndef __DB__
      _fillDB(AMSDATADIR.amsdatabase);
#endif
#ifdef __DB__
      _fillfromDB();
#endif

#ifdef __AMSDEBUG__
      if(_Nbytes%sizeof(uinteger)!=0){
        cerr <<"AMSTimeID-ctor-F-Nbytes not aligned "<<_Nbytes<<endl;
        exit(1);
      }
#endif
      //  Zero light saving hour
      //      begin.tm_isdst=0;
      //      end.tm_isdst=0;
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
AMSgObj::BookTimer.start("TDV");

#ifndef __DB__
int ok = read(AMSDATADIR.amsdatabase,reenter);
#endif

#ifdef __DB__
int ok = readDB();
#endif

if (Time >= _Begin && Time <= _End){
  if(ok==-1 || _CRC == _CalcCRC()){
     AMSgObj::BookTimer.stop("TDV");
     return 1;
  }
  else {
      cerr<<"AMSTimeID::validate-S-CRC Error "<<getname()<<" Old CRC "
      <<_CRC<<" New CRC "   <<_CalcCRC()<<endl;
  }
  if(!reenter)return validate(Time,1);
  AMSgObj::BookTimer.stop("TDV");
  return 0;
}
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
    //      if(AMSJob::gethead()->isCalibration()  & AMSJob::CTracker){
    //        char name[255];
    //        ostrstream ost(name,sizeof(name));
    //        ost << "."<<AMSTrIdCalib::getrun()<<ends;
    //        fnam+=name;     
    //      }
    //      else {
        char name[255];
        ostrstream ost(name,sizeof(name));
        ost << "."<<_Begin<<ends;
        fnam+=name;     
    //      }
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
      cerr<<"AMSTimeID::write-E-CouldNot open file "<<fnam<<endl;
    }
#endif

    return 0;

}


integer AMSTimeID::read(char * dir, integer reenter){

  // first get a run no from dbase
  integer dflt=0;
  integer run=_getDBRecord(AMSEvent::gethead()->gettime());
  enum open_mode{binary=0x80};
    fstream fbin;
    AString fnam(dir);
    //cout <<run<<" "<<reenter<<" "<<getname()<<endl;
    if(run>0 && !reenter){
     fnam+=getname();
     fnam+= getid()==0?".0":".1";
     char name[255];
     ostrstream ost(name,sizeof(name));
     ost << "."<<run<<ends;
     fnam+=name;     
    }

    else if(run==0 || reenter){
      fnam+=getname();
      fnam+= getid()==0?".0":".1";
      cout <<"AMSTimeID::read-W-Default value for TDV "<<getname()<<" will be used."<<endl;
      dflt=1;
    }
    else return -1;
    if(AMSFFKEY.Update){
      for(int i=0;i<AMSJob::gethead()->gettdvn();i++){
        if( strcmp(AMSJob::gethead()->gettdvc(i),getname())==0 ){
          // Never read tdv 
          cerr<<"AMSTimeID::read-W-UpdateTDVSet "<<getname()<<", reading from DB is disabled"<<endl;
          return -1;
        }
      }
    }
    fbin.open((const char *)fnam,ios::in|binary);
    if(fbin){

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
       if(dflt)_getDefaultEnd(AMSEvent::gethead()->gettime(),_End);
       cout <<"AMSTimeID::read-I-Open file "<<fnam<<endl;
#ifdef __AMSDEBUG__
       cout <<"AMSTimeID::read-I-Insert "<<ctime(&_Insert)<<endl;
       cout <<"AMSTimeID::read-I-Begin "<<ctime(&_Begin)<<endl;
       cout <<"AMSTimeID::read-I-End "<<ctime(&_End)<<endl;
#endif
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
      return 0;
    }
    return 1;

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
integer AMSTimeID::_getDBRecord(uinteger time){
 integer index=AMSbiel(_pDataBaseEntries[3],time,_DataBaseSize);
 //cout <<getname()<<" "<<index<<" "<<time<<" "<<_pDataBaseEntries[3][index]<<" "<<_DataBaseSize<<endl;
  int rec=-1;
 int insert= (time>=_Begin && time<=_End)?_Insert:0;
 for (int i=index<0?_DataBaseSize:index;i<_DataBaseSize;i++){
   if(time>=_pDataBaseEntries[2][i] && insert<_pDataBaseEntries[1][i]){
      insert=     _pDataBaseEntries[1][i];
      rec=i;
   }
 }

#ifndef __DB__
   if(time<_Begin || time>_End )return rec<0?0:_pDataBaseEntries[0][rec];
   else if (rec>=0)return  _pDataBaseEntries[0][rec];
   else return -1;
#endif
#ifdef __DB__
   return rec;
#endif

//Old
/*
 integer index=AMSbiel(_pDataBaseEntries[0],time,_DataBaseSize);
 if(index <0)return 0;
 else if (index>=_DataBaseSize)return _pDataBaseEntries[0][_DataBaseSize-1];
 else{
 if(_pDataBaseEntries[0][index] ==run)return run;
 else return index>0?_pDataBaseEntries[0][index-1]:0;
*/

}


void AMSTimeID::_getDefaultEnd(uinteger time, time_t & end){
 
 integer index=AMSbiel(_pDataBaseEntries[4],time,_DataBaseSize);
 //cout <<getname()<<" "<<index<<" "<<_pDataBaseEntries[4][0]<<" "<<_pDataBaseEntries[2][0]<<" "<<time<<" "<<_DataBaseSize<<endl;
 if(index>=0 && index<_DataBaseSize)end=_pDataBaseEntries[4][index];

}



AString * AMSTimeID::_selectEntry=0;

integer AMSTimeID::_select(dirent *entry){
return strstr(entry->d_name,(char*)*_selectEntry)!=NULL;    
}

void AMSTimeID::_fillDB(const char *dir){
int i;
for( i=0;i<5;i++)_pDataBaseEntries[i]=0;
    _DataBaseSize=0;
    AString fmap(dir);
    fmap+=getname();
    fmap+=getid()==0?".0.map":".1.map";
    fstream fbin;
    struct stat statbuf_map;
    struct stat statbuf_dir;
    if(!stat((const char *)fmap,&statbuf_map) && !stat(dir,&statbuf_dir) &&
      statbuf_dir.st_mtime < statbuf_map.st_mtime ){
       fbin.open(fmap,ios::in);
       if(fbin){
         fbin>>_DataBaseSize;
         for(i=0;i<5;i++)_pDataBaseEntries[i]=new uinteger[_DataBaseSize];
         for(i=0;i<5;i++){
           for(int k=0;k<_DataBaseSize;k++){
             fbin>>_pDataBaseEntries[i][k];
           }
         }
         fbin.close();
       }
       else cerr <<"AMSTimeID::_fillDB-S-CouldNot open map file "<<fmap<<endl; 
       
    }
    else {
      AString fnam(getname());
      fnam+= getid()==0?".0":".1";
      _selectEntry=&fnam;
      dirent ** namelist;
      int nptr=scandir(dir,&namelist,&_select,NULL);     
      if(nptr){
        for(i=0;i<5;i++)_pDataBaseEntries[i]=new uinteger[nptr];
        for(i=0;i<nptr;i++) {
          int valid=0;
          int kvalid=0;
          for(int k=strlen((char*)fnam);k<namelist[i]->d_namlen-1;k++){
            if((namelist[i]->d_name)[k]=='.' )valid++;
            if((namelist[i]->d_name)[k]=='.')kvalid=k;
          }
          if(valid==1 && isdigit(namelist[i]->d_name[kvalid+1])){
            sscanf((namelist[i]->d_name)+kvalid+1,"%d",
                   _pDataBaseEntries[0]+_DataBaseSize);
            AString ffile(dir);
            ffile+=namelist[i]->d_name;
            fbin.open((const char *)ffile,ios::in);
            uinteger temp[3];
            if(fbin){
              fbin.seekg(fbin.tellg()+_Nbytes+sizeof(_CRC));
              fbin.read((char*)temp,3*sizeof(temp[0]));
              if(fbin.good()){
                _convert(temp,3);
                _pDataBaseEntries[1][_DataBaseSize]=temp[0];
                _pDataBaseEntries[2][_DataBaseSize]=temp[1];
                _pDataBaseEntries[3][_DataBaseSize]=temp[2];
                _DataBaseSize++;
              }
              fbin.close();
            }
            
          }
          free(namelist[i]);
        }
        // sort
        //AMSsortNAGa(_pDataBaseEntries[0],_DataBaseSize);
        
        uinteger **padd= new uinteger*[_DataBaseSize];
        uinteger *tmp=  new uinteger[_DataBaseSize];
#ifdef __AMSDEBUG__
        assert(padd!=NULL && tmp!=NULL);
#endif
        for(i=0;i<_DataBaseSize;i++){
          tmp[i]=_pDataBaseEntries[3][i];
          _pDataBaseEntries[4][i]=_pDataBaseEntries[2][i];
          padd[i]=tmp+i;
        }
        AMSsortNAG(padd,_DataBaseSize);
        AMSsortNAGa(_pDataBaseEntries[4],_DataBaseSize);
        for(i=0;i<4;i++){
          for(int k=0;k<_DataBaseSize;k++){
            tmp[k]=_pDataBaseEntries[i][k];
          }
          for(k=0;k<_DataBaseSize;k++){
            _pDataBaseEntries[i][k]=*(padd[k]);
          }
        }
        delete[] padd;
        delete[] tmp;
        
        free(namelist);
      }
      // Rewrite map file;
      fbin.open(fmap,ios::out|ios::trunc);
      if(fbin){
#ifdef __AMSDEBUG__
        cout <<"AMSTimeID::_fillDB-I-updating map file "<<fmap<<endl; 
#endif
        fbin<<_DataBaseSize<<endl;
        for(i=0;i<5;i++){
          for(int k=0;k<_DataBaseSize;k++){
            fbin<<_pDataBaseEntries[i][k]<<endl;
          }
        }
        char cmd[255];
        sprintf(cmd,"chmod g+w %s",(const char*)fmap);
        system(cmd);
        fbin.close();
      }
      else cerr <<"AMSTimeID::_fillDB-S-CouldNot update map file "<<fmap<<endl; 
      cout <<"AMSTimeID::_fillDB-I-"<<_DataBaseSize<<" entries found for TDV "
           <<getname()<<endl; 
    }
}

void AMSTimeID::_fillfromDB()
  {
#ifdef __DB__

    const integer S = 0;
    const integer I = 1;
    const integer B = 2;
    const integer E = 3;
    integer nobj = 0;
 
    lms -> GetNTDV(getname(), getid(), nobj);

    integer *pS = new integer[nobj];
    time_t  *pI = new time_t[nobj];
    time_t  *pB = new time_t[nobj];
    time_t  *pE = new time_t[nobj];

    lms -> GetAllTDV(getname(), getid(), pS, pI, pB, pE, nobj);
    if(nobj > 0) {
     for(int l=0; l<4; l++) _pDataBaseEntries[l]=new uinteger[nobj];
      for(int i=0; i<nobj; i++) {
       _pDataBaseEntries[S][i]= pS[i];
       _pDataBaseEntries[I][i]= pI[i];
       _pDataBaseEntries[B][i]= pB[i];
       _pDataBaseEntries[E][i]= pE[i];
      }
    }

    _DataBaseSize = nobj;

    if (pS) delete [] pS;
    if (pI) delete [] pI;
    if (pB) delete [] pB;
    if (pE) delete [] pE;

    cout <<"AMSTimeID::_fillfromDB-I-"<<nobj<<" entries found for TDV "
         <<getname()<<endl; 
#endif
}

integer AMSTimeID::readDB(integer reenter){

  integer rec = -1;
#ifdef __DB__

  time_t  I, B, E;
  integer S;

  rec =_getDBRecord(AMSEvent::gethead()->gettime());
  if (rec != -1) {
   S    = _pDataBaseEntries[0][rec];
   if (S > 0) {
    I    = _pDataBaseEntries[1][rec];
    B    = _pDataBaseEntries[2][rec];
    E    = _pDataBaseEntries[3][rec];
    uinteger* buff = new uinteger[S];
     int rstat = lms -> ReadTDV(getname(), getid(), I, B, E, buff);
     if (rstat) {
      CopyIn((uinteger*)buff);
      SetTime(I,B,E);
     }
    delete [] buff;
   } else {
     cout<<"AMSTimeID::readDB -W- TDV object with zero size"<<endl;
   }
  }
#endif
  return rec;
}


