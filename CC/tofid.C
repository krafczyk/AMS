#include <tofid.h>
using namespace TOF2GC;
using namespace ANTI2C;
  int AMSSCIds::totswch[2];//total s/w-channels for TOF and ANTI
  int AMSSCIds::sidlst[SCRCMX];//swid-list(vs hwch) 
  int AMSSCIds::hidlst[SCRCMX];//hwid-list(vs swch)
  int AMSSCIds::hidls[SCRCMX];//hwid-list(vs hwch)
//outp.channels for each slot(card) type: 
  int16u AMSSCIds::ochpsty[TOF2GC::SCSLTY]={16,12,20,40,0,0};
//slot(card)-type map in crates  
  int16u AMSSCIds::sltymap[SCCRAT][SCSLTM]={5,1,1,6,1,1,2,4,4,  //top cr-1
                                            5,1,1,6,1,1,2,4,4,  //top cr-2
					    5,1,1,6,1,1,3,4,4,  //bot cr-3
					    5,1,1,6,1,1,3,4,4   //bot cr-4
					    };
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
  _dtype=1;//tof
  if(_layer==0)_dtype=2;//anti
  else _layer-=1;//tof L-numbering 0,1,...
  _bar=(swid%100000)/1000-1;
  _side=(swid%1000)/100-1;
  _pmt=(swid%100)/10;
  _mtyp=swid%10;

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
  _mtyp=mtyp;//0,1,2,3(fTDC,sTDC,Qh,Ql)
  
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
  _mtyp=mtyp;//1,2,3(no 0==fTDC)
  
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
    _dtype=1;//tof
    if(_layer==0)_dtype=2;//anti
    else _layer-=1;//tof L-numbering 0,1,...
    _bar=(_swid%100000)/1000-1;
    _side=(_swid%1000)/100-1;
    _pmt=(_swid%100)/10;
    _mtyp=_swid%10;
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
    if(cardids[_crate][i]==crdid)return(i);
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
  cout<<"AMSSCIds::init:total s/w-channel (tof/anti)="<<totswch[0]<<" "<<totswch[1]<<endl;
//
//--->fill sid's as function of sequential number of h/w-channel
//below used: tof_swid->LBBSPM, anti_swid->BSPM.
//
  int bias(0);
// <=========================================== crate-1 (top)
//--->TOF(anodes,SFET):
  sidlst[bias]=   101100;//t   l1/b1/s1  cr1/sl2
  sidlst[bias+1]= 101101;//h
  sidlst[bias+2]= 101102;//qh
  sidlst[bias+3]= 101103;//ql
  sidlst[bias+4]= 102100;//t      b2/s1
  sidlst[bias+5]= 102101;//h
  sidlst[bias+6]= 102102;//qh
  sidlst[bias+7]= 102103;//ql
  sidlst[bias+8]= 103100;//t      b3/s1
  sidlst[bias+9]= 103101;//h 
  sidlst[bias+10]=103102;//qh
  sidlst[bias+11]=103103;//ql
  sidlst[bias+12]=104100;//t     b4/s1
  sidlst[bias+13]=104101;//h
  sidlst[bias+14]=104102;//qh
  sidlst[bias+15]=104103;//ql
  bias+=16;
  sidlst[bias]=   201100;//t  l2/b1/s1  cr1/sl3
  sidlst[bias+1]= 201101;//h
  sidlst[bias+2]= 201102;//qh
  sidlst[bias+3]= 201103;//ql
  sidlst[bias+4]= 202100;//t     b2/s1
  sidlst[bias+5]= 202101;//h
  sidlst[bias+6]= 202102;//qh
  sidlst[bias+7]= 202103;//ql
  sidlst[bias+8]= 203100;//t     b3/s1
  sidlst[bias+9]= 203101;//h
  sidlst[bias+10]=203102;//qh
  sidlst[bias+11]=203103;//ql
  sidlst[bias+12]=204100;//t     b4/s1
  sidlst[bias+13]=204101;//h
  sidlst[bias+14]=204102;//qh
  sidlst[bias+15]=204103;//ql
  bias+=16;
  sidlst[bias]=   101200;//t  l1/b1/s2  cr1/sl5
  sidlst[bias+1]= 101201;//h
  sidlst[bias+2]= 101202;//qh
  sidlst[bias+3]= 101203;//ql
  sidlst[bias+4]= 102200;//t     b2/s2
  sidlst[bias+5]= 102201;//h
  sidlst[bias+6]= 102202;//qh
  sidlst[bias+7]= 102203;//ql
  sidlst[bias+8]= 103200;//t     b3/s2
  sidlst[bias+9]= 103201;//h
  sidlst[bias+10]=103202;//qh
  sidlst[bias+11]=103203;//ql
  sidlst[bias+12]=104200;//t     b4/s2
  sidlst[bias+13]=104201;//h
  sidlst[bias+14]=104202;//qh
  sidlst[bias+15]=104203;//ql
  bias+=16;
  sidlst[bias]=   201200;//t  l2/b1/s2  cr1/sl6
  sidlst[bias+1]= 201201;//h
  sidlst[bias+2]= 201202;//qh
  sidlst[bias+3]= 201203;//ql
  sidlst[bias+4]= 202200;//t     b2/s2
  sidlst[bias+5]= 202201;//h
  sidlst[bias+6]= 202202;//qh
  sidlst[bias+7]= 202203;//ql
  sidlst[bias+8]= 203200;//t     b3/s2
  sidlst[bias+9]= 203201;//h
  sidlst[bias+10]=203202;//qh
  sidlst[bias+11]=203203;//ql
  sidlst[bias+12]=204200;//t     b4/s2
  sidlst[bias+13]=204201;//h
  sidlst[bias+14]=204202;//qh
  sidlst[bias+15]=204203;//ql
  bias+=16;
