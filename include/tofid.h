#ifndef __TOF2ID__
#define __TOF2ID__
#include <cern.h>
#include <commons.h>
#include <amsdbc.h>
#include <extC.h>
#include <string.h>
#include <typedefs.h>
#include <stdlib.h>
#include <iostream.h>
#include <fstream.h>
#include <tofdbc02.h>
#include <job.h>
//
//
class AMSTOFIds{
 protected:
  int _dummy;
  int16u _swid;//software ID is LBBSM(Layer|Bar|Side|Meas_type)
  int16u _hwid;//hardware ID is CSRR(Crate|Slot|Readout_channel)
  int16 _layer; //0,1,...
  int16u _bar;   //0,1,...
  int16u _side;  //0,1
  int16u _mtyp;  //Meas.type:0/1/2/3/4->FastTDC/SlowTDC/AnodeAmp/DynHGainAmp/DynLGainAmp
  int16u _crate; //0,1,...
  int16u _slot;  //slot# 0,1,...
  int16u _sltyp;  //slot type : 0->SFET/1->SFEC/...
  int16u _crdid; //card id(may be different from slot#)
  int16u _rdch;  //readout channel(card output number)
  int16u _inpch; //card input number
  static int16u sidlst[TOF2GC::SCCRAT*TOF2GC::SCSLOT*TOF2GC::SCRDCH];//swid-list(vs hwch) 
  static int16u hidlst[TOF2GC::SCLRS*TOF2GC::SCMXBR*2*TOF2GC::SCMTYP];//hwid-list(vs swch)
  static int16u sltypes[TOF2GC::SCCRAT][TOF2GC::SCSLOT]; 
  static int16u cardids[TOF2GC::SCCRAT][TOF2GC::SCSLOT]; 
 public:
  AMSTOFIds():_dummy(1){};
  AMSTOFIds(int16u crate, int16u slot, int16u rdch);
  AMSTOFIds(int16u layer, int16u bar, int16u side, int16u mtyp);
  AMSTOFIds(int16u swid);
  static void inittable();
  integer getshortid()const {return (_layer+1)*1000+(_bar+1)*10+_side+1;}
  bool dummy(){return _dummy==1;}
  bool AchOK(){return TOF2Brcal::scbrcal[_layer][_bar].AchOK(_side) &&
                      TOFBPeds::scbrped[_layer][_bar].PedAchOK(_side);}
  bool HchOK(){return TOF2Brcal::scbrcal[_layer][_bar].HchOK(_side) &&
                      TOFBPeds::scbrped[_layer][_bar].PedHchOK(_side);}
		      
  bool LchOK(){return TOF2Brcal::scbrcal[_layer][_bar].LchOK(_side) &&
                      TOFBPeds::scbrped[_layer][_bar].PedLchOK(_side);}
		 
		 
  int16u getcrate(){return _crate;}
  int16u getslot(){return _slot;}
  int16u getsltyp(){return _sltyp;}
  int16u getcrdid(){return _crdid;}
  int16u getrdch(){return _rdch;}
  int16u getinpch(){return _inpch;}
  int16  crdid2sl(int16u crate, int16u crdid);
  int16u gethwid(){return _hwid;}
};


#endif
