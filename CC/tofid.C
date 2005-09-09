#include "tofid.h"
using namespace TOF2GC;
using namespace ANTI2C;
  int AMSSCIds::totswch[2];//total s/w-channels for TOF and ANTI
  int AMSSCIds::sidlst[SCRCMX];//swid-list(vs hwch) 
  int AMSSCIds::hidlst[SCRCMX];//hwid-list(vs swch)
  int AMSSCIds::hidls[SCRCMX];//hwid-list(vs hwch)
//outp.channels for each slot(card) type: 
  int16u AMSSCIds::ochpsty[TOF2GC::SCSLTY]={12,8,14,20,0,0};
//slot(card)-type map in crates  
  int16u AMSSCIds::sltymap[SCCRAT][SCSLTM]={5,1,1,6,1,1,2,4,4,  //top cr-1
                                            5,1,1,6,1,1,2,4,4,  //top cr-2
					    5,1,1,6,1,1,3,4,4,  //bot cr-3
					    5,1,1,6,1,1,3,4,4   //bot cr-4
					    };
//map of temp-sensors ids in crate's slots:
  int16u tmpsln[SCSLTM]={0,1,2,0,3,4,5,0,0};// same for all crates(id=sensor seg.number)
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
  _mtyp=mtyp;//0,1,2(sTDC,fTDC(hist),Q)
  
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
  _mtyp=mtyp;//1,2(no 0==sTDC)
  
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
    for(int ib=0;ib<TOF2DBc::getbppl(il);ib++){
      ttswch+=(TOF2DBc::npmtps(il,ib)*SCDMTS+SCAMTS);
    }
  }
  ttswch*=2;//2sides
  totswch[0]=ttswch;
//
//--->count total ANTI s/w-channels:
//
  int taswch(0);//total ANTI sequential sw-channels(incl mtyp)
  taswch=ANCHMX*ANAMTS;
  totswch[1]=taswch;
//
  cout<<"AMSSCIds::init:total s/w-channels(tof/anti)="<<totswch[0]<<" "<<totswch[1]<<endl;
//
//--->fill sid's as function of sequential number of h/w-channel
//below used: tof_swid->LBBSPM, anti_swid->BSPM.
//
  int bias(0);
// <=========================================== crate-SC0 (top/wake)
//--->TOF(anodes,SFET):
  sidlst[bias]=   101100;//t   l1/b1/s1  cr0/sl2
  sidlst[bias+1]= 101101;//h
  sidlst[bias+2]= 101102;//q
  sidlst[bias+3]= 102100;//t      b2/s1
  sidlst[bias+4]= 102101;//h
  sidlst[bias+5]= 102102;//q
  sidlst[bias+6]= 103100;//t      b3/s1
  sidlst[bias+7]= 103101;//h 
  sidlst[bias+8]= 103102;//q
  sidlst[bias+9]= 104100;//t      b4/s1
  sidlst[bias+10]=104101;//h
  sidlst[bias+11]=104102;//q
  bias+=12;
  sidlst[bias]=   201200;//t  l2/b1/s2  cr0/sl3
  sidlst[bias+1]= 201201;//h
  sidlst[bias+2]= 201202;//q
  sidlst[bias+3]= 202200;//t     b2/s2
  sidlst[bias+4]= 202201;//h
  sidlst[bias+5]= 202202;//q
  sidlst[bias+6]= 203200;//t     b3/s2
  sidlst[bias+7]= 203201;//h
  sidlst[bias+8]= 203202;//q
  sidlst[bias+9]= 204200;//t     b4/s2
  sidlst[bias+10]=204201;//h
  sidlst[bias+11]=204202;//q
  bias+=12;
  sidlst[bias]=   105100;//t  l1/b5/s1  cr0/sl5
  sidlst[bias+1]= 105101;//h
  sidlst[bias+2]= 105102;//q
  sidlst[bias+3]= 106100;//t     b6/s1
  sidlst[bias+4]= 106101;//h
  sidlst[bias+5]= 106102;//q
  sidlst[bias+6]= 107100;//t     b7/s1
  sidlst[bias+7]= 107101;//h
  sidlst[bias+8]= 107102;//q
  sidlst[bias+9]= 108100;//t     b8/s1
  sidlst[bias+10]=108101;//h
  sidlst[bias+11]=108102;//q
  bias+=12;
  sidlst[bias]=   205200;//t  l2/b5/s2  cr0/sl6
  sidlst[bias+1]= 205201;//h
  sidlst[bias+2]= 205202;//q
  sidlst[bias+3]= 206200;//t     b6/s2
  sidlst[bias+4]= 206201;//h
  sidlst[bias+5]= 206202;//q
  sidlst[bias+6]= 207200;//t     b7/s2
  sidlst[bias+7]= 207201;//h
  sidlst[bias+8]= 207202;//q
  sidlst[bias+9]= 208200;//t     b8/s2
  sidlst[bias+10]=208201;//h
  sidlst[bias+11]=208202;//q
  bias+=12;
