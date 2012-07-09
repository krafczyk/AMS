//  $Id: Tofrec02_ihep.h,v 1.3 2012/07/09 22:23:47 qyan Exp $

//Author Qi Yan 2012/June/09 10:03 qyan@cern.ch  /*IHEP TOF version*/
#ifndef __AMSTOFREC02_IHEP__
#define __AMSTOFREC02_IHEP__
#include "root.h"
#ifndef __ROOTSHAREDLIBRARY__
#include "link.h"
#include "tofrec02.h"
//////////////////////////////////////////////////////////////////////////
class AMSTOFClusterH;
class AMSCharge;
#endif
class TofBetaPar;

class TofRecH{
protected:
  static const double cvel=29.9792;//c vel(cm/ns)
  static TofBetaPar betapar;

public:
  TofRecH(){};
#ifndef __ROOTSHAREDLIBRARY__
  static int  BuildTofClusterH();
  static int  BuildBetaH(int mode=0);
  static int  TofSideRec(TOF2RawSide *ptr,number &adca, integer &nadcd,number adcd[],number &sdtm,uinteger &sstatus,
                        vector<number>&ltdcw, vector<number>&htdcw, vector<number>&shtdcw);
  static int  TimeCooRec(int idsoft,number sdtm[],number adca[],number tms[2],number &tm,number &etm,number &lcoo,number &elcoo,uinteger &status,int run=0,int force=0);//force(1) to reload TDV
  static int  EdepRec();
  static int  BetaFindTOFCl(AMSTrTrack *ptrack,int ilay,AMSTOFClusterH** tfhit,number &tklen,number &tklcoo,number cres[2],int &pattern,int mode);//mode(1) LMatch should be mode(0) not require
//----Fit
  static int  TRecover(AMSTOFClusterH *tofclh[4],number tklcoo[4]);//using hassid to recover other side
  static int  BetaFitC(AMSTOFClusterH *tofclh[4],number res[4][2],int partten[4],TofBetaPar &par,int mode);//
  static int  BetaFitT(AMSTOFClusterH *tofclh[4],number len[4],int partten[4],TofBetaPar &par,int mode);//mode same etime weight(0) or not(1)
#endif
//---
  static int  LTRefind(int idsoft,number trlcoo,number sdtm[2],number adca[2],uinteger &status, vector<number>ltdcw,int hassid);
  static int  TRecover(int idsoft,number trlcoo,number tms[2],number &tm,number &etm,uinteger &status,int hassid);
  static int  BetaFitT(number time[],number etime[],number len[],const int nhits,TofBetaPar &par,int mode=1);//mode same etime weight(0) or not(1)
//---
#ifndef __ROOTSHAREDLIBRARY__
  static int  BetaFitCheck(TofBetaPar &par);//if this is normal value
  static number BetaCorr(number zint,number z0,number part,uinteger &status);//to BetaC Vitaly
#endif
//--Cal Mass
  static int MassRec(TofBetaPar &par,number rig=0,number charge=0,number evrig=0,int isubetac=0);//evrig=E(1/rig)

#ifdef __WRITEROOT__
  friend class BetaHR; 
  friend class TofBetaPar; 
#endif
};