//--->ANTI(anodes,SFEA):
  sidlst[bias]=   1101;//h       b1/s1  cr1/sl7
  sidlst[bias+1]= 1102;//qh  
  sidlst[bias+2]= 1103;//ql  
  sidlst[bias+3]= 2101;//h       b2/s1 
  sidlst[bias+4]= 2102;//qh  
  sidlst[bias+5]= 2103;//ql  
  sidlst[bias+6]= 3101;//h       b3/s1 
  sidlst[bias+7]= 3102;//qh  
  sidlst[bias+8]= 3103;//ql  
  sidlst[bias+9]= 4101;//h       b4/s1 
  sidlst[bias+10]=4102;//qh  
  sidlst[bias+11]=4103;//ql
  bias+=12;
//--->TOF(dynodes,SFEC):
  sidlst[bias]=   101112;//qh  l1/b1/s1/d1 cr1/sl8(aux)
  sidlst[bias+1]= 101113;//ql  
  sidlst[bias+2]= 101122;//qh           d2 
  sidlst[bias+3]= 101123;//ql  
  sidlst[bias+4]= 101132;//qh           d3 
  sidlst[bias+5]= 101133;//ql  
  sidlst[bias+6]= 102112;//qh     b2/s1/d1 
  sidlst[bias+7]= 102113;//ql  
  sidlst[bias+8]= 102122;//qh           d2 
  sidlst[bias+9]= 102123;//ql  
  sidlst[bias+10]=103112;//qh     b3/s1/d1 
  sidlst[bias+11]=103113;//ql  
  sidlst[bias+12]=103122;//qh           d2 
  sidlst[bias+13]=103123;//ql  
  sidlst[bias+14]=104112;//qh     b4/s1/d1 
  sidlst[bias+15]=104113;//ql  
  sidlst[bias+16]=104122;//qh           d2 
  sidlst[bias+17]=104123;//ql  
  sidlst[bias+18]=201112;//qh  l2/b1/s1/d1 
  sidlst[bias+19]=201113;//ql  
  sidlst[bias+20]=201122;//qh           d2 
  sidlst[bias+21]=201123;//ql  
  sidlst[bias+22]=202112;//qh     b2/s1/d1 
  sidlst[bias+23]=202113;//ql  
  sidlst[bias+24]=202122;//qh           d2 
  sidlst[bias+25]=202123;//ql  
  sidlst[bias+26]=203112;//qh     b3/s1/d1 
  sidlst[bias+27]=203113;//ql  
  sidlst[bias+28]=203122;//qh           d2 
  sidlst[bias+29]=203123;//ql  
  sidlst[bias+30]=204112;//qh     b4/s1/d1 
  sidlst[bias+31]=204113;//ql  
  sidlst[bias+32]=204122;//qh           d2 
  sidlst[bias+33]=204123;//ql  
  sidlst[bias+34]=101212;//qh  l1/b1/s2/d1 
  sidlst[bias+35]=101213;//ql  
  sidlst[bias+36]=0;//             empty
  sidlst[bias+37]=0;//             empty
  sidlst[bias+38]=0;//             empty
  sidlst[bias+39]=0;//             empty
  bias+=40;  
  sidlst[bias]=   101222;//qh l1/b1/s2/d2 cr1/sl9(aux)
  sidlst[bias+1]= 101223;//ql  
  sidlst[bias+2]= 101232;//qh          d3 
  sidlst[bias+3]= 101233;//ql  
  sidlst[bias+4]= 102212;//qh    b2/s2/d1 
  sidlst[bias+5]= 102213;//ql  
  sidlst[bias+6]= 102222;//qh          d2 
  sidlst[bias+7]= 102223;//ql  
  sidlst[bias+8]= 103212;//qh    b3/s2/d1 
  sidlst[bias+9]= 103213;//ql  
  sidlst[bias+10]=103222;//qh          d2 
  sidlst[bias+11]=103223;//ql  
  sidlst[bias+12]=104212;//qh    b4/s2/d1 
  sidlst[bias+13]=104213;//ql  
  sidlst[bias+14]=104222;//qh          d2 
  sidlst[bias+15]=104223;//ql  
  sidlst[bias+16]=201212;//qh l2/b1/s2/d1 
  sidlst[bias+17]=201213;//ql  
  sidlst[bias+18]=201222;//qh          d2 
  sidlst[bias+19]=201223;//ql  
  sidlst[bias+20]=202212;//qh    b2/s2/d1 
  sidlst[bias+21]=202213;//ql  
  sidlst[bias+22]=202222;//qh          d2 
  sidlst[bias+23]=202223;//ql  
  sidlst[bias+24]=203212;//qh    b3/s2/d1 
  sidlst[bias+25]=203213;//ql  
  sidlst[bias+26]=203222;//qh          d2 
  sidlst[bias+27]=203223;//ql  
  sidlst[bias+28]=204212;//qh    b4/s2/d1 
  sidlst[bias+29]=204213;//ql  
  sidlst[bias+30]=204222;//qh          d2 
  sidlst[bias+31]=204223;//ql
  sidlst[bias+32]=0;//              empty  
  sidlst[bias+33]=0;//              empty  
  sidlst[bias+34]=0;//              empty  
  sidlst[bias+35]=0;//              empty  
  sidlst[bias+36]=0;//              empty  
  sidlst[bias+37]=0;//              empty  
  sidlst[bias+38]=0;//              empty  
  sidlst[bias+39]=0;//              empty
  bias+=40;  
