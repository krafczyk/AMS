#include "tofid.h"
using namespace TOF2GC;
using namespace ANTI2C;
  int AMSSCIds::totswch[2];//total s/w-channels for TOF and ANTI
  int AMSSCIds::sidlst[SCRCMX];//swid-list(vs hwch) 
  int AMSSCIds::hidlst[SCRCMX];//hwid-list(vs swch)
  int AMSSCIds::hidls[SCRCMX];//hwid-list(vs hwch)
//outp.channels for each slot(card) type: 
  int16u AMSSCIds::ochpsty[TOF2GC::SCSLTY]={13,9,20,0,0};
//slot(card)-type map in crates  
  int16u AMSSCIds::sltymap[SCCRAT][SCSLTM]={4,1,1,5,1,1,2,3,3,  //top cr-1
                                            4,1,1,5,1,1,2,3,3,  //top cr-2
					    4,1,1,5,1,1,2,3,3,  //bot cr-3
					    4,1,1,5,1,1,2,3,3   //bot cr-4
					    };
//map of temp-sensors ids in crate's slots:
  int16u AMSSCIds::tmpsln[SCSLTM]={0,1,2,0,3,4,5,0,0};// same for all crates(id=sensor_seq.number)
//  (because each SFET(A) has 1 temp-sensor, above array gives also SFET(A) sequential number =1,2,3,4,5)
//slot-id's map: 
  int16u AMSSCIds::cardids[SCCRAT][SCSLTM]={ 1, 2, 3, 4, 5, 6, 7, 8, 9, //cr-1
                                             10,11,12,13,14,15,16,17,18, //cr-2 
                                             19,20,21,22,23,24,25,26,27, //cr-3
					     28,29,30,31,32,33,34,35,36  //cr-4
					    };