//////////////////////////////////////////////////////////////////////////
#ifndef __ROOTSHAREDLIBRARY__
class AMSTOFClusterH: public AMSlink{

protected:
    uinteger  _sstatus[2];
    vector<number>_ltdcw[2];
    vector<number>_htdcw[2];
    vector<number>_shtdcw[2];
//---
    TOF2RawSide *_tfraws[2]; 
    integer  _nraws;
    integer  _pattern;//%10(2side information)+(/1000%10=used by beta number) information pattern
    integer  _idsoft;
    number   _adca[2];//raw adc
    number   _adcd[2][TOF2GC::PMTSMX];//dynode adc
    number   _sdtm[2];//raw time 
    number   _timers[2];//rec time ft-lt
    number   _timer; //correct time /ns lt-ft
    number   _etimer;//time err/ns  
    AMSPoint _coo; //coo
    AMSPoint _ecoo;//ecoo
    number   _edepa; //Anode
    number   _edepd; //Dynode
    number   _edep;  //Anode+Dynode
public: 
   const AMSPoint & getcoo()   {return _coo;}
   const AMSPoint & getecoo()  {return _ecoo;}
   integer         getpattern(){return _pattern;}
   integer         getbarid()  {return _idsoft;}
   number          gettime()   {return _timer;} 
   number          getetime()  {return _etimer;}
public:
    AMSTOFClusterH(){};
    AMSTOFClusterH(uinteger sstatus[2],uinteger status,integer pattern,integer idsoft,number adca[],number adcd[][TOF2GC::PMTSMX],
                   number sdtm[],number timers[],number timer,number etimer,
                   AMSPoint coo,AMSPoint ecoo,number edepa,number edepd,number edep,TOF2RawSide *tfraws[2],integer nraws,
                  vector<number>ltdcw[2],vector<number>htdcw[2],vector<number>shtdcw[2]):
                AMSlink(status),_pattern(pattern),_idsoft(idsoft),_timer(timer),_etimer(etimer),
                _coo(coo),_ecoo(ecoo), _edepa(edepa),_edepd(edepd),_edep(edep),_nraws(nraws){
                  for(int i=0;i<2;i++){_sstatus[i]=sstatus[i];_adca[i]=adca[i];_sdtm[i]=sdtm[i],_timers[i]=timers[i],_tfraws[i]=tfraws[i];
                                       for(int ipm=0;ipm<TOF2GC::PMTSMX;ipm++)_adcd[i][ipm]=adcd[i][ipm];
                                       _ltdcw[i]=ltdcw[i];_htdcw[i]=htdcw[i];_shtdcw[i]=shtdcw[i];
                                      }
               }
     ~AMSTOFClusterH(){};
     AMSTOFClusterH * next(){ return (AMSTOFClusterH *)_next;}
      
public:
   void _printEl(ostream &stream){stream <<"AMSTOFClusterH "<<_idsoft<<" "<<_status<<endl;}
   void _writeEl();
   void _copyEl();
   static integer Out(integer);
   integer operator < (AMSlink & o)const{
      return _idsoft < ((AMSTOFClusterH*)(&o)) ->_idsoft;
  }

#ifdef __WRITEROOT__
 friend class TofClusterHR;
 friend class AMSBetaH;
 friend class TofRecH;
#endif
};

//////////////////////////////////////////////////////////////////////////
class AMSBetaH: public AMSlink{

protected:
   AMSTOFClusterH * _phith[4];
   AMSTrTrack *     _ptrack;//trdtrack delete not exis/should be careful
   AMSTRDTrack *    _ptrdtrack;
   AMSCharge  *     _pcharge;

public:
    TofBetaPar       _betapar;
   
public:
    AMSBetaH(){};
    AMSBetaH(integer status,AMSTOFClusterH *phith[4],AMSTrTrack *ptrack,AMSTRDTrack *trdtrack,AMSCharge *amscharge,TofBetaPar &betapar):
         AMSlink(), _ptrack(ptrack),_ptrdtrack(trdtrack), _pcharge(amscharge)
       { 
        if((status&AMSDBc::TRDTRACK)==AMSDBc::TRDTRACK)_ptrack=0;//aleady delete next
        for(int ilay=0;ilay<4;ilay++){
          _phith[ilay]=phith[ilay];
          if(phith[ilay]){phith[ilay]->setstatus(AMSDBc::USED);phith[ilay]->_pattern+=1000;}
        }
        _betapar=betapar;_betapar.Status|=status;
       }
    ~AMSBetaH(){};
    AMSBetaH *        next(){ return (AMSBetaH *)_next;}
    AMSTrTrack *      gettrack(){return _ptrack;}
    AMSTRDTrack*      gettrdtrack(){return _ptrdtrack;} 
    void              settrdtrack(AMSTRDTrack* ptrdtrack){_ptrdtrack=ptrdtrack;}
    void              setcharge(AMSCharge *amscharge){_pcharge=amscharge;}
    const TofBetaPar& getbetapar(){return _betapar;}   
public:
   void _printEl(ostream &stream){stream <<"BetaH "<<endl;}
   void _writeEl();
   void _copyEl();
   static integer Out(integer);
};
#endif



#endif
