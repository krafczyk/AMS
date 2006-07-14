//  $Id: daqs2block.C,v 1.9 2006/07/14 13:17:16 choumilo Exp $
// 1.0 version 2.07.97 E.Choumilov

#include "typedefs.h"
#include <iostream.h>
#include "link.h"
#include <fstream.h>
#include "cern.h"
#include "commons.h"
#include "event.h"
#include "tofdbc02.h"
#include "daqs2block.h"
#include "tofrec02.h"
#include "tofsim02.h"
#include "antidbc02.h"
#include "antirec02.h"
//
using namespace TOF2GC;
using namespace ANTI2C;
//
integer DAQS2Block::format=1; // default format (reduced), redefined by data card
//
int16u DAQS2Block::subdmsk[TOF2GC::DAQSBLK]={3,5,3,5,7,5,7,5};
// ( mask of detectors in given block(each number: lsbit->msbit => tof/anti/ctc))
//
int16u DAQS2Block::slotadr[TOF2GC::DAQSSLT]={0,1,2,3,4,5};// h/w addr(card-ID) vs sequent. slot#
//
int16u DAQS2Block::sblids[TOF2GC::DAQSFMX][TOF2GC::DAQSBLK]=  //valid S-block_id's for each format
  {
    {0x1401,0x1441,0x1481,0x14C1,0x1501,0x1541,0x1581,0x15C1},  //  Raw
    {0x1400,0x1440,0x1480,0x14C0,0x1500,0x1540,0x1580,0x15C0} //  Reduced
  };
//
int16u DAQS2Block::tofmtyp[TOF2GC::SCTOFC][4]={
                                      {3,4,2,1},    // mtypes in 1st TOFchannel of SFET
                                      {1,2,3,4},    // mtypes in 2nd TOFchannel of SFET
                                      {3,4,2,1},    // mtypes in 3rd TOFchannel of SFET
                                      {1,2,3,4},    // mtypes in 4th TOFchannel of SFET
                                     };
//
int16u DAQS2Block::tempch[TOF2GC::SCCRAT][TOF2GC::SCSFET]={   // TOFch#, occupied by temper. in crate/sfet
                                          {0,0,4,0},  // cr=1(01) inverted !
                                          {0,0,4,0},  // cr=2(31) inverted !
                                          {0,0,4,0},  // cr=3(41) inverted !
                                          {0,0,4,0},  // cr=4(71) inverted !
                                         };
//
number DAQS2Block::rwtemp[TOF2GC::DAQSTMX]={0,0,0,0,0,0,0,0, // just mem. reservation
                                   0,0,0,0,0,0,0,0,
                                   0,0,0,0,0,0,0,0,
                                   0,0,0,0,0,0,0,0};