//
// <======================================== crate-2 (top)  
//--->TOF(anodes,SFET):
  sidlst[bias]=   105100;//t   l1/b5/s1  cr2/sl2
  sidlst[bias+1]= 105101;//h
  sidlst[bias+2]= 105102;//qh
  sidlst[bias+3]= 105103;//ql
  sidlst[bias+4]= 106100;//t      b6/s1
  sidlst[bias+5]= 106101;//h
  sidlst[bias+6]= 106102;//qh
  sidlst[bias+7]= 106103;//ql
  sidlst[bias+8]= 107100;//t      b7/s1
  sidlst[bias+9]= 107101;//h
  sidlst[bias+10]=107102;//qh
  sidlst[bias+11]=107103;//ql
  sidlst[bias+12]=108100;//t      b8/s1
  sidlst[bias+13]=108101;//h
  sidlst[bias+14]=108102;//qh
  sidlst[bias+15]=108103;//ql
  bias+=16;
  sidlst[bias]=   205100;//t  l2/b5/s1  cr2/sl3
  sidlst[bias+1]= 205101;//h
  sidlst[bias+2]= 205102;//qh
  sidlst[bias+3]= 205103;//ql
  sidlst[bias+4]= 206100;//t     b6/s1
  sidlst[bias+5]= 206101;//h
  sidlst[bias+6]= 206102;//qh
  sidlst[bias+7]= 206103;//ql
  sidlst[bias+8]= 207100;//t     b7/s1
  sidlst[bias+9]= 207101;//h
  sidlst[bias+10]=207102;//qh
  sidlst[bias+11]=207103;//ql
  sidlst[bias+12]=208100;//t     b8/s1
  sidlst[bias+13]=208101;//h
  sidlst[bias+14]=208102;//qh
  sidlst[bias+15]=208103;//ql
  bias+=16;
  sidlst[bias]=   105200;//t  l1/b5/s2  cr2/sl5
  sidlst[bias+1]= 105201;//h
  sidlst[bias+2]= 105202;//qh
  sidlst[bias+3]= 105203;//ql
  sidlst[bias+4]= 106200;//t     b6/s2
  sidlst[bias+5]= 106201;//h
  sidlst[bias+6]= 106202;//qh
  sidlst[bias+7]= 106203;//ql
  sidlst[bias+8]= 107200;//t     b7/s2
  sidlst[bias+9]= 107201;//h
  sidlst[bias+10]=107202;//qh
  sidlst[bias+11]=107203;//ql
  sidlst[bias+12]=108200;//t     b8/s2
  sidlst[bias+13]=108201;//h
  sidlst[bias+14]=108202;//qh
  sidlst[bias+15]=108203;//ql
  bias+=16;
  sidlst[bias]=   205200;//t  l2/b5/s2  cr2/sl6
  sidlst[bias+1]= 205201;//h
  sidlst[bias+2]= 205202;//qh
  sidlst[bias+3]= 205203;//ql
  sidlst[bias+4]= 206200;//t     b6/s2
  sidlst[bias+5]= 206201;//h
  sidlst[bias+6]= 206202;//qh
  sidlst[bias+7]= 206203;//ql
  sidlst[bias+8]= 207200;//t     b7/s2
  sidlst[bias+9]= 207201;//h
  sidlst[bias+10]=207202;//qh
  sidlst[bias+11]=207203;//ql
  sidlst[bias+12]=208200;//t     b8/s2
  sidlst[bias+13]=208201;//h
  sidlst[bias+14]=208202;//qh
  sidlst[bias+15]=208203;//ql
  bias+=16;
