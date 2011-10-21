//  $Id: timeid.C,v 1.132 2011/10/21 12:47:11 choutko Exp $
// 
// Feb 7, 1998. ak. do not write if DB is on
//
#ifdef _OPENMP
#include <omp.h>
#endif
#include "timeid.h"
#include "astring.h"
#include <fstream>
#include <sys/types.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/file.h>
#include "amsdbc.h"
#include "amsstl.h"
#include <cstdio>
#include <strstream>
#include <limits.h>
#if !defined(__CORBASERVER__) && !defined(__ROOTSHAREDLIBRARY__)
#include "commonsi.h"
#ifdef _OPENMP
#include "event.h"
#endif
#endif
#ifdef __CORBA__
#include "producer.h"
#endif
#if !defined(__CORBASERVER__) && !defined(__ROOTSHAREDLIBRARY__)
#include "ntuple.h"
#endif
#ifdef __ROOTSHAREDLIBRARY__
#include "root.h"
#endif
#include <dirent.h>



#ifdef __DB__
extern char *tdvNameTab[maxtdv];
extern int  tdvIdTab[maxtdv];
extern int  ntdvNames;

#include <dbS.h>

extern LMS* lms;

#endif


#ifdef _WEBACCESS_
#include "fopen.h"
int AMSTimeID::_WebAccess=0;
char AMSTimeID::_WebDir[200]="";
#endif
uinteger * AMSTimeID::_Table=0;
const uinteger AMSTimeID::CRC32=0x04c11db7;
AMSTimeID::AMSTimeID(AMSID  id, tm   begin, tm  end, integer nbytes, 
                     void *pdata, AMSTimeID::CType server,bool verify,trigfun_type fun):
  AMSNode(id),_pData((uinteger*)pdata),_UpdateMe(0),_verify(verify),_Type(server){
  _fname="";
   _trigfun=fun;

  _Nbytes=nbytes;
  _Nbytes+=sizeof(uinteger);



  if(_Type!=Client){
    _fillDB((const char*)AMSDBc::amsdatabase,0);
  }
  else{
    for( int i=0;i<5;i++) _pDataBaseEntries[i]=0;
    _DataBaseSize=0;
    cout<<"AMSTimeID::AMSTimeID-I-DataBaseSizeZeroed "<<id.getname()<<endl;
  }
#ifdef __AMSDEBUG__
  cout <<id.getname()<<" Size "<<_Nbytes<<endl;
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
  time(&_Insert);
  _CRC=_CalcCRC();
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


integer AMSTimeID::CopyIn(const void *pdata){
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

integer AMSTimeID::CopyOut  (void *pdata)const {
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


#ifndef __DB__
  int ok = readDB(AMSDBc::amsdatabase,Time,reenter);
#endif

#ifdef __DB__
  int ok = readDB();
#endif

  if (Time >= _Begin && Time <= _End){
    if(ok==-1 || _CRC == _CalcCRC()){

      return 1;
    }
    else {
      cerr<<"AMSTimeID::validate-F-CRC Error "<<getname()<<" Old CRC "
	  <<_CRC<<" New CRC "   <<_CalcCRC()<<endl;
      return 0; 
    }
    if(!reenter)return validate(Time,1);
    return 0;
  }
  return 0;
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

bool AMSTimeID::write(const char * dir, int slp){

  // add explicitely one second delay to prevent same insert time
  if(slp)sleep(1);
  if(_Type!=Client){
    enum open_mode{binary=0x80};
    fstream fbin;
again:
    AString fnam=dir;
    fnam+=getname();
    fnam+="/";
    fnam+=_getsubdirname(_Begin);
    fnam+="/";
    AString mkdir("mkdir -p ");
    mkdir+=fnam;
    system((const char*)mkdir);
    fnam+=getname();
    fnam+= getid()==0?".0":".1";
    char name[255];
    ostrstream ost(name,sizeof(name));
    ost << "."<<_Insert<<ends;
    fnam+=name;     
    //      }
    fbin.clear();
    fbin.open((const char *)fnam,ios::in);
    if(fbin){
        cerr <<"AMSTimeId::write-E-"<<fnam<<" exists "<<endl;
        _Insert++;
        fbin.close();
        goto again;
    }
    fbin.clear();
    fbin.open((const char *)fnam,ios::out|ios::trunc);
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
        cout <<"AMSTimeId::write-I-"<<fnam<<" written "<<_DataBaseSize<<endl;
	delete [] pdata;
	//  touch the directory

	AString buf("touch ");
	buf+=dir;
	buf+=getname();
	system((const char *)buf);     
	// now put the record intodb

	uinteger *ibe[5];
	for(int i=0;i<5;i++){
	  ibe[i]=new uinteger[_DataBaseSize+1];
	}
	for(int j=0;j<_DataBaseSize;j++){
	  ibe[0][j]=_pDataBaseEntries[0][j];
	  ibe[1][j]=_pDataBaseEntries[1][j];
	  ibe[2][j]=_pDataBaseEntries[2][j];
	  ibe[3][j]=_pDataBaseEntries[3][j];
	}
	ibe[0][_DataBaseSize]=_Insert;
	ibe[1][_DataBaseSize]=_Insert;
	ibe[2][_DataBaseSize]=_Begin;
	ibe[3][_DataBaseSize]=_End;
	fillDB(_DataBaseSize+1,ibe);

      
	updatemap(dir,true);       
	return fbin.good();
      }
      else{
	cerr<<"AMSTimeID::write-E-Failed to allocate memory "<<_Nbytes<<endl;
	return false;
      }
    }
    else {
      cerr<<"AMSTimeID::write-E-CouldNot open file "<<fnam<<endl;
    }

    return false;

  }
#ifdef __CORBA__
  else{
    return AMSProducer::gethead()->sendTDV(this);
  }
#endif
}


integer AMSTimeID::readDB(const char * dir, time_t asktime,integer reenter){

  // first get a id no from dbase
  if(asktime==0)asktime=(_Begin+_End)/2;
  integer index;
  integer id=_getDBRecord(asktime,index);
  if(id>0 && !reenter){
  }
  else if(id==0 || reenter){
    id=0;
  }
  else return -1;
  int ok;
  
  
#ifdef _OPENMP
#ifndef __ROOTSHAREDLIBRARY__
  cout <<" in barrier AMSTimeId::readDB-I-BarrierReachedFor "<<omp_get_thread_num()<<endl;
  AMSEvent::ResetThreadWait(1);
  AMSEvent::Barrier()=true;
#pragma omp barrier 
  if( omp_get_thread_num()==0) {
#else
if(1){
#endif
    ok= read(dir,id,asktime,index)?1:0;
#if !defined(__CORBASERVER__) && !defined(__ROOTSHAREDLIBRARY__)
     if(AMSNtuple::Get_setup02()){
        AMSNtuple::Get_setup02()->TDVRC_Add(asktime,this);
     }
     else{
       cerr<<"AMSTimeID::readDB-E-AMSNtuple::Getsetup02IsNull "<<endl;
     }
#endif
#ifdef __ROOTSHAREDLIBRARY__
if(AMSEventR::Head()  && AMSEventR::Head()->getsetup()){
        AMSEventR::Head()->getsetup()->TDVRC_Add(asktime,this);
}
#endif
    if(ok && _trigfun)_trigfun();

#ifndef __ROOTSHAREDLIBRARY__
    AMSEvent::Barrier()=false;
#endif
  }
  else ok=1;
#else
  
  if( read(dir,id,asktime,index)){
    if(_trigfun)_trigfun();
#if !defined(__CORBASERVER__) && !defined(__ROOTSHAREDLIBRARY__)
     if(AMSNtuple::Get_setup02()){
        AMSNtuple::Get_setup02()->TDVRC_Add(asktime,this);
     }
     else{
       cerr<<"AMSTimeID::readDB-E-AMSNtuple::Getsetup02IsNull "<<endl;
     }
#endif
#ifdef __ROOTSHAREDLIBRARY__
if(AMSEventR::Head()  && AMSEventR::Head()->getsetup()){
        AMSEventR::Head()->getsetup()->TDVRC_Add(asktime,this);
}
#endif
    return 1;
  }
  else 
    return 0;
  

#endif
#pragma omp barrier 
  return ok;
   
}

bool AMSTimeID::read(const char * dir,int run, time_t begin,int index){
#if !defined(__CORBASERVER__) && !defined(__ROOTSHAREDLIBRARY__)
  //   Add check remote client here
  ifstream fbin;
  fbin.open("/proc/self/where");
  if(fbin){
    int node;
    fbin>>node;
    if(node!=0){
      if(!AMSCommonsI::remote())cout <<"AMSTimeID::validate-I-Remote Client Detected "<<endl;
      AMSCommonsI::setremote(true);
    }
    else{
      AMSCommonsI::setremote(false); 
    }
  }
  else{
    AMSCommonsI::setremote(false); 
  }    
  fbin.close();

#endif

  if(_Type!=Client){
    enum open_mode{binary=0x80};
    fstream fbin;

    AString &fnam=_fname; 
    fnam="";
#ifdef _WEBACCESS_
    if(_WebAccess)
      fnam+=_WebDir;
    else
#endif
      fnam+=dir;

    if(run>0){
      fnam+=getname();
      fnam+="/";
      fnam+=_getsubdirname(begin);
      fnam+="/";
      fnam+=getname();
      fnam+= getid()==0?".0":".1";
      char name[255];
      ostrstream ost(name,sizeof(name));
      ost << "."<<run<<ends;
      fnam+=name;     
    }
    else{
      fnam+=".";
      fnam+=getname();
      fnam+= getid()==0?".0":".1";
      cout <<"AMSTimeID::read-W-Default value for TDV "<<getname()<<" will be used."<<endl;
    }
#ifdef _WEBACCESS_
    URL_FILE* ffbin=url_fopen((const char *)fnam,"r");
    if(ffbin){
#else
    fbin.open((const char *)fnam,ios::in);
    if(fbin){
#endif
      uinteger * pdata;
      integer ns=_Nbytes/sizeof(pdata[0])+3;
      pdata =new uinteger[ns];
      if(pdata){
#ifdef _WEBACCESS_
	int num=url_fread((char*)pdata,sizeof(pdata[0]),ns,ffbin);
	if(num){
#else
	fbin.read((char*)pdata,ns*sizeof(pdata[0]));
	if(fbin.good()){
#endif
	  _convert(pdata,ns);
	  {
	    CopyIn(pdata);
	    _Insert=time_t(pdata[_Nbytes/sizeof(pdata[0])]);
	    _Begin=time_t(pdata[_Nbytes/sizeof(pdata[0])+1]);
	    _End=time_t(pdata[_Nbytes/sizeof(pdata[0])+2]);
	    //       if(dflt)_getDefaultEnd(asktime,_End);
	    if(run==0){
	      _Insert=1;
	      _Begin=1;
	      _End=INT_MAX-1;
	    }
	  }
	  cout <<"AMSTimeID::read-I-Open file "<<fnam<<endl;
#ifdef __AMSDEBUG__
	  cout <<"AMSTimeID::read-I-Insert "<<ctime(&_Insert)<<endl;
	  cout <<"AMSTimeID::read-I-Begin "<<ctime(&_Begin)<<endl;
	  cout <<"AMSTimeID::read-I-End "<<ctime(&_End)<<endl;
#endif
#ifdef _WEBACCESS_
	  url_fclose(ffbin);
#else
	  fbin.close();
#endif
	  delete [] pdata;
	  return true;
	}
	else {
	  cerr<<"AMSTimeID::read-E-Problems to Read File "<<fnam<<" size declared "<<ns<<endl;
#ifdef _WEBACCESS_
	  url_fclose(ffbin);
#else
	  fbin.close();
#endif
	  delete [] pdata;
	  return false;
	}
      }
      else {
	cerr<<"AMSTimeID::read-E-Failed to allocate memory "<<_Nbytes<<endl;
	return false;
      }
    }
    else {
      cerr<<"AMSTimeID::read-E-CouldNot open file "<<fnam<<endl;
      return false;
    }
    return true;

  }
#ifdef __CORBA__
  else{
    if(run>0){
      _Insert=_pDataBaseEntries[1][index];
      _Begin=_pDataBaseEntries[2][index];
      _End=_pDataBaseEntries[3][index];
    }
    //return AMSProducer::gethead()->getTDV(this,run);
    return AMSProducer::gethead()->getSplitTDV(this,run);
  }
#endif
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
integer AMSTimeID::_getDBRecord(time_t & atime, int & rec){
  uinteger time(atime);
  integer index=AMSbiel(_pDataBaseEntries[3],time,_DataBaseSize);
  //cout <<getname()<<" "<<index<<" "<<time<<" "<<_pDataBaseEntries[3][index]<<" "<<_DataBaseSize<<endl;
  rec=-1;
  int insert= (time>=_Begin && time<=_End)?_Insert:0;
  for (int i=index<0?_DataBaseSize:index;i<_DataBaseSize;i++){
    if(time>=_pDataBaseEntries[2][i] && insert<_pDataBaseEntries[1][i]){
      insert=     _pDataBaseEntries[1][i];
      rec=i;
    }
  }

#ifndef __DB__
  if(rec>=0)atime=_pDataBaseEntries[2][rec];
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

#ifdef __DARWIN__
integer AMSTimeID::_select(  dirent *entry)
#endif
#ifdef __LINUXGNU__
integer AMSTimeID::_select(  const dirent64 *entry)
#endif
{
  return strstr(entry->d_name,(const char*)*_selectEntry)!=NULL;    
}

#ifdef __DARWIN__
integer AMSTimeID::_selectsdir( dirent *entry)
#endif
#ifdef __LINUXGNU__
integer AMSTimeID::_selectsdir(  const dirent64 *entry)
#endif
{
  return (entry->d_name)[0] != '.';   
}

void AMSTimeID::rereaddb(bool force){
if(_Type!=Server){
  for(int i=0;i<5;i++)delete _pDataBaseEntries[i]; 
  _fillDB((const char*)AMSDBc::amsdatabase,0,force); 
}
else{
  _fillDBServer((const char*)AMSDBc::amsdatabase,0,force); 
}
}

time_t AMSTimeID::_stat_adv(const char *dir){
  struct stat64 statbuf_dir;
  time_t tm=0;
  if(stat64 (dir,&statbuf_dir)){
    time(&tm);
    return tm;
  } 
  if(statbuf_dir.st_mtime>tm){
    tm=statbuf_dir.st_mtime;
  }
#ifdef __LINUXGNU__
  dirent64 ** namelistsubdir;
  int nptrdir=scandir64(dir,&namelistsubdir,NULL,NULL);
#endif
#ifdef __DARWIN__
  dirent ** namelistsubdir;
  int nptrdir=scandir(dir,&namelistsubdir,NULL,NULL);
#endif

  for(int is=0;is<nptrdir;is++){
    AString fsdir(dir);
    fsdir+=namelistsubdir[is]->d_name;
    if(namelistsubdir[is]->d_name[0]!= '.' && !stat64 ((const char*)fsdir,&statbuf_dir)){
      if(S_ISDIR(statbuf_dir.st_mode)){
	//          cout <<"  dirs "<<fsdir<<endl;
	if(statbuf_dir.st_mtime>tm){
          tm=statbuf_dir.st_mtime;
	}
        
      }
    }
    free( namelistsubdir[is]);
  }
  if(nptrdir>0){
    free (namelistsubdir);
  }
  return tm;
}
void AMSTimeID::_fillDB(const char *dir, int reenter, bool force){
  bool zero=false;
  int everythingok=1;
  int i;
  for( i=0;i<5;i++)_pDataBaseEntries[i]=0;
  _DataBaseSize=0;
   cout <<"AMSTimeId::_fillDB-I-DataBaseSizeZeroed"<<getname()<<endl;
  AString fmap("");
  AString fdir("");
#ifdef _WEBACCESS_
  if(_WebAccess){
    fmap+=_WebDir;
    fdir+=_WebDir;
  }else
#endif
    {
      fmap+=dir;
      fdir+=dir;
    }
  fdir+=getname();
  fdir+="/";
  fmap+=".";
  fmap+=getname();
  fmap+=getid()==0?".0.map":".1.map";
  fstream fbin;
  struct stat64 statbuf_map;
  time_t mtime=0;
#ifdef _WEBACCESS_
  if(!_WebAccess) 
#endif
    mtime=_stat_adv((const char*)fdir);
  //    cout <<"  fdir "<<fdir<<" " <<mtime<<endl;
  //    if((!stat((const char *)fmap,&statbuf_map) && 
  //        !stat((const char *)fdir,&statbuf_dir) &&
  //              statbuf_dir.st_mtime < statbuf_map.st_mtime) && !force ){

#ifdef _WEBACCESS_
  if(
     !url_stat((const char *)fmap,&statbuf_map) &&
     ( (mtime < statbuf_map.st_mtime && !force) || _WebAccess )
     ){
    
    char buf[100];
    URL_FILE* ffbin=url_fopen(fmap,"r");
      if(ffbin){
      url_fgets(buf,100,ffbin);
      _DataBaseSize=atoi(buf);
#else
    if((!stat64((const char *)fmap,&statbuf_map)&&
	mtime < statbuf_map.st_mtime) && !force){
usemap:    
      char buf[100];
      fbin.clear();
      fbin.close();
      fbin.open(fmap,ios::in);
      if(fbin){
	fbin>>_DataBaseSize;
        cout <<"AMSTimeId::_fillDB-I-DataBaseSizeReadFromMap"<<_DataBaseSize<<" "<<getname()<<endl;
#endif
      zero=false;
      if(!_DataBaseSize){
         fbin.close();
         cout<<"AMSTimeID::_fillDB-W-MapHasZeroEnrtries "<<(const char *)fmap<<endl;
         zero=true;
#ifdef   __CORBASERVER__     
          
         goto notrust;
#endif
     }
         cout<<"AMSTimeID::_fillDB-I-MapFilled "<<(const char *)fmap<<" "<<_DataBaseSize<<endl;

      for(i=0;i<5;i++)_pDataBaseEntries[i]=new uinteger[_DataBaseSize];
      for(i=0;i<5;i++)
	for(int k=0;k<_DataBaseSize;k++){
	  uinteger tmp;
#ifdef _WEBACCESS_
	  url_fgets(buf,100,ffbin);
	  tmp=atoi(buf);
#else
	  fbin>>tmp;
#endif
	  _pDataBaseEntries[i][k]=tmp;      
	}
#ifdef _WEBACCESS_     	  
      url_fclose(ffbin);
#else
      fbin.close();
#endif
    }
    else cerr <<"AMSTimeID::_fillDB-S-CouldNot open map file "<<fmap<<endl; 
    
  }
  else if(_verify) {
    notrust:
    cout <<"AMSTimeID::_fillDB-I-UpdatingDataBase for"<<(const char *)fmap<<" "<<_verify<<endl;
    //    Check if is in new mode
    _checkcompatibility(dir);
    AString fnam(getname());
    fnam+= getid()==0?".0":".1";
    _selectEntry=&fnam;
    int size=0;
	  for(int k=0;k<5;k++){
	    if(_DataBaseSize)delete[] _pDataBaseEntries[k];
          }
          _DataBaseSize=0;
          cout <<"AMSTimeId::_fillDB-I-DataBaseSizeZeroedAgain "<<getname()<<endl;

#ifdef __LINUXGNU__
    dirent64 ** namelistsubdir;
    int nptrdir=scandir64((const char *)fdir,&namelistsubdir,_selectsdir,NULL);
#endif
#ifdef __DARWIN__
    dirent ** namelistsubdir;
    int nptrdir=scandir((const char *)fdir,&namelistsubdir,_selectsdir,NULL);
#endif

    for(int is=0;is<nptrdir;is++){
      AString fsdir(fdir);
      fsdir+=namelistsubdir[is]->d_name;
      fsdir+="/";     
#ifdef __LINUXGNU__
      dirent64 ** namelist;
      int nptr=scandir64((const char *)fsdir,&namelist,&_select,NULL);     
#endif
#ifdef __DARWIN__
      dirent ** namelist;
      int nptr=scandir((const char *)fsdir,&namelist,&_select,NULL);     
#endif

      if(nptr>0){
//           cout <<"  dbsize "<<_DataBaseSize<<" "<<size<<" "<<nptr<<" "<<fsdir<<endl;
	if(_DataBaseSize && size<_DataBaseSize+nptr){
	  uinteger *tmp=new uinteger[_DataBaseSize];
	  for(int k=0;k<5;k++){
	    int l;
	    for ( l=0;l<_DataBaseSize;l++)tmp[l]=_pDataBaseEntries[k][l]; 
	    delete[] _pDataBaseEntries[k];
	    size=2*(_DataBaseSize+nptr);
            cout<<" AMSTimeID::_fillDB-I-UpdatingDataBase Size "<<size<<" "<<_DataBaseSize<<endl;
	    _pDataBaseEntries[k]=new uinteger[size];
	    for ( l=0;l<_DataBaseSize;l++)_pDataBaseEntries[k][l]=tmp[l]; 
	  }
	  delete[] tmp;
	} 
	else if(!_DataBaseSize){
	  size=2*nptr;
	  for(i=0;i<5;i++)_pDataBaseEntries[i]=new uinteger[size];
	}
	for(i=0;i<nptr;i++) {
	  int valid=0;
	  int kvalid=0;
	  for(int k=strlen((const char*)fnam);k<strlen(namelist[i]->d_name);k++){
	    if((namelist[i]->d_name)[k]=='.' )valid++;
	    if((namelist[i]->d_name)[k]=='.')kvalid=k;
	  }
	  if(valid==1 && isdigit(namelist[i]->d_name[kvalid+1])){
	    sscanf((namelist[i]->d_name)+kvalid+1,"%d",
		   _pDataBaseEntries[0]+_DataBaseSize);
	    AString ffile(fsdir);
	    ffile+=namelist[i]->d_name;
            ifstream ffbin;
	    ffbin.close();
	    ffbin.clear();
	    ffbin.open((const char *)ffile);
	    uinteger temp[3];
	    if(ffbin){
	      ffbin.seekg(integer(ffbin.tellg())+_Nbytes);
	      ffbin.read((char*)temp,3*sizeof(temp[0]));
	      if(ffbin.good()){
		_convert(temp,3);
		_pDataBaseEntries[1][_DataBaseSize]=temp[0];
		_pDataBaseEntries[2][_DataBaseSize]=temp[1];
		_pDataBaseEntries[3][_DataBaseSize]=temp[2];
		_DataBaseSize++;
		if(strcmp(namelistsubdir[is]->d_name,_getsubdirname(temp[1]))){
		  everythingok=0;
		  cerr<<"AMSTimeID::_fillDB-W-Dir/FileNameAreInconsistent "<<ffile<<" "<<"Recovering"<<endl;
		  ffbin.close();
		  _rewrite(dir,ffile);
		}
	      }
	      ffbin.close();
	    }
            
	  }
	  free(namelist[i]);
	}
	free(namelist);
        
      }
      free( namelistsubdir[is]);
    }
    if(nptrdir>0){
      free (namelistsubdir);
      // sort
      //AMSsortNAGa(_pDataBaseEntries[0],_DataBaseSize);
      uinteger **padd= new uinteger*[_DataBaseSize+1];
      uinteger *tmp=  new uinteger[_DataBaseSize+1];
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
	int k;
	for(k=0;k<_DataBaseSize;k++){
	  tmp[k]=_pDataBaseEntries[i][k];
	}
	for(k=0;k<_DataBaseSize;k++){
	  _pDataBaseEntries[i][k]=*(padd[k]);
	}
      }
      delete[] padd;
      delete[] tmp;
    }
    // Rewrite map file;

    if(!updatemap(dir,true)){
       cerr <<"AMSTimeID::_fillDB-S-CouldNot update map file "<<fmap<<endl; 
#ifdef __CORBASERVER__
       if(!_DataBaseSize && !zero){
       cerr <<"AMSTimeID::_fillDB-W-using old map file "<<fmap<<endl; 
       goto usemap;          
       }
#endif
    }   




    cout <<"AMSTimeID::_fillDB-I-"<<_DataBaseSize<<" entries found for TDV "
	 <<getname()<<" ok "<<everythingok<<endl; 
  }
  if( !everythingok){
    for( i=0;i<5;i++)delete _pDataBaseEntries[i];
    if(!reenter)_fillDB(dir,1);
    else {
      cerr<<"AMSTimeID::_fillDB-F-CouldnotUpdataDatabase,Exiting"<<endl;
      exit(1);
    }      
  }
}


void AMSTimeID::_fillDBServer(const char *dir, int reenter, bool force){
  bool zero=false;
  int everythingok=1;
  AString fmap("");
  AString fdir("");
    {
      fmap+=dir;
      fdir+=dir;
    }
  fdir+=getname();
  fdir+="/";
  fmap+=".";
  fmap+=getname();
  fmap+=getid()==0?".0.map":".1.map";
  fstream fbin;
  struct stat64 statbuf_map;
  time_t mtime=0;
again:
    mtime=_stat_adv((const char*)fdir);
    if((!stat64((const char *)fmap,&statbuf_map)&&
	mtime < statbuf_map.st_mtime) && !force){
usemap:    
      char buf[100];
      fbin.clear();
      fbin.close();
      fbin.open(fmap,ios::in);
      unsigned int dbs;
      if(fbin){
	fbin>>dbs;
        if(!fbin.good()){
         cout <<"AMSTimeId::_fillDBServer-E-UnableReadDataBaseSizeFromMap"<<getname()<<" "<<fmap<<endl;
         if(!_DataBaseSize){
           fbin.close();
           force=true;
           goto again;
        }
        else{
         updatemap(dir,true);
         return;
        }
        }
        else if(dbs<_DataBaseSize){
         cerr <<"AMSTimeId::_fillDBServer-E-DataBaseSizeShrinked "<<getname()<<" "<<fmap<<" "<<statbuf_map.st_mtime<<" "<<dbs<<" "<<_DataBaseSize<<endl;
        updatemap(dir,true);
        return;
        }
        _DataBaseSize=dbs;
   for(int i=0;i<5;i++){
         delete _pDataBaseEntries[i]; 
        _pDataBaseEntries[i]=0; 
   }
        cout <<"AMSTimeId::_fillDBServer-I-DataBaseSizeReadFromMap"<<_DataBaseSize<<" "<<getname()<<" "<<statbuf_map.st_mtime<<endl;
      zero=false;
      if(!_DataBaseSize){
         fbin.close();
         cout<<"AMSTimeID::_fillDBServer-W-MapHasZeroEnrtries "<<(const char *)fmap<<endl;
         zero=true;
          
         goto notrust;
     }
         cout<<"AMSTimeID::_fillDBServer-I-MapFilled "<<(const char *)fmap<<" "<<_DataBaseSize<<endl;

      for(int i=0;i<5;i++)_pDataBaseEntries[i]=new uinteger[_DataBaseSize];
      for(int i=0;i<5;i++){
	for(int k=0;k<_DataBaseSize;k++){
	  uinteger tmp;
	  fbin>>tmp;
	  _pDataBaseEntries[i][k]=tmp;      
	}
       }
      fbin.close();
    }
    else cerr <<"AMSTimeID::_fillDBServer-S-CouldNot open map file "<<fmap<<endl; 
    
  }
  else {
    notrust:
    cout <<"AMSTimeID::_fillDBServer-I-UpdatingDataBase for"<<(const char *)fmap<<endl;
    //    Check if is in new mode
    _checkcompatibility(dir);
    AString fnam(getname());
    fnam+= getid()==0?".0":".1";
    _selectEntry=&fnam;
    int size=0;
	  for(int k=0;k<5;k++){
	    if(_DataBaseSize){
              delete[] _pDataBaseEntries[k];
              _pDataBaseEntries[k]=0;
            }           
          }
          _DataBaseSize=0;

#ifdef __LINUXGNU__
    dirent64 ** namelistsubdir;
    int nptrdir=scandir64((const char *)fdir,&namelistsubdir,_selectsdir,NULL);
#endif
#ifdef __DARWIN__
    dirent ** namelistsubdir;
    int nptrdir=scandir((const char *)fdir,&namelistsubdir,_selectsdir,NULL);
#endif

    for(int is=0;is<nptrdir;is++){
      AString fsdir(fdir);
      fsdir+=namelistsubdir[is]->d_name;
      fsdir+="/";     
#ifdef __LINUXGNU__
      dirent64 ** namelist;
      int nptr=scandir64((const char *)fsdir,&namelist,&_select,NULL);     
#endif
#ifdef __DARWIN__
      dirent ** namelist;
      int nptr=scandir((const char *)fsdir,&namelist,&_select,NULL);     
#endif

      if(nptr>0){
//           cout <<"  dbsize "<<_DataBaseSize<<" "<<size<<" "<<nptr<<" "<<fsdir<<endl;
	if(_DataBaseSize && size<_DataBaseSize+nptr){
	  uinteger *tmp=new uinteger[_DataBaseSize];
	  for(int k=0;k<5;k++){
	    int l;
	    for ( l=0;l<_DataBaseSize;l++)tmp[l]=_pDataBaseEntries[k][l]; 
	    delete[] _pDataBaseEntries[k];
	    size=2*(_DataBaseSize+nptr);
            cout<<" AMSTimeID::_fillDBServer-I-UpdatingDataBase Size "<<size<<" "<<_DataBaseSize<<endl;
	    _pDataBaseEntries[k]=new uinteger[size];
	    for ( l=0;l<_DataBaseSize;l++)_pDataBaseEntries[k][l]=tmp[l]; 
	  }
	  delete[] tmp;
	} 
	else if(!_DataBaseSize){
	  size=2*nptr;
	  for(int i=0;i<5;i++)_pDataBaseEntries[i]=new uinteger[size];
	}
	for(int i=0;i<nptr;i++) {
	  int valid=0;
	  int kvalid=0;
	  for(int k=strlen((const char*)fnam);k<strlen(namelist[i]->d_name);k++){
	    if((namelist[i]->d_name)[k]=='.' )valid++;
	    if((namelist[i]->d_name)[k]=='.')kvalid=k;
	  }
	  if(valid==1 && isdigit(namelist[i]->d_name[kvalid+1])){
	    sscanf((namelist[i]->d_name)+kvalid+1,"%d",
		   _pDataBaseEntries[0]+_DataBaseSize);
	    AString ffile(fsdir);
	    ffile+=namelist[i]->d_name;
            ifstream ffbin;
	    ffbin.close();
	    ffbin.clear();
	    ffbin.open((const char *)ffile);
	    uinteger temp[3];
	    if(ffbin){
	      ffbin.seekg(integer(ffbin.tellg())+_Nbytes);
	      ffbin.read((char*)temp,3*sizeof(temp[0]));
	      if(ffbin.good()){
		_convert(temp,3);
		_pDataBaseEntries[1][_DataBaseSize]=temp[0];
		_pDataBaseEntries[2][_DataBaseSize]=temp[1];
		_pDataBaseEntries[3][_DataBaseSize]=temp[2];
		_DataBaseSize++;
		if(strcmp(namelistsubdir[is]->d_name,_getsubdirname(temp[1]))){
		  everythingok=0;
		  cerr<<"AMSTimeID::_fillDB-W-Dir/FileNameAreInconsistent "<<ffile<<" "<<"Recovering"<<endl;
		  ffbin.close();
		  _rewrite(dir,ffile);
		}
	      }
	      ffbin.close();
	    }
            
	  }
	  free(namelist[i]);
	}
	free(namelist);
        
      }
      free( namelistsubdir[is]);
    }
    if(nptrdir>0){
      free (namelistsubdir);
      // sort
      //AMSsortNAGa(_pDataBaseEntries[0],_DataBaseSize);
      uinteger **padd= new uinteger*[_DataBaseSize+1];
      uinteger *tmp=  new uinteger[_DataBaseSize+1];
#ifdef __AMSDEBUG__
      assert(padd!=NULL && tmp!=NULL);
#endif
      for(int i=0;i<_DataBaseSize;i++){
	tmp[i]=_pDataBaseEntries[3][i];
	_pDataBaseEntries[4][i]=_pDataBaseEntries[2][i];
	padd[i]=tmp+i;
      }
      AMSsortNAG(padd,_DataBaseSize);
      AMSsortNAGa(_pDataBaseEntries[4],_DataBaseSize);
      for(int i=0;i<4;i++){
	int k;
	for(k=0;k<_DataBaseSize;k++){
	  tmp[k]=_pDataBaseEntries[i][k];
	}
	for(k=0;k<_DataBaseSize;k++){
	  _pDataBaseEntries[i][k]=*(padd[k]);
	}
      }
      delete[] padd;
      delete[] tmp;
    }
    // Rewrite map file;

    if(!updatemap(dir,true)){
       cerr <<"AMSTimeID::_fillDBServer-S-CouldNot update map file "<<fmap<<endl; 
       if(!_DataBaseSize && !zero){
       cerr <<"AMSTimeID::_fillDBServer-W-using old map file "<<fmap<<endl; 
       goto usemap;          
       }
    }   




    cout <<"AMSTimeID::_fillDBServer-I-"<<_DataBaseSize<<" entries found for TDV "
	 <<getname()<<" ok "<<everythingok<<endl; 
  }
  if( !everythingok){
    if(!reenter)_fillDBServer(dir,1);
    else {
      cerr<<"AMSTimeID::_fillDBServer-E-CouldnotUpdataDatabase,Exiting"<<endl;
      return;
    }      
  }
}


char* AMSTimeID::_getsubdirname(time_t begin){
  static char  _buf[32];
  tm * tm1=localtime(&begin);
  int tzz=timezone;
  tm * tmp=gmtime(&begin);
  tmp->tm_hour=0;
  tmp->tm_min=0;
  tmp->tm_sec=0;
  sprintf(_buf,"%d",mktime(tmp)-3600-tzz);
  return _buf;
}

void AMSTimeID::_checkcompatibility(const char *dir){
  fstream fbin;
  AString fdir(dir);
  fdir+=getname();
  fdir+="/";
  AString fnam(getname());
  fnam+= getid()==0?".0":".1";
  _selectEntry=&fnam;

#ifdef __LINUXGNU__
  dirent64 ** namelist;
  int nptr=scandir64((const char *)fdir,&namelist,&_select,NULL);     
#endif
#ifdef __DARWIN__
  dirent ** namelist;
  int nptr=scandir((const char *)fdir,&namelist,&_select,NULL);     
#endif

  if(nptr>0){
    cerr <<"AMSTimeID::_checkcompatibility-W-OldStructureFound"<<endl;
    char y;
    //       cout <<" Do you want to   convert  "<<getname();
    //       cin >>y;
    y='y';
    if(y!='y' &&  y!='Y'){
      cerr<<"AMSTimeID::_checkcompatibility-F-CanNotContinue,Aborting"<<endl;
      abort();
    }
    int i; 
    for(i=0;i<5;i++)_pDataBaseEntries[i]=new uinteger[nptr];
    for(i=0;i<nptr;i++) {
      int valid=0;
      int kvalid=0;
      for(int k=strlen((const char*)fnam);k<strlen(namelist[i]->d_name);k++){
	if((namelist[i]->d_name)[k]=='.' )valid++;
	if((namelist[i]->d_name)[k]=='.')kvalid=k;
      }
      if(valid==1 && isdigit(namelist[i]->d_name[kvalid+1])){
	sscanf((namelist[i]->d_name)+kvalid+1,"%d",
	       _pDataBaseEntries[0]+_DataBaseSize);
	AString ffile(fdir);
	ffile+=namelist[i]->d_name;
	_rewrite(dir,ffile);
      }
      free(namelist[i]);
    }
        
    free(namelist);
    for(i=0;i<5;i++)delete[] _pDataBaseEntries[i];
  }


}

void AMSTimeID::_rewrite(const char *dir, AString & ffile){
  ifstream fbin;
  fbin.open((const char *)ffile,ios::in);
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
	cout <<"AMSTimeID::read-I-Open file "<<ffile<<endl;
#ifdef __AMSDEBUG__
	cout <<"AMSTimeID::read-I-Insert "<<ctime(&_Insert)<<endl;
	cout <<"AMSTimeID::read-I-Begin "<<ctime(&_Begin)<<endl;
	cout <<"AMSTimeID::read-I-End "<<ctime(&_End)<<endl;
#endif
	bool ok=write(dir,0);
	fbin.close();
	delete [] pdata;
	AString rm("rm -f ");
	rm+=ffile;
	if(ok)system((const char *)rm);
      }
      else {
	cout<<"AMSTimeID::read-W-Problems to Read File "<<
	  ffile<<" size declared "<<ns<<endl;//tempor
	fbin.close();
	delete [] pdata;
      }
    }
  }
            


}

void AMSTimeID::checkupdate(const char * tdvc){

  if( (
       strcmp(tdvc,getname())==0 ||
       strcmp(tdvc,"UpdateAllTDV")==0  || (strstr(getname(),tdvc)!=0 && strlen(tdvc)>1)) &&
      (_Begin < _End) ){
    _UpdateMe=1;
    time(&_Insert);
    cout <<"AMSTimeID-ctor-I-Update for "<<getname()<<" "<<getid()<<
      " requested. "<<endl;
    if(_Begin >= _End){
      cout <<" But validity period is zero. Request rejected. "<<endl;
      _UpdateMe=0;
    }
    time_t linuxcrazy=_Begin;
    cout <<" Begin "<<ctime(&linuxcrazy);
    linuxcrazy=_End;
    cout <<" End "<<ctime(&linuxcrazy);
    linuxcrazy=_Insert;
    cout << " Insert "<<ctime(&linuxcrazy)<<endl;
  }
}

void AMSTimeID::fillDB (int length, uinteger *ibe[5]){
  if(length==0)return;
  cout<<" AMSTimeID::fillDB-I-RedefiningDataBaseSize "<<_DataBaseSize<<" "<<length<<endl;
  _DataBaseSize=length;
  for(int  i=0;i<5;i++){
    delete _pDataBaseEntries[i];
    _pDataBaseEntries[i]=ibe[i];

  }

  uinteger **padd= new uinteger*[_DataBaseSize+1];
  uinteger *tmp=  new uinteger[_DataBaseSize+1];
  for(int i=0;i<_DataBaseSize;i++){
    tmp[i]=_pDataBaseEntries[3][i];
    _pDataBaseEntries[4][i]=_pDataBaseEntries[2][i];
    padd[i]=tmp+i;
  }
  AMSsortNAG(padd,_DataBaseSize);
  AMSsortNAGa(_pDataBaseEntries[4],_DataBaseSize);
  for(int i=0;i<4;i++){
    int k;
    for(k=0;k<_DataBaseSize;k++){
      tmp[k]=_pDataBaseEntries[i][k];
    }
    for(k=0;k<_DataBaseSize;k++){
      _pDataBaseEntries[i][k]=*(padd[k]);
    }
  }
  delete[] padd;
  delete[] tmp;
}





AMSTimeID::IBE & AMSTimeID::findsubtable(time_t begin, time_t end){
  if(_ibe.size())_ibe.clear();
  int insert=0;
  iibe a;
  for (int i=0;i<_DataBaseSize;i++){
    a.id=_pDataBaseEntries[0][i];
    a.insert=_pDataBaseEntries[1][i];
    a.begin=_pDataBaseEntries[2][i];
    a.end=_pDataBaseEntries[3][i];
    if(a.begin<end && a.end>begin){
      if(a.begin<begin && a.end>end){
	if(a.insert>insert){
	  if(insert){
	    _ibe.erase(_ibe.begin());
	  }
	  _ibe.push_front(a);
	  insert=a.insert;
	}
      }
      else _ibe.push_back(a);
    }
  }
#ifdef __AMSDEBUG__
  cout <<" AMSTimeID::findsubtable-I-"<<_ibe.size()<<" Entries Found "<<endl;
#endif
  return _ibe;
}



bool AMSTimeID::updatemap(const char *dir,bool slp){
  if(slp)sleep(1);
  AString fmap(dir);
  fmap+=".";
  fmap+=getname();
  fmap+=getid()==0?".0.map":".1.map";
  AString fmaptmp=fmap;
  if(!_DataBaseSize){
#ifdef __CORBASERVER__
    cout <<"AMSTimeID::_fillDB-W-CowardlyRefusesToupdate map file "<<fmaptmp<<" "<<_DataBaseSize<<endl; 
   return false;
#endif
  }
  char utmp[80];
  time_t timet;
  time(&timet);
  sprintf(utmp,".%d",timet);
  fmaptmp+=utmp;
  fstream fbin;
  unlink(fmaptmp);
  fbin.clear();
  fbin.open(fmaptmp,ios::out);
  if(fbin){
    cout <<"AMSTimeID::updatemap-I-updating map file "<<fmaptmp<<" "<<_DataBaseSize<<endl; 
    fbin<<_DataBaseSize<<endl;
    for(int i=0;i<5;i++){
      for(int k=0;k<_DataBaseSize;k++){
	fbin<<_pDataBaseEntries[i][k]<<endl;
      }
    }
    fbin.close();
    char cmd[1255];
    sprintf(cmd,"mv %s %s",(const char *)fmaptmp,(const char*)fmap);
       system(cmd);
    sprintf(cmd,"chmod g+w %s",(const char*)fmap);
    system(cmd);
    return true;
  }
  else{
    cerr <<"AMSTimeID::_fillDB-E-Unable to open  map file "<<fmaptmp<<" "<<_DataBaseSize<<endl; 
     return false;
  }
}

bool AMSTimeID::updatedb(){
  cout <<"AMSTimeID::updatedb-I-Size "<<_DataBaseSize<<endl;
  uinteger *tmpa[5];
  int i;
  for( i=0;i<5;i++){
    tmpa[i]=new uinteger[_DataBaseSize+1];
    for(int j=0;j<_DataBaseSize;j++)tmpa[i][j]=_pDataBaseEntries[i][j];
    delete [] _pDataBaseEntries[i];
    _pDataBaseEntries[i]=tmpa[i];
  }
  _pDataBaseEntries[0][_DataBaseSize]=_Insert;
  _pDataBaseEntries[1][_DataBaseSize]=_Insert;
  _pDataBaseEntries[2][_DataBaseSize]=_Begin;
  _pDataBaseEntries[3][_DataBaseSize]=_End;
  _DataBaseSize++;
  cout <<"AMSTimeID::updatedb-I-SizeGrow "<<_DataBaseSize<<endl;
       
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
    int k;
    for(k=0;k<_DataBaseSize;k++){
      tmp[k]=_pDataBaseEntries[i][k];
    }
    for(k=0;k<_DataBaseSize;k++){
      _pDataBaseEntries[i][k]=*(padd[k]);
    }
  }
  delete[] padd;
  delete[] tmp;
  return true;
}
