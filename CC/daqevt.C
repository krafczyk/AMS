#include <daqevt.h>
#include <event.h>
#include <commons.h>
uinteger * DAQEvent::_Table=0;
const uinteger DAQEvent::CRC32=0x04c11db7;

const integer lsub=2;
DAQEvent::~DAQEvent(){
UPool.udelete(_pData);
}

void DAQEvent::shrink(){
UPool.udelete(_pData);
_pData=0;
_Length=0;
_pcur=0;
}

void DAQEvent::addsubdetector(pid pgetid, pgetput pget, pgetput pput,
                            pgetl pgl){
  DAQSubDet * p= new DAQSubDet(pgetid, pget, pput,  pgl);
  if(_pFirst){
    DAQSubDet *paux=_pFirst;
    while(paux->_next)paux=paux->_next;
    paux->_next=p; 
  }
  else _pFirst=p;
}

DAQSubDet * DAQEvent::_pFirst=0;


char * DAQEvent::ifnam=0;
char * DAQEvent::ofnam=0;
fstream DAQEvent::fbin;
fstream DAQEvent::fbout;


void DAQEvent::setfiles(char *ifile, char *ofile){
  if(ifile){
   ifnam=new char[strlen(ifile)+1];
   strcpy(ifnam,ifile);
  }
  if(ofile){
   ofnam=new char[strlen(ofile)+1];
   strcpy(ofnam,ofile);
  }
}


void DAQEvent::buildDAQ(){
DAQSubDet *fpl=_pFirst;
if(_pFirst == NULL){
  cerr << "DAQEvent::build-S-NoSubdetecors in DAQ"<<endl;
  return;
}
integer ntot=0;
while(fpl){
fpl->_sdid=_findid((fpl->_pgetid)());
if( fpl->_sdid < 0)
  cerr << "DAQEvent::build-S-Invalid Subdetector Id"<<(fpl->_pgetid)()<<endl;
else fpl->_length=(fpl->_pgetlength)();
ntot+=fpl->_length+lsub;
fpl=fpl->_next;
}


// Make array



const integer lheader=1+1+1+1+1+1+1;
_Length=1+lheader+ntot;

#ifdef __AMSDEBUG__
assert(sizeof(time_t) == sizeof(integer));
#endif

if(_create() ){
 *(_pData+1)=lheader;
 *(_pData+2)=0;    // Header ID = 0
 eventno()=AMSEvent::gethead()->getid();
 runno()=AMSEvent::gethead()->getrun();
 runtype()=AMSEvent::gethead()->getruntype();
 time()=AMSEvent::gethead()->gettime();
 crc()=0;
 _pcur=_pData+1+lheader;
 fpl=_pFirst;
while(fpl){
 if(fpl->_sdid >=0 ){
 *_pcur=fpl->_length+lsub;
 *(_pcur+1)=fpl->_sdid;
 uinteger* psafe=_pcur+lsub;
 fpl->_pgetdata(fpl->_length,psafe);
 _pcur=_pcur+*_pcur;
}
fpl=fpl->_next;
}
crc()=_makeCRC(); 
}
}

 
uinteger DAQEvent::_makeCRC(){

 _InitTable();
  int i,j,k;
  if(_pData){
   integer n=integer(_pData[0]);
   uinteger crc;
   if( n < 1) return 0;
     crc=~_pData[0];
     for(i=1;i<n;i++){
       for(j=0;j<3;j++)crc=_Table[crc>>24]^(crc<<8);
      crc=crc^_pData[i];
     }
   return ~crc;
  }
  else return 0;

}

integer DAQEvent::_CRCok(){
  if(_pData){
   uinteger oldcrc=crc();
   crc()=0;
   if(_makeCRC() != oldcrc){
#ifdef __AMSDEBUG__
     cerr <<
    "DAQEvent::testCRC-S-CRCFailed Was "<<oldcrc<<" Now "<<_makeCRC()<<endl;
#endif
     return 0;
   }
   else {
     crc()=oldcrc;
     return 1;    
   }
  }
  else return 0;
}


  
integer DAQEvent::_findid(AMSID id){
if(strcmp(id.getname(),"LVL1") ==0)return 1;
else if(strcmp(id.getname(),"LVL3") ==0)return 3;
else if(strcmp(id.getname(),"TOF") ==0)return 10;
else if(strcmp(id.getname(),"Anti") ==0)return 20;
else if(strcmp(id.getname(),"Tracker") ==0)return 30;
else if(strcmp(id.getname(),"CTC") ==0)return 40;
else return -1;


}


void DAQEvent::buildRawStructures(){
  if(_CRCok()){
   DAQSubDet * fpl=_pFirst;
   while(fpl){
   fpl->_sdid=_findid((fpl->_pgetid)());
   for(_pcur=_pData+1;_pcur < _pData+*(_pData);_pcur=_pcur+*_pcur){
    if(*(_pcur+1) == fpl->_sdid){
     uinteger *psafe=_pcur+lsub;
     fpl->_pputdata(*_pcur-lsub,psafe);
     break;
    }
   }
   fpl=fpl->_next; 
   }
  }
}


