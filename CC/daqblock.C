// 0.0 version 2.07.97 E.Choumilov

#include <typedefs.h>
#include <iostream.h>
#include <link.h>
#include <fstream.h>
#include <cern.h>
#include <commons.h>
#include <event.h>
#include <daqblock.h>
#include <tofdbc.h>
#include <tofrec.h>
#include <tofsim.h>
#include <antidbc.h>
#include <antirec.h>
#include <ctcdbc.h>
#include <ctcrec.h>
#include <ctcsim.h>
//
integer DAQSBlock::format=1; // default format (reduced), redefined by data card
//
int16u DAQSBlock::subdmsk[DAQSBLK]={7,7,1,5,7,7,1,5};//defaul mask(lsbit->msbit: tof/anti/ctc)
//
int16u DAQSBlock::slotadr[DAQSSLT]={0,1,2,5,6,7};// h/w address of slots (card-ID's)
//
int16u DAQSBlock::sblids[DAQSFMX][DAQSBLK]=  //valid S-block_id's for each format
  {
    {0x1400,0x1440,0x1480,0x14C0,0x1500,0x1540,0x1580,0x15C0}, //  Raw
    {0x1401,0x1441,0x1481,0x14C1,0x1501,0x1541,0x1581,0x15C1}  //  Reduced
  };
//
int16u DAQSBlock::tofmtyp[SCTOFC][4]={
                                      {3,4,2,1},    // mtypes in 1st TOFchannel of SFET
                                      {1,2,3,4},    // mtypes in 2nd TOFchannel of SFET
                                      {3,4,2,1},    // mtypes in 3rd TOFchannel of SFET
                                      {1,2,3,4},    // mtypes in 4th TOFchannel of SFET
                                     };
//
number DAQSBlock::rwtemp[DAQSTMX]={0,0,0,0,0,0,0,0, // just mem. reservation
                                   0,0,0,0,0,0,0,0,
                                   0,0,0,0,0,0,0,0,
                                   0,0,0,0,0,0,0,0,
                                   0,0,0,0,0,0,0,0,
                                   0,0,0,0,0,0,0,0,
                                   0,0,0,0,0,0,0,0,
                                   0,0,0,0,0,0,0,0};