//--->ANTI(anodes,SFEA):
  sidlst[bias]=   1201;//h       b1/s2  cr0/sl7
  sidlst[bias+1]= 1202;//q  
  sidlst[bias+2]= 2201;//h       b2/s2 
  sidlst[bias+3]= 2202;//q  
  sidlst[bias+4]= 3201;//h       b3/s2 
  sidlst[bias+5]= 3202;//q  
  sidlst[bias+6]= 4201;//h       b4/s2 
  sidlst[bias+7]= 4202;//q  
  bias+=8;
//--->TOF(dynodes,SFEC):
  sidlst[bias]=   101112;//q  l1/b1/s1/d1  cr0/sl8(aux)
  sidlst[bias+1]= 101122;//q           d2 
  sidlst[bias+2]= 101132;//q           d3 
  sidlst[bias+3]= 102112;//q     b2/s1/d1 
  sidlst[bias+4]= 102122;//q           d2 
  sidlst[bias+5]= 103112;//q     b3/s1/d1 
  sidlst[bias+6]= 103122;//q           d2 
  sidlst[bias+7]= 104112;//q     b4/s1/d1 
  sidlst[bias+8]= 104122;//q           d2 
  sidlst[bias+9]= 201212;//q  l2/b1/s2/d1 
  sidlst[bias+10]=201222;//q           d2 
  sidlst[bias+11]=202212;//q     b2/s2/d1 
  sidlst[bias+12]=202222;//q           d2 
  sidlst[bias+13]=203212;//q     b3/s2/d1 
  sidlst[bias+14]=203222;//q           d2 
  sidlst[bias+15]=204212;//q     b4/s2/d1 
  sidlst[bias+16]=204222;//q           d2 
  sidlst[bias+17]=105112;//q  l1/b5/s1/d1 
  sidlst[bias+18]=0;//             empty
  sidlst[bias+19]=0;//             empty
  bias+=20;  
  sidlst[bias]=   105122;//q l1/b5/s1/d2  cr0/sl9(aux)
  sidlst[bias+1]= 106112;//q    b6/s1/d1 
  sidlst[bias+2]= 106122;//q          d2 
  sidlst[bias+3]= 107112;//q    b7/s1/d1 
  sidlst[bias+4]= 107122;//q          d2 
  sidlst[bias+5]= 108112;//q    b8/s1/d1 
  sidlst[bias+6]= 108122;//q          d2 
  sidlst[bias+7]= 108132;//q          d3 
  sidlst[bias+8]= 205212;//q l2/b5/s2/d1 
  sidlst[bias+9]= 205222;//q          d2 
  sidlst[bias+10]=206212;//q    b6/s2/d1 
  sidlst[bias+11]=206222;//q          d2 
  sidlst[bias+12]=207212;//q    b7/s2/d1 
  sidlst[bias+13]=207222;//q          d2 
  sidlst[bias+14]=208212;//q    b8/s2/d1 
  sidlst[bias+15]=208222;//q          d2 
  sidlst[bias+16]=0;//              empty  
  sidlst[bias+17]=0;//              empty  
  sidlst[bias+18]=0;//              empty  
  sidlst[bias+19]=0;//              empty  
  bias+=20;  
