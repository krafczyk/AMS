//  $Id: trigger102.h,v 1.8 2003/05/08 16:42:14 choutko Exp $
#ifndef __AMS2TRIGGER__
#define __AMS2TRIGGER__
#include <link.h>
#include <tofdbc02.h>
//
class Trigger2LVL1: public AMSlink{
protected:
/*
 class Scalers{
  public:
  uinteger lifetime;
  uinteger l1p[10];
  uinteger l1n[10];
  uinteger l2p[14];
  uinteger l2n[14];
  uinteger l3p[14];
  uinteger l3n[14];
  uinteger l4p[10];
  uinteger l4n[10];
  geant getsum() const;
 };
*/
 class Scalers{
  protected:
   uinteger _Nentries;
   uinteger _Tls[3][32];
   uinteger _GetIndex(time_t time);
  public:
   geant getsum(time_t time) {return _Tls[2][_GetIndex(time)]/96.;}
   geant getlifetime(time_t time)   {return _Tls[1][_GetIndex(time)]/16384.;}
 }; 
 static Scalers _scaler;
 uinteger _LifeTime;
 integer _tofflag;   // =0/1/2/3 -> "no_FastTrig"/"z>=1"/"z>1"/"z>2" 
 uinteger _tofpatt[TOF2GC::SCLRS];// TOF:  triggered paddles/layer pattern
 uinteger _tofpatt1[TOF2GC::SCLRS];// TOF:  triggered paddles/layer pattern for z>1
 uinteger _antipatt; //ANTI: triggered sectors pattern
 uinteger _ecalflag; //EC trig-flag(0->Empty,1->MIP,2->EMobject,+10->HiEnergy)
 geant   _ectrsum;//"EC tot.energy"(total sum of all dynode channels used for trigger,gev) 
 void _copyEl(){}
 void _printEl(ostream & stream){ stream << " LifeTime " << float(_LifeTime)/1000.<<endl;}
 void _writeEl();
public:
 Trigger2LVL1(integer lifetime, integer tofflag, uinteger tofpatt[], uinteger antipatt,
             uinteger ecflg, geant ectrsum):
	     _LifeTime(lifetime),_tofflag(tofflag), _ecalflag(ecflg), _ectrsum(ectrsum){
   for(int i=0;i<TOF2GC::SCLRS;i++)_tofpatt[i]=tofpatt[i];
   _antipatt=antipatt;

 }
 Trigger2LVL1(integer TriggerMode, integer tofflag, uinteger tofpatt[],
                uinteger tofpatt1[], uinteger antipatt, uinteger ecflg, geant ectrsum):
  _LifeTime(TriggerMode),_tofflag(tofflag),_antipatt(antipatt),_ecalflag(ecflg),
                                                                    _ectrsum(ectrsum){
   int i;
   for(i=0;i<TOF2GC::SCLRS;i++)_tofpatt[i]=tofpatt[i];
   for( i=0;i<TOF2GC::SCLRS;i++)_tofpatt1[i]=tofpatt1[i];
 }
 bool IsECHighEnergy()const {return _ecalflag/10>0;}
 bool IsECEMagEnergy()const {return _ecalflag%10==2;}
  static Scalers * getscalersp(){return &_scaler;}
  static integer getscalerssize(){return sizeof(_scaler);}
  uinteger getlifetime () const {return _LifeTime;}
 integer gettoflg() {return _tofflag;}
 uinteger getecflg() {return _ecalflag;}
 geant getectrsum(){return _ectrsum;}
 integer checktofpattor(integer tofc, integer paddle);
 integer checktofpattand(integer tofc, integer paddle);
 integer checkantipatt(integer counter){return _antipatt & (1<<counter);}
 static void build();

 // Interface with DAQ
      static int16u getdaqid(){return ( 2<<9 | 1<<6 );}      
      static integer checkdaqid(int16u id){return id == getdaqid()?1:0;}
      static integer calcdaqlength(int i){return 19;}
      static integer getmaxblocks(){return 1;}
      static void builddaq(integer i, integer n, int16u *p);
      static void buildraw(integer n, int16u *p);
#ifdef __WRITEROOT__
      friend class Level1R;
#endif
};



#endif
