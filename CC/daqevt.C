#include <stdio.h>

#include <daqevt.h>
#include <event.h>
#include <commons.h>

integer DAQEvent::_Buffer[512];
integer DAQEvent::_BufferLock=0;
const integer lover=2;
DAQEvent::~DAQEvent(){
shrink();
}

void DAQEvent::shrink(){

if(_pData && !_BufferOwner)UPool.udelete(_pData);
if(_BufferOwner)_BufferLock=0;
_pData=0;
_Length=0;
_pcur=0;
_Event=0;
_Run=0;
_RunType=0;
_Time=0;
_Checked=0;
_BufferOwner=0;
}

DAQSubDet * DAQEvent::_pSD[nbtps]={0,0,0,0,0,0,0,0};
DAQBlockType * DAQEvent::_pBT[nbtps]={0,0,0,0,0,0,0,0};

void DAQEvent::addsubdetector(pid pgetid, pputdata pput, uinteger btype){
  DAQSubDet * p= new DAQSubDet(pgetid, pput);
  if(_pSD[btype]){
    DAQSubDet *paux=_pSD[btype];
    while(paux->_next)paux=paux->_next;
    paux->_next=p; 
  }
  else _pSD[btype]=p;
}
void DAQEvent::addblocktype(pgetmaxblocks pgmb, pgetl pgl,pgetdata pget,uinteger btype){
  DAQBlockType * p= new DAQBlockType(pgmb,pgl,pget);
  if(_pBT[btype]){
    DAQBlockType *paux=_pBT[btype];
    while(paux->_next)paux=paux->_next;
    paux->_next=p; 
  }
  else _pBT[btype]=p;
  p->_maxbl=(p->_pgmb)();
  p->_plength=new integer[p->_maxbl];
  if(p->_plength == NULL){
    cerr<<"DAQEvent::addblock-F-Memory exhausted "<<p->_maxbl<<endl;  
    p->_maxbl=0;
    exit(1);
  }
}


const integer DAQEvent::_OffsetL=1;
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


void DAQEvent::buildDAQ(uinteger btype){
DAQBlockType *fpl=_pBT[btype];
if(fpl == NULL && btype){
  static int init=0;
  if(init++==0)cerr << "DAQEvent::build-S-NoSubdetectors in DAQ"<<endl;
  return;
}
integer ntot=0;
while(fpl){
 for(int i=0;i<fpl->_maxbl;i++){
   *(fpl->_plength+i)=(fpl->_pgetlength)(i)+1;
   if(*(fpl->_plength+i)>1)ntot+=*(fpl->_plength+i);
 }
 fpl=fpl->_next;
}


// Make array


if(ntot)_Length=lover+ntot;
#ifdef __AMSDEBUG__
if(_Length > 65535){
  cerr<<"DAQEvent::buildDAQ-F-lengthToobig "<<_Length<<endl;
  exit(1);
}
assert(sizeof(time_t) == sizeof(integer));
#endif

if(_create(btype) ){
 fpl=_pBT[btype];
 while(fpl){
 for(int i=0;i<fpl->_maxbl;i++){
   if(*(fpl->_plength+i)>1){
    *_pcur=*(fpl->_plength+i)-_OffsetL;
    int16u *psafe=_pcur+1;
    fpl->_pgetdata(i,*(fpl->_plength+i)-1,psafe);
    _pcur=_pcur+*_pcur+_OffsetL;
   }
 }
 fpl=fpl->_next;
 }
}
}

 

integer DAQEvent::_EventOK(){
  if(_Length >1 && _pData ){
    if( _pData[1]<<3 ==0){
      // env header
     integer ntot=0;
     _pcur=_pData+2;
     for(_pcur=_pData+2;_pcur<_pData+_Length;_pcur+=*(_pcur)+_OffsetL)
     ntot+=*(_pcur)+_OffsetL;
     if(ntot != _pData[0]+_OffsetL-2){
       cerr <<"DAQEvent::_Eventok-E-length mismatch: Header says length is "<<
         _pData[0]+_OffsetL<<" Blocks say length is "<<ntot+2<<endl;
       return 0;
     }
     else return 1;    
    }
    else return -1;
  }
  else return 0;
}

integer DAQEvent::_HeaderOK(){
  for(_pcur=_pData+lover;_pcur < _pData+_Length;_pcur=_pcur+*_pcur+_OffsetL){
    if(AMSEvent::checkdaqid(*(_pcur+1))){
      AMSEvent::buildraw(*(_pcur)+_OffsetL-1,_pcur+1, _Run,_Event,_RunType,_Time);
      _Checked=1;
      return 1;
    }
  }
  cerr<<"DAQEvent::_HeaderOK-E-NoHeaderinEvent Type "<<_pData[1]<<endl;
 return 0;
}


  