//
// <======================================== crate-SC1 (top/ram=L1/S1)  
//--->TOF(anodes,SFET):
  sidlst[bias]=   101200;//t   l1/b1/s2  cr1/sl2
  sidlst[bias+1]= 101201;//h
  sidlst[bias+2]= 101202;//q
  sidlst[bias+3]= 102200;//t      b2/s2
  sidlst[bias+4]= 102201;//h
  sidlst[bias+5]= 102202;//q
  sidlst[bias+6]= 103200;//t      b3/s2
  sidlst[bias+7]= 103201;//h
  sidlst[bias+8]= 103202;//q
  sidlst[bias+9]= 104200;//t      b4/s2
  sidlst[bias+10]=104201;//h
  sidlst[bias+11]=104202;//q
  bias+=12;
  sidlst[bias]=   201100;//t  l2/b1/s1  cr1/sl3
  sidlst[bias+1]= 201101;//h
  sidlst[bias+2]= 201102;//q
  sidlst[bias+3]= 202100;//t     b2/s1
  sidlst[bias+4]= 202101;//h
  sidlst[bias+5]= 202102;//q
  sidlst[bias+6]= 203100;//t     b3/s1
  sidlst[bias+7]= 203101;//h
  sidlst[bias+8]= 203102;//q
  sidlst[bias+9]= 204100;//t     b4/s1
  sidlst[bias+10]=204101;//h
  sidlst[bias+11]=204102;//q
  bias+=12;
  sidlst[bias]=   105200;//t  l1/b5/s2  cr1/sl5
  sidlst[bias+1]= 105201;//h
  sidlst[bias+2]= 105202;//q
  sidlst[bias+3]= 106200;//t     b6/s2
  sidlst[bias+4]= 106201;//h
  sidlst[bias+5]= 106202;//q
  sidlst[bias+6]= 107200;//t     b7/s2
  sidlst[bias+7]= 107201;//h
  sidlst[bias+8]= 107202;//q
  sidlst[bias+9]= 108200;//t     b8/s2
  sidlst[bias+10]=108201;//h
  sidlst[bias+11]=108202;//q
  bias+=12;
  sidlst[bias]=   205100;//t  l2/b5/s1  cr1/sl6
  sidlst[bias+1]= 205101;//h
  sidlst[bias+2]= 205102;//q
  sidlst[bias+3]= 206100;//t     b6/s1
  sidlst[bias+4]= 206101;//h
  sidlst[bias+5]= 206102;//q
  sidlst[bias+6]= 207100;//t     b7/s1
  sidlst[bias+7]= 207101;//h
  sidlst[bias+8]= 207102;//q
  sidlst[bias+9]= 208100;//t     b8/s1
  sidlst[bias+10]=208101;//h
  sidlst[bias+11]=208102;//q
  bias+=12;
//--->ANTI(anodes,SFEA):  
  sidlst[bias]=   5201;//h     b5/s2  cr1/sl7
  sidlst[bias+1]= 5202;//q  
  sidlst[bias+2]= 6201;//h     b6/s2 
  sidlst[bias+3]= 6202;//q  
  sidlst[bias+4]= 7201;//h     b7/s2 
  sidlst[bias+5]= 7202;//q  
  sidlst[bias+6]= 8201;//h     b8/s2 
  sidlst[bias+7]= 8202;//q  
  bias+=8;
