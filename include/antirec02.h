//  $Id: antirec02.h,v 1.7 2005/05/04 10:27:47 choumilo Exp $
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
  static integer nscoinc;//# of logic sectors in coinc.with FT
  static uinteger trpatt; //bits 1-8 => log.sect# in coinc.with FT
  int16u idsoft; // BBS (Bar/Side)
  int16u status; // status (0/1/... -> alive/dead/...)
  int16u nadca; // number of anode pulse-charge hits 
  int16u adca[ANTI2C::ANAHMX];// anode pulse-charge hits(ADC-chan)
  int16u ntdct; // number of History-TDC hits (edges)
  int16u tdct[ANTI2C::ANTHMX*2];// Hist-TDC edges (edge time is in TDC-channels)
public:
  Anti2RawEvent(int16u _idsoft, int16u _status, int16u _nadca, int16u _adca[],
                  int16u _ntdct, int16u _tdct[]):idsoft(_idsoft),status(_status)
  {
    int i;
    nadca=_nadca;
    for(i=0;i<nadca;i++)adca[i]=_adca[i];
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
  int16u getnadca(){return nadca;}
  int16u getnzchn(){if(nadca>0)return(1);
                          else return(0);}
//
  int16u getadca(int16u arr[]){
  for(int i=0;i<nadca;i++)arr[i]=adca[i];
  return nadca;
  }
  void putadca(int16u nelem, int16u arr[]){
    nadca=nelem;
    for(int i=0;i<nadca;i++)adca[i]=arr[i];
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
  static void setpatt(uinteger patt){
    trpatt=patt;
  }
  static void setncoinc(integer nc){nscoinc=nc;}
  static uinteger getpatt(){
    return trpatt;
  }
  static integer getncoinc(){return nscoinc;}
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
  stream <<"nadca="<<dec<<nadca<<endl;
  for(i=0;i<nadca;i++)stream <<hex<<adca[i]<<endl;
  stream <<"ntdct="<<dec<<ntdct<<endl;
  for(i=0;i<ntdct;i++)stream <<hex<<tdct[i]<<endl;
  stream <<dec<<endl<<endl;
}
void _writeEl(){};
void _copyEl(){};
//
};
//===================================================================
class AMSAntiCluster: public AMSlink{
protected:
 integer _sector;  // Sector number 1 - MAXANTI
 integer _ntimes;  //number of time-hits
 integer _npairs;  //number of true(paired) time-hits
 number  _times[ANTI2C::ANTHMX*2];//time-hits(ns)
 number  _etimes[ANTI2C::ANTHMX*2];// approx.time errors(ns)
 number _edep;    // SectorEdep (MeV)
 AMSPoint _coo;   // R, phi , Z
 AMSPoint _ecoo;  // Error to _coo 
 void _copyEl();
 void _printEl(ostream & stream);
 void _writeEl();
public:

 static integer Out(integer);
 AMSID crgid(int kk=0){
   return AMSID("ANTS",_sector);
 }
//
 AMSAntiCluster(integer status, integer sector, integer ntimes,
                integer npairs, number times[ANTI2C::ANTHMX*2], 
                number etimes[ANTI2C::ANTHMX*2],number edep, 
		AMSPoint coo, AMSPoint ecoo):
                            AMSlink(status,0), _sector(sector),
		               _ntimes(ntimes),_npairs(npairs),
                             _edep(edep),_coo(coo),_ecoo(ecoo){
    for(int i=0;i<_ntimes;i++){
      _times[i]=times[i];
      _etimes[i]=etimes[i];
    }
 }
//
 AMSAntiCluster(integer status, integer sector):
                AMSlink(status,0), _sector(sector),_ntimes(0),_npairs(0),
		_edep(0),_coo(0,0,0),_ecoo(0,0,0){}
//
 AMSAntiCluster *  next(){return (AMSAntiCluster*)_next;}
//
 integer getsector()const {return _sector;}
 number getedep()const {return _edep;}
//
 integer gettimes(number arr[]){
   for(int i=0;i<_ntimes;i++)arr[i]=_times[i];
   return _ntimes;
 }
 integer getntimes(){return _ntimes;}
 number gettime(int i){return _times[i];}
 number getetime(int i){return _etimes[i];}
 integer getnpairs(){return _npairs;}
//
 static void build2(int &stat); 
 static void print();
#ifdef __WRITEROOT__
 friend class AntiClusterR;
#endif
};
#endif
