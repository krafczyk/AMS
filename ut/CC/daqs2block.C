//  $Id: daqs2block.C,v 1.2 2001/01/22 17:32:19 choutko Exp $
// 1.0 version 2.07.97 E.Choumilov

#include <typedefs.h>
#include <iostream.h>
#include <link.h>
#include <fstream.h>
#include <cern.h>
#include <commons.h>
#include <event.h>
#include <tofdbc02.h>
#include <daqs2block.h>
#include <tofrec02.h>
#include <tofsim02.h>
#include <antidbc02.h>
#include <antirec02.h>
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
                                          {0,4,0,0},  // cr=5(03)
                                          {0,4,0,0},  // cr=6(33)
                                          {0,4,0,0},  // cr=7(43)
                                          {0,4,0,0},  // cr=8(73)
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
  integer i,j,lent,dlen,im;
  int16u blid,btyp,ntyp,naddr,dtyp,msk,chan;
//
  blid=(*p) & ~60;       // New genial idea
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
  if(TOFRECFFKEY.reprtf[1]>=1 || ANTIRECFFKEY.reprtf[1]>=1 || CTCRECFFKEY.reprtf[1]>=1){
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
//---> TOF decoding:
  if(lent<len && (msk&1)>0){
    dlen=lent;// bias to first TOF_data_word (=1 here)
    TOF2RawEvent::buildraw(blid, dlen, p);
    lent+=dlen;
  }
//---> Print TOF Crate temperatures:
  if(TOFRECFFKEY.reprtf[1]>=1){
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
    if(TOFRECFFKEY.reprtf[1]>=1 || ANTIRECFFKEY.reprtf[1]>=1 || CTCRECFFKEY.reprtf[1]>=1){
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
  fmt=TOFMCFFKEY.daqfmt;
  format=fmt;// class variable is redefined by data card 
  blid=sblids[format][ibl];// valid block_id
  msk=subdmsk[ibl];// subdetectors in crate (mask)
  if((msk&1)>0)tofl=TOF2RawEvent::calcdaqlength(blid);
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
  if((msk&1)>0)TOF2RawEvent::builddaq(blid, dlen, next);
//
  lent+=dlen;
  next+=dlen;
//---------------
//  dlen=0;
//  if((msk&4)>0)AMSCTCRawEvent::builddaq(blid, dlen, next);
//  lent+=dlen;
//  next+=dlen;
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

    AMSContainer *ptr1=AMSEvent::gethead()->getC("TOF2RawEvent",0);
    if(ptr1)ptr1->eraseC();
//
    AMSContainer *ptr2=AMSEvent::gethead()->getC("Anti2RawEvent",0);
    if(ptr2)ptr2->eraseC();
//
//    AMSContainer *ptr3=AMSEvent::gethead()->getC("AMSCTCRawEvent",0);
//    if(ptr3)ptr3->eraseC();
//
#endif
//    for(i=0;i<2;i++){
//      AMSContainer *ptr4=AMSEvent::gethead()->getC("AMSCTCRawHit",i);
//      if(ptr4)ptr4->eraseC();
//    }
  }
//--------------
}