void DAQEvent::write(){

   _convert();
   fbout.write((unsigned char*)_pData,sizeof(_pData[0])*_Length);
   // Unfortunately we shoulf flush output for each event
   //
   fbout.flush();


}


integer DAQEvent::read(){
    if(fbin.good() && !fbin.eof()){
     fbin.read((unsigned char*)(&_Length),sizeof(_Length));
     if(fbin.good() && !fbin.eof()){
      _convertl();
      if(_create()){
       fbin.seekg(fbin.tellg()-sizeof(_pData[0]));
       fbin.read((unsigned char*)(_pData),sizeof(_pData[0])*(_Length));
       _convert();
      }
      else{
       fbin.seekg(fbin.tellg()+sizeof(_pData[0])*(_Length-1));
      }
     }
    }   
   return fbin.good() && !fbin.eof() ;
}





void DAQEvent::init(integer mode, integer format){
  enum open_mode{binary=0x80};
  if(mode%10 ==1 ){
    if(ifnam){
    fbin.open(ifnam,ios::in|binary);
    if(fbin){ 
    if(SELECTFFKEY.Run){
     DAQEvent daq;
     integer run=0;
     integer ok=1;
     integer iposr=0;
     while(ok){
      ok=daq.read();
      if(ok){
       iposr++;
         if(SELECTFFKEY.Event >=0 && daq.runno() == SELECTFFKEY.Run &&
         daq.eventno() >= SELECTFFKEY.Event)break;
         if(daq.runno() == SELECTFFKEY.Run && iposr ==
         -SELECTFFKEY.Event)break;
       run=daq.runno();
       daq.shrink();
      }
     }
     // pos back if fbin.good
     if(ok){
            fbin.seekg(fbin.tellg()-daq.getlength());
            cout<<"DAQEvent::init-I-Selected Run = "<<SELECTFFKEY.Run<<
              " Event = "<<daq.eventno()<< " Position = "<<iposr<<endl;

     }
     else {
       if(format==0){
        cerr <<"DAQEvent::init-F-Failed to select Run = "<<
        SELECTFFKEY.Run<<" Event >= "<<SELECTFFKEY.Event<<endl;
        exit(1);
       }
       else{
        cout <<"Total of "<<iposr<<" Events of Run " <<run<<
         " has been read."<<endl;
       }
     }

    }
    }
    else{
      cerr<<"DAQEvent::init-F-cannot open file "<<ifnam<<" in mode "<<mode<<endl;
      exit(1);
    }
    }
    else {
      cerr <<"DAQEvent::init-F-no input file to init "<<endl;
      exit(1);
    }
  }
  if(mode/10 ){
   if(ofnam){
    if(mode/10 ==1)fbout.open(ofnam,ios::out|binary|ios::noreplace);
    if(mode/10 ==2)fbout.open(ofnam,ios::out|binary|ios::app);
     if(fbout){ 
       // Associate buffer
      static char buffer[1000+1];
      fbout.setbuf(buffer,1000);

     }
     else{
      cerr<<"DAQEvent::init-F-cannot open file "<<ofnam<<" in mode "<<mode<<endl;
      exit(1);
     }
   }
   else {
      cerr <<"DAQEvent::init-F-no output file to init "<<endl;
      exit(1);
   }
  }
}


void DAQEvent::_convert(){
  if(AMSDBc::BigEndian){
   unsigned char tmp;
   unsigned char *pc = (unsigned char*)(_pData);
   int i;
   for(i=0;i<_Length;i++){
     tmp=*pc;
     *pc=*(pc+3);
     *(pc+3)=tmp;
     tmp=*(pc+1);
     *(pc+1)=*(pc+2);
     *(pc+2)=tmp;
     pc=pc+sizeof(uinteger);
   }
  }
}




void DAQEvent::_convertl(){
  if(AMSDBc::BigEndian){
   unsigned char tmp;
   unsigned char *pc = (unsigned char*)(&_Length);
     tmp=*pc;
     *pc=*(pc+3);
     *(pc+3)=tmp;
     tmp=*(pc+1);
     *(pc+1)=*(pc+2);
     *(pc+2)=tmp;
  }
}


void DAQEvent::_copyEl(){
   if(DAQCFFKEY.mode/10)write();


}


void DAQEvent::_InitTable(){
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



integer DAQEventI::_Count=0;
DAQEventI::DAQEventI(){
  _Count++;
}
DAQEventI::~DAQEventI(){
  if(--_Count==0){
    if(DAQEvent::fbin)DAQEvent::fbin.close();
    if(DAQEvent::fbout)DAQEvent::fbout.close();
  }
}

integer DAQEvent::_create(){
#ifdef __AMSDEBUG__
assert (_Length >0);
#endif
_pData= (uinteger*)UPool.insert(sizeof(_pData[0])*_Length);
if(_pData)_pData[0]=_Length;
return _pData != NULL ;
}
