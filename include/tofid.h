#ifndef __SC2ID__
#define __SC2ID__
#include "cern.h"
#include "commons.h"
#include "amsdbc.h"
#include "extC.h"
#include <string.h>
#include "typedefs.h"
#include <stdlib.h>
#include <iostream.h>
#include <fstream.h>
#include "tofdbc02.h"
#include "antidbc02.h"
#include "job.h"
//
//
class AMSSCIds{
 protected:
  int _dummy;
  int _dtype;//Detector-type(1-TOF,2-ANTI)
  int _swid;//TOF software ID is LBBSPM(Layer|Bar|Side|Pmt|Meas_type)
//         or ANTI ..............  BSPM(LogicBar|Side|Pmt=0|Meas_type)
//                                                                        
  int16u _layer; //0,1,...
  int16u _bar;   //0,1,...
  int16u _side;  //0,1
  int16u _mtyp;  //Meas.type:0/1/2->SlowTDC/FastTDC(hist)/Charge
  int16u _pmt; // 0->anode, 1-3 ->dynodes
  int16u _swch;//sequential s/w-channels numbering, for TOF:S=1->LBBSP(0)M(0),LBBSP(0)M(1),
//                                   LBBSP(0)M(2),LBBSP(1)M(2),LBBSP(2)M(2),(for P(3) if any);
//                                                     next S=2->.....the same sequence.......
//
//                                                  for ANTI:S=1->BSP(0)M(1),BSP(0)M(2),
//                                                     next S=2->...the same sequence
//---
  int _hwid;//hardware ID is CSRR(Crate|Slot|SlotReadout_channel)
  int16u _crate; //0,1,...
  int16u _slot;  //slot# 0,1,...
  int16u _sltyp;  //slot(card) type : 1->SFET/2->SFEA1/3->SFEA2/4->SFEC/5->SDR/6->SPT...
//                    SFEA1=pureANTI(4inputs), SFEA2=combined(4ANTI-inpts, 2TOF-inpts)
  int16u _crdid; //card id(may be different from slot#)
  int16u _rdch;  //readout channel(internal card-output numbering)
  int16u _inpch; //inp.channel(internal card-input numbering)
  int16u _hwch;//sequential h/w-channels numbering: C(1)S(1)RR(1),...C(1)S(1)RR(Nmax for slot_1),
// C(1)S(2)RR(1),...C(1)S(2)RR(max for slot_2),..............
// C(Nmax)S(max slots in this cr)RR(max for this slot)
//--- 
  static int sidlst[TOF2GC::SCRCMX];//swid-list(vs hwch) 
  static int hidlst[TOF2GC::SCRCMX];//hwid-list(vs swch)
  static int hidls[TOF2GC::SCRCMX];//hwid-list(vs hwch)
  static int16u sltymap[TOF2GC::SCCRAT][TOF2GC::SCSLTM];//map of slot-types in crates 
  static int16u cardids[TOF2GC::SCCRAT][TOF2GC::SCSLTM];
  static int16u ochpsty[TOF2GC::SCSLTY];//outp.channels per slot-type
  static int totswch[2];//total s/w-channels for TOF, ANTI
  static int tmpsln[TOF2GC::SCSLTM];//map of temp-sensor numbers in slots
 public:
  AMSSCIds():_dummy(1){};
  AMSSCIds(int16u crate, int16u slot, int16u rdch);//used for tof,anti or temperature
  AMSSCIds(int16u layer, int16u bar, int16u side, int16u pmt, int16u mtyp);//used for tof
  AMSSCIds(int16u bar, int16u side, int16u pmt, int16u mtyp);//used for anti
  AMSSCIds(int swid);//used for tof,anti or temper
  static void inittable();
  bool dummy(){return _dummy==1;}
  bool AchOK(){return TOF2Brcal::scbrcal[_layer][_bar].AchOK(_side) &&
                      TOFBPeds::scbrped[_layer][_bar].PedAchOK(_side);}
  bool DchOK(){return TOF2Brcal::scbrcal[_layer][_bar].DchOK(_side,_pmt) &&
                      TOFBPeds::scbrped[_layer][_bar].PedDchOK(_side,_pmt);}
		      
		 
		 
  integer getshortid()const {return (_layer+1)*1000+(_bar+1)*10+_side+1;}
  int16u getcrate(){return _crate;}
  int16u getslot(){return _slot;}
  int16u getsltyp(){return _sltyp;}
  int16u getcrdid(){return _crdid;}
  int16u getrdch(){return _rdch;}
  int16u getinpch(){return _inpch;}
  int16u getpmt(){return _pmt;}
  int16u gettsn(int16u slot){return tmpsln[slot];}//1,...5, or "0" if not temper. slot(0,1,...)
  int gethwid(){return _hwid;}
  static int16u swseqn(int dt, int16u il, int16u ib, int16u is, int16u ip, int16u im);
  static int16u hwseqn(int16u cr, int16u sl, int16u ch);
  static int hw2swid(int16u cr, int16u sl, int16u ch);
  static int16  crdid2sl(int16u crate, int16u crdid);
};


#endif
