//  $Id: tofhit.h,v 1.3 2012/09/09 16:28:47 qyan Exp $

//Author Qi Yan 2012/Jul/19 10:03 qyan@cern.ch
#ifndef __AMSTOFHIT__
#define __AMSTOFHIT__
#include "root.h"
#include "link.h"
#include "Tofdbc.h"
#include "charge.h"
#include "trdrec.h"
#include "Tofrec02_ihep.h"
//////////////////////////////////////////////////////////////////////////
class TOF2RawSide;
class AMSCharge;
class AMSTrTrack;
class AMSTRDTrack;
//////////////////////////////////////////////////////////////////////////
class AMSTOFClusterH: public AMSlink,public TofClusterHR {

protected:
    integer  _idsoft;
    TOF2RawSide *_tfraws[2];
public:
    AMSTOFClusterH(){};
    AMSTOFClusterH(uinteger sstatus[2],uinteger status,integer pattern,integer idsoft,number adca[],number adcd[][TOF2GC::PMTSMX],
                   number sdtm[],number timers[],number timer,number etimer,AMSPoint coo,AMSPoint ecoo,
                   number q2pa[],number q2pd[][TOF2GC::PMTSMX],number edepa,number edepd,TOF2RawSide *tfraws[2],TofRawSideR *raws[2]): AMSlink(status),
                   TofClusterHR(sstatus,status,pattern,idsoft,adca,adcd,sdtm,timers,timer,etimer,coo,ecoo,q2pa,q2pd,edepa,edepd,raws)
      {
          for(int i=0;i<2;i++)_tfraws[i]=tfraws[i]; _idsoft=idsoft;
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
/////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
class AMSBetaH: public AMSlink,public BetaHR {

protected:
   AMSTOFClusterH * _phith[4];
   AMSTrTrack *     _ptrack;//trdtrack delete not exis/should be careful
   AMSTRDTrack *    _ptrdtrack;
   AMSCharge  *     _pcharge;

public:
    AMSBetaH(){};
    AMSBetaH(TofClusterHR *phith[4],AMSTrTrack *ptrack,AMSTRDTrack *trdtrack,TofBetaPar betapar):
         AMSlink(), _ptrack(ptrack),_ptrdtrack(trdtrack),BetaHR(){
        if((betapar.Status&TOFDBcN::TRDTRACK)==TOFDBcN::TRDTRACK)_ptrack=0;//aleady delete next
        for(int ilay=0;ilay<4;ilay++){
           _phith[ilay]=dynamic_cast<AMSTOFClusterH *>(phith[ilay]);
          if(_phith[ilay]){_phith[ilay]->Status|=TOFDBcN::USED;_phith[ilay]->Pattern+=1000;}
        }
        BetaPar=betapar;
        _pcharge=0;
       }
    ~AMSBetaH(){};
    AMSBetaH *        next(){ return (AMSBetaH *)_next;}
    AMSTrTrack *      gettrack(){return _ptrack;}
    AMSTRDTrack*      gettrdtrack(){return _ptrdtrack;}
    void              settrdtrack(AMSTRDTrack* ptrdtrack){_ptrdtrack=ptrdtrack;}
    void              setcharge(AMSCharge *amscharge){_pcharge=amscharge;}
    const TofBetaPar& getbetapar(){return BetaPar;}
public:
   void  MassRec(number rig=0,number charge=0,number evrig=0,int isubetac=0){TofRecH::MassRec(BetaPar,rig,charge,evrig,isubetac);}
   void _printEl(ostream &stream){stream <<"BetaH "<<endl;}
   void _writeEl();
   void _copyEl();
   static integer Out(integer);
   friend class TofRecH;
};

/////////////////////////////////////////////////////////////////////////

#endif