//--->ANTI(anodes,SFEA):  
  sidlst[bias]=   5101;//h     b5/s1  cr2/sl7
  sidlst[bias+1]= 5102;//qh  
  sidlst[bias+2]= 5103;//ql  
  sidlst[bias+3]= 6101;//h     b6/s1 
  sidlst[bias+4]= 6102;//qh  
  sidlst[bias+5]= 6103;//ql  
  sidlst[bias+6]= 7101;//h     b7/s1 
  sidlst[bias+7]= 7102;//qh  
  sidlst[bias+8]= 7103;//ql  
  sidlst[bias+9]= 8101;//h     b8/s1 
  sidlst[bias+10]=8102;//qh  
  sidlst[bias+11]=8103;//ql
  bias+=12;
//--->TOF(dynodes,SFEC):
  sidlst[bias]=   105112;//qh  l1/b5/s1/d1 cr2/sl8(aux)
  sidlst[bias+1]= 105113;//ql  
  sidlst[bias+2]= 105122;//qh           d2 
  sidlst[bias+3]= 105123;//ql  
  sidlst[bias+4]= 106112;//qh     b6/s1/d1 
  sidlst[bias+5]= 106113;//ql  
  sidlst[bias+6]= 106122;//qh           d2 
  sidlst[bias+7]= 106123;//ql  
  sidlst[bias+8]= 107112;//qh     b7/s1/d1 
  sidlst[bias+9]= 107113;//ql  
  sidlst[bias+10]=107122;//qh           d2 
  sidlst[bias+11]=107123;//ql  
  sidlst[bias+12]=108112;//qh     b8/s1/d1 
  sidlst[bias+13]=108113;//ql  
  sidlst[bias+14]=108122;//qh           d2 
  sidlst[bias+15]=108123;//ql  
  sidlst[bias+16]=108132;//qh           d3
  sidlst[bias+17]=108133;//ql            
  sidlst[bias+18]=205112;//qh  l2/b5/s1/d1 
  sidlst[bias+19]=205113;//ql  
  sidlst[bias+20]=205122;//qh           d2 
  sidlst[bias+21]=205123;//ql  
  sidlst[bias+22]=206112;//qh     b6/s1/d1 
  sidlst[bias+23]=206113;//ql  
  sidlst[bias+24]=206122;//qh           d2 
  sidlst[bias+25]=206123;//ql  
  sidlst[bias+26]=207112;//qh     b7/s1/d1 
  sidlst[bias+27]=207113;//ql  
  sidlst[bias+28]=207122;//qh           d2 
  sidlst[bias+29]=207123;//ql  
  sidlst[bias+30]=208112;//qh     b8/s1/d1 
  sidlst[bias+31]=208113;//ql  
  sidlst[bias+32]=208122;//qh           d2 
  sidlst[bias+33]=208123;//ql  
  sidlst[bias+34]=105212;//qh  l1/b5/s2/d1 
  sidlst[bias+35]=105213;//ql  
  sidlst[bias+36]=0;//             empty
  sidlst[bias+37]=0;//             empty
  sidlst[bias+38]=0;//             empty
  sidlst[bias+39]=0;//             empty
  bias+=40;  
  sidlst[bias]=   105222;//qh  l1/b5/s2/d2 cr2/sl9(aux)
  sidlst[bias+1]= 105223;//ql  
  sidlst[bias+2]= 106212;//qh     b6/s2/d1 
  sidlst[bias+3]= 106213;//ql  
  sidlst[bias+4]= 106222;//qh           d2 
  sidlst[bias+5]= 106223;//ql  
  sidlst[bias+6]= 107212;//qh     b7/s2/d1 
  sidlst[bias+7]= 107213;//ql  
  sidlst[bias+8]= 107222;//qh           d2 
  sidlst[bias+9]= 107223;//ql  
  sidlst[bias+10]=108212;//qh     b8/s2/d1 
  sidlst[bias+11]=108213;//ql  
  sidlst[bias+12]=108222;//qh           d2 
  sidlst[bias+13]=108223;//ql  
  sidlst[bias+14]=108232;//qh           d3 
  sidlst[bias+15]=108233;//ql  
  sidlst[bias+16]=205212;//qh  l2/b5/s2/d1 
  sidlst[bias+17]=205213;//ql  
  sidlst[bias+18]=205222;//qh           d2 
  sidlst[bias+19]=205223;//ql  
  sidlst[bias+20]=206212;//qh     b6/s2/d1 
  sidlst[bias+21]=206213;//ql  
  sidlst[bias+22]=206222;//qh           d2 
  sidlst[bias+23]=206223;//ql  
  sidlst[bias+24]=207212;//qh     b7/s2/d1 
  sidlst[bias+25]=207213;//ql  
  sidlst[bias+26]=207222;//qh           d2 
  sidlst[bias+27]=207223;//ql  
  sidlst[bias+28]=208212;//qh     b8/s2/d1 
  sidlst[bias+29]=208213;//ql  
  sidlst[bias+30]=208222;//qh           d2 
  sidlst[bias+31]=208223;//ql
  sidlst[bias+32]=0;//              empty  
  sidlst[bias+33]=0;//              empty  
  sidlst[bias+34]=0;//              empty  
  sidlst[bias+35]=0;//              empty  
  sidlst[bias+36]=0;//              empty  
  sidlst[bias+37]=0;//              empty  
  sidlst[bias+38]=0;//              empty  
  sidlst[bias+39]=0;//              empty
  bias+=40;
