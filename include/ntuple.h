//  $Id: ntuple.h,v 1.51 2002/03/20 09:43:10 choumilo Exp $
#ifndef __AMSNTUPLE__
#define __AMSNTUPLE__

#include <node.h>
#include <root.h>
#ifdef __WRITEROOT__
class AMSNtuple : public TObject, public AMSNode{
#else
class AMSNtuple : public AMSNode{
#endif
protected:
  integer _lun;
  integer _Nentries;
  EventNtuple02 _event02;
  BetaNtuple02 _beta02;
  ChargeNtuple02 _charge02;
  ParticleNtuple02 _part02;
  TOFClusterNtuple _tof;
  TOFMCClusterNtuple _tofmc;
  TrClusterNtuple _trcl;
  TrMCClusterNtuple _trclmc;
  TRDMCClusterNtuple _trdclmc;
  TRDClusterNtuple _trdcl;
  TRDSegmentNtuple _trdseg;
  TRDTrackNtuple _trdtrk;
  TRDRawHitNtuple _trdht;
  TrRecHitNtuple02 _trrh02;
  TrTrackNtuple02 _trtr02;
  MCTrackNtuple _mct;// tempor 02??
  MCEventGNtuple02 _mcg02;
  AntiClusterNtuple _anti;
  ANTIMCClusterNtuple _antimc;
  LVL3Ntuple02 _lvl302;
  LVL1Ntuple02 _lvl102;
  TrRawClusterNtuple _trraw;
  AntiRawClusterNtuple _antiraw;
  TOFRawClusterNtuple _tofraw;
  EcalClusterNtuple _ecclust;
  Ecal2DClusterNtuple _ec2dclust;
  EcalShowerNtuple _ecshow;
  EcalHitNtuple _ecalhit;
  RICMCNtuple _richmc;
  RICEventNtuple _richevent;
  RICRing _ring;
#ifdef __WRITEROOT__
  static TTree* _tree;  
  static TFile* _rfile;
//  static TROOT _troot;
#endif
  virtual void _init(){};

public:
  AMSNtuple():AMSNode(AMSID()),_lun(0){}
  AMSNtuple(char* name);
  ~AMSNtuple();
  AMSNtuple(integer lun, char* name);
  void init();
  uinteger getrun();
  void reset(int i=0);
  void write(integer addentry=0);
  void writeR();
  void endR();
  void initR(char* name);
  integer getentries(){return _Nentries;}
  EventNtuple02* Get_event02() {return &_event02;}
  BetaNtuple02* Get_beta02() { return &_beta02;}
  ChargeNtuple02* Get_charge02() { return &_charge02;}
  ParticleNtuple02* Get_part02() {return &_part02;}
  TOFClusterNtuple* Get_tof() {return &_tof;}
  TOFMCClusterNtuple* Get_tofmc() {return &_tofmc;}
  TrClusterNtuple* Get_trcl() {return &_trcl;}
  TrMCClusterNtuple* Get_trclmc() {return &_trclmc;}
  TRDMCClusterNtuple* Get_trdclmc() {return &_trdclmc;}
  TRDClusterNtuple* Get_trdcl() {return &_trdcl;}
  TRDSegmentNtuple* Get_trdseg() {return &_trdseg;}
  TRDTrackNtuple* Get_trdtrk() {return &_trdtrk;}
  TRDRawHitNtuple* Get_trdht() {return &_trdht;}
  TrRecHitNtuple02* Get_trrh02() {return &_trrh02;}
  TrTrackNtuple02* Get_trtr02() {return &_trtr02;}
  MCTrackNtuple* Get_mct() {return &_mct;}
  MCEventGNtuple02* Get_mcg02() {return &_mcg02;}
  AntiClusterNtuple* Get_anti() {return &_anti;}
  ANTIMCClusterNtuple* Get_antimc() {return &_antimc;}
  LVL3Ntuple02* Get_lvl302() {return &_lvl302;}
  LVL1Ntuple02* Get_lvl102() {return &_lvl102;}
  TrRawClusterNtuple* Get_trraw() {return &_trraw;}
  AntiRawClusterNtuple* Get_antiraw() {return &_antiraw;}
  TOFRawClusterNtuple* Get_tofraw() {return &_tofraw;}
  EcalClusterNtuple* Get_ecclust() {return &_ecclust;}
  Ecal2DClusterNtuple* Get_ec2dclust() {return &_ec2dclust;}
  EcalShowerNtuple* Get_ecshow() {return &_ecshow;}
  EcalHitNtuple* Get_ecalhit() {return &_ecalhit;}
  RICMCNtuple* Get_richmc() {return &_richmc;}
  RICEventNtuple* Get_richevent() {return &_richevent;}
  RICRing* Get_ring(){return &_ring;}

#ifdef __WRITEROOT__
  //ClassDef(AMSNtuple ,1)       //AMSNtuple
#endif
};

#endif