//------
AMSSCIds::AMSSCIds(int swid):_dummy(0){
//
int16u slt;
  _swid=swid;//LBBSPM(tof) or BSPM(anti)
  _layer=swid/100000;
  _mtyp=swid%10;
  if(_layer==0)_dtype=2;//anti
  else{
    _dtype=1;//tof
    _layer-=1;
  }
  _bar=(swid%100000)/1000-1;
  _side=(swid%1000)/100-1;
  _pmt=(swid%100)/10;

//_swch => glogal sequential sw-channel(TOF+ANTI, incl pmts and mtyps)
  _swch=swseqn(_dtype,_layer,_bar,_side,_pmt,_mtyp);
  
  _hwid=hidlst[_swch];//CSRR
  if(_hwid>0){
    _crate=_hwid/1000-1;
    _slot=(_hwid%1000)/100-1;
    _rdch=_hwid%100-1;
    _sltyp=sltymap[_crate][_slot];
    _crdid=cardids[_crate][_slot];
    _hwch=hwseqn(_crate,_slot,_rdch);//global sequential hw/channel
  }
  else{
    _dummy=1;
    cout<<"TofAntConstructor: non-phys s/w-id,(L)BBSPM="<<swid<<" swch="<<_swch<<endl;
  }
}
//------
AMSSCIds::AMSSCIds(int16u layer, int16u bar, int16u side, int16u pmt, int16u mtyp):_dummy(0){
//constructor for TOF
  _dtype=1;
  _swid=100000*(layer+1)+1000*(bar+1)+100*(side+1)+10*pmt+mtyp;//LBBSPM
  _layer=layer;//0,1,...
  _bar=bar;//0,1,...
  _side=side;
  _pmt=pmt;//0,1,2,3(Anode,Dyn1,Dyn2,Dyn3)
  _mtyp=mtyp;//0,1,2,3,4(t,Q,ft,sumHT,sumSHT)
  
//_swch => glogal sequential sw-channel(TOF+ANTI, incl pmts and mtyps)
  _swch=swseqn(_dtype,_layer,_bar,_side,_pmt,_mtyp);
  
  _hwid=hidlst[_swch];//CSRR
  if(_hwid>0){
    _crate=_hwid/1000-1;
    _slot=(_hwid%1000)/100-1;
    _rdch=_hwid%100-1;
    _sltyp=sltymap[_crate][_slot];
    _crdid=cardids[_crate][_slot];
    _hwch=hwseqn(_crate,_slot,_rdch);//global sequential hw/channel
  }
  else{
    _dummy=1;
    cout<<"TofConstructor:non-phys. s/w-chan: L/BB/S/P/M="<<layer<<"/"<<bar<<"/"<<side<<"/"<<pmt<<"/"<<mtyp<<endl;
    cout<<"                         swid="<<_swid<<" swch="<<_swch<<endl;
  }
}
//------
AMSSCIds::AMSSCIds(int16u bar, int16u side, int16u pmt, int16u mtyp):_dummy(0){
//constructor for ANTI
  _dtype=2;
  _swid=1000*(bar+1)+100*(side+1)+10*pmt+mtyp;//BSPM
  _layer=0;//0
  _bar=bar;//0,1,...
  _side=side;//0,1
  _pmt=pmt;//0(Anode only)
  _mtyp=mtyp;//0,1,2(t,Q,ft)
  
//_swch => glogal sequential sw-channel(TOF+ANTI, incl pmts and mtyps)
  _swch=swseqn(_dtype,_layer,_bar,_side,_pmt,_mtyp);

  _hwid=hidlst[_swch];//CSRR
  if(_hwid>0){
    _crate=_hwid/1000-1;
    _slot=(_hwid%1000)/100-1;
    _rdch=_hwid%100-1;
    _sltyp=sltymap[_crate][_slot];
    _crdid=cardids[_crate][_slot];
    _hwch=hwseqn(_crate,_slot,_rdch);//global sequential hw-channel
  }
  else{
    _dummy=1;
    cout<<"AntiConstructor:non-phys. s/w-chan: bar/side/pm/mtyp="<<bar<<"/"<<side<<"/"<<pmt<<"/"<<mtyp<<endl;
    cout<<"   swid="<<_swid<<" swch="<<_swch<<" hwid="<<_hwid<<endl;
  }
}
//------
AMSSCIds::AMSSCIds(int16u crate, int16u slot, int16u rdch):_crate(crate),_slot(slot),
                                          _rdch(rdch),_dummy(0){
//
  _hwid=(_crate+1)*1000+(_slot+1)*100+(_rdch+1);//CSRR
  _sltyp=sltymap[_crate][_slot];
  _crdid=cardids[_crate][_slot];
  _hwch=hwseqn(crate,slot,rdch);//global sequential hw-channel
  _swid=sidlst[_hwch];//LBBSPM or BSPM

  if(_swid>0){
    _layer=_swid/100000;
    _mtyp=_swid%10;
    if(_layer==0)_dtype=2;//anti
    else{
      _dtype=1;//tof
      _layer-=1;
    }
    _bar=(_swid%100000)/1000-1;
    _side=(_swid%1000)/100-1;
    _pmt=(_swid%100)/10;
//  _swch => glogal sequential sw-channel(TOF+ANTI, incl pmts and mtyps)
    _swch=swseqn(_dtype,_layer,_bar,_side,_pmt,_mtyp);
  }//endof "swid>0" check
  else{
    _dummy=1;
    cout<<"TofAntiConstructor: empty readout-channel, crat/slot/rdch="<<crate<<"/"<<slot<<"/"<<rdch<<endl;
  }
}
//--------------
int16 AMSSCIds::crdid2sl(int16u crate, int16u crdid){//sequential slot# vs crdid
  for(int i=0;i<SCSLTM;i++){
    if(cardids[crate][i]==crdid)return(i);
  }
  return(-1);//if illegal(not existing) crdid
}
//--------------
void AMSSCIds::inittable(){
//
//--->count total TOF s/w-channels:
//
  int ttswch(0);//total TOF sequential sw-channels(incl pmt and mtyp)
  for(int il=0;il<TOF2DBc::getnplns();il++){
    for(int ib=0;ib<TOF2DBc::getbppl(il)+2;ib++){//"+2"-> 2 fict.paddles for 2 
//                                                 group-measurements(1st and 2nd half of the plane)
      if(ib>=TOF2DBc::getbppl(il))ttswch+=SCHPMTS;//measur.types for fict.paddles
      else ttswch+=(TOF2DBc::npmtps(il,ib)*SCDMTS+SCAMTS);
    }
  }
  ttswch*=2;//2sides
  totswch[0]=ttswch;
//
//--->count total ANTI s/w-channels:
//
  int taswch(0);//total ANTI sequential sw-channels(incl mtyp)
  taswch=MAXANTI*ANAMTS+2*ANAGMTS;//"2" means 2 fict.sectors for 2 group-measurements(cylinder's 1st/2nd half)
  taswch*=2;//2sides
  totswch[1]=taswch;
//
  cout<<"AMSSCIds::init:total s/w-channels(real tof/anti)="<<totswch[0]<<" "<<totswch[1]<<endl;
//
//--->fill sid's as function of sequential number of h/w-channel
//below used: tof_swid->LBBSPM, anti_swid->BSPM.
//
  int bias(0);
// <=========================================== crate-SC0 (top/wake)
//--->TOF(anodes,SFET):
  sidlst[bias]=   209202;//ft l2/fb9/s2  cr0/sl_2
  sidlst[bias+1]= 201200;//t      b1/s2  
  sidlst[bias+2]= 201201;//q
  sidlst[bias+3]= 203200;//t      b3/s2
  sidlst[bias+4]= 203201;//q
  sidlst[bias+5]= 205200;//t      b5/s2
  sidlst[bias+6]= 205201;//q
  sidlst[bias+7]= 207200;//t      b7/s2
  sidlst[bias+8]= 207201;//q
  sidlst[bias+9]= 0;
  sidlst[bias+10]=0;
  sidlst[bias+11]=209203;//SumHT, fb9/s2
  sidlst[bias+12]=209204;//SumSHT fb9/s2
  bias+=13;
  sidlst[bias]=   210202;//ft l2/fb10/s2 cr0/sl_3
  sidlst[bias+1]= 202200;//t       b2/s2 
  sidlst[bias+2]= 202201;//q
  sidlst[bias+3]= 204200;//t       b4/s2
  sidlst[bias+4]= 204201;//q
  sidlst[bias+5]= 206200;//t       b6/s2
  sidlst[bias+6]= 206201;//q
  sidlst[bias+7]= 208200;//t       b8/s2
  sidlst[bias+8]= 208201;//q
  sidlst[bias+9]= 0;
  sidlst[bias+10]=0;
  sidlst[bias+11]=210203;//SumHT, fb10/s2
  sidlst[bias+12]=210204;//SumSHT,fb10/s2
  bias+=13;
  sidlst[bias]=   109102;//ft l1/fb9/s1  cr0/sl_5
  sidlst[bias+1]= 101100;//t      b1/s1  
  sidlst[bias+2]= 101101;//q
  sidlst[bias+3]= 103100;//t      b3/s1
  sidlst[bias+4]= 103101;//q
  sidlst[bias+5]= 105100;//t      b5/s1
  sidlst[bias+6]= 105101;//q
  sidlst[bias+7]= 107100;//t      b7/s1
  sidlst[bias+8]= 107101;//q
  sidlst[bias+9]= 0;
  sidlst[bias+10]=0;
  sidlst[bias+11]=109103;//SumHT, fb9/s1
  sidlst[bias+12]=109104;//SumSHT fb9/s1
  bias+=13;
  sidlst[bias]=   110102;//ft l1/fb10/s1 cr0/sl_6
  sidlst[bias+1]= 102100;//t       b2/s1 
  sidlst[bias+2]= 102101;//q
  sidlst[bias+3]= 104100;//t       b4/s1
  sidlst[bias+4]= 104101;//q
  sidlst[bias+5]= 106100;//t       b6/s1
  sidlst[bias+6]= 106101;//q
  sidlst[bias+7]= 108100;//t       b8/s1
  sidlst[bias+8]= 108101;//q
  sidlst[bias+9]= 0;
  sidlst[bias+10]=0;
  sidlst[bias+11]=110103;//SumHT, fb10/s1
  sidlst[bias+12]=110104;//SumSHT,fb10/s1 
  bias+=13;
//--->ANTI(anodes,SFEA):
  sidlst[bias]=   9202;//ft     fb9/s2  cr0/sl_7
  sidlst[bias+1]= 1200;//t       b1/s2
  sidlst[bias+2]= 1201;//q  
  sidlst[bias+3]= 2200;//t       b2/s2 
  sidlst[bias+4]= 2201;//q  
  sidlst[bias+5]= 3200;//t       b3/s2 
  sidlst[bias+6]= 3201;//q  
  sidlst[bias+7]= 4200;//t       b4/s2 
  sidlst[bias+8]= 4201;//q  
  bias+=9;
//--->TOF(dynodes,SFEC):
  sidlst[bias]=   101111;//q  l1/b1/s1/d1  cr0/sl_8(aux)
  sidlst[bias+1]= 102111;//q     b2/s1/d1 
  sidlst[bias+2]= 103111;//q     b3/s1/d1 
  sidlst[bias+3]= 104111;//q     b4/s1/d1 
  sidlst[bias+4]= 105111;//q     b5/s1/d1 
  sidlst[bias+5]= 106111;//q     b6/s1/d1 
  sidlst[bias+6]= 107111;//q     b7/s1/d1 
  sidlst[bias+7]= 108111;//q     b8/s1/d1 
  sidlst[bias+8]= 101131;//q     b1/s1/d3 
  sidlst[bias+9]= 0;//             empty
  sidlst[bias+10]=101121;//q     b1/s1/d2 
  sidlst[bias+11]=102121;//q     b2/s1/d2 
  sidlst[bias+12]=103121;//q     b3/s1/d2 
  sidlst[bias+13]=104121;//q     b4/s1/d2 
  sidlst[bias+14]=105121;//q     b5/s1/d2 
  sidlst[bias+15]=106121;//q     b6/s1/d2 
  sidlst[bias+16]=107121;//q     b7/s1/d2 
  sidlst[bias+17]=108121;//q     b8/s1/d2 
  sidlst[bias+18]=108131;//q     b8/s1/d3 
  sidlst[bias+19]=0;//             empty
  bias+=20;  
  sidlst[bias]=  201211;//q  l2/b1/s2/d1  cr0/sl_9(aux)
  sidlst[bias+1]=202211;//q     b2/s2/d1 
  sidlst[bias+2]=203211;//q     b3/s2/d1 
  sidlst[bias+3]=204211;//q     b4/s2/d1 
  sidlst[bias+4]=205211;//q     b5/s2/d1 
  sidlst[bias+5]=206211;//q     b6/s2/d1 
  sidlst[bias+6]=207211;//q     b7/s2/d1 
  sidlst[bias+7]=208211;//q     b8/s2/d1 
  sidlst[bias+8]=0;//              empty  
  sidlst[bias+9]=0;//              empty  
  sidlst[bias+10]=201221;//q    b1/s2/d2 
  sidlst[bias+11]=202221;//q    b2/s2/d2 
  sidlst[bias+12]=203221;//q    b3/s2/d2 
  sidlst[bias+13]=204221;//q    b4/s2/d2 
  sidlst[bias+14]=205221;//q    b5/s2/d2 
  sidlst[bias+15]=206221;//q    b6/s2/d2 
  sidlst[bias+16]=207221;//q    b7/s2/d2 
  sidlst[bias+17]=208221;//q    b8/s2/d2 
  sidlst[bias+18]=0;//              empty  
  sidlst[bias+19]=0;//              empty  
  bias+=20;  
//
// <======================================== crate-SC1 (top/ram=L1/S1)  
//--->TOF(anodes,SFET):
  sidlst[bias]=   209102;//ft l2/fb9/s1  cr1/sl_2
  sidlst[bias+1]= 201100;//t      b1/s1  
  sidlst[bias+2]= 201101;//q
  sidlst[bias+3]= 203100;//t      b3/s1
  sidlst[bias+4]= 203101;//q
  sidlst[bias+5]= 205100;//t      b5/s1
  sidlst[bias+6]= 205101;//q
  sidlst[bias+7]= 207100;//t      b7/s1
  sidlst[bias+8]= 207101;//q
  sidlst[bias+9]= 0;
  sidlst[bias+10]=0;
  sidlst[bias+11]=209103;//SumHT, fb9/s1
  sidlst[bias+12]=209104;//SumSHT fb9/s1
  bias+=13;
  sidlst[bias]=   210102;//ft l2/fb10/s1 cr1/sl_3
  sidlst[bias+1]= 202100;//t       b2/s1 
  sidlst[bias+2]= 202101;//q
  sidlst[bias+3]= 204100;//t       b4/s1
  sidlst[bias+4]= 204101;//q
  sidlst[bias+5]= 206100;//t       b6/s1
  sidlst[bias+6]= 206101;//q
  sidlst[bias+7]= 208100;//t       b8/s1
  sidlst[bias+8]= 208101;//q
  sidlst[bias+9]= 0;
  sidlst[bias+10]=0;
  sidlst[bias+11]=210103;//SumHT, fb10/s1
  sidlst[bias+12]=210104;//SumSHT,fb10/s1
  bias+=13;
  sidlst[bias]=   109202;//ft l1/fb9/s2  cr1/sl_5
  sidlst[bias+1]= 101200;//t      b1/s2  
  sidlst[bias+2]= 101201;//q
  sidlst[bias+3]= 103200;//t      b3/s2
  sidlst[bias+4]= 103201;//q
  sidlst[bias+5]= 105200;//t      b5/s2
  sidlst[bias+6]= 105201;//q
  sidlst[bias+7]= 107200;//t      b7/s2
  sidlst[bias+8]= 107201;//q
  sidlst[bias+9]= 0;
  sidlst[bias+10]=0;
  sidlst[bias+11]=109203;//SumHT, fb9/s2
  sidlst[bias+12]=109204;//SumSHT fb9/s2
  bias+=13;
  sidlst[bias]=   110202;//ft l1/fb10/s2 cr1/sl_6
  sidlst[bias+1]= 102200;//t       b2/s2 
  sidlst[bias+2]= 102201;//q
  sidlst[bias+3]= 104200;//t       b4/s2
  sidlst[bias+4]= 104201;//q
  sidlst[bias+5]= 106200;//t       b6/s2
  sidlst[bias+6]= 106201;//q
  sidlst[bias+7]= 108200;//t       b8/s2
  sidlst[bias+8]= 108201;//q
  sidlst[bias+9]= 0;
  sidlst[bias+10]=0;
  sidlst[bias+11]=110203;//SumHT, fb10/s2
  sidlst[bias+12]=110204;//SumSHT,fb10/s2 
  bias+=13;
//--->ANTI(anodes,SFEA):
  sidlst[bias]=  10202;//ft    fb10/s2  cr1/sl_7
  sidlst[bias+1]= 5200;//t       b1/s2
  sidlst[bias+2]= 5201;//q  
  sidlst[bias+3]= 6200;//t       b2/s2 
  sidlst[bias+4]= 6201;//q  
  sidlst[bias+5]= 7200;//t       b3/s2 
  sidlst[bias+6]= 7201;//q  
  sidlst[bias+7]= 8200;//t       b4/s2 
  sidlst[bias+8]= 8201;//q  
  bias+=9;
//--->TOF(dynodes,SFEC):
  sidlst[bias]=   101211;//q  l1/b1/s2/d1  cr1/sl_8(aux)
  sidlst[bias+1]= 102211;//q     b2/s2/d1 
  sidlst[bias+2]= 103211;//q     b3/s2/d1 
  sidlst[bias+3]= 104211;//q     b4/s2/d1 
  sidlst[bias+4]= 105211;//q     b5/s2/d1 
  sidlst[bias+5]= 106211;//q     b6/s2/d1 
  sidlst[bias+6]= 107211;//q     b7/s2/d1 
  sidlst[bias+7]= 108211;//q     b8/s2/d1 
  sidlst[bias+8]= 101231;//q     b1/s2/d3 
  sidlst[bias+9]= 0;//             empty
  sidlst[bias+10]=101221;//q     b1/s2/d2 
  sidlst[bias+11]=102221;//q     b2/s2/d2 
  sidlst[bias+12]=103221;//q     b3/s2/d2 
  sidlst[bias+13]=104221;//q     b4/s2/d2 
  sidlst[bias+14]=105221;//q     b5/s2/d2 
  sidlst[bias+15]=106221;//q     b6/s2/d2 
  sidlst[bias+16]=107221;//q     b7/s2/d2 
  sidlst[bias+17]=108221;//q     b8/s2/d2 
  sidlst[bias+18]=108231;//q     b8/s2/d3 
  sidlst[bias+19]=0;//             empty
  bias+=20;  
  sidlst[bias]=  201111;//q  l2/b1/s1/d1  cr1/sl_9(aux)
  sidlst[bias+1]=202111;//q     b2/s1/d1 
  sidlst[bias+2]=203111;//q     b3/s1/d1 
  sidlst[bias+3]=204111;//q     b4/s1/d1 
  sidlst[bias+4]=205111;//q     b5/s1/d1 
  sidlst[bias+5]=206111;//q     b6/s1/d1 
  sidlst[bias+6]=207111;//q     b7/s1/d1 
  sidlst[bias+7]=208111;//q     b8/s1/d1 
  sidlst[bias+8]=0;//              empty  
  sidlst[bias+9]=0;//              empty  
  sidlst[bias+10]=201121;//q    b1/s1/d2 
  sidlst[bias+11]=202121;//q    b2/s1/d2 
  sidlst[bias+12]=203121;//q    b3/s1/d2 
  sidlst[bias+13]=204121;//q    b4/s1/d2 
  sidlst[bias+14]=205121;//q    b5/s1/d2 
  sidlst[bias+15]=206121;//q    b6/s1/d2 
  sidlst[bias+16]=207121;//q    b7/s1/d2 
  sidlst[bias+17]=208121;//q    b8/s1/d2 
  sidlst[bias+18]=0;//              empty  
  sidlst[bias+19]=0;//              empty  
  bias+=20;  
//  
// <=========================================== crate-SC2 (bot/wake)
//--->TOF(anodes,SFET):
  sidlst[bias]=   409202;//ft l4/fb9/s2  cr2/sl_2
  sidlst[bias+1]= 401200;//t      b1/s2  
  sidlst[bias+2]= 401201;//q
  sidlst[bias+3]= 403200;//t      b3/s2
  sidlst[bias+4]= 403201;//q
  sidlst[bias+5]= 405200;//t      b5/s2
  sidlst[bias+6]= 405201;//q
  sidlst[bias+7]= 407200;//t      b7/s2
  sidlst[bias+8]= 407201;//q
  sidlst[bias+9]= 0;
  sidlst[bias+10]=0;
  sidlst[bias+11]=409203;//SumHT, fb9/s2
  sidlst[bias+12]=409204;//SumSHT fb9/s2
  bias+=13;
  sidlst[bias]=   410202;//ft l4/fb10/s2 cr2/sl_3
  sidlst[bias+1]= 402200;//t       b2/s2 
  sidlst[bias+2]= 402201;//q
  sidlst[bias+3]= 404200;//t       b4/s2
  sidlst[bias+4]= 404201;//q
  sidlst[bias+5]= 406200;//t       b6/s2
  sidlst[bias+6]= 406201;//q
  sidlst[bias+7]= 408200;//t       b8/s2
  sidlst[bias+8]= 408201;//q
  sidlst[bias+9]= 0;
  sidlst[bias+10]=0;
  sidlst[bias+11]=410203;//SumHT, fb10/s2
  sidlst[bias+12]=410204;//SumSHT,fb10/s2
  bias+=13;
  sidlst[bias]=   311202;//ft l3/fb11/s2  cr2/sl_5
  sidlst[bias+1]= 301200;//t       b1/s2  
  sidlst[bias+2]= 301201;//q
  sidlst[bias+3]= 303200;//t       b3/s2
  sidlst[bias+4]= 303201;//q
  sidlst[bias+5]= 305200;//t       b5/s2
  sidlst[bias+6]= 305201;//q
  sidlst[bias+7]= 307200;//t       b7/s2
  sidlst[bias+8]= 307201;//q
  sidlst[bias+9]= 309200;//t       b9/s2
  sidlst[bias+10]=309201;//q
  sidlst[bias+11]=311203;//SumHT, fb11/s2
  sidlst[bias+12]=311204;//SumSHT fb11/s2
  bias+=13;
  sidlst[bias]=   312202;//ft l3/fb12/s2 cr2/sl_6
  sidlst[bias+1]= 302200;//t       b2/s2 
  sidlst[bias+2]= 302201;//q
  sidlst[bias+3]= 304200;//t       b4/s2
  sidlst[bias+4]= 304201;//q
  sidlst[bias+5]= 306200;//t       b6/s2
  sidlst[bias+6]= 306201;//q
  sidlst[bias+7]= 308200;//t       b8/s2
  sidlst[bias+8]= 308201;//q
  sidlst[bias+9]= 310200;//t      b10/s2
  sidlst[bias+10]=310201;//q
  sidlst[bias+11]=312203;//SumHT, fb12/s2
  sidlst[bias+12]=312204;//SumSHT,fb12/s2 
  bias+=13;
//--->ANTI(anodes,SFEA):
  sidlst[bias]=   9102;//ft     fb9/s1  cr2/sl_7
  sidlst[bias+1]= 1100;//t       b1/s1
  sidlst[bias+2]= 1101;//q  
  sidlst[bias+3]= 2100;//t       b2/s1 
  sidlst[bias+4]= 2101;//q 
  sidlst[bias+5]= 3100;//t       b3/s1 
  sidlst[bias+6]= 3101;//q  
  sidlst[bias+7]= 4100;//t       b4/s1 
  sidlst[bias+8]= 4101;//q  
  bias+=9;
//--->TOF(dynodes,SFEC):
  sidlst[bias]=   301211;//q  l3/b1/s2/d1  cr2/sl_8(aux)
  sidlst[bias+1]= 302211;//q     b2/s2/d1 
  sidlst[bias+2]= 303211;//q     b3/s2/d1 
  sidlst[bias+3]= 304211;//q     b4/s2/d1 
  sidlst[bias+4]= 305211;//q     b5/s2/d1 
  sidlst[bias+5]= 306211;//q     b6/s2/d1 
  sidlst[bias+6]= 307211;//q     b7/s2/d1 
  sidlst[bias+7]= 308211;//q     b8/s2/d1 
  sidlst[bias+8]= 309211;//q     b9/s2/d1 
  sidlst[bias+9]= 310211;//q    b10/s2/d1
  sidlst[bias+10]=301221;//q     b1/s2/d2 
  sidlst[bias+11]=302221;//q     b2/s2/d2 
  sidlst[bias+12]=303221;//q     b3/s2/d2 
  sidlst[bias+13]=304221;//q     b4/s2/d2 
  sidlst[bias+14]=305221;//q     b5/s2/d2 
  sidlst[bias+15]=306221;//q     b6/s2/d2 
  sidlst[bias+16]=307221;//q     b7/s2/d2 
  sidlst[bias+17]=308221;//q     b8/s2/d2 
  sidlst[bias+18]=309221;//q     b9/s2/d2 
  sidlst[bias+19]=310221;//q    b10/s2/d2
  bias+=20;  
  sidlst[bias]=  401211;//q  l4/b1/s2/d1  cr2/sl_9(aux)
  sidlst[bias+1]=402211;//q     b2/s2/d1 
  sidlst[bias+2]=403211;//q     b3/s2/d1 
  sidlst[bias+3]=404211;//q     b4/s2/d1 
  sidlst[bias+4]=405211;//q     b5/s2/d1 
  sidlst[bias+5]=406211;//q     b6/s2/d1 
  sidlst[bias+6]=407211;//q     b7/s2/d1 
  sidlst[bias+7]=408211;//q     b8/s2/d1 
  sidlst[bias+8]=401231;//q     b1/s2/d3  
  sidlst[bias+9]=0;//              empty  
  sidlst[bias+10]=401221;//q    b1/s2/d2 
  sidlst[bias+11]=402221;//q    b2/s2/d2 
  sidlst[bias+12]=403221;//q    b3/s2/d2 
  sidlst[bias+13]=404221;//q    b4/s2/d2 
  sidlst[bias+14]=405221;//q    b5/s2/d2 
  sidlst[bias+15]=406221;//q    b6/s2/d2 
  sidlst[bias+16]=407221;//q    b7/s2/d2 
  sidlst[bias+17]=408221;//q    b8/s2/d2 
  sidlst[bias+18]=408231;//q    b8/s2/d3  
  sidlst[bias+19]=0;//              empty  
  bias+=20;  
//
// <======================================== crate-SC3 (bot/ram)  
//--->TOF(anodes,SFET):
  sidlst[bias]=   409102;//ft l4/fb9/s1  cr3/sl_2
  sidlst[bias+1]= 401100;//t      b1/s1  
  sidlst[bias+2]= 401101;//q
  sidlst[bias+3]= 403100;//t      b3/s1
  sidlst[bias+4]= 403101;//q
  sidlst[bias+5]= 405100;//t      b5/s1
  sidlst[bias+6]= 405101;//q
  sidlst[bias+7]= 407100;//t      b7/s1
  sidlst[bias+8]= 407101;//q
  sidlst[bias+9]= 0;
  sidlst[bias+10]=0;
  sidlst[bias+11]=409103;//SumHT, fb9/s1
  sidlst[bias+12]=409104;//SumSHT fb9/s1
  bias+=13;
  sidlst[bias]=   410102;//ft l4/fb10/s1 cr3/sl_3
  sidlst[bias+1]= 402100;//t       b2/s1 
  sidlst[bias+2]= 402101;//q
  sidlst[bias+3]= 404100;//t       b4/s1
  sidlst[bias+4]= 404101;//q
  sidlst[bias+5]= 406100;//t       b6/s1
  sidlst[bias+6]= 406101;//q
  sidlst[bias+7]= 408100;//t       b8/s1
  sidlst[bias+8]= 408101;//q
  sidlst[bias+9]= 0;
  sidlst[bias+10]=0;
  sidlst[bias+11]=410103;//SumHT, fb10/s1
  sidlst[bias+12]=410104;//SumSHT,fb10/s1
  bias+=13;
  sidlst[bias]=   311102;//ft l3/fb11/s1  cr3/sl_5
  sidlst[bias+1]= 301100;//t       b1/s1  
  sidlst[bias+2]= 301101;//q
  sidlst[bias+3]= 303100;//t       b3/s1
  sidlst[bias+4]= 303101;//q
  sidlst[bias+5]= 305100;//t       b5/s1
  sidlst[bias+6]= 305101;//q
  sidlst[bias+7]= 307100;//t       b7/s1
  sidlst[bias+8]= 307101;//q
  sidlst[bias+9]= 309100;//t       b9/s1
  sidlst[bias+10]=309101;//q
  sidlst[bias+11]=311103;//SumHT, fb11/s1
  sidlst[bias+12]=311104;//SumSHT fb11/s1
  bias+=13;
  sidlst[bias]=   312102;//ft l3/fb12/s1 cr3/sl_6
  sidlst[bias+1]= 302100;//t       b2/s1 
  sidlst[bias+2]= 302101;//q
  sidlst[bias+3]= 304100;//t       b4/s1
  sidlst[bias+4]= 304101;//q
  sidlst[bias+5]= 306100;//t       b6/s1
  sidlst[bias+6]= 306101;//q
  sidlst[bias+7]= 308100;//t       b8/s1
  sidlst[bias+8]= 308101;//q
  sidlst[bias+9]= 310100;//t      b10/s1
  sidlst[bias+10]=310101;//q
  sidlst[bias+11]=312103;//SumHT, fb12/s1
  sidlst[bias+12]=312104;//SumSHT,fb12/s1 
  bias+=13;
//--->ANTI(anodes,SFEA):
  sidlst[bias]=  10102;//ft    fb10/s1  cr3/sl_7
  sidlst[bias+1]= 5100;//t       b5/s1
  sidlst[bias+2]= 5101;//q  
  sidlst[bias+3]= 6100;//t       b6/s1 
  sidlst[bias+4]= 6101;//q 
  sidlst[bias+5]= 7100;//t       b7/s1 
  sidlst[bias+6]= 7101;//q  
  sidlst[bias+7]= 8100;//t       b8/s1 
  sidlst[bias+8]= 8101;//q  
  bias+=9;
//--->TOF(dynodes,SFEC):
  sidlst[bias]=   301111;//q  l3/b1/s1/d1  cr3/sl_8(aux)
  sidlst[bias+1]= 302111;//q     b2/s1/d1 
  sidlst[bias+2]= 303111;//q     b3/s1/d1 
  sidlst[bias+3]= 304111;//q     b4/s1/d1 
  sidlst[bias+4]= 305111;//q     b5/s1/d1 
  sidlst[bias+5]= 306111;//q     b6/s1/d1 
  sidlst[bias+6]= 307111;//q     b7/s1/d1 
  sidlst[bias+7]= 308111;//q     b8/s1/d1 
  sidlst[bias+8]= 309111;//q     b9/s1/d1 
  sidlst[bias+9]= 310111;//q    b10/s1/d1
  sidlst[bias+10]=301121;//q     b1/s1/d2 
  sidlst[bias+11]=302121;//q     b2/s1/d2 
  sidlst[bias+12]=303121;//q     b3/s1/d2 
  sidlst[bias+13]=304121;//q     b4/s1/d2 
  sidlst[bias+14]=305121;//q     b5/s1/d2 
  sidlst[bias+15]=306121;//q     b6/s1/d2 
  sidlst[bias+16]=307121;//q     b7/s1/d2 
  sidlst[bias+17]=308121;//q     b8/s1/d2 
  sidlst[bias+18]=309121;//q     b9/s1/d2 
  sidlst[bias+19]=310121;//q    b10/s1/d2
  bias+=20;  
  sidlst[bias]=  401111;//q  l4/b1/s1/d1  cr3/sl_9(aux)
  sidlst[bias+1]=402111;//q     b2/s1/d1 
  sidlst[bias+2]=403111;//q     b3/s1/d1 
  sidlst[bias+3]=404111;//q     b4/s1/d1 
  sidlst[bias+4]=405111;//q     b5/s1/d1 
  sidlst[bias+5]=406111;//q     b6/s1/d1 
  sidlst[bias+6]=407111;//q     b7/s1/d1 
  sidlst[bias+7]=408111;//q     b8/s1/d1 
  sidlst[bias+8]=401131;//q     b1/s1/d3  
  sidlst[bias+9]=0;//              empty  
  sidlst[bias+10]=401121;//q    b1/s1/d2 
  sidlst[bias+11]=402121;//q    b2/s1/d2 
  sidlst[bias+12]=403121;//q    b3/s1/d2 
  sidlst[bias+13]=404121;//q    b4/s1/d2 
  sidlst[bias+14]=405121;//q    b5/s1/d2 
  sidlst[bias+15]=406121;//q    b6/s1/d2 
  sidlst[bias+16]=407121;//q    b7/s1/d2 
  sidlst[bias+17]=408121;//q    b8/s1/d2 
  sidlst[bias+18]=408131;//q    b8/s1/d3  
  sidlst[bias+19]=0;//              empty  
  bias+=20;
  cout<<"AMSSCIds::init: total TOF+ANTI h/w-channels initialized(incl.empty): "<<bias<<endl;
//
//---> prepare hwch->hwid table(incl. empty channels):
//
  int swid,hwid;
  int16u slt,chmx,hwch(0);
  for(int cr=0;cr<SCCRAT;cr++){
    for(int sl=0;sl<SCSLTM;sl++){
      slt=sltymap[cr][sl];
      chmx=ochpsty[slt-1];//outp.channels per slot(incl.empty)unirun.job
      for(int ch=0;ch<chmx;ch++){
        hwid=(cr+1)*1000+(sl+1)*100+(ch+1);//CSRR
        hidls[hwch]=hwid;
        hwch+=1;
      }
    }
  }
  cout<<"AMSSCIds::init: S-crates capacity="<<hwch<<" h/w-channels(incl.empty)"<<endl;
//
//--->create invers table (hwid=F(swch)):
//
  int mtmn,mtmx,pmt,hch,ipmx;
  int16u swch=0;//1st- for TOF-channels
  for(int il=0;il<TOF2DBc::getnplns();il++){
//    cout<<"L="<<il+1<<endl;
    for(int ib=0;ib<TOF2DBc::getbppl(il)+2;ib++){//"+2" -> 2 half-planes
//      cout<<"   Bar="<<ib+1<<endl;
      for(int is=0;is<2;is++){//<--- sides loop
//        cout<<"       Side="<<is+1<<endl;
        if(ib<TOF2DBc::getbppl(il)){//single paddle channels
          ipmx=TOF2DBc::npmtps(il,ib);//max index for anode+dynodes counting(2(3))
	  mtmn=0;//min. anode mtyp
	  mtmx=SCAMTS-1;//max. anode mtyp (1)(i.e. 0/1->LT/Q)
        }
        else{//half-plane channels
          ipmx=0;//only anode
	  mtmn=SCAMTS;//mtyp-range from 2
	  mtmx=mtmn+SCHPMTS-1;// to 4 (i.e. 2/3/4->FT/sumHT/sumSHT)
        }
        for(int ip=0;ip<=ipmx;ip++){//<--- anode,dynodes loop
//	  cout<<"          pm="<<ip<<endl;
	  if(ip>0)mtmn=1;//mtyp=1 for dynodes
	  for(int mt=mtmn;mt<=mtmx;mt++){//<--- mtyp loop
//	    cout<<"               mtyp="<<mt<<endl;
	    swid=100000*(il+1)+1000*(ib+1)+100*(is+1)+10*ip+mt;//LBBSPM
	    for(int hc=0;hc<bias;hc++){//<-search in sidlst
	      if(swid==sidlst[hc]){
	        hch=hc;
	        goto Found1;
	      }
	    }//hwch
	    cout<<"AMSSCIds::init-Err:missing TOF-swid in sidlst, swid="<<swid<<endl;
	    exit(1);
Found1:
	    hwid=hidls[hch];
	    hidlst[swch]=hwid;
	    swch+=1;
//	    cout<<"               swid="<<swid<<" hwid="<<hwid<<" swch="<<swch<<endl;
	  }//mtyp
	}//an+dynodes
      }//side
    }//bar
  }//layer
//----
  if(swch!=totswch[0]){
    cout<<"AMSSCIds::init:Error in TOF-swch counting !!!"<<endl;
    exit(1);
  }
//2nd- for ANTI-channels
  for(int ib=0;ib<MAXANTI+2;ib++){//<---sectors loop, "+2" for two half-cylinders
    for(int is=0;is<2;is++){//<---sides loop
      if(ib<MAXANTI){
        mtmn=0;//0(t)
	mtmx=ANAMTS-1;//1(q)
      }
      else{
        mtmn=ANAMTS;//2(FT)
	mtmx=mtmn+ANAGMTS-1;//2(FT)
      }
      pmt=0;
      for(int mt=mtmn;mt<=mtmx;mt++){//<--- mtyps loop
        swid=1000*(ib+1)+100*(is+1)+10*pmt+mt;//BSPM
	for(int hc=0;hc<bias;hc++){//<-search in sidlst
	  if(swid==sidlst[hc]){
	    hch=hc;
	    goto Found2;
	  }
	}//hwch
	cout<<"AMSSCIds::init:missing ANTI-swid in sidlst, swid="<<swid<<endl;
	exit(1);
Found2:
	hwid=hidls[hch];
	hidlst[swch]=hwid;
	swch+=1;
      }//mtyp
    }//side
  }//logic-sector
//  
  if(swch!=(totswch[0]+totswch[1])){
    cout<<"AMSSCIds::init:Error in ANTI-swch counting !!!"<<endl;
    exit(1);
  }
  cout<<"=====> AMSSCIds::init: successfully done !!!"<<endl;
//  int16u cr,sl,rc,ll,bb,ss,pm,mt,lll,dt;
//  swch=331;
//  dt=2;
//  hwid=hidlst[swch-1];
//  cr=hwid/1000;
//  sl=(hwid%1000)/100;
//  rc=hwid%100;
//  hwch=hwseqn(cr-1,sl-1,rc-1);
//  swid=hw2swid(cr-1,sl-1,rc-1);
//  ll=swid/100000;
//  bb=(swid%100000)/1000;
//  ss=(swid%1000)/100;
//  pm=(swid%100)/10;
//  mt=swid%10;
//  cout<<"swch="<<swch<<" hwid="<<hwid<<" hwch="<<hwch+1<<" swid="<<swid<<endl;
//  if(dt==1)lll=ll-1;
//  else lll=0;
//  swch=swseqn(dt,lll,bb-1,ss-1,pm,mt);
//  cout<<" swch(bk)="<<swch+1<<endl;
}
//---------------
int16u AMSSCIds::swseqn(int dtype, int16u layer, int16u bar, int16u side,
                                                 int16u pmt, int16u mtyp){
//
//dtype=1/2;layer=0,1...;bar=0,1,...;side=0,1;pmt=0,1,2,3;mtyp=0,1,2,3,4
//
  int16u swch(0);//glogal sequential sw-channel(incl pmts and mtyps)
  if(dtype==1){//TOF  
    assert(bar<(TOF2DBc::getbppl(layer)+2) && mtyp<(SCAMTS+SCHPMTS));
    if(bar>=TOF2DBc::getbppl(layer))assert(pmt==0 && mtyp>(SCAMTS-1));
//
    for(int il=0;il<layer;il++){// 1->count in layers befor current one
      for(int ib=0;ib<TOF2DBc::getbppl(il);ib++){
        swch+=(TOF2DBc::npmtps(il,ib)*SCDMTS+SCAMTS)*2;//2 sides
      }
      swch+=(SCHPMTS*2*2);//(2 half-planes FT/HT/SHT-measurements)*2sides
    }
    for(int ib=0;ib<bar;ib++){// 2->count in curr.layer (bars befor current one)
      if(ib<TOF2DBc::getbppl(layer))swch+=(TOF2DBc::npmtps(layer,ib)*SCDMTS+SCAMTS)*2;//2 sides
      else swch+=SCHPMTS*2;//2 sides of half-plane
    }
    for(int is=0;is<side;is++){// 3->count in curr.bar (compl.sides befor current one)
        if(bar<TOF2DBc::getbppl(layer))swch+=(TOF2DBc::npmtps(layer,bar)*SCDMTS+SCAMTS);
	else swch+=SCHPMTS;
    }
// 4->count in current side pmts*mtypes:
    if(pmt==0){//anode
      if(bar<TOF2DBc::getbppl(layer))swch+=mtyp;
      else swch+=(mtyp-SCAMTS);
    } 
    else{//dynode(means automatically that it is normal bar)
      swch+=SCAMTS;//all normal bar anode-measurements
      for(int pm=1;pm<pmt;pm++)swch+=SCDMTS;//+ prev.dynodes
      swch+=(mtyp-SCDMTS);//current_dynode mtypes befor current mtyp
    }
  }
//----
  else{//ANTI
    assert(bar<(MAXANTI+2) && mtyp<(ANAMTS+ANAGMTS));
    if(bar>=MAXANTI)assert(mtyp>(ANAMTS-1));
//
    swch+=totswch[0];//ANTI-channels i count after TOF ones
//
    if(bar<MAXANTI){//normal bar
//count in bars befor current one:
      swch+=(bar*2*ANAMTS);
//count in sides befor current one:
      if(side==1)swch+=ANAMTS;//mtypes in prev.side
      swch+=mtyp;//add mtypes befor the current mtyp
    }
    else{//half-cylinder
//count in bars befor current one:
      swch+=(MAXANTI*2*ANAMTS+(bar-MAXANTI)*2*ANAGMTS);
//count in sides befor current one:
      if(side==1)swch+=ANAGMTS;//mtypes in prev.side
      swch+=(mtyp-ANAMTS);//add mtypes befor the current mtyp
    }
  }
  return swch;//0,1,...
}
//-----------------
int16u AMSSCIds::hwseqn(int16u crate, int16u slot, int16u rdch){
//crate=0,1,...;slot=0,1,...;rdch=0,1,...
// calc. glogal sequential hw-channel:
  int16u hwch(0),slt;
//
  for(int cr=0;cr<crate;cr++){//1->count in crates befor current one
    for(int sl=0;sl<SCSLTM;sl++){
      slt=sltymap[cr][sl];
      hwch+=ochpsty[slt-1];
    }
  }
  for(int sl=0;sl<slot;sl++){//2->count in slots befor current one(in crate crate)
    slt=sltymap[crate][sl];
    hwch+=ochpsty[slt-1];
  }
  hwch+=rdch;//3->count in current slot
  return hwch;//0,1,...
} 
//-----------------
int AMSSCIds::hw2swid(int16u crate, int16u slot, int16u rdch){
//crate=0,1,...;slot=0,1,...;rdch=0,1,...
// transform h/w-id(address) into s/w-id:
  int16u hwch;
  int swid;
  hwch=hwseqn(crate,slot,rdch);
  swid=sidlst[hwch];
  return swid;
//
}