//  
// <=========================================== crate-3 (bot)
//--->TOF(anodes,SFET):
  sidlst[bias]=   301100;//t   l3/b1/s1  cr3/sl2
  sidlst[bias+1]= 301101;//h
  sidlst[bias+2]= 301102;//qh
  sidlst[bias+3]= 301103;//ql
  sidlst[bias+4]= 302100;//t      b2/s1
  sidlst[bias+5]= 302101;//h
  sidlst[bias+6]= 302102;//qh
  sidlst[bias+7]= 302103;//ql
  sidlst[bias+8]= 303100;//t      b3/s1
  sidlst[bias+9]= 303101;//h 
  sidlst[bias+10]=303102;//qh
  sidlst[bias+11]=303103;//ql
  sidlst[bias+12]=304100;//t      b4/s1
  sidlst[bias+13]=304101;//h
  sidlst[bias+14]=304102;//qh
  sidlst[bias+15]=304103;//ql
  bias+=16;
  sidlst[bias]=   305100;//t      b5/s1  cr3/sl3
  sidlst[bias+1]= 305101;//h
  sidlst[bias+2]= 305102;//qh
  sidlst[bias+3]= 305103;//ql
  sidlst[bias+4]= 401100;//t   l4/b1/s1  
  sidlst[bias+5]= 401101;//h
  sidlst[bias+6]= 401102;//qh
  sidlst[bias+7]= 401103;//ql
  sidlst[bias+8]= 402100;//t      b2/s1
  sidlst[bias+9]= 402101;//h
  sidlst[bias+10]=402102;//qh
  sidlst[bias+11]=402103;//ql
  sidlst[bias+12]=403100;//t      b3/s1
  sidlst[bias+13]=403101;//h
  sidlst[bias+14]=403102;//qh
  sidlst[bias+15]=403103;//ql
  bias+=16;
  sidlst[bias]=   404100;//t      b4/s1  cr3/sl5
  sidlst[bias+1]= 404101;//h
  sidlst[bias+2]= 404102;//qh
  sidlst[bias+3]= 404103;//ql
  sidlst[bias+4]= 301200; //t  l3/b1/s2
  sidlst[bias+5]= 301201;//h
  sidlst[bias+6]= 301202;//qh
  sidlst[bias+7]= 301203;//ql
  sidlst[bias+8]= 302200;//t      b2/s2
  sidlst[bias+9]= 302201;//h
  sidlst[bias+10]=302202;//qh
  sidlst[bias+11]=302203;//ql
  sidlst[bias+12]=303200;//t      b3/s2
  sidlst[bias+13]=303201;//h
  sidlst[bias+14]=303202;//qh
  sidlst[bias+15]=303203;//ql
  bias+=16;
  sidlst[bias]=   304200;//t      b4/s2  cr3/sl6
  sidlst[bias+1]= 304201;//h
  sidlst[bias+2]= 304202;//qh
  sidlst[bias+3]= 304203;//ql
  sidlst[bias+4]= 305200;//t      b5/s2  
  sidlst[bias+5]= 305201;//h
  sidlst[bias+6]= 305202;//qh
  sidlst[bias+7]= 305203;//ql
  sidlst[bias+8]= 401200;//t   l4/b1/s2
  sidlst[bias+9]= 401201;//h
  sidlst[bias+10]=401202;//qh
  sidlst[bias+11]=401203;//ql
  sidlst[bias+12]=402200;//t      b2/s2
  sidlst[bias+13]=402201;//h
  sidlst[bias+14]=402202;//qh
  sidlst[bias+15]=402203;//ql
  bias+=16;
//--->ANTI(anodes,SFEA):
  sidlst[bias]=   1201;//h       b1/s2  cr3/sl7
  sidlst[bias+1]= 1202;//qh  
  sidlst[bias+2]= 1203;//ql  
  sidlst[bias+3]= 2201;//h       b2/s2 
  sidlst[bias+4]= 2202;//qh  
  sidlst[bias+5]= 2203;//ql  
  sidlst[bias+6]= 3201;//h       b3/s2 
  sidlst[bias+7]= 3202;//qh  
  sidlst[bias+8]= 3203;//ql  
  sidlst[bias+9]= 4201;//h       b4/s2 
  sidlst[bias+10]=4202;//qh  
  sidlst[bias+11]=4203;//ql
//--->TOF(anodes,SFEA):
  sidlst[bias+12]=403200;//t   l4/b3/s2
  sidlst[bias+13]=403201;//h
  sidlst[bias+14]=403202;//qh
  sidlst[bias+15]=403203;//ql
  sidlst[bias+16]=404200;//t      b4/s2
  sidlst[bias+17]=404201;//h
  sidlst[bias+18]=404202;//qh
  sidlst[bias+19]=404203;//ql
  bias+=20;