integer DAQS2Block::totbll=0;
//
// functions for S-blocks class:
//
integer DAQS2Block::isSFET(int16u slota){ // "slota" means card-id (h/w address)
  if(slota==0 || slota==1 || slota==2 || slota==3)return(1);
  else return(0);
}
//
int16u DAQS2Block::isSFETT(int16u crat, int16u slota){
  int16u i,j,sln(0);
  for(i=0;i<TOF2GC::SCSFET;i++){ // get sequential number of slot
    if(slotadr[i]==slota)break;
  }
  if(tempch[crat][i]>0){//get sequential number of "temperature"-slot
    for(j=0;j<=i;j++)if(tempch[crat][j]>0)sln+=1;
  }
  return(sln);
}
//
int16u DAQS2Block::isSFETT2(int16u crat, int16u slotn){
  int16u i,sln(0);
  if(tempch[crat][slotn]>0){//get sequential number of "temperature"-slot
    for(i=0;i<=slotn;i++)if(tempch[crat][i]>0)sln+=1;
  }
  return(sln);
}
//
int16u DAQS2Block::gettempch(int16u crat, int16u sfet){ // tempch=1-4 !!! (0 if missing)
  return(tempch[crat][sfet]);
}
//
integer DAQS2Block::isSFEA(int16u slota){
  if(slota==4)return(1);
  else return(0);
}
//
integer DAQS2Block::isSFEC(int16u slota){
  if(slota==5)return(1);
  else return(0);
}
//
integer DAQS2Block::checkblockid(int16u blid){
  int valid(0);
  for(int i=0;i<TOF2GC::DAQSFMX;i++)
                     for(int j=0;j<TOF2GC::DAQSBLK;j++)
                                          if((blid & (~60)) == sblids[i][j])valid=1;
  return valid;
}
//-------------------------------------------------------
void DAQS2Block::buildraw(integer len, int16u *p){
//
// on input: len=tot_block_length as given at the beginning of block
//           *p=pointer_to_beggining_of_block_data (block-id word address)
//
  integer i,j,il,ib,is,ic,icn,nh,lent,dlen,im;
  int swid,sswid,swidn,sswidn,pmmx;
  int16u blid,btyp,ntyp,mt,pmt,naddr,mtyp,dtyp,msk,chan,slot,crat,rdch,cardid,tsens;
  geant temp,charge;
//
  blid=(*p) & ~60;      
  btyp=blid>>13;// block_type ("0" for event data)
  ntyp=(blid>>9)&15;// node_type ("10" for TOF/ANTI/CTC)
  naddr=(blid>>6)&7;// node_address (0-7 -> DAQ crate #)
  dtyp=1-(blid&63);// data_type ("0"->RawTDC; "1"->ReducedTDC)
  msk=subdmsk[naddr];
  TOF2JobStat::addaq1(naddr,dtyp);
  if(dtyp==0 && len>1)TOF2JobStat::addaq2(naddr,dtyp);// raw-data
  if(dtyp==1 && len>5)TOF2JobStat::addaq2(naddr,dtyp);// reduced
  totbll+=len;//summing to have event(scint) data length
#ifdef __AMSDEBUG__
  if(TFREFFKEY.reprtf[1]>=1 || ATREFFKEY.reprtf[1]>=1){
    cout<<"-----------------------------------------------------------"<<endl;
    cout<<"   DAQS2Block:decoding: block_id="<<hex<<blid<<dec<<endl; 
    cout<<"          block_type="<<btyp<<" node_type="<<ntyp<<endl;
    cout<<"          node_address(crate)="<<naddr<<" data_type(fmt)="<<dtyp<<endl;
    cout<<"          block_length="<<len<<endl<<endl;
//
    cout<<"  Block hex/binary dump follows :"<<endl<<endl;
    int16u tstw,tstb;
    for(i=0;i<len;i++){
      tstw=*(p+i);
      cout<<hex<<setw(4)<<tstw<<"  |"<<dec;
      for(j=0;j<16;j++){
        tstb=(1<<(15-j));
        if((tstw&tstb)!=0)cout<<"x"<<"|";
        else cout<<" "<<"|";
      }
      cout<<endl;
    }
    cout<<dec<<endl<<endl;
  }
#endif
//
  lent=1;//initial block length (block_id word was read)
//
/*
  int16u swcbuf[SCRCMX][SCTHMX2],  swnbuf[SCRCMX],   swibuf[SCRCMX],hwibuf[SCRCMX];
//keep:  LT/FT/HT-time+ampl hits   LT/FT/HT hits#       LBBSPMs       hwid=CSRR
//also filled static arrays:
// TOF2RawSide::FTtime[SCCRAT][SCFETA][SCTHMX1];//FT-channel time-hits(incl.ANTI)
// TOF2RawSide::FThits[SCCRAT][SCFETA];//number of FT-channel hits ....... 
// TOF2RawSide::SumHTt[SCCRAT][SCFETA-1][SCTHMX2];//TOF SumHT-channel time-hits
// TOF2RawSide::SumHTh[SCCRAT][SCFETA-1];// number of SumHT-channel time-hits 
// TOF2RawSide::SumSHTt[SCCRAT][SCFETA-1][TOF2GC::SCTHMX2];//TOF SumSHT-channel time-hits
// TOF2RawSide::SumSHTh[SCCRAT][SCFETA-1];// number of SumSHT-channel time-hits
//
// here SCRCMX is used as max. number(even higher) of sw-channels
  for(i=0;i<SCRCMX;i++){
    swnbuf[i]=0;
    swibuf[i]=0;
    for(j=0;j<SCTHMX2;j++)swcbuf[i][j]=0;
  }
// fill here buffers with TOF/ANTI sw-channels hits and temperature 
// in current crate as arrays of sequential sw-channel:
// (number of hits in each channel should be verified here !!!)
//
  AMSSCIds scinid(crat,slot,rdch);
  mtyp=scinid.getmtyp();//0->LTtime
  pmt=scinid.getpmt();//0->anode
  chan=scinid.getswch();//seq. sw-channel
//  slot=scinid.getslot();//sequential slot#(0,1,...8)(2 last are fictitious for d-adcs)
  if(mtyp==0 && pmt==0){//<--SFET(A) slot which have to have one temp-sensor
    tsens=scinid.gettempsn();//sequential sensor#(1,2,...,5)(not all slots have temp-sensor!)
    if(tsens>0){
      temp=...;
      if(temp>0)TOF2JobStat::puttemp(crat,tsens-1,temp);
    }
    else{
      cout<<"DAQS2Block::buildraw-Fatal- wrong tsens# for crate/slot="<<crat<<" "<<slot<<endl;
      exit(1);
    }
  }
//.......................filling_vs_chan.......................
//
// Now scan buffers and create tof/anti raw-event obj.:
//
  int16u nftdc;         // number of FT-tdc hits
  int16u ftdc[SCTHMX2]; // FT-tdc" hits (in TDC channels)
  int16u nstdc;         // number of LT-tdc hits
  int16u stdc[SCTHMX3]; // LT-tdc" hit (in TDC channels)
  int16u adca; // Anode-channel ADC hit (in DAQ-bin units !)
  int16u nadcd;         // number of NONZERO Dynode-channels(max PMTSMX)
  int16u adcd[PMTSMX]; // ALL Dynodes ADC hits(positional, in DAQ-bin units !)
//  
  adca=0;
  nftdc=0;
  nstdc=0;
  nadcd=0;
  for(i=0;i<PMTSMX;i++){
    adcd[i]=0;
  }
  sswid=0;
  sswidn=0;
  for(ic=0;ic<SCRCMX;ic++){//1-pass scan
    swid=swibuf[ic];//LBBSPM
    if(swid>0){//!=0 LBBSPM found
      sswid=swid/100;//LBBS
      for(icn=ic+1;icn<SCRCMX;icn++){//find next !=0 LBBSPM
        swidn=swibuf[icn];
        if(swidn>0)break;
      }
      if(swidn>0)sswidn=swidn/100;//next LBBS
      else sswidn=9999;//means all icn>ic are "0"
      il=swid/100000;
      mtyp=swid%10;
      if(il==0)dtyp=2;//anti
      else{
        dtyp=1;//tof
        il-=1;
      }
      ib=(swid%100000)/1000-1;
      is=(swid%1000)/100-1;
      pmt=(swid%100)/10;
      if(dtyp==1)pmmx=TOF2DBc::npmtps(il,ib);
      if(dtyp==2)pmmx=0;
      nh=swnbuf[ic];//#of hits fir given sw-chan(in accordance with mtyp)
      hwid=hwibuf[ic];//CSRR
      aslt=(hwid/100)%10;//abs. Slot #(1,2,...9)
      sslt=AMSSCIds::sl2tsid(aslt-1);//temp.sens.id=seq.slot#(or 0)(should be 1,2,3,4,5)
      assert(sslot>0 && sslot<=SCFETA);
//
      switch(mtyp){//fill RawEvent arrays
        case 0:
	  for(i=0;i<nh;i++)stdc[i]=swcbuf[ic][i];
	  nstdc=nh;
	  break;
        case 1:
	  if(pmt>pmmx)cout<<"scan: Npm>Max in ADC-h, swid="<<swid<<endl;
	  else{
	    if(pmt==0)adca=swcbuf[ic][0];//anode
	    else adcd[pmt-1]=swcbuf[ic][0];//dynode
	  }
	  break;
        case 2:
	  TOF2RawSide::FThits[crat][sslt-1]=nh;
	  for(i=0;i<nh;i++)TOF2RawSide::FTtime[crat][sslt-1][i]=swcbuf[ic][i];
	  break;
	case 3:
          TOF2RawSide::SumHTh[crat][sslt-1]=nh; 
          for(i=0;i<nh;i++)TOF2RawSide::SumHTt[crat][sslt-1][i]=swcbuf[ic][i];
	  break;
	case 4:
          TOF2RawSide::SumSHTh[crat][sslt-1]=nh; 
          for(i=0;i<nh;i++)TOF2RawSide::SumSHTt[crat][sslt-1][i]=swcbuf[ic][i];
	  break;
        default:
	  cout<<"scan:unknown measurement type ! swid="<<swid<<endl;
      }//-->endof switch
    }//-->endof "!=0 LBBSPM found"
//
    if(sswid!=sswidn){//new/last LBBS found -> create RawEvent-obj for current LBBS
// (after 1st swid>0 sswid is = last filled LBBS, sswidn is = LBBS of next nonempty channel or =9999)
//  at this stage temp may be not defined, it will be redefined at validation-stage using static job-store)
      if(dtyp==1){//TOF
	if(nstdc>0 || adca>0 || nadcd>0){//create tof-raw-event obj
	  nftdc=0;//dummy(filled later at valid. stage from [cr][sl] static stores)
	  nsumh=0;
	  nsumsh=0;
          AMSEvent::gethead()->addnext(AMSID("TOF2RawEvent",0),
                 new TOF2RawEvent(sswid,0,charge,temp,nftdc,ftdc,nstdc,stdc,
		                                                   nsumh,sumht,
								   nsumsh,sumsht,
                                                                   adca,
			                                           nadcd,adcd));
	}
      }
      else{//ANTI 
        AMSEvent::gethead()->addnext(AMSID("Anti2RawEvent",0),
                                    new Anti2RawEvent(sswid,0,temp,adca,nftdc,ftdc,nstdc,stdc));
      }
      adca=0;//reset RawEvent buffer
      nftdc=0;
      nstdc=0;
      nadcd=0;
      for(i=0;i<PMTSMX;i++){
        adcd[i]=0;
      }
    }//-->endof next/last LBBS check
  }//-->endof scan
	
      
    
  
*/  
//---> TOF decoding:
  if(lent<len && (msk&1)>0){
    dlen=lent;// bias to first TOF_data_word (=1 here)
    TOF2RawSide::buildraw(blid, dlen, p);
    lent+=dlen;
  }
//---> Print TOF Crate temperatures:
  if(TFREFFKEY.reprtf[1]>=1){
    im=TOF2GC::DAQSTSC*TOF2GC::DAQSTCS;
    cout<<"TOF_Crate Temperatures :"<<endl; 
    for(i=0;i<TOF2GC::DAQSTSC;i++){//loop over temp. SFETs in crate (1)
      for(j=0;j<TOF2GC::DAQSTCS;j++){//loop over temp. channels in SFET (4)
        chan=im*naddr+i*TOF2GC::DAQSTCS+j;
        cout<<" "<<rwtemp[chan];
      }
      cout<<endl;
    }
    cout<<endl;
  }
//
//---> CTC decoding:
//  if(lent<len && (msk&4)>0){
//    dlen=lent;//bias to first CTC_data_word (=1+TOF_data_length)
//    AMSCTCRawEvent::buildraw(blid, dlen, p);
//    lent+=dlen;
//  }
//---> ANTI decoding:
  if(lent<len && (msk&2)>0){
    dlen=lent;//bias to first Anti_data_word (=1+TOF_data_length+CTC_data_length)
    Anti2RawEvent::buildraw(blid, dlen, p);
    lent+=dlen;
  }
//
//---
#ifdef __AMSDEBUG__
  if(lent != len){
    TOF2JobStat::addaq3(naddr,dtyp);
    if(TFREFFKEY.reprtf[1]>=1 || ATREFFKEY.reprtf[1]>=1){
      cout<<"DAQS2Block::buildraw: length mismatch !!! for crate "<<naddr<<endl;
      cout<<"Length from bloc_header "<<len<<" but was read "<<lent<<endl;
    }
  }
#endif
}
//----------------------------------------------------
integer DAQS2Block::calcblocklength(integer ibl){
  integer tofl(0),antil(0),ctcl(0),fmt,lent;
  int16u blid,msk;
//
  fmt=TFMCFFKEY.daqfmt;
  format=fmt;// class variable is redefined by data card 
  blid=sblids[format][ibl];// valid block_id
  msk=subdmsk[ibl];// subdetectors in crate (mask)
  if((msk&1)>0)tofl=TOF2RawSide::calcdaqlength(blid);
  if((msk&2)>0)antil=Anti2RawEvent::calcdaqlength(blid);
//  if((msk&4)>0)ctcl=AMSCTCRawEvent::calcdaqlength(blid);
  lent=(1+tofl+antil+ctcl);//"1" for block-id word.
  return lent;
}
//----------------------------------------------------
integer DAQS2Block::getmaxblocks(){return TOF2GC::DAQSBLK;}
//----------------------------------------------------
void DAQS2Block::buildblock(integer ibl, integer len, int16u *p){
//
// on input: len=tot_block_length as was defined by call to calcblocklength
//           *p=pointer_to_beggining_of_block_data (block-id word)
//
  integer i,dlen,clen,fmt,lent(0);
  int16u *next=p;
  int16u blid,msk;
//
  blid=sblids[format][ibl];// valid block_id
// ---> wrire block-id :
  *p=blid;
  next+=1;//now points to first subdet_data word (usually TOF)
  lent+=1;
//
  msk=subdmsk[ibl];
//--------------
  dlen=0;
  if((msk&1)>0)TOF2RawSide::builddaq(blid, dlen, next);
//
  lent+=dlen;
  next+=dlen;
//---------------
  dlen=0;
  if((msk&2)>0)Anti2RawEvent::builddaq(blid, dlen, next);
  lent+=dlen;
//---------------
  if(len != lent){
    cout<<"DAQS2Block::buildblock: length-mismatch !!! for block "<<ibl<<endl;
    cout<<"Initially declared length="<<len<<" but was written "<<lent<<endl;
  }
//---------------
  if(ibl==(TOF2GC::DAQSBLK-1))  //clear RawEvent/Hit container after last block processed
  {
#ifdef __AMSDEBUG__

    AMSContainer *ptr1=AMSEvent::gethead()->getC("TOF2RawSide",0);
    if(ptr1)ptr1->eraseC();
//
    AMSContainer *ptr2=AMSEvent::gethead()->getC("Anti2RawEvent",0);
    if(ptr2)ptr2->eraseC();
//
//    AMSContainer *ptr3=AMSEvent::gethead()->getC("AMSCTCRawEvent",0);
//    if(ptr3)ptr3->eraseC();
//
#endif
  }
//--------------
}
