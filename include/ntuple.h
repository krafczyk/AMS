#ifndef __AMSNTUPLE__
#define __AMSNTUPLE__

#include <node.h>
#include <root.h>

class AMSNtuple : public AMSNode{

protected:
  integer _lun;
  integer _Nentries;
  EventNtuple _event;
  BetaNtuple _beta;
  ChargeNtuple _charge;
  ParticleNtuple _part;
  TOFClusterNtuple _tof;
  TOFMCClusterNtuple _tofmc;
  TrClusterNtuple _trcl;
  TrMCClusterNtuple _trclmc;
  TrRecHitNtuple _trrh;
  TrTrackNtuple _trtr;
  MCEventGNtuple _mcg;
  CTCClusterNtuple _ctccl;
  CTCMCClusterNtuple _ctcclmc;
  AntiClusterNtuple _anti;
  ANTIMCClusterNtuple _antimc;
  LVL3Ntuple _lvl3;
  LVL1Ntuple _lvl1;
  CTCHitNtuple _ctcht;
  TrRawClusterNtuple _trraw;
  AntiRawClusterNtuple _antiraw;
  TOFRawClusterNtuple _tofraw;
  EcalClusterNtuple _ecclust;
#ifdef __WRITEROOT__
  static TTree* _tree;  
  static TFile* _rfile;
#endif
  virtual void _init(){};

public:
  AMSNtuple():AMSNode(AMSID()),_lun(0){}
  AMSNtuple(char* name);
  ~AMSNtuple();
  AMSNtuple(integer lun, char* name);
  void init();
  void reset(int i=0);
  void write(integer addentry=0);
  void writeR();
  void initR(char* name);
  integer getentries(){return _Nentries;}
  EventNtuple* Get_event() {return &_event;}
  BetaNtuple* Get_beta() { return &_beta;}
  ChargeNtuple* Get_charge() { return &_charge;}
  ParticleNtuple* Get_part() {return &_part;}
  TOFClusterNtuple* Get_tof() {return &_tof;}
  TOFMCClusterNtuple* Get_tofmc() {return &_tofmc;}
  TrClusterNtuple* Get_trcl() {return &_trcl;}
  TrMCClusterNtuple* Get_trclmc() {return &_trclmc;}
  TrRecHitNtuple* Get_trrh() {return &_trrh;}
  TrTrackNtuple* Get_trtr() {return &_trtr;}
  MCEventGNtuple* Get_mcg() {return &_mcg;}
  CTCClusterNtuple* Get_ctccl() { return &_ctccl;}
  CTCMCClusterNtuple* Get_ctcclmc() {return &_ctcclmc;}
  AntiClusterNtuple* Get_anti() {return &_anti;}
  ANTIMCClusterNtuple* Get_antimc() {return &_antimc;}
  LVL3Ntuple* Get_lvl3() {return &_lvl3;}
  LVL1Ntuple* Get_lvl1() {return &_lvl1;}
  CTCHitNtuple* Get_ctcht() {return &_ctcht;}
  TrRawClusterNtuple* Get_trraw() {return &_trraw;}
  AntiRawClusterNtuple* Get_antiraw() {return &_antiraw;}
  TOFRawClusterNtuple* Get_tofraw() {return &_tofraw;}
  EcalClusterNtuple* Get_ecclust() {return &_ecclust;}

//ClassDef(AMSNtuple ,1)       //AMSNtuple
};

#endif