//--->TOF(dynodes,SFEC):
  sidlst[bias]=   301112;//qh  l3/b1/s1/d1 cr3/sl8(aux)
  sidlst[bias+1]= 301113;//ql  
  sidlst[bias+2]= 301122;//qh           d2 
  sidlst[bias+3]= 301123;//ql  
  sidlst[bias+4]= 302112;//qh     b2/s1/d1 
  sidlst[bias+5]= 302113;//ql  
  sidlst[bias+6]= 302122;//qh           d2 
  sidlst[bias+7]= 302123;//ql  
  sidlst[bias+8]= 303112;//qh     b3/s1/d1 
  sidlst[bias+9]= 303113;//ql  
  sidlst[bias+10]=303122;//qh           d2 
  sidlst[bias+11]=303123;//ql  
  sidlst[bias+12]=304112;//qh     b4/s1/d1 
  sidlst[bias+13]=304113;//ql  
  sidlst[bias+14]=304122;//qh           d2 
  sidlst[bias+15]=304123;//ql  
  sidlst[bias+16]=305112;//qh     b5/s1/d1 
  sidlst[bias+17]=305113;//ql  
  sidlst[bias+18]=305122;//qh           d2 
  sidlst[bias+19]=305123;//ql  
  sidlst[bias+20]=401112;//qh  l4/b1/s1/d1 
  sidlst[bias+21]=401113;//ql  
  sidlst[bias+22]=401122;//qh           d2 
  sidlst[bias+23]=401123;//ql  
  sidlst[bias+24]=401132;//qh           d3 
  sidlst[bias+25]=401133;//ql  
  sidlst[bias+26]=402112;//qh     b2/s1/d1 
  sidlst[bias+27]=402113;//ql  
  sidlst[bias+28]=402122;//qh           d2 
  sidlst[bias+29]=402123;//ql  
  sidlst[bias+30]=403112;//qh     b3/s1/d1 
  sidlst[bias+31]=403113;//ql  
  sidlst[bias+32]=403122;//qh           d2 
  sidlst[bias+33]=403123;//ql  
  sidlst[bias+34]=404112;//qh     b4/s1/d1 
  sidlst[bias+35]=404113;//ql  
  sidlst[bias+36]=404122;//qh           d2 
  sidlst[bias+37]=404123;//ql  
  sidlst[bias+38]=301212;//qh  l3/b1/s2/d1 
  sidlst[bias+39]=301213;//ql  
  bias+=40;  
  sidlst[bias]=   301222;//qh  l3/b1/s2/d2 cr3/sl9(aux)
  sidlst[bias+1]= 301223;//ql  
  sidlst[bias+2]= 302212;//qh     b2/s2/d1 
  sidlst[bias+3]= 302213;//ql  
  sidlst[bias+4]= 302222;//qh           d2 
  sidlst[bias+5]= 302223;//ql  
  sidlst[bias+6]= 303212;//qh     b3/s2/d1 
  sidlst[bias+7]= 303213;//ql  
  sidlst[bias+8]= 303222;//qh           d2 
  sidlst[bias+9]= 303223;//ql  
  sidlst[bias+10]=304212;//qh     b4/s2/d1 
  sidlst[bias+11]=304213;//ql  
  sidlst[bias+12]=304222;//qh           d2 
  sidlst[bias+13]=304223;//ql  
  sidlst[bias+14]=305212;//qh     b5/s2/d1 
  sidlst[bias+15]=305213;//ql  
  sidlst[bias+16]=305222;//qh           d2 
  sidlst[bias+17]=305223;//ql  
  sidlst[bias+18]=401212;//qh  l4/b1/s2/d1 
  sidlst[bias+19]=401213;//ql  
  sidlst[bias+20]=401222;//qh           d2 
  sidlst[bias+21]=401223;//ql  
  sidlst[bias+22]=401232;//qh           d3 
  sidlst[bias+23]=401233;//ql  
  sidlst[bias+24]=402212;//qh     b2/s2/d1 
  sidlst[bias+25]=402213;//ql  
  sidlst[bias+26]=402222;//qh           d2 
  sidlst[bias+27]=402223;//ql  
  sidlst[bias+28]=403212;//qh     b3/s2/d1 
  sidlst[bias+29]=403213;//ql  
  sidlst[bias+30]=403222;//qh           d2 
  sidlst[bias+31]=403223;//ql  
  sidlst[bias+32]=404212;//qh     b4/s2/d1 
  sidlst[bias+33]=404213;//ql  
  sidlst[bias+34]=404222;//qh           d2 
  sidlst[bias+35]=404223;//ql
  sidlst[bias+36]=0;//              empty  
  sidlst[bias+37]=0;//              empty  
  sidlst[bias+38]=0;//              empty  
  sidlst[bias+39]=0;//              empty  
  bias+=40;  