//--->TOF(dynodes,SFEC):
  sidlst[bias]=   101212;//q  l1/b1/s2/d1 cr1/sl8(aux)
  sidlst[bias+1]= 101222;//q           d2 
  sidlst[bias+2]= 101232;//q           d3 
  sidlst[bias+3]= 102212;//q     b2/s2/d1 
  sidlst[bias+4]= 102222;//q           d2 
  sidlst[bias+5]= 103212;//q     b3/s2/d1 
  sidlst[bias+6]= 103222;//q           d2 
  sidlst[bias+7]= 104212;//q     b4/s2/d1 
  sidlst[bias+8]= 104222;//q           d2 
  sidlst[bias+9]= 201112;//q  l2/b1/s1/d1 
  sidlst[bias+10]=201122;//q           d2 
  sidlst[bias+11]=202112;//q     b2/s1/d1 
  sidlst[bias+12]=202122;//q           d2 
  sidlst[bias+13]=203112;//q     b3/s1/d1 
  sidlst[bias+14]=203122;//q           d2 
  sidlst[bias+15]=204112;//q     b4/s1/d1 
  sidlst[bias+16]=204122;//q           d2 
  sidlst[bias+17]=105212;//q  l1/b5/s2/d1 
  sidlst[bias+18]=0;//             empty
  sidlst[bias+19]=0;//             empty
  bias+=20;  
  sidlst[bias]=   105222;//q  l1/b5/s2/d2 cr1/sl9(aux)
  sidlst[bias+1]= 106212;//q     b6/s2/d1 
  sidlst[bias+2]= 106222;//q           d2 
  sidlst[bias+3]= 107212;//q     b7/s2/d1 
  sidlst[bias+4]= 107222;//q           d2 
  sidlst[bias+5]= 108212;//q     b8/s2/d1 
  sidlst[bias+6]= 108222;//q           d2 
  sidlst[bias+7]= 108232;//q           d3 
  sidlst[bias+8]= 205112;//q  l2/b5/s1/d1 
  sidlst[bias+9]= 205122;//q           d2 
  sidlst[bias+10]=206112;//q     b6/s1/d1 
  sidlst[bias+11]=206122;//q           d2 
  sidlst[bias+12]=207112;//q     b7/s1/d1 
  sidlst[bias+13]=207122;//q           d2 
  sidlst[bias+14]=208112;//q     b8/s1/d1 
  sidlst[bias+15]=208122;//q           d2 
  sidlst[bias+16]=0;//              empty  
  sidlst[bias+17]=0;//              empty  
  sidlst[bias+18]=0;//              empty  
  sidlst[bias+19]=0;//              empty  
  bias+=20;
//  
// <=========================================== crate-SC2 (bot/wake)
//--->TOF(anodes,SFET):
  sidlst[bias]=   301200;//t   l3/b1/s2  cr2/sl2
  sidlst[bias+1]= 301201;//h
  sidlst[bias+2]= 301202;//q
  sidlst[bias+3]= 302200;//t      b2/s2
  sidlst[bias+4]= 302201;//h
  sidlst[bias+5]= 302202;//q
  sidlst[bias+6]= 303200;//t      b3/s2
  sidlst[bias+7]= 303201;//h 
  sidlst[bias+8]= 303202;//q
  sidlst[bias+9]= 304200;//t      b4/s2
  sidlst[bias+10]=304201;//h
  sidlst[bias+11]=304202;//q
  bias+=12;
  sidlst[bias]=   305200;//t      b5/s2  cr2/sl3
  sidlst[bias+1]= 305201;//h
  sidlst[bias+2]= 305202;//q
  sidlst[bias+3]= 401200;//t   l4/b1/s2  
  sidlst[bias+4]= 401201;//h
  sidlst[bias+5]= 401202;//q
  sidlst[bias+6]= 402200;//t      b2/s2
  sidlst[bias+7]= 402201;//h
  sidlst[bias+8]= 402202;//q
  sidlst[bias+9]= 403200;//t      b3/s2
  sidlst[bias+10]=403201;//h
  sidlst[bias+11]=403202;//q
  bias+=12;
  sidlst[bias]=   404200;//t      b4/s2  cr2/sl5
  sidlst[bias+1]= 404201;//h
  sidlst[bias+2]= 404202;//q
  sidlst[bias+3]= 306200;//t   l3/b6/s2
  sidlst[bias+4]= 306201;//h
  sidlst[bias+5]= 306202;//q
  sidlst[bias+6]= 307200;//t      b7/s2
  sidlst[bias+7]= 307201;//h
  sidlst[bias+8]= 307202;//q
  sidlst[bias+9]= 308200;//t      b8/s2
  sidlst[bias+10]=308201;//h
  sidlst[bias+11]=308202;//q
  bias+=12;
  sidlst[bias]=   309200;//t      b9/s2  cr2/sl6
  sidlst[bias+1]= 309201;//h
  sidlst[bias+2]= 309202;//q
  sidlst[bias+3]= 310200;//t     b10/s2  
  sidlst[bias+4]= 310201;//h
  sidlst[bias+5]= 310202;//q
  sidlst[bias+6]= 405200;//t   l4/b5/s2
  sidlst[bias+7]= 405201;//h
  sidlst[bias+8]= 405202;//q
  sidlst[bias+9]= 406200;//t      b6/s2
  sidlst[bias+10]=406201;//h
  sidlst[bias+11]=406202;//q
  bias+=12;
