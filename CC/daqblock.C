// 0.0 version 2.07.97 E.Choumilov

#include <typedefs.h>
#include <iostream.h>
#include <link.h>
#include <fstream.h>
#include <event.h>
#include <daqblock.h>
#include <tofdbc.h>
#include <tofrec.h>
#include <tofsim.h>
//
integer DAQSBlock::format=1; // default format (reduced), redefined by data card
//
int16u DAQSBlock::subdmsk[DAQSBLK]={7,7,1,5,7,7,1,5};//defaul mask(lsbit->msbit: tof/anti/ctc)
//
int16u DAQSBlock::sblids[DAQSFMX][DAQSBLK]=  //valid S-block_id's for each format
  {
    {0x1400,0x1440,0x1480,0x14C0,0x1500,0x1540,0x1580,0x15C0}, //  Raw
    {0x1401,0x1441,0x1481,0x14C1,0x1501,0x1541,0x1581,0x15C1}  //  Reduced
  };
//
// functions for S-blocks class:
//
integer DAQSBlock::checkblockid(int16u blid){
  int valid(0);
  for(int i=0;i<DAQSFMX;i++)
                     for(int j=0;j<DAQSBLK;j++)
                                          if(blid == sblids[i][j])valid=1;
  return valid;
}
//------------
void DAQSBlock::buildraw(integer len, int16u *p){
  integer dlen;
  integer clen(1);
  int16u blid,btyp,ntyp,naddr,dtyp,msk;
//
  blid=*p;
  btyp=blid>>13;// block_type ("0" for event data)
  ntyp=(blid>>9)&15;// node_type ("10" for TOF/ANTI/CTC)
  naddr=(blid>>6)&7;// node_address (0-7 -> DAQ crate #)
  dtyp=blid&63;// data_type ("0"->RawTDC; "1"->ReducedTDC)
  assert(btyp==0 && ntyp==10);// TOF/ANTI/CTC-data 
  msk=subdmsk[naddr];
//
  dlen=1;
  if(clen<len && (msk&1)>0)AMSTOFRawEvent::buildraw(dlen, p);
  clen+=dlen;
//  if(clen<len && (msk&2)>0)AMSANTIRawEvent::buildraw(dlen, p)
  clen+=dlen;
//  if(clen<len && (msk&4)>0)AMSCTCRawEvent::buildraw(dlen, p)
}
//------------
integer DAQSBlock::calcblocklength(integer ibl){
  integer tofl(0),antil(0),ctcl(0),fmt;
  int16u blid,msk;
  assert(ibl>=0 && ibl<DAQSBLK);// 0-7
  fmt=TOFMCFFKEY.daqfmt;
  assert(fmt>=0 && fmt<=1);
  format=fmt;// redefined by data card value
  blid=sblids[format][ibl];// valid block_id
  msk=subdmsk[ibl];// subdetectors in crate (mask)
  if((msk&1)>0)tofl=AMSTOFRawEvent::calcdaqlength(blid);
//  if((msk&2)>0)antil=AMSANTIRawEvent::calcdaqlength(blid);
//  if((msk&4)>0)ctcl=AMSCTCRawEvent::calcdaqlength(blid);
  return (tofl+antil+ctcl);
}
//------------
integer DAQSBlock::getmaxblocks(){return DAQSBLK;}
//------------
void DAQSBlock::buildblock(integer ibl, integer len, int16u *p){
  integer dlen,clen,fmt;
  int16u *next=p;
  int16u blid,msk;
//
  assert(ibl>=0 && ibl<DAQSBLK);// 0-7
  blid=sblids[format][ibl];// valid block_id
  msk=subdmsk[ibl];
  dlen=len;
  if((msk&1)>0)AMSTOFRawEvent::builddaq(blid, dlen, next);
#ifdef __AMSDEBUG__
  if(ibl==(DAQSBLK-1))  //clear AMSTOFRawEvent container after last block processed
  {
// cout<<"clear container after block "<<ibl<<endl;
    AMSContainer *ptr=AMSEvent::gethead()->getC("AMSTOFRawEvent",0);
    ptr->eraseC();
  }
#endif
//
  next+=dlen;
//  if((msk&2)>0)AMSANTIRawEvent::builddaq(blid, dlen, next)
//
  next+=dlen;
//  if((msk&4)>0)AMSCTCRawEvent::builddaq(blid, dlen, next)
}