//
// <======================================== crate-4 (bot)  
//--->TOF(anodes,SFET):
  sidlst[bias]=   306100;//t   l3/b6/s1  cr4/sl2
  sidlst[bias+1]= 306101;//h
  sidlst[bias+2]= 306102;//qh
  sidlst[bias+3]= 306103;//ql
  sidlst[bias+4]= 307100;//t      b7/s1
  sidlst[bias+5]= 307101;//h
  sidlst[bias+6]= 307102;//qh
  sidlst[bias+7]= 307103;//ql
  sidlst[bias+8]= 308100;//t      b8/s1
  sidlst[bias+9]= 308101;//h 
  sidlst[bias+10]=308102;//qh
  sidlst[bias+11]=308103;//ql
  sidlst[bias+12]=309100;//t      b9/s1
  sidlst[bias+13]=309101;//h
  sidlst[bias+14]=309102;//qh
  sidlst[bias+15]=309103;//ql
  bias+=16;
  sidlst[bias]=   310100;//t     b10/s1  cr4/sl3
  sidlst[bias+1]= 310101;//h
  sidlst[bias+2]= 310102;//qh
  sidlst[bias+3]= 310103;//ql
  sidlst[bias+4]= 405100;//t   l4/b5/s1  
  sidlst[bias+5]= 405101;//h
  sidlst[bias+6]= 405102;//qh
  sidlst[bias+7]= 405103;//ql
  sidlst[bias+8]= 406100;//t      b6/s1
  sidlst[bias+9]= 406101;//h
  sidlst[bias+10]=406102;//qh
  sidlst[bias+11]=406103;//ql
  sidlst[bias+12]=407100;//t      b7/s1
  sidlst[bias+13]=407101;//h
  sidlst[bias+14]=407102;//qh
  sidlst[bias+15]=407103;//ql
  bias+=16;
  sidlst[bias]=   408100;//t      b8/s1  cr4/sl5
  sidlst[bias+1]= 408101;//h
  sidlst[bias+2]= 408102;//qh
  sidlst[bias+3]= 408103;//ql
  sidlst[bias+4]= 306200; //t  l3/b6/s2
  sidlst[bias+5]= 306201;//h
  sidlst[bias+6]= 306202;//qh
  sidlst[bias+7]= 306203;//ql
  sidlst[bias+8]= 307200;//t      b7/s2
  sidlst[bias+9]= 307201;//h
  sidlst[bias+10]=307202;//qh
  sidlst[bias+11]=307203;//ql
  sidlst[bias+12]=308200;//t     b8/s2
  sidlst[bias+13]=308201;//h
  sidlst[bias+14]=308202;//qh
  sidlst[bias+15]=308203;//ql
  bias+=16;
  sidlst[bias]=   309200;//t      b9/s2  cr4/sl6
  sidlst[bias+1]= 309201;//h
  sidlst[bias+2]= 309202;//qh
  sidlst[bias+3]= 309203;//ql
  sidlst[bias+4]= 310200;//t     b10/s2  
  sidlst[bias+5]= 310201;//h
  sidlst[bias+6]= 310202;//qh
  sidlst[bias+7]= 310203;//ql
  sidlst[bias+8]= 405200;//t   l4/b5/s2
  sidlst[bias+9]= 405201;//h
  sidlst[bias+10]=405202;//qh
  sidlst[bias+11]=405203;//ql
  sidlst[bias+12]=406200;//t      b6/s2
  sidlst[bias+13]=406201;//h
  sidlst[bias+14]=406202;//qh
  sidlst[bias+15]=406203;//ql
  bias+=16;
//--->ANTI(anodes,SFEA):
  sidlst[bias]=   5201;//h       b5/s2  cr4/sl7
  sidlst[bias+1]= 5202;//qh  
  sidlst[bias+2]= 5203;//ql  
  sidlst[bias+3]= 6201;//h       b6/s2 
  sidlst[bias+4]= 6202;//qh  
  sidlst[bias+5]= 6203;//ql  
  sidlst[bias+6]= 7201;//h       b7/s2 
  sidlst[bias+7]= 7202;//qh  
  sidlst[bias+8]= 7203;//ql  
  sidlst[bias+9]= 8201;//h       b8/s2 
  sidlst[bias+10]=8202;//qh  
  sidlst[bias+11]=8203;//ql
//--->TOF(anodes,SFEA):
  sidlst[bias+12]=407200;//t   l4/b7/s2
  sidlst[bias+13]=407201;//h
  sidlst[bias+14]=407202;//qh
  sidlst[bias+15]=407203;//ql
  sidlst[bias+16]=408200;//t      b8/s2
  sidlst[bias+17]=408201;//h
  sidlst[bias+18]=408202;//qh
  sidlst[bias+19]=408203;//ql
  bias+=20;