//--->ANTI(anodes,SFEA):
  sidlst[bias]=   1101;//h       b1/s1  cr2/sl7
  sidlst[bias+1]= 1102;//q  
  sidlst[bias+2]= 2101;//h       b2/s1 
  sidlst[bias+3]= 2102;//q  
  sidlst[bias+4]= 3101;//h       b3/s1 
  sidlst[bias+5]= 3102;//q  
  sidlst[bias+6]= 4101;//h       b4/s1 
  sidlst[bias+7]= 4102;//q  
//--->TOF(anodes,SFEA):
  sidlst[bias+8]= 407200;//t   l4/b7/s2
  sidlst[bias+9]= 407201;//h
  sidlst[bias+10]=407202;//q
  sidlst[bias+11]=408200;//t      b8/s2
  sidlst[bias+12]=408201;//h
  sidlst[bias+13]=408202;//q
  bias+=14;
//--->TOF(dynodes,SFEC):
  sidlst[bias]=   301212;//q  l3/b1/s2/d1 cr2/sl8(aux)
  sidlst[bias+1]= 301222;//q           d2 
  sidlst[bias+2]= 302212;//q     b2/s2/d1 
  sidlst[bias+3]= 302222;//q           d2 
  sidlst[bias+4]= 303212;//q     b3/s2/d1 
  sidlst[bias+5]= 303222;//q           d2 
  sidlst[bias+6]= 304212;//q     b4/s2/d1 
  sidlst[bias+7]= 304222;//q           d2 
  sidlst[bias+8]= 305212;//q     b5/s2/d1 
  sidlst[bias+9]= 305222;//q           d2 
  sidlst[bias+10]=401212;//q  l4/b1/s2/d1 
  sidlst[bias+11]=401222;//q           d2 
  sidlst[bias+12]=401232;//q           d3 
  sidlst[bias+13]=402212;//q     b2/s2/d1 
  sidlst[bias+14]=402222;//q           d2 
  sidlst[bias+15]=403212;//q     b3/s2/d1 
  sidlst[bias+16]=403222;//q           d2 
  sidlst[bias+17]=404212;//q     b4/s2/d1 
  sidlst[bias+18]=404222;//q           d2 
  sidlst[bias+19]=306212;//q  l3/b6/s2/d1 
  bias+=20;  
  sidlst[bias]=   306222;//q  l3/b6/s2/d2 cr2/sl9(aux)
  sidlst[bias+1]= 307212;//q     b7/s2/d1 
  sidlst[bias+2]= 307222;//q           d2 
  sidlst[bias+3]= 308212;//q     b8/s2/d1 
  sidlst[bias+4]= 308222;//q           d2 
  sidlst[bias+5]= 309212;//q     b9/s2/d1 
  sidlst[bias+6]= 309222;//q           d2 
  sidlst[bias+7]= 310212;//q    b10/s2/d1 
  sidlst[bias+8]= 310222;//q           d2 
  sidlst[bias+9]= 405212;//q  l4/b5/s2/d1 
  sidlst[bias+10]=405222;//q           d2 
  sidlst[bias+11]=406212;//q     b6/s2/d1 
  sidlst[bias+12]=406222;//q           d2 
  sidlst[bias+13]=407212;//q     b7/s2/d1 
  sidlst[bias+14]=407222;//q           d2 
  sidlst[bias+15]=408212;//q     b8/s2/d1 
  sidlst[bias+16]=408222;//q           d2 
  sidlst[bias+17]=408232;//q           d3 
  sidlst[bias+18]=0;//              empty  
  sidlst[bias+19]=0;//              empty  
  bias+=20;  
