#include <io.h>
#include <commons.h>
char * AMSIO::fnam=0;
fstream AMSIO::fbin;
AMSIO::AMSIO(integer run, integer event, time_t time, integer ipart, 
integer seed[], AMSPoint coo,AMSDir dir, number mom, number pole,
number stheta, number sphi, integer nskip): _run(run),_event(event),
  _ipart(ipart),_mom(mom),_time(time),_polephi(pole),_stationtheta(stheta),
_stationphi(sphi),_nskip(nskip){
_seed[0]=seed[0];
_seed[1]=seed[1];
int i;
for (i=0;i<3;i++)_coo[i]=coo[i];
for (i=0;i<3;i++)_dir[i]=dir[i];
}

void AMSIO::setfile(char *f){
  if(f){
   fnam=new char[strlen(f)+1];
   strcpy(fnam,f);
  }
}

void AMSIO::init(integer mode,integer format){
  enum open_mode{binary=0x80};

  if(fnam){
    if(mode ==1 ) {
        fbin.open(fnam,ios::in|binary);
        if(fbin && SELECTFFKEY.Run){
          // read one by one 
          AMSIO io;
          integer ipos=0;
          integer iposr=0;
          integer runold=0;
          integer pidold=0;
          integer ok=1;
          integer seed0,seed1;
          number theta,phi,pole;
          while(ok){
           ok=io.read();
           if(ok){
             seed0=io.getseed(0);
             seed1=io.getseed(1);
             theta=io.getstheta()*180./3.1415926;
             phi=(io.getsphi()-io.getpolephi())*180./3.1415926+290.;
             phi=fmod(phi,360.);
           }
           if(format==1 && (io.getrun()!=runold || ok==0)){
             if(iposr>0)cout <<"AMSIO::init-I-Run "<<runold<<" has "<<iposr<<
                          " events with pid = "<<pidold<<endl;
             if(io.getrun()<0){
               cout <<"AMSIO::init-F-Negative run number "<< io.getrun()<<endl;
               exit(1);
             }
             iposr=0;
             pidold=io.getpid();
             runold=io.getrun();
           }
           ipos++;
           iposr++;
           if(io.getrun() == SELECTFFKEY.Run && io.getevent() >= 
             SELECTFFKEY.Event)break; 
          }
          // pos back if fbin.good
          if(ok){
            fbin.seekg(fbin.tellg()-sizeof(io));
            cout<<"AMSIO::init-I-Selected Run = "<<SELECTFFKEY.Run<<
              " Event = "<<io.getevent()<< " Position = "<<ipos<<endl;
          
          }
          else {
            if(format==0)cerr <<"AMSIO::init-F-Failed to select Run = "<<SELECTFFKEY.Run<<
              " Event >= "<<SELECTFFKEY.Event<<endl;
          if(format==1){
             cout<<"AMSIO::init-I-Total of "<<ipos-1
             <<" events have been read."<<endl;
             cout << " Last Random Number "<<seed0<<" "<<seed1    <<endl;
             cout << " Theta "<< theta<< " Phi "<<phi<<endl;
             fbin.close();
          }
          else  exit(1);
          }
          return;
        }
    }
    else fbin.open(fnam,ios::out|binary|ios::app);
    if(fbin==0){
      cerr<<"AMSIO::init-F-cannot open file "<<fnam<<" in mode "<<mode<<endl;
      exit(1);
    }
  }
  else {
    cerr <<"AMSIO::init-F-no file to init "<<endl;
    exit(1);
  }
}
void AMSIO::write(){
   convert();
   fbin.write((char*)this,sizeof(*this));
}
integer AMSIO::read(){
   if(fbin.good() && !fbin.eof()){
     fbin.read((char*)this,sizeof(AMSIO)-CCFFKEY.oldformat*4*sizeof(integer));
     convert();
     if(CCFFKEY.oldformat){
      _polephi=0;
      _stationphi=0;
      _stationtheta=0;
      _nskip=0;
     }
   }
   return fbin.good() && !fbin.eof();
}


integer AMSIOI::_Count=0;
AMSIOI::AMSIOI(){
  _Count++;
}
AMSIOI::~AMSIOI(){
  if(--_Count==0){
   if(AMSIO::fbin)AMSIO::fbin.close();
  }
}
ostream & operator << (ostream &o, const AMSIO &b ){
  return o<<b._run<<b._event<<b._time<<b._ipart<<b._coo[0]<<
    b._coo[1]<<b._coo[2]<<b._dir[0]<<b._dir[1]<<b._dir[2]<<b._mom<<b._seed[0]
          <<b._seed[1];
}   

void AMSIO::convert(){
#ifndef __ALPHA__
  // Dec alpha has to be converted to big endian...
  unsigned char tmp;
  unsigned char *pc = (unsigned char*)this;
  int i;
  int n=sizeof(*this)/sizeof(integer);
  for(i=0;i<n;i++){
    tmp=*pc;
    *pc=*(pc+3);
    *(pc+3)=tmp;
    tmp=*(pc+1);
    *(pc+1)=*(pc+2);
    *(pc+2)=tmp;
    pc=pc+sizeof(integer);
  }    
#endif
}
