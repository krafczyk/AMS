#include <tofid.h>
using namespace TOF2GC;
  int16u AMSTOFIds::hidlst[TOF2GC::SCLRS*TOF2GC::SCMXBR*2*TOF2GC::SCMTYP];//hwid-list(vs swch)
  int16u AMSTOFIds::sltypes[TOF2GC::SCCRAT][TOF2GC::SCSLOT]; 
  int16u AMSTOFIds::cardids[TOF2GC::SCCRAT][TOF2GC::SCSLOT]; 
//------
AMSTOFIds::AMSTOFIds(int16u swid):_dummy(0){
  int16u swch;
//
  _swid=swid;//LBBSM
  _layer=swid/10000-1;
  _bar=(swid%10000)/100-1;
  _side=(swid%100)/10-1;
  _mtyp=swid%10-1;
  swch=_layer*SCMXBR*2*SCMTYP+_bar*2*SCMTYP+_side*SCMTYP+_mtyp;
  _hwid=hidlst[swch];//CSRR
  if(_hwid>0){
    _crate=_hwid/1000-1;
    _slot=(_hwid%1000)/100-1;
    _rdch=_hwid%100-1;
    _sltyp=sltypes[_crate][_slot];
    _crdid=cardids[_crate][_slot];
  }
  else{
    _dummy=1;
  }
}
//------
AMSTOFIds::AMSTOFIds(int16u layer, int16u bar, int16u side, int16u mtyp):_dummy(0){
  int16u swch;
//
  _swid=10000*(layer+1)+100*(bar+1)+10*(side+1)+mtyp+1;//LBBSM
  _layer=layer;
  _bar=bar;
  _side=side;
  _mtyp=mtyp;
  swch=_layer*SCMXBR*2*SCMTYP+_bar*2*SCMTYP+_side*SCMTYP+_mtyp;
  _hwid=hidlst[swch];//CSRR
  if(_hwid>0){
    _crate=_hwid/1000-1;
    _slot=(_hwid%1000)/100-1;
    _rdch=_hwid%100-1;
    _sltyp=sltypes[_crate][_slot];
    _crdid=cardids[_crate][_slot];
  }
  else{
    _dummy=1;
  }
}
//------
AMSTOFIds::AMSTOFIds(int16u crate, int16u slot, int16u rdch):_crate(crate),_slot(slot),
                                          _rdch(rdch),_dummy(0){
  int16u hwch;
//
  _hwid=(_crate+1)*1000+(_slot+1)*100+(_rdch+1);//CSRR
  hwch=_crate*SCSLOT*SCRDCH+_slot*SCRDCH+_rdch;
  _sltyp=sltypes[_crate][_slot];
  _crdid=cardids[_crate][_slot];
  _swid=sidlst[hwch];//LBBSM
  if(_swid>0){
    _layer=_swid/10000-1;
    _bar=(_swid%10000)/100-1;
    _side=(_swid%100)/10-1;
    _mtyp=_swid%10-1;
  }
  else{
    _dummy=1;
  }
}
//-------
int16 AMSTOFIds::crdid2sl(int16u crate, int16u crdid){//sequential slot# vs crdid
  for(int i=0;i<SCSLOT;i++){
    if(cardids[_crate][i]==crdid)return(i);
  }
  return(-1);//if illegal(not existing) crdid
}
//--------
void AMSTOFIds::inittable(){
}
  int16u AMSTOFIds::sidlst[TOF2GC::SCCRAT*TOF2GC::SCSLOT*TOF2GC::SCRDCH]={
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,  //sl1cr1(24ch)						       
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,  //sl2cr1(24ch)						       
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,  //sl3cr1(24ch)						       
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,  //sl4cr1(24ch)						       
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,  //sl5cr1(24ch)						       
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,  //sl6cr1(24ch)
    						       
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,  //sl1cr2(24ch)						       
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,  //sl2cr2(24ch)						       
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,  //sl3cr2(24ch)						       
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,  //sl4cr2(24ch)						       
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,  //sl5cr2(24ch)						       
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,  //sl6cr2(24ch)
    						       
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,  //sl1cr3(24ch)						       
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,  //sl2cr3(24ch)						       
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,  //sl3cr3(24ch)						       
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,  //sl4cr3(24ch)						       
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,  //sl5cr3(24ch)						       
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,  //sl6cr3(24ch)
    						       
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,  //sl1cr4(24ch)						       
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,  //sl2cr4(24ch)						       
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,  //sl3cr4(24ch)						       
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,  //sl4cr4(24ch)						       
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,  //sl5cr4(24ch)						       
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0   //sl6cr4(24ch)
    						       
  }; 