//
// <======================================== crate-SC3 (bot/ram)  
//--->TOF(anodes,SFET):
  sidlst[bias]=   301100;//t   l3/b1/s1  cr3/sl2
  sidlst[bias+1]= 301101;//h
  sidlst[bias+2]= 301102;//q
  sidlst[bias+3]= 302100;//t      b2/s1
  sidlst[bias+4]= 302101;//h
  sidlst[bias+5]= 302102;//q
  sidlst[bias+6]= 303100;//t      b3/s1
  sidlst[bias+7]= 303101;//h 
  sidlst[bias+8]= 303102;//q
  sidlst[bias+9]= 304100;//t      b4/s1
  sidlst[bias+10]=304101;//h
  sidlst[bias+11]=304102;//q
  bias+=12;
  sidlst[bias]=   305100;//t      b5/s1  cr3/sl3
  sidlst[bias+1]= 305101;//h
  sidlst[bias+2]= 305102;//q
  sidlst[bias+3]= 401100;//t   l4/b1/s1  
  sidlst[bias+4]= 401101;//h
  sidlst[bias+5]= 401102;//q
  sidlst[bias+6]= 402100;//t      b2/s1
  sidlst[bias+7]= 402101;//h
  sidlst[bias+8]= 402102;//q
  sidlst[bias+9]= 403100;//t      b3/s1
  sidlst[bias+10]=403101;//h
  sidlst[bias+11]=403102;//q
  bias+=12;
  sidlst[bias]=   404100;//t      b4/s1  cr3/sl5
  sidlst[bias+1]= 404101;//h
  sidlst[bias+2]= 404102;//q
  sidlst[bias+3]= 306100;//t   l3/b6/s1
  sidlst[bias+4]= 306101;//h
  sidlst[bias+5]= 306102;//q
  sidlst[bias+6]= 307100;//t      b7/s1
  sidlst[bias+7]= 307101;//h
  sidlst[bias+8]= 307102;//q
  sidlst[bias+9]= 308100;//t      b8/s1
  sidlst[bias+10]=308101;//h
  sidlst[bias+11]=308102;//q
  bias+=12;
  sidlst[bias]=   309100;//t      b9/s1  cr3/sl6
  sidlst[bias+1]= 309101;//h
  sidlst[bias+2]= 309102;//q
  sidlst[bias+3]= 310100;//t     b10/s1  
  sidlst[bias+4]= 310101;//h
  sidlst[bias+5]= 310102;//q
  sidlst[bias+6]= 405100;//t   l4/b5/s1
  sidlst[bias+7]= 405101;//h
  sidlst[bias+8]= 405102;//q
  sidlst[bias+9]= 406100;//t      b6/s1
  sidlst[bias+10]=406101;//h
  sidlst[bias+11]=406102;//q
  bias+=12;
