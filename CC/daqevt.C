#include <stdio.h>

#include <daqevt.h>
#include <event.h>
#include <commons.h>

integer DAQEvent::_Buffer[75000];
integer DAQEvent::_BufferLock=0;
const integer lover=2;
DAQEvent::~DAQEvent(){
shrink();
}

void DAQEvent::shrink(){

if(_pData && !_BufferOwner)UPool.udelete(_pData);
if(_BufferOwner)_BufferLock=0;
_pData=0;
//_Length=0;
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
char ** DAQEvent::ifnam=0;
integer DAQEvent::InputFiles=0;
integer DAQEvent::KIFiles=0;
char * DAQEvent::ofnam=0;
fstream DAQEvent::fbin;
fstream DAQEvent::fbout;


void DAQEvent::setfiles(char *ifile, char *ofile){
  if(ifile){
   InputFiles=parser(ifile,ifnam);
   cout <<"DAQEvent::setfiles-I-"<<InputFiles<<" input files parsed"<<endl;
   //   ifnam=new char[strlen(ifile)+1];
   //   strcpy(ifnam,ifile);
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
if(_Length > 65535*4){
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
    if( (_pData[1] & ~63)<<3 ==0){
      // envelop header
     integer ntot=0;
     _pcur=_pData+2;
     for(_pcur=_pData+2;_pcur<_pData+_Length;_pcur+=*(_pcur)+_OffsetL)
     ntot+=*(_pcur)+_OffsetL;
     if(ntot != _Length-2){
       cerr <<"DAQEvent::_Eventok-E-length mismatch: Header says length is "<<
         _Length<<" Blocks say length is "<<ntot+2<<endl;
#ifdef __AMSDEBUG__
       cerr <<" SubBlock dump follows"<<endl;
     _pcur=_pData+2;
     for(_pcur=_pData+2;_pcur<_pData+_Length;_pcur+=*(_pcur)+_OffsetL)
       cerr <<" ID " <<*(_pcur+1)<<" Length "<< *(_pcur)+_OffsetL<<endl;
     //     int ic=0;
     //     for(_pcur=_pData;_pcur<_pData+_Length;_pcur++)
     //       cout <<ic++ <<" "<<*(_pcur)<<endl;
#endif   
       return 0;
     }
     else return 1;    
    }
    else {
      cout <<"DAQEvent::_EventOK-I-NonEventBlockFound-ID "<<_pData[1]<<" Length "<<
        _Length<<endl;
     return -1;
    }
  }
  else return 0;
}

integer DAQEvent::_HeaderOK(){
  for(_pcur=_pData+lover;_pcur < _pData+_Length;_pcur=_pcur+*_pcur+_OffsetL){
    if(AMSEvent::checkdaqid(*(_pcur+1))){
      AMSEvent::buildraw(*(_pcur)+_OffsetL-1,_pcur+1, _Run,_Event,_RunType,_Time,_usec);
      _Checked=1;
#ifdef __AMSDEBUG__
      //      cout << "Run "<<_Run<<" Event "<<_Event<<" RunType "<<_RunType<<endl;
      //      cout <<ctime(&_Time)<<" usec "<<_usec<<endl;
#endif
      // fix against event 0
      if(_Event==0)return 0;
      // fix against crazy run;
      if(_Run==305419896 && DAQCFFKEY.RunChanger>0){
        time_t tm=time()-DAQCFFKEY.RunChanger;
        DAQCFFKEY.RunChanger=-tm;        
      }
      if(_Run==305419896 && DAQCFFKEY.RunChanger<0){
        time_t tm=time()-DAQCFFKEY.RunChanger;
        _Run=-DAQCFFKEY.RunChanger;
      }
            
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
  enum open_mode{binary=0x80};
  do{
    if(fbin.eof() && KIFiles<InputFiles-1){
     fbin.close();
     for(;;){
       fbin.open(ifnam[++KIFiles],ios::in|binary);
       if(fbin){ 
        cout <<"DAQEvent::read-I-opened file "<<ifnam[KIFiles]<<endl;
        break;
       }    
       else{
        cerr<<"DAQEvent::read-F-cannot open file "<<ifnam[KIFiles++]<<endl;
        if(KIFiles>=InputFiles-1)return 0;
       }
     }
    }
    if(fbin.good() && !fbin.eof()){
     int16u l16;
#ifdef __AMSDEBUG__
     assert(sizeof(_pData[0]) == sizeof(l16));
#endif
     fbin.read((unsigned char*)(&l16),sizeof(_pData[0]));
     _convertl(l16);
     _Length=l16+_OffsetL;
     // get more length (if any)
     fbin.read((unsigned char*)(&l16),sizeof(_pData[0]));
     _convertl(l16);
     _Length= _Length | ((l16 & 63)<<16);
     //cout <<" Length "<<_Length<<endl;

    if(fbin.eof() && KIFiles<InputFiles-1){
     fbin.close();
     for(;;){
     fbin.open(ifnam[++KIFiles],ios::in|binary);
     if(fbin){ 
       cout <<"DAQEvent::read-I-opened file "<<ifnam[KIFiles]<<endl;
      fbin.read((unsigned char*)(&l16),sizeof(_pData[0]));
      _convertl(l16);
      _Length=l16+_OffsetL;
      // get more length (if any)
      fbin.read((unsigned char*)(&l16),sizeof(_pData[0]));
      _convertl(l16);
      _Length= _Length | ((l16 & 63)<<16);
      //cout <<" Length "<<_Length<<endl;
      break;
      

     }    
     else{
        cerr<<"DAQEvent::read-F-cannot open file "<<ifnam[KIFiles++]<<endl;
        if(KIFiles>=InputFiles-1)return 0;
     }
     }

    }
     if(fbin.good() && !fbin.eof()){
      if(_create()){
       fbin.seekg(fbin.tellg()-2*sizeof(_pData[0]));
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
    if(ifnam[0]){
    fbin.open(ifnam[0],ios::in|binary);
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
       if(format==0 ){
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
      cerr<<"DAQEvent::init-F-cannot open file "<<ifnam[KIFiles]<<" in mode "<<mode<<endl;
      exit(1);
    }
    }
    else {
      cerr <<"DAQEvent::init-F-no input file to init "<<endl;
      exit(1);
    }
  }
}

void DAQEvent::initO(integer run){
  enum open_mode{binary=0x80};
  integer mode=DAQCFFKEY.mode;
  if(mode/10 ){
   if(ofnam){
     char name[255];
     ostrstream ost(name,sizeof(name));
     ost << ofnam<<"."<<run<<ends;
    if(fbout)fbout.close();
    if(mode/10 ==1)fbout.open(name,ios::out|binary|ios::noreplace);
    if(mode/10 ==2)fbout.open(name,ios::out|binary|ios::app);
     if(fbout){ 
       // Associate buffer
      static char buffer[1000+1];
      fbout.setbuf(buffer,1000);
      cout<<"DAQEvent::initO-I- opened file "<<name<<" in mode "<<mode<<endl;

     }
     else{
      cerr<<"DAQEvent::initO-F-cannot open file "<<name<<" in mode "<<mode<<endl;
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
 _pData[0]=(_Length-_OffsetL)%65536;
 _pData[1]=(btype<<13) | (_Length-_OffsetL)/65536;  // Event ID
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




int DAQEvent::parser(char a[], char **& fname){


  int ntot=0;
  for(int kl=strlen(a);kl>=0;kl--){
    if(a[kl]=='/'){
     kl++;
     break;
    }
  }
  //  cout << " kl "<<kl<<endl;
  {
  int coma=kl-1;
  for(int i=kl;i<strlen(a)+1;i++){
    if(a[i]==',' || i==strlen(a)){
      if(i-coma > 1){
        // find -
        int tire=0;
        for(int j=coma+1;j<i;j++){
          if(a[j]=='-' && j != coma+1 && j != i-1){
           istrstream osta(a+coma+1,j-coma-1);
           int ia= 1000000000;
           int ib=-1000000000;
           osta >>ia;   
           istrstream ostb(a+j+1,i-j-1);
           ostb >>ib;   
           if(ib >=ia)ntot+=ib-ia+1;
           tire=1;  
          }
        }
        if(tire==0)ntot++; 
        coma=i;
      }
      }
    }
}
  //   cout << " ntot "<<ntot<<endl;
  fname =new char*[ntot];
  ntot=0;
  {
  int coma=kl-1;
  for(int i=kl;i<strlen(a)+1;i++){
    if(a[i]==',' || i==strlen(a)){
      if(i-coma > 1){
        // find -
        int tire=0;
        for(int j=coma+1;j<i;j++){
          if(a[j]=='-' && j!=coma+1 && j!=i-1){
           istrstream osta(a+coma+1,j-coma-1);
           int ia= 1000000000;
           int ib=-1000000000;
           osta >>ia;   
           istrstream ostb(a+j+1,i-j-1);
           ostb >>ib;   
           if(ib >=ia){
             //add leading zero(s)
             int lz=0;
             for(int l=coma+1;l<j;l++){
               if(a[l]=='0')lz++;
               else break;
             }
             for(int k=ia;k<=ib;k++){
              fname[ntot++]=new char[255];
              for(l=0;l<kl;l++)fname[ntot-1][l]=a[l];
              for(l=kl;l<kl+lz;l++)fname[ntot-1][l]='0';
              for(l=kl+lz;l<255;l++)fname[ntot-1][l]='\0';
              ostrstream ost(fname[ntot-1]+kl+lz,255-kl-lz);
              ost <<k;
             }
           }
           tire=1;  
          }
        }
        if(tire==0){
              fname[ntot++]=new char[255];
              for(int l=0;l<255;l++)fname[ntot-1][l]='\0';
              for(l=0;l<kl;l++)fname[ntot-1][l]=a[l];
              for(l=kl;l<i-coma-1+kl;l++)fname[ntot-1][l]=a[l-kl+coma+1];
        }
        coma=i;
      }
      }
    }
  }
  return ntot;  


}