//
// functions for S-blocks class:
//
integer DAQSBlock::isSFET(int16u slota){
  if(slota==0 || slota==1 || slota==2 || slota==5)return(1);
  else return(0);
}
//
integer DAQSBlock::isSFETT(int16u slota){
  if(slota==1)return(1);
  else if(slota==5)return(2);
  else return(0);
}
//
integer DAQSBlock::isSFEA(int16u slota){
  if(slota==6)return(1);
  else return(0);
}
//
integer DAQSBlock::isSFEC(int16u slota){
  if(slota==7)return(1);
  else return(0);
}
//
integer DAQSBlock::checkblockid(int16u blid){
  int valid(0);
  for(int i=0;i<DAQSFMX;i++)
                     for(int j=0;j<DAQSBLK;j++)
                                          if(blid == sblids[i][j])valid=1;
  return valid;
}
//-------------------------------------------------------
void DAQSBlock::buildraw(integer len, int16u *p){
//
// on input: len=tot_block_length as given at the beginning of block
//           *p=pointer_to_beggining_of_block_data (block-id word address)
//
  integer i,j,lent,dlen,im;
  int16u blid,btyp,ntyp,naddr,dtyp,msk,chan;
//
  blid=*p;
  btyp=blid>>13;// block_type ("0" for event data)
  ntyp=(blid>>9)&15;// node_type ("10" for TOF/ANTI/CTC)
  naddr=(blid>>6)&7;// node_address (0-7 -> DAQ crate #)
  dtyp=blid&63;// data_type ("0"->RawTDC; "1"->ReducedTDC)
  msk=subdmsk[naddr];
#ifdef __AMSDEBUG__
  if(TOFRECFFKEY.reprtf[1]>=1 || ANTIRECFFKEY.reprtf[1]>=1 || CTCRECFFKEY.reprtf[1]>=1){
    cout<<"-----------------------------------------------------------"<<endl;
    cout<<"DAQ::decoding: block_id="<<hex<<blid<<dec<<endl; 
    cout<<"  block_type="<<btyp<<" node_type="<<ntyp<<endl;
    cout<<"  node_address(crate)="<<naddr<<" data_type(fmt)="<<dtyp<<endl;
    cout<<"  block_length="<<len<<endl<<endl;
  }
#endif
//
// clear crate-temperatures starting new crate decoding:
//
  im=DAQSTSC*DAQSTCS;//2*4
  for(i=0;i<im;i++){
    chan=im*naddr+i;
    rwtemp[chan]=0;
  }
  lent=1;//initial block length (block_id word)
//
//---> TOF decoding:
  if(lent<len && (msk&1)>0){
    dlen=lent;// bias to first TOF_data_word (=1 here)
    AMSTOFRawEvent::buildraw(blid, dlen, p);
    lent+=dlen;
  }
//---> Print TOF Crate temperatures:
  if(TOFRECFFKEY.reprtf[1]>=1){
    cout<<"TOF_Crate Temperatures :"<<endl; 
    for(i=0;i<DAQSTSC;i++){//loop over temp. SFETs in crate (2)
      for(j=0;j<DAQSTCS;j++){//loop over temp. channels in SFET (4)
        chan=im*naddr+i*DAQSTCS+j;
        cout<<" "<<rwtemp[chan];
      }
      cout<<endl;
    }
    cout<<endl;
  }
//
//---> CTC decoding:
  if(lent<len && (msk&4)>0){
    dlen=lent;//bias to first CTC_data_word (=1+TOF_data_length)
    AMSCTCRawEvent::buildraw(blid, dlen, p);
    lent+=dlen;
  }
//
//---> ANTI decoding:
  if(lent<len && (msk&2)>0){
    dlen=lent;//bias to first Anti_data_word (=1+TOF_data_length+CTC_data_length)
    AMSAntiRawEvent::buildraw(blid, dlen, p);
    lent+=dlen;
  }
//---
  if(lent != len){
    cout<<"DAQSBlock::buildraw: length mismatch !!! for crate "<<naddr<<endl;
    cout<<"Length from bloc_header "<<len<<" but was read "<<lent<<endl;
  }
}
//----------------------------------------------------
integer DAQSBlock::calcblocklength(integer ibl){
  integer tofl(0),antil(0),ctcl(0),fmt,lent;
  int16u blid,msk;
//
  fmt=TOFMCFFKEY.daqfmt;
  format=fmt;// class variable is redefined by data card 
  blid=sblids[format][ibl];// valid block_id
  msk=subdmsk[ibl];// subdetectors in crate (mask)
  if((msk&1)>0)tofl=AMSTOFRawEvent::calcdaqlength(blid);
  if((msk&2)>0)antil=AMSAntiRawEvent::calcdaqlength(blid);
  if((msk&4)>0)ctcl=AMSCTCRawEvent::calcdaqlength(blid);
  lent=(1+tofl+antil+ctcl);//"1" for block-id word.
  return lent;
}
//----------------------------------------------------
integer DAQSBlock::getmaxblocks(){return DAQSBLK;}
//----------------------------------------------------
void DAQSBlock::buildblock(integer ibl, integer len, int16u *p){
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
  if((msk&1)>0)AMSTOFRawEvent::builddaq(blid, dlen, next);
//
  lent+=dlen;
  next+=dlen;
//---------------
  dlen=0;
  if((msk&4)>0)AMSCTCRawEvent::builddaq(blid, dlen, next);
  lent+=dlen;
  next+=dlen;
//---------------
  dlen=0;
  if((msk&2)>0)AMSAntiRawEvent::builddaq(blid, dlen, next);
  lent+=dlen;
//---------------
  if(len != lent){
    cout<<"DAQSBlock::buildblock: length-mismatch !!! for block "<<ibl<<endl;
    cout<<"Initially declared length="<<len<<" but was written "<<lent<<endl;
  }
//---------------
  if(ibl==(DAQSBLK-1))  //clear RawEvent/Hit container after last block processed
  {
#ifdef __AMSDEBUG__

    AMSContainer *ptr1=AMSEvent::gethead()->getC("AMSTOFRawEvent",0);
    ptr1->eraseC();
//
    AMSContainer *ptr2=AMSEvent::gethead()->getC("AMSAntiRawEvent",0);
    ptr2->eraseC();
//
    AMSContainer *ptr3=AMSEvent::gethead()->getC("AMSCTCRawEvent",0);
    ptr3->eraseC();
//
#endif
    for(i=0;i<2;i++){
      AMSContainer *ptr4=AMSEvent::gethead()->getC("AMSCTCRawHit",i);
      ptr4->eraseC();
    }
  }
//--------------
}