//--->ANTI(anodes,SFEA):
  sidlst[bias]=   5101;//h       b5/s1  cr3/sl7
  sidlst[bias+1]= 5102;//q  
  sidlst[bias+2]= 6101;//h       b6/s1 
  sidlst[bias+3]= 6102;//q  
  sidlst[bias+4]= 7101;//h       b7/s1 
  sidlst[bias+5]= 7102;//q  
  sidlst[bias+6]= 8101;//h       b8/s1 
  sidlst[bias+7]= 8102;//q  
//--->TOF(anodes,SFEA):
  sidlst[bias+8]= 407100;//t   l4/b7/s1
  sidlst[bias+9]= 407101;//h
  sidlst[bias+10]=407102;//q
  sidlst[bias+11]=408100;//t      b8/s1
  sidlst[bias+12]=408101;//h
  sidlst[bias+13]=408102;//q
  bias+=14;
//--->TOF(dynodes,SFEC):
  sidlst[bias]=   301112;//q  l3/b1/s1/d1 cr3/sl8(aux)
  sidlst[bias+1]= 301122;//q           d2 
  sidlst[bias+2]= 302112;//q     b2/s1/d1 
  sidlst[bias+3]= 302122;//q           d2 
  sidlst[bias+4]= 303112;//q     b3/s1/d1 
  sidlst[bias+5]= 303122;//q           d2 
  sidlst[bias+6]= 304112;//q     b4/s1/d1 
  sidlst[bias+7]= 304122;//q           d2 
  sidlst[bias+8]= 305112;//q     b5/s1/d1 
  sidlst[bias+9]= 305122;//q           d2 
  sidlst[bias+10]=401112;//q  l4/b1/s1/d1 
  sidlst[bias+11]=401122;//q           d2 
  sidlst[bias+12]=401132;//q           d3 
  sidlst[bias+13]=402112;//q     b2/s1/d1 
  sidlst[bias+14]=402122;//q           d2 
  sidlst[bias+15]=403112;//q     b3/s1/d1 
  sidlst[bias+16]=403122;//q           d2 
  sidlst[bias+17]=404112;//q     b4/s1/d1 
  sidlst[bias+18]=404122;//q           d2 
  sidlst[bias+19]=306112;//q  l3/b6/s1/d1 
  bias+=20;  
  sidlst[bias]=   306122;//q  l3/b6/s1/d2 cr3/sl9(aux)
  sidlst[bias+1]= 307112;//q     b7/s1/d1 
  sidlst[bias+2]= 307122;//q           d2 
  sidlst[bias+3]= 308112;//q     b8/s1/d1 
  sidlst[bias+4]= 308122;//q           d2 
  sidlst[bias+5]= 309112;//q     b9/s1/d1 
  sidlst[bias+6]= 309122;//q           d2 
  sidlst[bias+7]= 310112;//q    b10/s1/d1 
  sidlst[bias+8]= 310122;//q           d2 
  sidlst[bias+9]= 405112;//q  l4/b5/s1/d1 
  sidlst[bias+10]=405122;//q           d2 
  sidlst[bias+11]=406112;//q     b6/s1/d1 
  sidlst[bias+12]=406122;//q           d2 
  sidlst[bias+13]=407112;//q     b7/s1/d1 
  sidlst[bias+14]=407122;//q           d2 
  sidlst[bias+15]=408112;//q     b8/s1/d1 
  sidlst[bias+16]=408122;//q           d2 
  sidlst[bias+17]=408132;//q           d3 
  sidlst[bias+18]=0;//              empty  
  sidlst[bias+19]=0;//              empty  
  bias+=20;
  cout<<"AMSSCIds::init: total TOF+ANTI h/w-channels initialized(incl.empty): "<<bias<<endl;
