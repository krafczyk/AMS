//  $Id: tofrec02.h,v 1.6 2002/04/10 10:06:02 choumilo Exp $
// June, 23, 1996. ak. add getNumbers function
//
// Oct  04, 1996.  ak _ContPos is moved to AMSLink
//
// Last Edit Oct 04, 1996. ak.
// Last edit 8.1.97 EC (raw cluster format changed)
//
#ifndef __AMSTOF2REC__
#define __AMSTOF2REC__
#include <tofdbc02.h>
#include <typedefs.h>
#include <point.h>
#include <link.h>
#include <event.h>
#include <trrec.h>
//
class TOF2RawCluster: public AMSlink{
protected:
 static integer trflag; // =1/2/3 -> "z>=1"/"z>1"/"z>2", =0->no trig.
 static integer trpatt[TOF2GC::SCLRS]; // triggered bars pattern
 integer _ntof;    // number of TOF-plane(layer) (1-top,...,4-bot)
 integer _plane;   //  number of sc. bar in given plane(1->...)
 number _z;        // z coord of sc.bar
 number _adch[2]; // high(anode) ADC for 2 sides (ADC-chan in float)
 number _adcl[2]; // low(dinode) ADC for 2 sides (ADC-chan in float)
 number _edeph;    // reco. via high(anode) channel (no angle correction)
 number _edepl;    // reco. via low(dinode) channel (..................)
 number _sdtm[2];  // A-noncorrected side times (ns, for TZSL-calibr) 
 number _time;   // A-corrected time=0.5*(t1+t2) (ns);
 number _timeD; // Y-coo(long)/(cm,loc.r.s,A-corrected) calc.from 0.5(t1-t2)
 number _etimeD;// Y-coord.error(cm)

 void _copyEl();
 void _printEl(ostream & stream);
 void _writeEl();
public:
 static integer Out(integer);
 integer getstatus() const{return _status;}
 integer getntof()const {return _ntof;}
 integer getplane()const {return _plane;}
 number gettime()const {return _time;}
 number gettimeD()const {return _timeD;}
 number getetimeD()const {return _etimeD;}
 number getedeph()const {return _edeph;}
 number getedepl()const {return _edepl;}
 number getz()const {return _z;}
 TOF2RawCluster(integer status, integer xy, integer plane, 
   number z, number adch[2], number adcl[2], number de, number ded, 
   number sdtm[2], number time, number timed, number etimed):
   AMSlink(status,0), _ntof(xy),_plane(plane),_z(z),_edeph(de),
   _edepl(ded), _time(time), _timeD(timed), _etimeD(etimed){
   for(int i=0;i<2;i++){
     _adch[i]=adch[i];
     _adcl[i]=adcl[i];
     _sdtm[i]=sdtm[i];
   }
 }
 void getadch(number adch[2]){
   adch[0]=_adch[0];
   adch[1]=_adch[1];
 }
 void getadcl(number adcl[2]){
   adcl[0]=_adcl[0];
   adcl[1]=_adcl[1];
 }
 void getsdtm(number sdtm[2]){
   sdtm[0]=_sdtm[0];
   sdtm[1]=_sdtm[1];
 }
 void recovers(number x);
//
 TOF2RawCluster *  next(){return (TOF2RawCluster*)_next;}
//
 static void setpatt(integer patt[]){
   for(int i=0;i<TOF2GC::SCLRS;i++)trpatt[i]=patt[i];
 }
 static void getpatt(uinteger patt[]){
   for(int i=0;i<TOF2GC::SCLRS;i++)patt[i]=trpatt[i];
 }
 static void settrfl(integer trfl){trflag=trfl;}
 static integer gettrfl(){return trflag;}
 static void sitofdigi(int &);// tempor. MCCluster->RawCluster
 static void build(int &);   // RawEvent->RawCluster
};
//===================================================================
class AMSTOFCluster: public AMSlink{
protected:
 static AMSTOFCluster *_Head[4];
 integer _ntof;  // TOF plane number where cluster was found(1->) 
 integer _plane; // bar number of the "peak" bar in cluster(1->)  
 number _edep;   // clust. Etot/nmemb (MeV) (continious through high-to-low chan)
 number _edepd;  // clust. Etot/nmemb (MeV) via low(dinode)
 number _time;   // average cluster time (sec)
 number _etime;  // error on cluster time (sec)
 AMSPoint _Coo;  // cluster center of gravity coordinates (cm)
 AMSPoint _ErrorCoo; // Error to coordinates
 integer _nmemb; // number of TOFRawCluster members(1-3)
 AMSlink * _mptr[3]; // list of pointers to members(raw clust)
 static integer _planes; // real number of planes
 static integer _padspl[TOF2GC::SCLRS]; // real number of paddles/plane
 
