#include <io.h>
#include <commons.h>
char * AMSIO::fnam=0;
fstream AMSIO::fbin;
AMSIO::AMSIO(integer run, integer event, time_t time, integer ipart, integer seed[], 
             AMSPoint coo,AMSDir dir, number mom): _run(run),_event(event),
  _ipart(ipart),_mom(mom),_time(time){
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

void AMSIO::init(integer mode){
  enum open_mode{binary=0x80};

  if(fnam){
    if(mode ==1 ) {
        fbin.open(fnam,ios::in|binary);
        if(fbin && SELECTFFKEY.Run){
          // read one by one 
          AMSIO io;
          integer ipos=0;
          while(fbin.good() && !fbin.eof()){
           fbin.read( (char*)&io,sizeof(io));
           ipos++;
           if(io.getrun() == SELECTFFKEY.Run && io.getevent() >= 
             SELECTFFKEY.Event)break; 
          }
          // pos back if fbin.good
          if(fbin.good() && !fbin.eof()){
            fbin.seekg(fbin.tellg()-sizeof(io));
            cout<<"AMSIO-init-I-Selected Run = "<<SELECTFFKEY.Run<<
              " Event = "<<io.getevent()<< " Position = "<<ipos<<endl;
          }
          else {
            cerr <<"AMSIO::init-F-Failed to select Run = "<<SELECTFFKEY.Run<<
              " Event >= "<<SELECTFFKEY.Event<endl;
            exit(1);
          }
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
   fbin.write((char*)this,sizeof(*this));
}
integer AMSIO::read(){
   fbin.read((char*)this,sizeof(AMSIO));
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
