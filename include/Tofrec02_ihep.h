//  $Id: Tofrec02_ihep.h,v 1.4 2012/07/19 13:17:52 qyan Exp $

//Author Qi Yan 2012/June/09 10:03 qyan@cern.ch  /*IHEP TOF version*/
#ifndef __AMSTOFREC02_IHEP__
#define __AMSTOFREC02_IHEP__
#include "root.h"
#include "TObject.h"
#ifndef __ROOTSHAREDLIBRARY__
#include "link.h"
//#include "tofrec02.h"
//////////////////////////////////////////////////////////////////////////
class AMSTOFClusterH;
class AMSCharge;
#endif
class TofBetaPar;

/*!
  IHEP Recontruction Code both Support Production and Root Mode Refit(B584 or earier production data)
*/
/*!
 \author qyan@cern.ch
*/
//////////////////////////////////////////////////////////////////////////
class TofRecH{

/// Data part
protected:
/// light velocity
  static const double cvel=29.9792;//c vel(cm/ns)
/// Fit Beta Par
  static TofBetaPar betapar;
/// Process Event
  static AMSEventR *ev;
 
/// TofRawSideR vector for TofClusterH build
  static vector<TofRawSideR> tfraws;
#ifndef __ROOTSHAREDLIBRARY__
  static vector<TOF2RawSide*> tf2raws;
#endif

/// TofClusterH 4layer vector  pointer
  static vector<TofClusterHR*>tofclh[4];
/// Track vector pointer
  static vector<TrTrackR*> track;
#ifndef __ROOTSHAREDLIBRARY__
  static vector<AMSTrTrack*> amstrack;
#endif
/// TrdTrack vector
  static vector<TrdTrackR>trdtrack;
#ifndef __ROOTSHAREDLIBRARY__
  static vector<AMSTRDTrack*>amstrdtrack;
#endif
  
/// Sum Build Part
public:
/// default construction
  TofRecH(){};
/// TDV initial function
  static int  Init();
/// Root Mode Refit //if want to rebuild BetaH in Root Mode, Only need one function
  static int  ReBuild();
/// Build TofClusterH function
  static int  BuildTofClusterH();
/// Build BetaH function
  static int  BuildBetaH(int mode=0);

/// TofClusterH Build Part
public:
/// Finding LT from TofRawSide 
  static int  TofSideRec(TofRawSideR *ptr,number &adca, integer &nadcd,number adcd[],number &sdtm,uinteger &sstatus,
                        vector<number>&ltdcw, vector<number>&htdcw, vector<number>&shtdcw);
/// Time build from TofRawSide
  static int  TimeCooRec(int idsoft,number sdtm[],number adca[],number tms[2],number &tm,number &etm,number &lcoo,number &elcoo,uinteger &status);
/// Energy build Module
  static int  EdepRec();
/// ReFind LT if many LT not Associate with HT
  static int  LTRefind(int idsoft,number trlcoo,number sdtm[2],number adca[2],uinteger &status, vector<number>ltdcw,int hassid);

/// BetaH Build Part
public:
/// Find TofClusterH for ilay with Track
#if (defined _PGTRACK_) || (defined __ROOTSHAREDLIBRARY__)
  static int  BetaFindTOFCl(TrTrackR *ptrack,   int ilay,TofClusterHR** tfhit,number &tklen,number &tklcoo,number cres[2],int &pattern);
#else
  static int  BetaFindTOFCl(AMSTrTrack *ptrack,int ilay,TofClusterHR** tfhit,number &tklen,number &tklcoo,number cres[2],int &pattern);
#endif
/// Recover Time information if One Side lost Signal
  static int  TRecover(TofClusterHR *tfhit[4],number tklcoo[4]);//using hassid to recover other side
  static int  TRecover(int idsoft,geant trlcoo,geant tms[2],geant &tm,geant &etm,uinteger &status,int hassid);
/// Coo Chi2 Fit function 
  static int  BetaFitC(TofClusterHR *tfhit[4],number res[4][2],int partten[4],TofBetaPar &par,int mode);//
/// Beta Fit function
  static int  BetaFitT(TofClusterHR *tfhit[4],number len[4],int partten[4],TofBetaPar &par,int mode);//mode same etime weight(0) or not(1)
  static int  BetaFitT(number time[],number etime[],number len[],const int nhits,TofBetaPar &par,int mode=1);//mode same etime weight(0) or not(1)
/// Beta Check function
  static int  BetaFitCheck(TofBetaPar &par);//if this is normal value
#ifndef __ROOTSHAREDLIBRARY__
  static number BetaCorr(number zint,number z0,number part,uinteger &status);//to BetaC Vitaly
#endif
/// Mass Cal function
  static int  MassRec(TofBetaPar &par,number rig=0,number charge=0,number evrig=0,int isubetac=0);//evrig=E(1/rig)

/// Other function
public:
/// Sort TofRawSide accoding to BarId
  static bool SideCompare(const TofRawSideR& a,const TofRawSideR& b){return a.swid<b.swid;}
/// Sort TofRawSide Index 
  static bool IdCompare(const pair<integer,integer> &a,const pair<integer,integer> &b){return a.second<b.second;}
/// ParticleR ChargeR Build Link index to BetaH
  static int  BetaHLink(TrTrackR* ptrack,TrdTrackR *trdtrack);
#ifdef __WRITEROOT__
  friend class BetaHR; 
  friend class TofBetaPar; 
#endif

  ClassDef(TofRecH,1)
};

//////////////////////////////////////////////////////////////////////////
#ifndef __ROOTSHAREDLIBRARY__
class AMSTOFClusterH: public AMSlink,public TofClusterHR {

protected:
    integer  _idsoft;
    TOF2RawSide *_tfraws[2]; 
public:
    AMSTOFClusterH(){};
    AMSTOFClusterH(uinteger sstatus[2],uinteger status,integer pattern,integer idsoft,number adca[],number adcd[][TOF2GC::PMTSMX],
                   number sdtm[],number timers[],number timer,number etimer,
                    AMSPoint coo,AMSPoint ecoo,number edepa,number edepd,TOF2RawSide *tfraws[2]): AMSlink(status)
                 {
                      TofRawSideR *raws[2]={0};
                      TofClusterHR(sstatus,status,pattern,idsoft,adca,adcd,sdtm,timers,timer,etimer,coo,ecoo,edepa,edepd,raws);
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
#endif



#endif