 void _copyEl();
 void _printEl(ostream & stream);
 void _writeEl();
#ifdef __PRIVATE__
 static integer goodch[4][14];
 static geant   tcoef[4][14];
#endif


public:

  integer operator < (AMSlink & o)const{
   AMSTOFCluster *p =dynamic_cast<AMSTOFCluster*>(&o);
   if (checkstatus(AMSDBc::USED) && !(o.checkstatus(AMSDBc::USED)))return 1;
   else if(!checkstatus(AMSDBc::USED) && (o.checkstatus(AMSDBc::USED)))return 0;
   else return !p || _time < p->_time;
  }


 static integer Out(integer);
 static void init();
 static integer planes(){return _planes;}
 static integer padspl(integer npl){return _padspl[npl];}
 AMSID crgid(integer i=0);
 integer getntof()const {return _ntof;}
 integer getplane()const {return _plane;}
 number gettime()const {return _time;}
 number getetime()const {return _etime;}
 void setetime(number et){_etime=et;}
 number getedep()const {return _edep;}
 number getedepd()const {return _edepd;}
 integer getnmemb()const {return _nmemb;}
 integer getmemb(AMSlink *ptr[]){
   for(int i=0;i<_nmemb;i++)ptr[i]=_mptr[i];
   return _nmemb;
 }
 void recovers(AMSTrTrack * ptr);
 void recovers2(AMSTrTrack * ptr);
//+
#ifdef __DB__
   friend class AMSTOFClusterD;
#endif
//-
 AMSPoint getcoo()const {return _Coo;}
 AMSPoint getecoo()const {return _ErrorCoo;}
//+
 AMSTOFCluster(): AMSlink() {}
//-
 AMSTOFCluster(integer status, integer ntof, integer plane, number edep, 
    number edepd, AMSPoint coo, AMSPoint ecoo, number time, number etime, integer nm,
                                                                     AMSlink * ptr[]):
 AMSlink(status,0),_ntof(ntof),_plane(plane),_edep(edep),_edepd(edepd),
 _Coo(coo),_ErrorCoo(ecoo),_time(time), _etime(etime), _nmemb(nm){
   for(int i=0;i<nm;i++)_mptr[i]=ptr[i];
 }
//
 AMSTOFCluster(integer status, integer ntof, integer plane):
        AMSlink(status,0),_ntof(ntof),_plane(plane),_edep(0),_edepd(0),
                 _Coo(0,0,0),_ErrorCoo(0,0,0),_time(0), _etime(0), _nmemb(0){
   for(int i=0;i<2;i++)_mptr[i]=0;
 }
//
 AMSTOFCluster *  next(){return (AMSTOFCluster*)_next;}
 ~AMSTOFCluster(){for(int i=0;i<4;i++)_Head[i]=0;};
 static void build(int &);
 static void build2(int &);
 static void build();   // fast algo build
 static void print();
 void  resethash(integer i, AMSlink *head){
   if(i>=0 && i <4)_Head[i]=(AMSTOFCluster*)head;
 }

 static AMSTOFCluster * gethead(integer i=0){
  if(i>=0 && i<4){
    if(!_Head[i])_Head[i]=(AMSTOFCluster*)AMSEvent::gethead()
    ->getheadC("AMSTOFCluster",i,1);
    return _Head[i];
  }
  else {
#ifdef __AMSDEBUG__
    cerr <<"AMSTOFCluster:gethead-S-Wrong Head "<<i;
#endif
    return 0;
  }
 }
};
//===================================================================
class TOF2User {
 public:
  void static Event();
  void static InitJob();
  void static EndJob();
};
#endif