//
//---> prepare hwch->hwid table:
//
  int swid,hwid;
  int16u slt,chmx,hwch(0);
  for(int cr=0;cr<SCCRAT;cr++){
    for(int sl=0;sl<SCSLTM;sl++){
      slt=sltymap[cr][sl];
      chmx=ochpsty[slt-1];
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
  int mtmn,pmt,hch;
  int16u swch=0;//1st- for TOF-channels
  for(int il=0;il<TOF2DBc::getnplns();il++){
    for(int ib=0;ib<TOF2DBc::getbppl(il);ib++){
      for(int is=0;is<2;is++){
        for(int ip=0;ip<TOF2DBc::npmtps(il,ib)+1;ip++){
	  if(ip==0)mtmn=0;//anodes
	  else mtmn=SCAMTS-SCDMTS;
	  for(int mt=mtmn;mt<SCAMTS;mt++){
	    swid=100000*(il+1)+1000*(ib+1)+100*(is+1)+10*ip+mt;//LBBSPM
	    for(int hc=0;hc<bias;hc++){//<-search in sidlst
	      if(swid==sidlst[hc]){
	        hch=hc;
	        goto Found1;
	      }
	    }//hwch
	    cout<<"AMSSCIds::init:missing TOF-swid in sidlst, swid="<<swid<<endl;
	    exit(1);
Found1:
	    hwid=hidls[hch];
	    hidlst[swch]=hwid;
	    swch+=1;
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
  for(int ib=0;ib<MAXANTI;ib++){
    for(int is=0;is<2;is++){
      mtmn=1;
      pmt=0;
      for(int mt=mtmn;mt<=ANAMTS;mt++){
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
}
//---------------
int16u AMSSCIds::swseqn(int dtype, int16u layer, int16u bar, int16u side,
                                                 int16u pmt, int16u mtyp){
//
//dtype=1/2;layer=0,1...;bar=0,1,...;side=0,1;pmt=0,1,2,3;mtyp=0,1,2.
//
  int16u swch(0);//glogal sequential sw-channel(incl pmts and mtyps)
  if(dtype==1){//TOF  
    for(int il=0;il<layer;il++){// 1->count in layers befor current one
      for(int ib=0;ib<TOF2DBc::getbppl(il);ib++){
        swch+=(TOF2DBc::npmtps(il,ib)*SCDMTS+SCAMTS)*2;//2 sides
      }
    }
    for(int ib=0;ib<bar;ib++){// 2->count in curr.layer (bars befor current one)
      swch+=(TOF2DBc::npmtps(layer,ib)*SCDMTS+SCAMTS)*2;//2 sides
    }
    for(int is=0;is<side;is++){// 3->count in curr.bar (sides befor current one)
        swch+=(TOF2DBc::npmtps(layer,bar)*SCDMTS+SCAMTS);
    }
    if(pmt==0)swch+=mtyp;// 4->count in current side pmts*mtypes 
    else{
      swch+=SCAMTS;
      for(int pm=0;pm<pmt;pm++)swch+=(TOF2DBc::npmtps(layer,bar)*SCDMTS);
      swch+=(mtyp-2);
    }
  }
  else{//ANTI
    swch+=totswch[0];//ANTI-channels i count after TOF ones
    swch+=(bar*2*ANAMTS);//count in bars befor current one
    if(side==1)swch+=ANAMTS;//count in side befor current one
    else swch+=(mtyp-1);//add mtypes in current bar/side
  }
  return swch;
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
      hwch+=ochpsty[slt];
    }
  }
  for(int sl=0;sl<slot;sl++){//2->count in slots befor current one(in crate crate)
    slt=sltymap[crate][sl];
    hwch+=ochpsty[slt];
  }
  hwch+=rdch;//3->count in current slot
  return hwch;
} 
//-----------------
int AMSSCIds::hw2swid(int16u crate, int16u slot, int16u rdch){
//crate=0,1,...;slot=0,1,...;rdch=0,1,...
// transform h/w-address into s/w-id:
  int16u hwch;
  int swid;
  hwch=hwseqn(crate,slot,rdch);
  swid=sidlst[hwch];
  return swid;
//
}