void DAQEvent::buildRawStructures(){
  if(_Checked ||(_EventOK()==1 && (_HeaderOK()))){
   DAQSubDet * fpl=_pSD[_GetBlType()];
   while(fpl){
   for(_pcur=_pData+lover;_pcur < _pData+_Length;_pcur=_pcur+*_pcur+_OffsetL){
    if(fpl->_pgetid(*(_pcur+1))){
     int16u *psafe=_pcur+1;
     fpl->_pputdata(*_pcur+_OffsetL-1,psafe);
    }
   }
   fpl=fpl->_next; 
   }
  }
  else if(_EventOK()==-1){
    // without envelop only one block
   DAQSubDet * fpl=_pSD[_GetBlType()];
   while(fpl){
    _pcur=_pData;
    if(fpl->_pgetid(*(_pcur+1))){
      fpl->_pputdata(*_pcur+_OffsetL-1,_pcur+1);
    }
   fpl=fpl->_next; 
   }
  }
}


void DAQEvent::write(){
  if(_Length){
    _convert();
    fbout.write((unsigned char*)_pData,sizeof(_pData[0])*_Length);
   // Unfortunately we shoulf flush output for each event
   //
   fbout.flush();
  }

}


integer DAQEvent::read(){
  do{
    if(fbin.good() && !fbin.eof()){
     int16u l16;
#ifdef __AMSDEBUG__
     assert(sizeof(_pData[0]) == sizeof(l16));
#endif
     fbin.read((unsigned char*)(&l16),sizeof(_pData[0]));
     _convertl(l16);
     _Length=l16+_OffsetL;
     if(fbin.good() && !fbin.eof()){
      if(_create()){
       fbin.seekg(fbin.tellg()-sizeof(_pData[0]));
       fbin.read((unsigned char*)(_pData),sizeof(_pData[0])*(_Length));
       _convert();
      }
      else{
       fbin.seekg(fbin.tellg()+sizeof(_pData[0])*(_Length-1));
       _Length=0;
      }
     }
     else break;
    }
    else break;  
  }while(_EventOK()==0 || (_HeaderOK()==0 ));
   return fbin.good() && !fbin.eof();
}





void DAQEvent::init(integer mode, integer format){
  enum open_mode{binary=0x80};
  if(mode%10 ==1 ){
    if(ifnam){
    fbin.open(ifnam,ios::in|binary);
    if(fbin){ 
    if(SELECTFFKEY.Run){
     DAQEvent daq;
     integer run=-1;
     integer ok=1;
     integer iposr=0;
     while(ok){
      ok=daq.read();
      if(ok){
       iposr++;
         if (daq.runno() != run){
          cout <<" DAQEvent::init-I-New Run "<<daq.runno()<<endl;
          run=daq.runno();
        } 
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
     *pc=*(pc+1);
     *(pc+1)=tmp;
     pc=pc+sizeof(int16u);
   }
  }
}




void DAQEvent::_convertl(int16u & l16){
  if(AMSDBc::BigEndian){
   unsigned char tmp;
   unsigned char *pc = (unsigned char*)(&l16);
     tmp=*pc;
     *pc=*(pc+1);
     *(pc+1)=tmp;
  }
}


void DAQEvent::_copyEl(){
   if(DAQCFFKEY.mode/10)write();


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

integer DAQEvent::_create(uinteger btype){
if(_pData)shrink();
if(_Length <=0 )return 0;
if(_Length*sizeof(_pData[0])> sizeof(_Buffer) || _BufferLock)
_pData= (int16u*)UPool.insert(sizeof(_pData[0])*_Length);
else {
_pData=(int16u*)_Buffer;
_BufferOwner=1;
_BufferLock=1;
}
if(_pData){
 _pData[0]=_Length-_OffsetL;
 _pData[1]=btype<<13;  // Event ID
 _pcur=_pData+2;
}
return _pData != NULL ;
}


//+

uint16  DAQEvent::sdetlength(uint16 sdetid) {
  enum {block_offset = 3};
  integer offset = block_offset;
  uint16 id;
  uint16 l;
  for (;;) {
   id = _pData[offset];
   l  = _pData[offset - 1];
   if (id == sdetid) return l;
   offset = offset  + l + 1;
   if (offset > eventlength()) break;
  }
  return -1;
}  

integer DAQEvent::sdet(uint16 sdetid) {
  enum {block_offset = 3};
  integer offset = block_offset;
  uint16 id;
  uint16 l;
  for (;;) {
   id = _pData[offset];
   l  = _pData[offset-1];
   if (id == sdetid) return offset;
   offset = offset  + l + 1;
   if (offset > eventlength()) break;
  }
  return -1;
}  

void DAQEvent::dump(uint16 sdetid) {
// dump event
// if sdetid == -1 dump whole event
//
  cout<<"run, event, length "<<runno()<<", "<<eventno()<<", "<<eventlength()
      <<endl;
  enum {block_offset = 3};
  integer offset = block_offset;
  uint16  id;
  integer l;
  for (;;) {
   id = _pData[offset];
   l  = _pData[offset - 1];
   if (id == sdetid || sdetid == 0) {
    printf("sub.detector id... %#x, length... %d\n ",id,l);
    if (l > 0) {
     for (int i = 1; i < l-1; i++) {
      cout<<*((uint16*)(_pData + offset + i))<<" ";
     }
     cout<<endl;
    } else {
      cout<<"Error : invalid length. Quit"<<endl;
      return;
    }
    offset = offset  + l + 1;
    if (offset > eventlength()) break;
   }
  }
}  
//-  

