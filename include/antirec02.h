//  $Id: antirec02.h,v 1.2 2001/01/22 17:32:42 choutko Exp $
//
// May 29   1997. V. Choutko primitive version
// July 18 1997 E.Choumilov RawEvent added + RawCluster/Cluster modified
//
#ifndef __AMSANTI2REC__
#define __AMSANTI2REC__
#include <typedefs.h>
#include <point.h>
#include <link.h>
#include <event.h>
#include <antidbc02.h>
//
//---------------------------------------
class Anti2RawEvent: public AMSlink{
private:
  static integer trpatt; // bits 1-16-> 2-side-AND, bits 17-32-> 2-side-OR
  int16u idsoft; // BBS (Bar/Side)
  int16u status; // status (0/1/... -> alive/dead/...)
  int16u ntdca; // number of anode TovT_pulse hits (edges)
  int16u tdca[ANTI2C::ANAHMX*2];// anode TovT_pulse hits (edge time is in TDC-channels)
  int16u ntdct; // number of Fast_Trig time hits (edges)
  int16u tdct[ANTI2C::ANAHMX*2];// Fast_Trig edges (edge time is in TDC-channels)
public:
  Anti2RawEvent(int16u _idsoft, int16u _status, int16u _ntdca, int16u _tdca[],
                  int16u _ntdct, int16u _tdct[]):idsoft(_idsoft),status(_status)
  {
    int i;
    ntdca=_ntdca;
    for(i=0;i<ntdca;i++)tdca[i]=_tdca[i];
    ntdct=_ntdct;
    for(i=0;i<ntdct;i++)tdct[i]=_tdct[i];
  };
  ~Anti2RawEvent(){};
  Anti2RawEvent * next(){return (Anti2RawEvent*)_next;}
//
  integer operator < (AMSlink & o)const{
                return idsoft<((Anti2RawEvent*)(&o))->idsoft;}
//
  int16u getid() const {return idsoft;}
  int16u getstat() const {return status;}
  int16u getntdca(){return ntdca;}
  int16u getnzchn(){if(ntdca>0)return(1);
                          else return(0);}
//
  int16u gettdca(int16u arr[]){
  for(int i=0;i<ntdca;i++)arr[i]=tdca[i];
  return ntdca;
  }
  void puttdca(int16u nelem, int16u arr[]){
    ntdca=nelem;
    for(int i=0;i<ntdca;i++)tdca[i]=arr[i];
  }
//
  int16u gettdct(int16u arr[]){
  for(int i=0;i<ntdct;i++)arr[i]=tdct[i];
  return ntdct;
  }
  void puttdct(int16u nelem, int16u arr[]){
    ntdct=nelem;
    for(int i=0;i<ntdct;i++)tdct[i]=arr[i];
  }
//
//
  static void setpatt(integer patt){
    trpatt=patt;
  }
  static integer getpatt(){
    return trpatt;
  }
  static void mc_build(int &stat);
  static void validate(int &stat);
//
// interface with DAQ :
static integer calcdaqlength(int16u blid);
static void builddaq(int16u blid, integer &len, int16u *p);
static void buildraw(int16u blid, integer &len, int16u *p);
static int16u hw2swid(int16u a1, int16u a2);
static int16u sw2hwid(int16u a1, int16u a2); 
//
protected:
void _printEl(ostream &stream){
  int i;
  stream <<"Anti2RawEvent: id="<<dec<<idsoft<<endl;
  stream <<"ntdca="<<dec<<ntdca<<endl;
  for(i=0;i<ntdca;i++)stream <<hex<<tdca[i]<<endl;
  stream <<"ntdct="<<dec<<ntdct<<endl;
  for(i=0;i<ntdct;i++)stream <<hex<<tdct[i]<<endl;
  stream <<dec<<endl<<endl;
}
void _writeEl(){};
void _copyEl(){};
//
};
//---------------------------------------
class AMSAntiRawCluster: public AMSlink{
protected:
 static integer _trpatt;
 integer _sector;    // number of sector 1-16
 integer _updown;    // 0 - up 1 -down
  number _signal;     // number of ph electrons

 void _copyEl();
 void _printEl(ostream & stream);
 void _writeEl();
public:
  static void setpatt(integer patt){
    _trpatt=patt;
  }
 integer getstatus() const{return _status;}
 integer getsector()const {return _sector;}
 integer getupdown()const {return _updown;}
 number getsignal()const {return _signal;}
 AMSAntiRawCluster(integer status, integer sector, integer updown, 
 number signal):
   AMSlink(status,0), _sector(sector),_updown(updown),_signal(signal){}
 AMSAntiRawCluster *  next(){return (AMSAntiRawCluster*)_next;}
 static void siantidigi();//  MCCluster->RawCluster
 static void build(int &stat);// AMSAntiRawEvent->RawCluster
 static void build2(int &stat);// Anti2RawEvent->RawCluster
 static void siantinoise(number cc1[], number cc2[],integer max);
 static integer Out(integer);
};

//===================================================================
class AMSAntiCluster: public AMSlink{
protected:
 integer _sector;  // Sector number 1 - MAXANTI+1
 number _edep;   // cluster total energy deposition (MeV)
 AMSPoint _coo;   // R, phi , Z
 AMSPoint _ecoo; // Error to _coo 
 void _copyEl();
 void _printEl(ostream & stream);
 void _writeEl();
public:

static integer Out(integer);
AMSID crgid(int kk=0){
return AMSID("ANTS",_sector);
}


 integer getsector()const {return _sector;}
 number getedep()const {return _edep;}
 AMSAntiCluster(integer status, integer sector, number edep, AMSPoint coo, AMSPoint ecoo):
   AMSlink(status,0), _sector(sector),_edep(edep),_coo(coo),_ecoo(ecoo){}
 AMSAntiCluster(integer status, integer sector):
   AMSlink(status,0), _sector(sector),_edep(0),_coo(0,0,0),_ecoo(0,0,0){}
 AMSAntiCluster *  next(){return (AMSAntiCluster*)_next;}
 static void build(); 
 static void build2(); 
 static void print();
#ifdef __DB__
friend class AMSAntiClusterD;
#endif

};
#endif