//--->TOF(dynodes,SFEC):
  sidlst[bias]=   306112;//qh  l3/b6/s1/d1 cr4/sl8(aux)
  sidlst[bias+1]= 306113;//ql  
  sidlst[bias+2]= 306122;//qh           d2 
  sidlst[bias+3]= 306123;//ql  
  sidlst[bias+4]= 307112;//qh     b7/s1/d1 
  sidlst[bias+5]= 307113;//ql  
  sidlst[bias+6]= 307122;//qh           d2 
  sidlst[bias+7]= 307123;//ql  
  sidlst[bias+8]= 308112;//qh     b8/s1/d1 
  sidlst[bias+9]= 308113;//ql  
  sidlst[bias+10]=308122;//qh           d2 
  sidlst[bias+11]=308123;//ql  
  sidlst[bias+12]=309112;//qh     b9/s1/d1 
  sidlst[bias+13]=309113;//ql  
  sidlst[bias+14]=309122;//qh           d2 
  sidlst[bias+15]=309123;//ql  
  sidlst[bias+16]=310112;//qh    b10/s1/d1 
  sidlst[bias+17]=310113;//ql  
  sidlst[bias+18]=310122;//qh           d2 
  sidlst[bias+19]=310123;//ql  
  sidlst[bias+20]=405112;//qh  l4/b5/s1/d1 
  sidlst[bias+21]=405113;//ql  
  sidlst[bias+22]=405122;//qh           d2 
  sidlst[bias+23]=405123;//ql  
  sidlst[bias+24]=406112;//qh     b6/s1/d1 
  sidlst[bias+25]=406113;//ql  
  sidlst[bias+26]=406122;//qh           d2 
  sidlst[bias+27]=406123;//ql  
  sidlst[bias+28]=407112;//qh     b7/s1/d1 
  sidlst[bias+29]=407113;//ql  
  sidlst[bias+30]=407122;//qh           d2 
  sidlst[bias+31]=407123;//ql  
  sidlst[bias+32]=408112;//qh     b8/s1/d1 
  sidlst[bias+33]=408113;//ql  
  sidlst[bias+34]=408122;//qh           d2 
  sidlst[bias+35]=408123;//ql  
  sidlst[bias+36]=408132;//qh           d3 
  sidlst[bias+37]=408133;//ql  
  sidlst[bias+38]=306212;//qh  l3/b6/s2/d1 
  sidlst[bias+39]=306213;//ql  
  bias+=40;  
  sidlst[bias]=   306222;//qh  l3/b6/s2/d2 cr4/sl9(aux)
  sidlst[bias+1]= 306223;//ql  
  sidlst[bias+2]= 307212;//qh     b7/s2/d1 
  sidlst[bias+3]= 307213;//ql  
  sidlst[bias+4]= 307222;//qh           d2 
  sidlst[bias+5]= 307223;//ql  
  sidlst[bias+6]= 308212;//qh     b8/s2/d1 
  sidlst[bias+7]= 308213;//ql  
  sidlst[bias+8]= 308222;//qh           d2 
  sidlst[bias+9]= 308223;//ql  
  sidlst[bias+10]=309212;//qh     b9/s2/d1 
  sidlst[bias+11]=309213;//ql  
  sidlst[bias+12]=309222;//qh           d2 
  sidlst[bias+13]=309223;//ql  
  sidlst[bias+14]=310212;//qh    b10/s2/d1 
  sidlst[bias+15]=310213;//ql  
  sidlst[bias+16]=310222;//qh           d2 
  sidlst[bias+17]=310223;//ql  
  sidlst[bias+18]=405212;//qh  l4/b5/s2/d1 
  sidlst[bias+19]=405213;//ql  
  sidlst[bias+20]=405222;//qh           d2 
  sidlst[bias+21]=405223;//ql  
  sidlst[bias+22]=406212;//qh     b6/s2/d1 
  sidlst[bias+23]=406213;//ql  
  sidlst[bias+24]=406222;//qh           d2 
  sidlst[bias+25]=406223;//ql  
  sidlst[bias+26]=407212;//qh     b7/s2/d1 
  sidlst[bias+27]=407213;//ql  
  sidlst[bias+28]=407222;//qh           d2 
  sidlst[bias+29]=407223;//ql  
  sidlst[bias+30]=408212;//qh     b8/s2/d1 
  sidlst[bias+31]=408213;//ql  
  sidlst[bias+32]=408222;//qh           d2 
  sidlst[bias+33]=408223;//ql
  sidlst[bias+34]=408232;//qh           d3 
  sidlst[bias+35]=408233;//ql  
  sidlst[bias+36]=0;//              empty  
  sidlst[bias+37]=0;//              empty  
  sidlst[bias+38]=0;//              empty  
  sidlst[bias+39]=0;//              empty  
  bias+=40;
  cout<<"AMSSCIds::init: total TOF+ANTI h/w-channels initialized: "<<bias<<endl;
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
  cout<<"AMSSCIds::init: S-crates capacity="<<hwch<<" h/w-channels"<<endl;
//
//--->create invers table (hwid=F(swch)):
//
  int mtmn,pmt,hch;
  int16u swch=0;//1st- for TOF-channels
  for(int il=0;il<TOF2DBc::getnplns();il++){
    for(int ib=0;ib<TOF2DBc::getbppl(il);ib++){
      for(int is=0;is<2;is++){
        for(int ip=0;ip<TOF2DBc::npmtps(il,ib)+1;ip++){
	  if(ip==0)mtmn=0;
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
      for(int mt=mtmn;mt<ANAMTS;mt++){
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
}
//---------------
int16u AMSSCIds::swseqn(int dtype, int16u layer, int16u bar, int16u side,
                                                 int16u pmt, int16u mtyp){
//
//dtype=1/2;layer=0,1...;bar=0,1,...;side=0,1;pmt=0,1,2,3;mtyp=0,1,2,3.
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



