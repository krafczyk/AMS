#include <daqevt.h>
#include <event.h>
const integer lsub=2;
DAQEvent::~DAQEvent(){
UPool.udelete(_pData);
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
_pData= (uinteger*)UPool.insert(sizeof(_pData[0])*(1+lheader+ntot));

#ifdef __AMSDEBUG__
assert(sizeof(time_t) == sizeof(integer));
#endif

if(_pData ){
 *_pData=1+lheader+ntot;
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
 fpl->_pgetdata(fpl->_length,_pcur+2);
 _pcur=_pcur+*_pcur;
}
fpl=fpl->_next;
}
crc()=_makeCRC(); 
}
}

 
uinteger DAQEvent::_makeCRC(){
#ifdef __AMSDEBUG__
return 0;
#else
return 0;
#endif
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

DAQSubDet * fpl=_pFirst;
while(fpl){
 fpl->_sdid=_findid((fpl->_pgetid)());
 for(_pcur=_pData+1;_pcur < _pData+*(_pData);_pcur=_pcur+*_pcur){
   if(*(_pcur+1) == fpl->_sdid){
    fpl->_pputdata(*_pcur-lsub,_pcur+lsub);
    break;
   }
 }
 fpl=fpl->_next; 
}

}


void DAQEvent::write(){
}


integer DAQEvent::read(){
return _CRCok();
}


void DAQEvent::init(integer mode){
}


void DAQEvent::_convert(){
  // Little Endian -> Big Endian

}

void DAQEvent::_copyEl(){


}
