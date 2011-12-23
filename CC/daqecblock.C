// AMS02 version 16.11.06 by E.Choumilov
#include "typedefs.h"
#include "link.h"
#include "cern.h"
#include "commons.h"
#include "event.h"
#include "ecaldbc.h"
#include "ecid.h"
#include "daqecblock.h"
#include "ecalrec.h"
#include "ecalcalib.h"
//
using namespace ecalconst;
//
 uinteger DAQECBlock::_PrevRunNum(0);
 integer DAQECBlock::_NReqEdrs(0);
 bool DAQECBlock::_FirstPedBlk(true);
 integer DAQECBlock::_GoodPedBlks(0);
 integer DAQECBlock::_FoundPedBlks(0);
 integer DAQECBlock::_PedBlkCrat[ECRT][ECEDRS]={-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
 bool DAQECBlock::_CalFirstSeq(true);//is set inside particular calib-job routines
//
int16u DAQECBlock::format=0; // default format (raw)
//
int16u DAQECBlock::nodeids[ecalconst::ECRT]=//valid EC_nodes(JINFs) id(JINJ link#(port))(2 JINF => 2crates)  
  {
    12,13, // crate 1/2 
  };
//
integer DAQECBlock::totbll=0;
//
// functions for EC-blocks class:
//
void DAQECBlock::node2crs(int16u nodeid, int16u &crat){//called by me only
//  on input: slaveid(JINF-nodes only!!!)=(blid&0x001F)
//  on output: crate=1-2, =0 if ID is not found;
  crat=0;
  crat=atoi(DAQEvent::getportname(nodeid)+5)+1;//1-2, no validity check -> all is verified during VC's call to checkblockid
}
//---
integer DAQECBlock::checkblockid(int16u blid){//JINF's ids as Ports("VC"'s blid), return crt#(1-2)
  int valid(0);
  char sstr[128];
  for(int i=0;i<ecalconst::ECRT;i++){
    sprintf(sstr,"JINFE%X",i);
    valid=DAQEvent::ismynode(blid,sstr)?(i+1):0;
    if(valid>0)return valid;
  }
 return 0;
}



integer DAQECBlock::checkblockidJ(int16u blid){//JINF's ids as Ports("VC"'s blid), return crt#(1-2)
  char sstr[128];
   for(int i=0;i<ecalconst::ECRT;i++){
  sprintf(sstr,"JF-E%d",i);
  if(DAQEvent::ismynode(blid,sstr))return i+1; 
 }
 return 0;
}

integer DAQECBlock::getportid(int16u crat){
// on input crat=0,1; return portid >=0, -1 if not found
  integer valid(-1);
  char sstr[128];
  if(crat<2){
    for(int16u i=0;i<32;i++){
      sprintf(sstr,"JINFE%X",crat);
      valid=DAQEvent::ismynode(i,sstr)?i:-1;
      if(valid>=0)break;
    }
  }
  return valid;
}
//----
integer DAQECBlock::checkblockidP(int16u blid){//EDR's and JINF's ids as Nodes("VC"'s blid)
  int valid(0);
  int id;
  char sstr[128];
  char side[5]="ABPS";
  char str[2];
  id=((blid>>5)&(0x1FF));
//  if(id>=206 && id<=241)
//    cout<<"---> In DAQECBlock::checkblockidP, blid(hex)="<<hex<<blid<<",addr:"<<dec<<id<<endl;
//  else cout<<"---> In DAQECBlock::checkblockidP, blid(hex)="<<hex<<blid<<",addr:"<<dec<<blid<<endl;
//
  for(int i=0;i<ecalconst::ECRT;i++){//check id for EDRs
   for(int k=0;k<ecalconst::ECEDRS;k++){
    for(int j=0;j<4;j++){
      str[0]=side[j];
      str[1]='\0';
      sprintf(sstr,"EDR%X%X%s",i,k,str);
      if(DAQEvent::ismynode(blid,sstr)){
        valid=100*(i+1)+10*(k+1)+j+1;//C(rate)|S(lot)|S(ide)
//cout<<"<--- Found "<<sstr<<" CratSlotSide="<<valid<<endl;
        return valid;
      }
    }
   } 
  }
//
  if(valid==0){
    for(int i=0;i<ecalconst::ECRT;i++){//....for JINFs
    for(int j=0;j<4;j++){
      str[0]=side[j];
      str[1]='\0';
      sprintf(sstr,"JF-E%X%s",i,str);
      if(DAQEvent::ismynode(blid,sstr)){
        valid=10*(i+1)+j+1;//Cr|Side
//cout<<"<--- Found "<<sstr<<"  CratSlot="<<valid<<endl;
        return valid;
      }
    } 
    }
  }
//
  if(valid==0){
    for(int i=0;i<ecalconst::ECRT;i++){
      sprintf(sstr,"JINFE%X",i);
      if(DAQEvent::ismynode(blid,sstr)){
//cout<<"<--- Found "<<sstr<<" Crat="<<i+1<<endl;
        return i+1;
      }
    }
  }
  return 0;
}
//-------------------------------------------------------
void DAQECBlock::buildrawJ(int n , int16u *pbeg){
 buildraw(-n+1,pbeg+1);
}



void DAQECBlock::buildraw(integer leng_j, int16u *p){
// it is implied that event_fragment is EC-JINF's one (validity is checked in calling procedure)
//           *p=pointer_to_beggining_of_block_data(i.e. NEXT to len-word !!!)
// this event fragment may keep EDR2,ETRG blocks in arbitrary order;
// Length counting does not include length-word itself !!!
//
  integer i,j,ic,icn;
  bool jinf=leng_j<0;
  int leng=abs(leng_j);
  integer swid,hwid,crsta(0);
  int16u swch,rdch,slot,aslt,crat,val16,ibit,slay,pmt,pix,gain,trppmt;
  int16u dtyp,datyp,lenraw(0),lencom(0),formt,evnum;
  int16u jbias,ebias,jblid,eblid,jleng,eleng,jaddr,eaddr,csid;
  int16u rawlen;
  integer comlen;
  int16u word,nword,nnword;
  int16u fmt;
  int16u osbit;
  int16 slots;
  integer bufpnt(0),lbbs;
  uinteger val32;
  geant padc[3];
  int16u cdpmsk1(0),cdpmsk2(0);
  bool newdataf(false);
// for class/DownScaled ped-cal
  bool PedCal(false);//means PedCal job, using event-by-event in RawFMT or DownScaled 
  bool DownScal(false);
  static int FirstDScalBlk(0);
  int16u PedSubt[ECSLOTS]={1,1,1,1,1,1,1};//0/1->no/yes PedSubtr at RawEvent creation
  int16u SlRawFmt[ECSLOTS]={1,1,1,1,1,1,1};//1/0-> raw/not format in slot
// for PedCalTable(onboard calib)
  static integer FirstPedBlk(0);
  static integer TotPedBlks(0);
//  static integer PedBlkCrat[ECRT][ECEDRS]={0,0,0,0,0,0,0,0,0,0,0,0};
  bool PedBlkOK(false);
  geant ped,sig;
  int16u sts,nblkok;
//
  uinteger swvbuf[2*ECRCMX];
// keep:       ADC-values; 1st half -> comp/raw when alone, 2nd half -> comp when in mixt mode 
  integer  swibuf[2*ECRCMX],hwibuf[2*ECRCMX];
//keep:        swids=LTTPG         hwids=CSSRRR   
// these buffers will be filled with Ecal sw-channels hits in current JINF(crate) as 
// arrays of sequential sw-channel index 
// Also filled at EcalRawEvent objects creation the dynode static arrays:
// AMSEcalRawEvent::dynadc[ECSLMX][ECPMSMX];//Dunode hits vs (SL,PMT)
//
  for(ic=0;ic<2*ECRCMX;ic++){//clear buffs
    swibuf[ic]=0;
    hwibuf[ic]=0;
    swvbuf[ic]=0;
  }
//
  EcalJobStat::daqs1(0);//count entries
//
  if(DAQCFFKEY.DAQVersion==1)newdataf=true;
  else newdataf=false;
  if(!AMSJob::gethead()->isRealData())newdataf=true;//tempor fix
//cout<<"====> newdataf="<<newdataf<<" DAQVersion="<<DAQCFFKEY.DAQVersion<<endl;
//
  p=p-1;//to follow VC-convention
  jleng=int16u(leng&(0xFFFFL));//fragment's 1st word(block length) call value
  jblid=*(p+jleng);// JINF fragment's last word: Status+slaveID(its id)
  crat=(leng>>16) +1;
  if(newdataf){
    cdpmsk2=*(p+jleng-1);
    cdpmsk1=*(p+jleng-2);
  }
//
  bool dataf=((jblid&(0x8000))>0) || jinf;//data-fragment
  bool crcer=((jblid&(0x4000))>0);//CRC-error
  bool asser=((jblid&(0x2000))>0);//assembly-error
  bool amswer=((jblid&(0x1000))>0);//amsw-error   
  bool timoer=((jblid&(0x0800))>0);//timeout-error   
  bool fpower=((jblid&(0x0400))>0);//FEpower-error   
  bool seqer=((jblid&(0x0200))>0);//sequencer-error
  bool cdpnod=((jblid&(0x0020))>0);//CDP-node(like EDR2-node with no futher fragmentation)
  bool noerr;
  bool empty;
  bool badtyp;
  bool pedbl(false);
  jaddr=(jblid&(0x001F));//slaveID(="NodeAddr"=JINFaddr here)(one of 4 permitted)
//
  if(ECREFFKEY.reprtf[2]>1){//debug (ECRE 3=)
    cout<<"====> In DAQECBlock::buildraw: JINF_leng(incall)="<<*p<<"("<<jleng<<") slave_id:"<<jaddr<<endl;
    if(newdataf)cout<<"      New(Ass1=final) JINF data format..."<<endl;
    else cout<<"      Old(PreAss) JINF data format..."<<endl;
    if(ECREFFKEY.reprtf[2]>2)EventBitDump(jleng,p,"Complete JINF-block:");
  }
//
//  node2crs(jaddr,crat);//get crate#(1-2, =0,if notfound)
  csid=1;//here is dummy(no redundancy for JINFs), later it is defined from card_id(EDR(a,b),ETRG(a,b))
//  cout<<"      crate="<<crat<<endl;
//modify jleng(to skip 2 msk-words) if new dataformat:
  if(newdataf)jleng=jleng-2;
//
  if(jleng>1 && crat>0)EcalJobStat::daqs1(1);//<=== count non-empty, valid JINF-fragments
  else{
    if(crat==0)EcalJobStat::daqs1(30);//badExit caused by JINF badID
    if(jleng<=1)EcalJobStat::daqs1(31);//badExit caused by JINF empty
    goto BadExit;
  }
//
  if(dataf){
    if(crcer)EcalJobStat::daqs1(10+7*(crat-1));
    if(asser)EcalJobStat::daqs1(11+7*(crat-1));
    if(amswer)EcalJobStat::daqs1(12+7*(crat-1));
    if(timoer)EcalJobStat::daqs1(13+7*(crat-1));
    if(fpower)EcalJobStat::daqs1(14+7*(crat-1));
    if(seqer)EcalJobStat::daqs1(15+7*(crat-1));
    if(cdpnod)EcalJobStat::daqs1(16+7*(crat-1));
//    if(cdpnod)cout<<" <---Error:JINF with cdpnod set !!!"<<endl;
  }
  else{
    EcalJobStat::daqs1(2+crat-1);//<=== count JINF's notData segments
    goto BadExit;//tempor
  }
//
  noerr=(dataf && !crcer && !asser && !amswer 
                                       && !timoer && !fpower && !seqer && !cdpnod);
  if(noerr)EcalJobStat::daqs1(4+crat-1);//<=== count JINF-reply status OK     
  else{
    EcalJobStat::daqs1(32+crat-1);//<=== count JINF's err.stat
    goto BadExit;
  }
//-----------
  if(ECREFFKEY.relogic[1]==4)PedCal=true;
//
  if(ECREFFKEY.relogic[1]==5)PedCal=true;// DownScaledEvents PedCalib job is requested
// 
//-----------
  jbias=1;
  while(jbias<jleng){//<---- JINF-words loop
    eleng=*(p+jbias);//deeper level (EDR2/ETRG) fragment's length
    eblid=*(p+jbias+eleng);//deeper level (EDR2/ETRG) fragment's Status+slaveID(EDR/ETRG id) 
    eaddr=(eblid&(0x001F));//slaveID(="NodeAddr"=ERD/ETRGaddr here)(one of 7/side permitted)
    datyp=((eblid&(0x00C0))>>6);//0,1,2(only raw or compr)
    if(datyp==1)setrawf();
    if(datyp==2)setcomf();
    if(datyp==3)setmixf();
    pedbl=false;//tempor
    badtyp=(datyp<1);
    empty=(eleng==1);
    formt=getformat();//0/1/2->raw/compr/mix flag for current EDR/ETRG
    if(ECREFFKEY.reprtf[2]>1)cout<<" ---> XDR_length/addr="<<eleng<<" "<<eaddr<<"  datyp="<<datyp<<
                                                                           " formt="<<formt<<endl;
    EcalJobStat::daqs2(crat-1,formt);//<--- entries per crate/format
    slots=AMSECIds::crdid2sl(csid,eaddr);//get seq.slot#(0-5 =>EDRs; =6 =>ETRG;-1==>not_found) ans side
    csid=csid+1;//1/2->active EDR/ETRG side
    if(ECREFFKEY.reprtf[2]>1)cout<<"      SeqSlot(0-6)="<<slots<<endl;
    if(ECREFFKEY.reprtf[2]>1)EventBitDump(eleng,p+jbias,"---> Block-by-block EvDump");
    if(slots<0){//not found in the list of ids
#ifdef __AMSDEBUG__
      cout<<"ECBlock::Error:illegal CardID, crate/side/fmt/id="<<crat<<" "<<csid<<" "<<formt
                                                                <<" "<<hex<<eaddr<<dec<<endl;
#endif
      EcalJobStat::daqs2(crat-1,3+formt);//illegal CardId
      goto NextBlock;    
    }
    slot=int16u(slots);//0-5,6
    EcalJobStat::daqs3(crat-1,slot,formt);//entries per crate/slot vs fmt
//edr/etrg status-bits:
    dataf=((eblid&(0x8000))>0) || jinf;//data-fragment
    crcer=((eblid&(0x4000))>0);//CRC-error
    asser=((eblid&(0x2000))>0);//assembly-error
    amswer=((eblid&(0x1000))>0);//amsw-error   
    timoer=((eblid&(0x0800))>0);//timeout-error   
    fpower=((eblid&(0x0400))>0);//FEpower-error   
    seqer=((eblid&(0x0400))>0);//sequencer-error
    cdpnod=((eblid&(0x0020))>0);//CDP-node(like EDR2/ETRG-node with no futher fragmentation)
//
    if(dataf && !badtyp){
      if(crcer)EcalJobStat::daqs3(crat-1,slot,7+12*formt);
      if(asser)EcalJobStat::daqs3(crat-1,slot,8+12*formt);
      if(amswer)EcalJobStat::daqs3(crat-1,slot,9+12*formt);
      if(timoer)EcalJobStat::daqs3(crat-1,slot,10+12*formt);
      if(fpower)EcalJobStat::daqs3(crat-1,slot,11+12*formt);
      if(seqer)EcalJobStat::daqs3(crat-1,slot,12+12*formt);
      if(empty)EcalJobStat::daqs3(crat-1,slot,13+12*formt);
      if(cdpnod)EcalJobStat::daqs3(crat-1,slot,14+12*formt);
    }
    else if(!dataf && !badtyp){//datatypeok, but notData(Peds ???)
      EcalJobStat::daqs3(crat-1,slot,3);
      goto NextBlock;//not peds ??? skip block
    }//--->endof datatype-ok, but nonData(peds ???)
//
    else if(badtyp){
//      cout<<"ECBlock::Error:bad slot-datatype, crate/slot/datyp="<<crat<<" "<<slot<<" "<<datyp<<endl;
//      EventBitDump(eleng,p+jbias,"---> Bad slot datyp EvDump");
      EcalJobStat::daqs3(crat-1,slot,6);//<=== bad slot-datatype(not raw,comp,mixt) - take next block
      goto NextBlock;
    }
//    
    noerr=(dataf && !crcer && !asser && !amswer 
                                       && !timoer && !fpower && !seqer && cdpnod);
    if(noerr)EcalJobStat::daqs3(crat-1,slot,15+12*formt);//"EDR/ETRG-reply status OK" entries per crate/format
    else goto NextBlock;
//
    ebias=1;
//------
    if(slot==6){//<===== ETRG-block processing(modifying ebias)
      if(eleng==8)EcalJobStat::daqs3(crat-1,slot,16+12*formt);//block-length OK
      else{
        EcalJobStat::daqs1(36+crat-1);//<=== count JINF's bad ETRG len
        goto BadExit;//bad length - fatal
      }
//cout<<"      ETRG-len OK"<<endl;
      for(int16u iwd=0;iwd<eleng-1;iwd++){//ETRG-words loop
	val16=*(p+jbias+ebias);//
        for(int16u ibt=0;ibt<16;ibt++){
	  ibit=16*iwd+ibt;//0-111(really used 0-107(3sl*36pm),108(XA),109(XF))
	  slay=ibit/36;//0-2
	  if(crat==1)slay=2*slay;//trig.layers counting: 0,2,4(X-proj,Face B(D))
	  if(crat==2)slay=2*slay+1;//trig.layers counting: 1,3,5(Y-proj,Face A(C))
	  pmt=ibit%36;//0-35
	  if((val16&(1<<ibt))>0){
//	    cout<<"   SettingTRPbits:ProjLay/pmt="<<slay<<" "<<pmt<<" crat="<<crat<<endl;
	    AMSEcalRawEvent::settrpbit(slay,pmt);//set bit in static trpatt[6][3]-array)
	  }
	  if(ibit==107)break;
	}
	if(ebias==(eleng-1)){//this last word contains Fast/Lev1(angle) decision bits
	  if((val16 & 0x8000)>0){//was FastTr-bit
	    if(crat==1)trppmt=38;//X-pr
	    if(crat==2)trppmt=39;//Y-pr
//	  cout<<"     SettingFastTrig-bit, trppmt="<<trppmt<<endl;
	    AMSEcalRawEvent::settrpbit(5,trppmt);
	  }
	  if((val16 & 0x4000)>0){//was Lev1Tr-bit(angle)
	    if(crat==1)trppmt=36;//X-pr
	    if(crat==2)trppmt=37;//Y-pr
//	  cout<<"     SettingLVL1Trig-bit, trppmt="<<trppmt<<endl;
	    AMSEcalRawEvent::settrpbit(5,trppmt);
	  }
	} 
        ebias+=1;
      }//--->endof ETRG-words loop
      goto NextBlock;
    }
//------
    if(slot<=5){//<===== EDR-block processing
//------------
      if(formt==0){//<===================== RawFormat(alone) processing:
//
        if(eleng==(ECEDRC+1)){//rawfmt length ok
	  EcalJobStat::daqs3(crat-1,slot,16+12*formt);//true RawSegment & length OK
//cout<<"    RawFMT,EDR_length OK"<<endl;
          ebias=1;
          while(ebias<eleng){//<---- EDR-words loop (243 ADC-values)
	    rdch=ebias-1;//0-242
	    val16=*(p+jbias+ebias);//ADC-value(multiplied by 16 in DSP)
//	  cout<<"  ebias="<<ebias<<" rdch/val="<<rdch<<" "<<val16<<endl;
	    AMSECIds ecid(crat-1,csid-1,slot,rdch);//create ecid-obj
	    swid=ecid.getswid();//long sw_id=LTTPG
	    swch=ecid.getswch();//glob.seq. sw-chan(0-2915)
	    hwid=ecid.gethwid();//hw_id=CSSRRR
//	  cout<<"   swid/hwid/swch="<<swid<<" "<<hwid<<" "<<swch<<endl;
	    swibuf[swch]=swid;//fill buffs
	    hwibuf[swch]=hwid;
	    swvbuf[swch]=val16;
	    ebias+=1;
	  }//--->endof EDR-words loop(RawFmt)
	  if(PedCal){//pedcal(just because it is requested !)
            PedSubt[slot]=0;//my internal flag to subtr(1)/not(0) peds at RawEventObj-creation
            if(FirstDScalBlk==0){
              ECPedCalib::BTime()=AMSEvent::gethead()->gettime();//store Begin-time
	      FirstDScalBlk=1;
            }
	  }//--->endof "pedcal"
	}//--->endof rawfmt leng.ok 
//
        else{//bad leng
          EcalJobStat::daqs1(38+crat-1);//<=== count JINF's any bad EDR len(raw)
	  goto BadExit;//wrong length ==> stop any further processing
	}
      }//================================> endof RawFormat(alone) processing
//------------
      else if(formt==1){//<==================== ComprFormat(alone) processing :
// 
        if(eleng<=(2*ECEDRC+1) && (eleng%2==1)){//<--comprfmt length ok
//	  if(ECREFFKEY.reprtf[0]>0)HF1(ECHISTR+70+6*(crat-1)+slot,geant(eleng),1.);
	  EcalJobStat::daqs3(crat-1,slot,16+12*formt);//lengthOK
//cout<<"    ComprFMT,EDR_length OK"<<endl;
	  SlRawFmt[slot]=0;//not the raw format
          while(ebias<eleng){//<---- EDR-words loop (max 2*243 (rdch# + ADC-valie))
	    rdch=*(p+jbias+ebias);//rdch(ADC-addr)(0-242)
	    if(rdch<=242){// addr ok
	      val16=*(p+jbias+ebias+1);//ADC-value(multiplied by 16 in DSP)
//	  cout<<"  ebias="<<ebias<<" rdch/val="<<rdch<<" "<<val16<<endl;
	      AMSECIds ecid(crat-1,csid-1,slot,rdch);//create ecid-obj
	      swid=ecid.getswid();//long sw_id=LTTPG
	      swch=ecid.getswch();//glob.seq. sw-chan(0-2915)
	      hwid=ecid.gethwid();//hw_id=CSSRRR
//	  cout<<"   swid/hwid/swch="<<swid<<" "<<hwid<<" "<<swch<<endl;
	      swibuf[swch]=swid;//fill buffs
	      hwibuf[swch]=hwid;
	      swvbuf[swch]=val16;
//	  cout<<"    rdch/val="<<rdch<<" "<<val16<<"  swid/hwid="<<swid<<" "<<hwid<<"  swch="<<swch<<endl;
	    }
	    else{//illegal ADC-addr number
	      if(ECREFFKEY.reprtf[2]>0)cout<<"<--- ComprFMT: wrong ADC-addr:"<<rdch<<" for crat/sslot="<<crat<<" "<<slot<<endl;
              if(ECREFFKEY.reprtf[2]>0)EventBitDump(eleng,p+jbias,"---> CompFMT:bad ADC-addr EvDump");
	      EcalJobStat::daqs3(crat-1,slot,18+12*formt);
	      goto NextBlock;
	    }
	    ebias+=2;
	  }//--->endof EDR-words loop(comprF)
          if(strstr(AMSJob::gethead()->getsetup(),"PreAss"))PedSubt[slot]=1;// only for this period
	  else PedSubt[slot]=0;//DON'T SUBTRACT peds at RawEventObj-creation, IT IS DONE ON BOARD
	}//--->endof comprfmt leng.ok
//---
	else if(eleng==(ECEDRC+1)){//<--true "downscaled" event(rawfmt in comp.stream, but fmt.bit=comp!!!)
//          if(ECREFFKEY.reprtf[2]>0)EventBitDump(eleng,p+jbias,"---> CompFMT:downscaled EvDump");
	  DownScal=true;
	  EcalJobStat::daqs3(crat-1,slot,16+12*formt);//lengthOK
          EcalJobStat::daqs3(crat-1,slot,4);//count downscaled events
          ebias=1;
          while(ebias<eleng){//<---- EDR-words loop (243 ADC-values)
	    rdch=ebias-1;//0-242
	    val16=*(p+jbias+ebias);//ADC-value(multiplied by 16 in DSP)
//	  cout<<"  ebias="<<ebias<<" rdch/val="<<rdch<<" "<<val16<<endl;
	    AMSECIds ecid(crat-1,csid-1,slot,rdch);//create ecid-obj
	    swid=ecid.getswid();//long sw_id=LTTPG
	    swch=ecid.getswch();//glob.seq. sw-chan(0-2915)
	    hwid=ecid.gethwid();//hw_id=CSSRRR
//	  cout<<"   swid/hwid/swch="<<swid<<" "<<hwid<<" "<<swch<<endl;
	    swibuf[swch]=swid;//fill buffs
	    hwibuf[swch]=hwid;
	    swvbuf[swch]=val16;
	    ebias+=1;
	  }//--->endof EDR-words loop(RawFmt)
          PedSubt[slot]=1;
	  if(PedCal){//pedcal was requested on downscaled events
            PedSubt[slot]=0;//my internal flag to subtr(1)/not(0) peds at RawEventObj-creation
            if(FirstDScalBlk==0){
              ECPedCalib::BTime()=AMSEvent::gethead()->gettime();//store Begin-time
	      FirstDScalBlk=1;
            }
	  }//--->endof pedcal
	}
//---
        else{
          EcalJobStat::daqs1(40+crat-1);//<=== count JINF's any bad EDR len(compr)
          if(ECREFFKEY.reprtf[2]>0)EventBitDump(eleng,p+jbias,"---> CompFMT:Bad BlockLength EvDump");
	  goto BadExit;
	}
//
      }//================================> endof ComprFormat(alone) processing
//------------
      else{//<===================================== MixedFormat processing :
//
        rawlen=int16u(ECEDRC);//243
	comlen=eleng-rawlen-1;//0-486(2x243)
//-------
        if(eleng>=(rawlen+1) && comlen<=2*rawlen && comlen>=0 && comlen%2==0 ){//<--true mixfmt leng
	  EcalJobStat::daqs3(crat-1,slot,16+12*formt);//MixtSegment-length OK
//cout<<"    MixtFMT,EDR_length OK, sslot="<<slot<<" TotL/raw/com="<<eleng<<" "<<rawlen<<" "<<comlen<<endl;
//  ------> raw sub-block decoding:
          ebias=1;
          while(ebias < rawlen+1){//<---- EDR-words loop (243 ADC-values)
	    rdch=ebias-1;//0-242
	    val16=*(p+jbias+ebias);//ADC-value(multiplied by 16 in DSP)
//	cout<<"  RawSubS:ebias="<<ebias<<" rdch/val="<<rdch<<" "<<val16<<endl;
	    AMSECIds ecid(crat-1,csid-1,slot,rdch);//create ecid-obj
	    swid=ecid.getswid();//long sw_id=LTTPG
	    swch=ecid.getswch();//glob.seq. sw-chan(0-2915)
	    hwid=ecid.gethwid();//hw_id=CSSRRR
//	  cout<<"   swid/hwid/swch="<<swid<<" "<<hwid<<" "<<swch<<endl;
	    swibuf[swch]=swid;//fill buffs
	    hwibuf[swch]=hwid;
	    swvbuf[swch]=val16;
	    ebias+=1;
	  }//--->endof EDR-words loop(RawFmt)
	  if(PedCal){//pedcal was requested on rawfmt-subblock of mixt format
            PedSubt[slot]=0;//my internal flag to subtr(1)/not(0) peds at RawEventObj-creation
            if(FirstDScalBlk==0){
              ECPedCalib::BTime()=AMSEvent::gethead()->gettime();//store Begin-time
	      FirstDScalBlk=1;
            }
	  }//--->endof pedcal 
//  ------> compr sub-block decoding(if present):
          if(comlen>0){
            EcalJobStat::daqs3(crat-1,slot,17+12*formt);
            while(ebias<eleng){//<---- EDR-words loop (max 2*243 (rdch# + ADC-valie))
	      rdch=*(p+jbias+ebias);//rdch(ADC-addr)(0-242)
	      if(rdch<=242){// addr ok
	        val16=*(p+jbias+ebias+1);//ADC-value(multiplied by 16 in DSP)
//	cout<<"  ComSubS:ebias="<<ebias<<" rdch/val="<<rdch<<" "<<val16<<endl;
	        AMSECIds ecid(crat-1,csid-1,slot,rdch);//create ecid-obj
	        swid=ecid.getswid();//long sw_id=LTTPG
	        swch=ecid.getswch();//glob.seq. sw-chan(0-2915)
	        hwid=ecid.gethwid();//hw_id=CSSRRR
//	  cout<<"   swid/hwid/swch="<<swid<<" "<<hwid<<" "<<swch<<endl;
	        swibuf[swch+ECRCMX]=swid;//fill 2nd half of buffers
	        hwibuf[swch+ECRCMX]=hwid;
	        swvbuf[swch+ECRCMX]=val16;
//	  cout<<"    rdch/val="<<rdch<<" "<<val16<<"  swid/hwid="<<swid<<" "<<hwid<<"  swch="<<swch<<endl;
	      }
	      else{//illegal ADC-addr number
	        if(ECREFFKEY.reprtf[2]>0)cout<<"<--- ComprInMixtFMT: wrong ADC-addr:"<<rdch<<" for crat/sslot="<<crat<<" "<<slot<<endl;
                if(ECREFFKEY.reprtf[2]>0)EventBitDump(eleng,p+jbias,"---> CompInMixtFMT:bad ADC-addr EvDump");
	        EcalJobStat::daqs3(crat-1,slot,18+12*formt);
	        goto NextBlock;
	      }
	      ebias+=2;
            }//--->endof EDR-words loop(comprF)
	  }//--->endof compr-subblock check
//
	}//--->endof true mixtfmt leng
//-------
        else if(eleng==(2*ECEDRC+1)){// raw+downscaled block during mixt mode
//          if(ECREFFKEY.reprtf[2]>0)EventBitDump(eleng,p+jbias,"---> MixtFMT:downscaled EvDump");
	  DownScal=true;
	  EcalJobStat::daqs3(crat-1,slot,16+12*formt);//lengthOK
          EcalJobStat::daqs3(crat-1,slot,4);//count downscaled events
          ebias=1;
          while(ebias<(ECEDRC+1)){//<-- words loop (use only raw half(243 ADC-values), 2nd half repeat 1st)
	    rdch=ebias-1;//0-242
	    val16=*(p+jbias+ebias);//ADC-value(multiplied by 16 in DSP)
//	  cout<<"  ebias="<<ebias<<" rdch/val="<<rdch<<" "<<val16<<endl;
	    AMSECIds ecid(crat-1,csid-1,slot,rdch);//create ecid-obj
	    swid=ecid.getswid();//long sw_id=LTTPG
	    swch=ecid.getswch();//glob.seq. sw-chan(0-2915)
	    hwid=ecid.gethwid();//hw_id=CSSRRR
//	  cout<<"   swid/hwid/swch="<<swid<<" "<<hwid<<" "<<swch<<endl;
	    swibuf[swch]=swid;//fill buffs
	    hwibuf[swch]=hwid;
	    swvbuf[swch]=val16;
	    ebias+=1;
	  }//--->endof EDR-words loop
//
	  if(PedCal){//pedcal was requested on downscaled events
            PedSubt[slot]=0;//my internal flag to subtr(1)/not(0) peds at RawEventObj-creation
            if(FirstDScalBlk==0){
              ECPedCalib::BTime()=AMSEvent::gethead()->gettime();//store Begin-time
	      FirstDScalBlk=1;
            }
	  }//--->endof pedcal 
          goto NextBlock;//next block
        }//--->endof raw+downscaled block
//-------
        else{//bad mixfmt leng
          EcalJobStat::daqs1(42+crat-1);//<=== count JINF's any bad EDR len(mixFMT)
          if(ECREFFKEY.reprtf[2]>0)EventBitDump(eleng,p+jbias,"---> MixtFMT:bad length EvDump");
	  goto BadExit;//wrong length ==> stop any further processing
	}//--->endof bad mixfmt leng
//
      }//====================================>endof MixedFormat processing
//
    }//--->endof EDR(slot<=5) check
//----
NextBlock:
    jbias+=eleng+1;//"+1" to include eleng-word itself
  }//---->endof JINF-level loop
//
//----------------------------> scan swch-buffers and create RawEvent-Objects:
//
  integer sswid,swidn,sswidn,shwid,subtrped,sta;
  geant peda,siga,pedd,sigd,athr,dthr,adca,adcd;
//
  athr=ECALVarp::ecalvpar.daqthr(0);//daq-thr. for anode(hi/low)
  dthr=ECALVarp::ecalvpar.daqthr(4);//daq-thr. for dynode
//
  for(i=0;i<3;i++){//reset RawEvent adc-buff
    padc[i]=0;
  }
  sswid=0;
  sswidn=0;
  for(ic=0;ic<ECRCMX;ic++){//1-pass scan
    swid=swibuf[ic];//LTTPG
    if(swid>0){//!=0 LTTPG found
      sswid=swid/10;//LTTP ("short swid")
      for(icn=ic+1;icn<ECRCMX;icn++){//find next !=0 LTTPG
        swidn=swibuf[icn];
        if(swidn>0)break;
      }
      if(swidn>0)sswidn=swidn/10;//next LTTP
      else sswidn=9999;//means all icn>ic are "0"
      slay=swid/10000;//1-9
      gain=swid%10;//1-2(hi/low)
      pmt=(swid%10000)/100;//1-36
      pix=(swid%100)/10;//1-4(=5->dyn)
      hwid=hwibuf[ic];//CSSRRR
      shwid=hwid/1000;//CSS("short hwid")
      aslt=shwid%100;//abs. Slot #(1,2,...5)
      subtrped=PedSubt[aslt-1];//1/0->subtr/not peds at RawEvent creation
      peda=ECPMPeds::pmpeds[slay-1][pmt-1].ped(pix-1,gain-1);//current Aped
      siga=ECPMPeds::pmpeds[slay-1][pmt-1].sig(pix-1,gain-1);//current Asig
      pedd=ECPMPeds::pmpeds[slay-1][pmt-1].pedd();//current Dped
      sigd=ECPMPeds::pmpeds[slay-1][pmt-1].sigd();//current Dsig
//
      switch(gain){//fill RawEvent arrays
        case 1:
	  if(pix<=4){//anode-adc(hi)
	    adca=geant(swvbuf[ic])/16;//"16" to go back to true ADC-value
	    if(subtrped){
	      if((adca-peda)>athr*siga)padc[0]=adca-peda;//subtr.ped and apply DAQ-threshold
	      else padc[0]=0;
	    }
	    else padc[0]=adca;
	    if(ECREFFKEY.reprtf[2]>1){
//	      cout<<"A_hig_pix#="<<pix<<" ped/sig="<<peda<<" "<<siga<<" adc="<<padc[0]<<endl;
	      HF1(ECHISTR+60+4*(gain-1)+pix-1,geant(padc[0]),1.);
	    }
	  }
	  else{//dynode(pix=5)
	    adcd=geant(swvbuf[ic])/16;
	    if(subtrped){
	      if((adcd-pedd)>dthr*sigd)padc[2]=adcd-pedd;//subtr.ped and apply DAQ-threshold
	      else padc[2]=0;
	    }
	    else padc[2]=adcd;
	    if(ECREFFKEY.reprtf[2]>1){
//	      cout<<"A_dyn#="<<pix<<" ped/sig="<<pedd<<" "<<sigd<<" adc="<<padc[2]<<endl;
	      HF1(ECHISTR+68,geant(padc[2]),1.);
	    }
	    AMSEcalRawEvent::setadcd(slay-1,pmt-1,padc[2]);
	  }
	  break;
//
        case 2:
	  adca=geant(swvbuf[ic])/16;//anode-adc(low)
	  if(subtrped){
	    if((adca-peda)>athr*siga)padc[1]=adca-peda;//subtr.ped and apply DAQ-threshold
	    else padc[1]=0;
	  }
	  else padc[1]=adca;
	  if(ECREFFKEY.reprtf[2]>1){
//	    cout<<"A_low_pix#="<<pix<<" ped/sig="<<peda<<" "<<siga<<" adc="<<padc[1]<<endl;
	    HF1(ECHISTR+60+4*(gain-1)+pix-1,geant(padc[1]),1.);
	  }
	  break;
//
        default:
	  if(ECREFFKEY.reprtf[2]>0)cout<<"EcalDAQBuf-scan:unknown measurement type(gain) ! swid="<<swid<<endl;
      }//-->endof switch
    }//-->endof "!=0 LTTPG found"
//
//
    if(sswid!=sswidn){//new/last LTTP found -> create RawEvent for current LTTP(i.e. all gains for given pixel)
// (after 1st swid>0 sswid is = last filled LTTP, sswidn is = LTTP of next nonempty channel or =9999)
//General: it is implied that given pixel info(ah/al/d) is always contained within one slot(EDR)
      crsta=0;
      if(padc[0]>0 || padc[1]>0){//create EcalRawEvent obj
        if(ECREFFKEY.reprtf[2]>1){//debug-prints
	  cout<<"    ==> Create EcalRawEvent: short swid/hwid="<<sswid<<" "<<shwid<<endl;
	  cout<<"    Aadc(hi/low)="<<padc[0]<<" "<<padc[1]<<endl;
	  cout<<endl;
        }
	if(PedCal){
	  if(SlRawFmt[aslt-1]==1 && PedSubt[aslt-1]==0)sta=1;//here is the signal of calib(not pedsubtr) hit
	  else sta=-1;//something wrong
	}
	else{
	  sta=0;//ok(normal RawEvent object with subtracted ped)
	  if(SlRawFmt[aslt-1]==1 && PedSubt[aslt-1]==0)sta=-1;//something wrong
	}
//
        if(sta>=0){
          padc[2]=0;//no valid Dyn-info in RawEvent for the moment(will be added in Validation)
          if(AMSEvent::gethead()->addnext(AMSID("AMSEcalRawEvent",crat-1),
                 new AMSEcalRawEvent(sswid,sta,csid,padc)))crsta=1;
        }
	else{
	  if(ECREFFKEY.reprtf[2]>0)
	    cout<<"<=== DAQECBlock::buildraw:Error in Fmt/SubtPeds flags, no hit created !!!"<<endl;
	}
      }
      for(i=0;i<3;i++){//reset RawEvent adc-buffer
        padc[i]=0;
      }
    }//-->endof next/last LTTP check
//
  }//-->endof scan
//----------------
//
  if(ECREFFKEY.reprtf[2]>1){//debug-prints
    cout<<"===> Fired Dynodes map after processing of crate="<<crat<<":"<<endl;
    cout<<"           (reading direction: pm=1--->pm=36)"<<endl<<endl;
    for(slay=0;slay<ECSLMX;slay++){
      for(pmt=0;pmt<ECPMMX;pmt++){
        adcd=AMSEcalRawEvent::getadcd(slay,pmt);
        cout<<adcd<<" ";
        if(pmt==17)cout<<endl;
      }
      cout<<endl<<endl;
    }
    cout<<"===> TrigPattern(slayer=1-6, pmt=1-36):"<<endl<<endl;
    for(slay=0;slay<ECSLMX-3;slay++){
      for(pmt=0;pmt<ECPMMX;pmt++){
        if(AMSEcalRawEvent::gettrpbit(slay,pmt)>0)cout<<1<<"|";
        else cout<<0<<"|";
      }
      cout<<endl;
    }
    cout<<"FastTrigBits(XF/YF)="<<AMSEcalRawEvent::gettrpbit(5,38)<<" "<<AMSEcalRawEvent::gettrpbit(5,39)<<endl;
    cout<<"Lvl1TrigBits(XA/YA)="<<AMSEcalRawEvent::gettrpbit(5,36)<<" "<<AMSEcalRawEvent::gettrpbit(5,37)<<endl;
  }
//
  return;
BadExit:
  EcalJobStat::daqs1(ECJSTA-1);//count rejected JINF-entries(segments)    
//  
}
//-------------------------------------------------------
void DAQECBlock::buildonbP1(integer leng, int16u *p){
// (Should be used only for PreAss period (no dynamic pedestals !!!)
// it is implied that event_fragment is EC-JINF's one (validity is checked in calling procedure)
//           *p=pointer_to_beggining_of_block_data(i.e. NEXT to len-word !!!)
// this event fragment may keep EDR2,ETRG blocks in arbitrary order;
// Length counting does not include length-word itself !!!
//
  integer i,j,ic,icn,isl,isd;
  integer swid,hwid,crsta(0);
  int16u swch,rdch,slot,aslt,crat,val16,ibit,slay,pmt,pix,gain,trppmt;
  int16u dtyp,datyp,lenraw(0),lencom(0),formt,evnum;
  int16u jbias,ebias,jblid,eblid,jleng,eleng,jaddr,eaddr,csid;
  int16u word,nword,nnword;
  int16u fmt;
  int16u osbit;
  int16 slots;
  integer bufpnt(0),lbbs;
  uinteger val32;
  integer portid,crdid;
  uinteger runn;
  bool PedBlkOK(false);
  bool newrun;
  geant ped,sig,dped,thrs;
  int16u sts,nblkok;
  bool bad;
//
  bool pcreq(false);
  bool sidedoubled(false);
  bool dataf;//data-fragment
  bool crcer;//CRC-error
  bool asser;//assembly-error
  bool amswer;//amsw-error   
  bool timoer;//timeout-error   
  bool fpower;//FEpower-error   
  bool seqer;//sequencer-error
  bool cdpnod;//CDP-node(like EDR2-node with no futher fragmentation)
  bool noerr;
  bool empty;
  bool badtyp;
  char * p2tdvnam;
  bool badexit;
//
#pragma omp critical (ec_pedc_onb)
{
  badexit=true;
  if((CALIB.SubDetInCalib%10)!=1){
    if(_CalFirstSeq){//CalFirstSeq is set to true only once in initjob(initb)
      cout<<endl<<"<======= Warning: ECAL OnBoardPed DB-writing is blocked !!!"<<endl<<endl;
//      _CalFirstSeq is set to false after the 1st good sequence (was call to outb(flg))
    }
  }
//
  EcalJobStat::daqs1(0);//count entries
  DAQEvent * pdaq = (DAQEvent*)AMSEvent::gethead()->getheadC("DAQEvent",6);
  bad=false;//ok
  runn=AMSEvent::gethead()->getrun();
  newrun=(_PrevRunNum!=runn);
//
  if(newrun || _FirstPedBlk){//count/mark requested EDRs on 1st call or new run
//    cout<<"  Ntdv="<<AMSJob::gethead()->gettdvn()<<endl;
//    for(i=0;i<AMSJob::gethead()->gettdvn();i++){     
//      p2tdvnam=AMSJob::gethead()->gettdvc(i);
//      cout<<" TDV-name="<<p2tdvnam<<" at "<<i<<endl;
//    }
//
    cout<<endl;
    cout<<"=========> DAQECBlock::buildOnbPed: NewRun/FirstPedBlock call, run:"<<runn<<endl;
    cout<<"           Good PedBlocks in prev.run :"<<_GoodPedBlks
                                            <<", total found:"<<_FoundPedBlks<<endl;
    _FirstPedBlk=false;
    _PrevRunNum=runn;
    ECPedCalib::BRun()=runn;
    ECPedCalib::resetb();//reset histograms, cumulative vars(GoodPedBlks,FoundPedBlks,NReqEdrs,PedBlkCrat[],...)
//
    for(ic=0;ic<ECRT;ic++){
      for(isl=0;isl<ECEDRS;isl++){//sequential slots(EDRs only)
        portid=DAQECBlock::getportid(ic);//>=0 if exist(12,13 here)
	for(isd=0;isd<2;isd++){//sides(a/b)
	  crdid=AMSECIds::sl2crdid(isd,isl);
          pcreq=(pdaq && portid>=0 && crdid>=0
	            && pdaq->CalibRequested(uinteger(portid),uinteger(crdid)));//ped-cal requested for given crate/slot ?
          if(pcreq){
	    _PedBlkCrat[ic][isl]+=1;//mark requested EDRs
	    _NReqEdrs+=1;
	  }
	}
	if(_PedBlkCrat[ic][isl]>0){//error: both sides requested for given crat/slot
	  cout<<"      <--- Warning: found that both sides are requested for crt/slt="<<ic<<" "<<isl<<endl;
	  _PedBlkCrat[ic][isl]=0;//preset by hands
	  sidedoubled=true;
	}
      }
    }
//    cout<<"           Total EDRs requested:"<<_NReqEdrs<<endl;
    if(!sidedoubled && _NReqEdrs<(ECRT*ECEDRS)){
      cout<<"      <--- DAQECBlock::buildonbP:Warning-> not all EDRs requested ?!"<<endl;
//      bad=true;
    }
    else if(sidedoubled){
      cout<<"<--------- Error: Some sides are doubled, NrequestedEDRs="<<_NReqEdrs<<endl;
      bad=true;
    }
  }//--->endof 1st PedBlk(new run)
//
  if(bad)goto Exit;
//--------
  p=p-1;//to follow VC-convention
  jleng=int16u(leng&(0xFFFFL));//fragment's 1st word(block length) call value
  jblid=*(p+jleng);// JINF fragment's last word: Status+slaveID(its id)
//
  dataf=((jblid&(0x8000))>0);//data-fragment
  crcer=((jblid&(0x4000))>0);//CRC-error
  asser=((jblid&(0x2000))>0);//assembly-error
  amswer=((jblid&(0x1000))>0);//amsw-error   
  timoer=((jblid&(0x0800))>0);//timeout-error   
  fpower=((jblid&(0x0400))>0);//FEpower-error   
  seqer=((jblid&(0x0400))>0);//sequencer-error
  cdpnod=((jblid&(0x0020))>0);//CDP-node(like EDR2-node with no futher fragmentation)
  jaddr=(jblid&(0x001F));//slaveID(="NodeAddr"=JINFaddr here)(one of 4 permitted)
//
  if(ECREFFKEY.reprtf[2]>0){//debug
    cout<<"  <---- In DAQECBlock::buildraw: JINF_leng(incall)="<<*p<<"("<<jleng<<") slave_id:"<<jaddr<<endl;
    if(ECREFFKEY.reprtf[2]>2)EventBitDump(leng,p,"Event-by-event:");
  }
//
  node2crs(jaddr,crat);//get crate#(1-2, =0,if notfound)
  csid=1;//here is dummy(no redundancy for JINFs), later it is defined from card_id(EDR(a,b),ETRG(a,b))
//  cout<<"      crate="<<crat<<endl;
  if(jleng>1 && crat>0)EcalJobStat::daqs1(1);//<=== count non-empty, valid JINF-fragments
  else{
    if(crat==0)EcalJobStat::daqs1(30);//badExit caused by JINF badID
    if(jleng<=1)EcalJobStat::daqs1(31);//badExit caused by JINF empty
    goto Exit;
  }
//
  if(dataf){
    if(crcer)EcalJobStat::daqs1(10+7*(crat-1));
    if(asser)EcalJobStat::daqs1(11+7*(crat-1));
    if(amswer)EcalJobStat::daqs1(12+7*(crat-1));
    if(timoer)EcalJobStat::daqs1(13+7*(crat-1));
    if(fpower)EcalJobStat::daqs1(14+7*(crat-1));
    if(seqer)EcalJobStat::daqs1(15+7*(crat-1));
    if(cdpnod)EcalJobStat::daqs1(16+7*(crat-1));
  }
  else{
    EcalJobStat::daqs1(2+crat-1);//<=== count JINF's notData segments
  }
//
  noerr=(dataf && !crcer && !asser && !amswer 
                                       && !timoer && !fpower && !seqer && !cdpnod);
  if(noerr)EcalJobStat::daqs1(4+crat-1);//<=== count JINF-reply status OK     
  else goto Exit;
//-----------
  jbias=1;
  while(jbias<jleng){//<---- JINF-words loop
    eleng=*(p+jbias);//deeper level (EDR2/ETRG) fragment's length
    eblid=*(p+jbias+eleng);//deeper level (EDR2/ETRG) fragment's Status+slaveID(EDR/ETRG id) 
    eaddr=(eblid&(0x001F));//slaveID(="NodeAddr"=ERD/ETRGaddr here)(one of 7/side permitted)
    datyp=((eblid&(0x00C0))>>6);//0,1,2(only raw or compr)
    if(datyp==0 || datyp==1)setrawf();//or OnBoardPeds
    if(datyp==2)setcomf();
    if(datyp==3)setmixf();
    badtyp=(datyp>0);//not OnBoardPeds format 
    empty=(eleng==1);
    formt=getformat();//0/1/2->raw(OnBpeds)/compr/mix flag for current EDR/ETRG
    if(ECREFFKEY.reprtf[2]>1)cout<<" <--- XDR_length/addr="<<eleng<<" "<<eaddr<<"  datyp="<<datyp<<
                                                                           " formt="<<formt<<endl;
    EcalJobStat::daqs2(crat-1,formt);//<--- entries per crate/format
    slots=AMSECIds::crdid2sl(csid,eaddr);//get seq.slot#(0-5 =>EDRs; =6 =>ETRG;-1==>not_found) ans side
    csid=csid+1;//1/2->active EDR/ETRG side
    if(ECREFFKEY.reprtf[2]>1)cout<<"      SeqSlot(0-6)="<<slots<<endl;
    if(ECREFFKEY.reprtf[2]>1)EventBitDump(eleng,p+jbias,"---> Block-by-block EvDump");
    if(slots<0){//not found in the list of ids
#ifdef __AMSDEBUG__
      cout<<"   <--- ECBlock::Error:illegal CardID, crate/side/fmt/id="<<crat<<" "<<csid<<" "<<formt
                                                                <<" "<<hex<<eaddr<<dec<<endl;
#endif
      EcalJobStat::daqs2(crat-1,3+formt);//illegal CardId
      goto NextBlock;    
    }
    slot=int16u(slots);//0-5,6
    EcalJobStat::daqs3(crat-1,slot,formt);//entries per crate/slot vs fmt
//edr/etrg status-bits:
    dataf=((eblid&(0x8000))>0);//data-fragment(=1 for OnBoardPed Table as for normal data)
    crcer=((eblid&(0x4000))>0);//CRC-error
    asser=((eblid&(0x2000))>0);//assembly-error
    amswer=((eblid&(0x1000))>0);//amsw-error   
    timoer=((eblid&(0x0800))>0);//timeout-error   
    fpower=((eblid&(0x0400))>0);//FEpower-error   
    seqer=((eblid&(0x0400))>0);//sequencer-error
    cdpnod=((eblid&(0x0020))>0);//CDP-node(like EDR2/ETRG-node with no futher fragmentation)
//
    if(dataf && !badtyp){
      if(crcer)EcalJobStat::daqs3(crat-1,slot,7+12*formt);
      if(asser)EcalJobStat::daqs3(crat-1,slot,8+12*formt);
      if(amswer)EcalJobStat::daqs3(crat-1,slot,9+12*formt);
      if(timoer)EcalJobStat::daqs3(crat-1,slot,10+12*formt);
      if(fpower)EcalJobStat::daqs3(crat-1,slot,11+12*formt);
      if(seqer)EcalJobStat::daqs3(crat-1,slot,12+12*formt);
      if(empty)EcalJobStat::daqs3(crat-1,slot,13+12*formt);
      if(cdpnod)EcalJobStat::daqs3(crat-1,slot,14+12*formt);
    }
    else if(!dataf && !badtyp){//format ok, but notData(not OnBoardPeds)
      EcalJobStat::daqs3(crat-1,slot,3);
      goto NextBlock;// skip block
    }//--->endof datatype-ok, but nonData(peds ???)
//
    else if(badtyp){//not requested(implied) format 
      EcalJobStat::daqs3(crat-1,slot,6);//<=== bad slot-format(not raw=OnBped) - take next block
      goto NextBlock;
    }
//    
    noerr=(dataf && !crcer && !asser && !amswer 
                                       && !timoer && !fpower && !seqer && cdpnod);
    if(noerr)EcalJobStat::daqs3(crat-1,slot,15+12*formt);//"EDR/ETRG-reply status OK" entries per crate/format
    else goto NextBlock;
//
    ebias=1;
//------
    if(slot<=5){//<===== EDR-block processing
//-------
      bad=false;
      _FoundPedBlks+=1;
      PedBlkOK=false;
      if(eleng==(ECEDRC+1)){//<-------- PedTable length OK
	EcalJobStat::daqs3(crat-1,slot,5);//PedTable entrie with length OK
        PedBlkOK=(_PedBlkCrat[crat-1][slot]==0);//true if requested and leng-ok 
        while(ebias<eleng){//<---- EDR-words loop (243 ADC-values)
          word=*(p+jbias+ebias);//ped, ADC-value(multiplied by 16 in DSP)
	  rdch=(ebias-1);//0-242
	  AMSECIds ecid(crat-1,csid-1,slot,rdch);//create ecid-obj
	  swid=ecid.getswid();//long sw_id=LTTPG
          ped=geant(word&0xFFFF)/16;//tempor
          sig=0.6;//tempor(missing in real data)
	  dped=-1;//dummy
	  thrs=-1;//dummy
	  ECPedCalib::filltb(swid, ped, dped, thrs, sig);//tempor
	  ebias+=1;
	}//--->endof EDR-words loop(PedTable)
        if(PedBlkOK){
	  _PedBlkCrat[crat-1][slot]=1;//mark processed(no errrors) crate/edr
          _GoodPedBlks+=1;//counts tot# of requested&processed PedBlocks
	}
//               call DB- and pedfile-writing if last :
        if(_GoodPedBlks==_NReqEdrs ||
	                     _FoundPedBlks >= _NReqEdrs){//<---last(from requested) ped-block processed
          nblkok=0;
          for(i=0;i<ECRT;i++)for(j=0;j<ECEDRS;j++)if(_PedBlkCrat[i][j]==1)nblkok+=1;
          if(nblkok==_NReqEdrs){// complete set of good blocks - call output
	    ECPedCalib::outptb((CALIB.SubDetInCalib%10));//1/2/3->write2db/hist_only/write2file+hist
	  }
	  if(ECREFFKEY.reprtf[2]>0)
	    cout<<"   <--- EcalOnbPedCalib sequence is over: blocks found/requested/good:"<<_FoundPedBlks<<
	                                              " "<<_NReqEdrs<<" "<<_GoodPedBlks<<endl<<endl;
        }//---<endof "last PedBlock processed"
      }//--->endof PedTable length check
      else bad=true;//wrong length
//
      if(bad)goto Exit;//wrong length ==> stop any further processing(skip the rest of given JINF)
    }//--->endof "slot<=5"(EDR) check
//----
NextBlock:
    jbias+=eleng+1;//"+1" to include eleng-word itself
  }//---->endof JINF-level loop
//-------
  badexit=false;
Exit:
  if(badexit)EcalJobStat::daqs1(ECJSTA-1);//count rejected JINF-entries(segments)
//    
}//--->endof critical
//
  return;  
}
//-------------------------------------------------------
void DAQECBlock::buildonbP(integer leng, int16u *p){
//(Is used starting from June 2009 when dynamic pedestals logic was introduced)
// it is implied that event_fragment is EC-JINF's one (validity is checked in calling procedure)
//           *p=pointer_to_beggining_of_block_data(i.e. calstat word !!!)
// this event fragment may keep EDR2,ETRG blocks in arbitrary order;
// Length counting does not include length-word itself !!!
//
  integer i,j,ic,icn,isl,isd;
  integer swid,hwid,crsta(0);
  int16u swch,rdch,slot,aslt,crat,val16,ibit,slay,pmt,pix,gain,trppmt;
  int16u dtyp,datyp,lenraw(0),lencom(0),formt,evnum;
  int16u jbias,ebias,jblid,eblid,jleng,eleng,jaddr,eaddr,csid;
  int16u word,nword,nnword,nnnword;
  int16u bias1,bias2;
  int16u fmt;
  int16u osbit;
  int16 slots;
  integer bufpnt(0),lbbs;
  uinteger val32;
  integer portid,crdid;
  uinteger runn;
  bool PedBlkOK(false);
  bool newrun;
  bool OnbFmtOK(false);
  geant ped,sig,dped,thr;
  int16u sts,nblkok;
  int16u calstat(0);
  int16u crslsd;
  int16u bllen;
  int16u headw1;
  int16u headw2;
  int16u naddr;
  int16u len;
  bool bad;
//<-- check ped-block sections: 
  integer spatt=ECCAFFKEY.onbpedspat;//bit-patt for onb.ped-table sections (bit set if section is present)
  bool dpedin=((spatt&4)==4);//dyn.peds-section present(243 words)
  bool thrsin=((spatt&2)==2);//thresholds ..............(243...)
  integer reflen=ECEDRC+ECEDRC;//ped&width-sections always pres.
  if(dpedin)reflen+=ECEDRC;
  if(thrsin)reflen+=ECEDRC;
  reflen+=2;//add calstat+slavestat, now =729(3x243)+2=731
// total block length=731+2(headers)+1crc=734 
//
  bool pcreq(false);
  bool sidedoubled(false);
  bool dataf;//data-fragment
  bool crcer;//CRC-error
  bool asser;//assembly-error
  bool amswer;//amsw-error   
  bool timoer;//timeout-error   
  bool fpower;//FEpower-error   
  bool seqer;//sequencer-error
  bool cdpnod;//CDP-node(like EDR2-node with no futher fragmentation)
  bool noerr;
  bool empty;
  bool badtyp;
  char * p2tdvnam;
  bool badexit;
//
#pragma omp critical (ec_pedc_onb)
{
  badexit=true;
  if((CALIB.SubDetInCalib%10)!=1){
    if(_CalFirstSeq){//CalFirstSeq is set to true only once in initjob(initb)
      cout<<endl<<"<======= Warning: ECAL OnBoardPed DB-writing is blocked !!!"<<endl<<endl;
//      _CalFirstSeq is set to false after the 1st good sequence (was call to outb(flg))
    }
  }
//
  EcalJobStat::daqs1(50);//count OnbP calls
  bad=false;//ok
  runn=AMSEvent::gethead()->getrun();
  newrun=(_PrevRunNum!=runn);
//
  if(newrun || _FirstPedBlk){//count/mark requested EDRs on 1st call or new run
//    cout<<"  Ntdv="<<AMSJob::gethead()->gettdvn()<<endl;
//    for(i=0;i<AMSJob::gethead()->gettdvn();i++){     
//      p2tdvnam=AMSJob::gethead()->gettdvc(i);
//      cout<<" TDV-name="<<p2tdvnam<<" at "<<i<<endl;
//    }
//
    cout<<endl;
    cout<<"=========> DAQECBlock::buildOnbPed: NewRun/FirstPedBlock call, run:"<<runn<<endl;
    cout<<"           Good PedBlocks in prev.run :"<<_GoodPedBlks
                                            <<", total found:"<<_FoundPedBlks<<endl;
    _FirstPedBlk=false;
    _PrevRunNum=runn;
    ECPedCalib::BRun()=runn;
    ECPedCalib::resetb();//reset histograms, cumulative vars(GoodPedBlks,FoundPedBlks,NReqEdrs,PedBlkCrat[],...)
//
//these codes replace ones which are 4 lines below (thank to onboard s/w developers !)
    for(ic=0;ic<ECRT;ic++){
      for(isl=0;isl<ECEDRS;isl++){//sequential slots(EDRs only)
	_PedBlkCrat[ic][isl]+=1;//mark requested EDRs
	_NReqEdrs+=1;
      }
    }
//
//    for(ic=0;ic<ECRT;ic++){
//      for(isl=0;isl<ECEDRS;isl++){//sequential slots(EDRs only)
//        portid=DAQECBlock::getportid(ic);//>=0 if exist(12,13 here)
//	for(isd=0;isd<2;isd++){//sides(a/b)
//	  crdid=AMSECIds::sl2crdid(isd,isl);
//          pcreq=(pdaq && portid>=0 && crdid>=0
//	            && pdaq->CalibRequested(uinteger(portid),uinteger(crdid)));//ped-cal requested for given crate/slot ?
//          if(pcreq){
//	    _PedBlkCrat[ic][isl]+=1;//mark requested EDRs
//	    _NReqEdrs+=1;
//	  }
//	}
//	if(_PedBlkCrat[ic][isl]>0){//error: both sides requested for given crat/slot
//	  cout<<"      <--- Warning: found that both sides are requested for crt/slt="<<ic<<" "<<isl<<endl;
//	  _PedBlkCrat[ic][isl]=0;//preset by hands
//	  sidedoubled=true;
//	}
//      }
//    }
    cout<<"           Total EDRs requested:"<<_NReqEdrs<<endl;
    if(!sidedoubled && _NReqEdrs<(ECRT*ECEDRS)){
      cout<<"      <--- DAQECBlock::buildonbP:Warning-> not all EDRs requested ?!"<<endl;
//      bad=true;
    }
    else if(sidedoubled){
      cout<<"<--------- Error: Some sides are doubled, NrequestedEDRs="<<_NReqEdrs<<endl;
      bad=true;
    }
  }//--->endof 1st PedBlk(new run)
//
  if(bad)goto Exit;
  EcalJobStat::daqs1(51);// requested entries
//--------
  bllen=(*(p-3))/2;//BlLength (in front of header) bytes->words
  headw1=*(p-2);
  headw2=*(p-1);
  calstat=*p;
  naddr=int16u((headw1>>5)&0x1FF);//node addr from 1st header word
  len=int16u(leng&(0xFFFFL));//fragment's length in 16b-words(not including length word itself)
  crslsd=int16u((leng>>16)&(0xFFFFL));//CrSlSd(c=1-2,sl=1,8,sd=1-3=>a,b,p) as return by my "checkblockidP"-1
  crat=(crslsd+1)/100;
  slot=((crslsd+1)%100)/10;//1-6
  csid=(crslsd+1)%10;
  slot-=1;//0-5
  if(csid==3)csid=1;//primary=a ?
//
  EcalJobStat::daqs3(crat-1,slot,0);//entries
//
  if(ECREFFKEY.reprtf[2]>1){//debug
    cout<<"  <---- In DAQECBlock::builOnbP: EDR_length(incall)="<<len<<" reflen="<<reflen<<endl;
    if(ECREFFKEY.reprtf[2]>1)EventBitDump(reflen-1,p,"Event-by-event bit Dump :");
  }
//
  if(calstat&(0x4000)==0){
    if(ECREFFKEY.reprtf[2]>0){
      cout<<"<------ In BuilOnbPed:Bad CalStat,crat="<<crat<<" EDR="<<slot+1<<endl;
    }
    EcalJobStat::daqs3(crat-1,slot,1);//bad status entries
    goto Exit;
  }
//
  if(calstat&(0xF)>0){
    EcalJobStat::daqs3(crat-1,slot,2);//crazy bit entries
  }
//---
  if((len-reflen-3)!=0){
    if(ECREFFKEY.reprtf[2]>0){
      cout<<"<------ In BuilOnbPed:Bad length,crat="<<crat<<" EDR="<<slot+1<<endl;
    }
    EcalJobStat::daqs3(crat-1,slot,3);//bad length
    goto Exit;
  }
//
  EcalJobStat::daqs3(crat-1,slot,4);//length/calstat OK
//---
//-----------
    eleng=reflen;//EDR fragment's length
    eblid=*(p-1+eleng);//EDR fragment's Status+slaveID(EDR/ETRG id) 
    eaddr=(eblid&(0x001F));//slaveID(="NodeAddr"=ERD here) here =0(imply direct JMDC->EDR request)
    datyp=((eblid&(0x00C0))>>6);//0,1,2(only raw or compr)
    dataf=((eblid&(0x8000))>0);//data-fragment(=0 for OnBoardPed Table)
    OnbFmtOK=(dataf==0);// OnBoardFmt OK (ignore datyp for ped-calibr)
    if(!OnbFmtOK)goto Exit;
    EcalJobStat::daqs3(crat-1,slot,5);//format OK
    formt=0;
    crcer=((eblid&(0x4000))>0);//CRC-error
    asser=((eblid&(0x2000))>0);//assembly-error
    amswer=((eblid&(0x1000))>0);//amsw-error   
    timoer=((eblid&(0x0800))>0);//timeout-error   
    fpower=((eblid&(0x0400))>0);//FEpower-error   
    seqer=((eblid&(0x0400))>0);//sequencer-error
    cdpnod=((eblid&(0x0020))>0);//CDP-node(like EDR2/ETRG-node with no futher fragmentation)
//
    if(crcer)EcalJobStat::daqs3(crat-1,slot,7+12*formt);
    if(asser)EcalJobStat::daqs3(crat-1,slot,8+12*formt);
    if(amswer)EcalJobStat::daqs3(crat-1,slot,9+12*formt);
    if(timoer)EcalJobStat::daqs3(crat-1,slot,10+12*formt);
    if(fpower)EcalJobStat::daqs3(crat-1,slot,11+12*formt);
    if(seqer)EcalJobStat::daqs3(crat-1,slot,12+12*formt);
    if(cdpnod)EcalJobStat::daqs3(crat-1,slot,14+12*formt);
//    
    noerr=(!dataf && !crcer && !asser && !amswer 
                                       && !timoer && !fpower && !seqer && cdpnod);
    if(noerr)EcalJobStat::daqs3(crat-1,slot,6);//"EDR/ETRG-reply status OK" entries per crate/format
    else{ 
      EcalJobStat::daqs1(52+crat-1);// count bad reply status
      goto Exit;
    }
//
    ebias=1;
//------
    if(slot<=5){//<===== EDR-block processing
//-------
      bad=false;
      _FoundPedBlks+=1;
      PedBlkOK=false;
      PedBlkOK=(_PedBlkCrat[crat-1][slot]==0);//true if requested 
      while(ebias<=ECEDRC){//<---- EDR-words loop (243 ADC-values)
        word=*(p+ebias);//ped, ADC-value(multiplied by 16 in DSP)
        if(dpedin)nword=*(p+ebias+ECEDRC);//dped(+ECEDRC to skip ped)
        else nword=0;
        bias1=0;//add.bias to thresh-section
        if(dpedin)bias1+=ECEDRC;
        if(thrsin)nnword=*(p+ebias+ECEDRC+bias1);//thrs
        else nnword=0;
        bias2=bias1;//add.bias to sig-section
        if(thrsin)bias2+=ECEDRC; 
        nnnword=*(p+ebias+ECEDRC+bias2);//sig
	rdch=(ebias-1);//0-242
	AMSECIds ecid(crat-1,csid-1,slot,rdch);//create ecid-obj
	swid=ecid.getswid();//long sw_id=LTTPG
        ped=geant(word&0xFFFF)/16;//tempor
        dped=geant(nword&0xFFFF)/16;
        thr=geant(nnword&0xFFFF)/16;
        sig=geant(nnnword&0xFFFF)/16;
	ECPedCalib::filltb(swid, ped, dped, thr, sig);//tempor
	ebias+=1;
      }//--->endof EDR-words loop(PedTable)
      if(PedBlkOK){
	_PedBlkCrat[crat-1][slot]=1;//mark processed(no errrors) crate/edr
        _GoodPedBlks+=1;//counts tot# of requested&processed PedBlocks
      }
//               call DB- and pedfile-writing if last :
      if(_GoodPedBlks==_NReqEdrs 
	                   ||  DAQEvent::CalibDone(4)
	                   || _FoundPedBlks >= _NReqEdrs){//<---last(from requested) ped-block processed
        if(ECREFFKEY.reprtf[2]>0 && !DAQEvent::CalibDone(4))
                cout<<"<---- ECOnbPedCalib Error: Vitaly's CalDone-flag not set when sequence is over !!!?"<<endl;
        nblkok=0;
        for(i=0;i<ECRT;i++)for(j=0;j<ECEDRS;j++)if(_PedBlkCrat[i][j]==1)nblkok+=1;
        if(nblkok==_NReqEdrs){// complete set of good blocks - call output
	  ECPedCalib::outptb((CALIB.SubDetInCalib%10));//1/2/3->write2db/hist_only/write2file+hist
	}
	if(ECREFFKEY.reprtf[2]>0)
	  cout<<"   <--- EcalOnbPedCalib sequence is over: blocks found/requested/good:"<<_FoundPedBlks<<
	                                              " "<<_NReqEdrs<<" "<<_GoodPedBlks<<endl<<endl;
      }//---<endof "last PedBlock processed"
//
    }//--->endof "slot<=5"(EDR) check
//-------
  badexit=false;
Exit:
  if(badexit)EcalJobStat::daqs1(ECJSTA-1);//count rejected entries(EDRs)
//    
}//--->endof critical
//
  return;  
}
//-------------------------------------------------------
void DAQECBlock::buildonbP2(integer leng, int16u *p){
//(??? Is used starting from June 2009 when dynamic pedestals logic was introduced)
// it is implied that event_fragment is EC-JINF's one (validity is checked in calling procedure)
//           *p=pointer_to_beggining_of_block_data(i.e. NEXT to len-word !!!)
// this event fragment may keep EDR2,ETRG blocks in arbitrary order;
// Length counting does not include length-word itself !!!
//
  integer i,j,ic,icn,isl,isd;
  integer swid,hwid,crsta(0);
  int16u swch,rdch,slot,aslt,crat,val16,ibit,slay,pmt,pix,gain,trppmt;
  int16u dtyp,datyp,lenraw(0),lencom(0),formt,evnum;
  int16u jbias,ebias,jblid,eblid,jleng,eleng,jaddr,eaddr,csid;
  int16u word,nword,nnword,nnnword;
  int16u bias1,bias2;
  int16u fmt;
  int16u osbit;
  int16 slots;
  integer bufpnt(0),lbbs;
  uinteger val32;
  integer portid,crdid;
  uinteger runn;
  bool PedBlkOK(false);
  bool newrun;
  geant ped,sig,dped,thr;
  int16u sts,nblkok;
  int16u calstat(0);
  bool bad;
//<-- check ped-block sections: 
  integer spatt=TFCAFFKEY.onbpedspat;//bit-patt for onb.ped-table sections (bit set if section is present)
  bool dpedin=((spatt&4)==1);//dyn.peds-section present(243 words)
  bool thrsin=((spatt&2)==1);//thresholds ..............(243...)
  integer reflen=ECEDRC+ECEDRC;//ped&width-sections always pres.
  if(dpedin)reflen+=ECEDRC;
  if(thrsin)reflen+=ECEDRC;//1 EDR-data  reflen does not count calstat words 
//
  bool pcreq(false);
  bool sidedoubled(false);
  bool dataf;//data-fragment
  bool crcer;//CRC-error
  bool asser;//assembly-error
  bool amswer;//amsw-error   
  bool timoer;//timeout-error   
  bool fpower;//FEpower-error   
  bool seqer;//sequencer-error
  bool cdpnod;//CDP-node(like EDR2-node with no futher fragmentation)
  bool noerr;
  bool empty;
  bool badtyp;
  char * p2tdvnam;
  bool badexit;
//
#pragma omp critical (ec_pedc_onb)
{
  badexit=true;
  if((CALIB.SubDetInCalib%10)!=1){
    if(_CalFirstSeq){//CalFirstSeq is set to true only once in initjob(initb)
      cout<<endl<<"<======= Warning: ECAL OnBoardPed DB-writing is blocked !!!"<<endl<<endl;
//      _CalFirstSeq is set to false after the 1st good sequence (was call to outb(flg))
    }
  }
//
  EcalJobStat::daqs1(0);//count entries
  DAQEvent * pdaq = (DAQEvent*)AMSEvent::gethead()->getheadC("DAQEvent",6);
  bad=false;//ok
  runn=AMSEvent::gethead()->getrun();
  newrun=(_PrevRunNum!=runn);
//
  if(newrun || _FirstPedBlk){//count/mark requested EDRs on 1st call or new run
//    cout<<"  Ntdv="<<AMSJob::gethead()->gettdvn()<<endl;
//    for(i=0;i<AMSJob::gethead()->gettdvn();i++){     
//      p2tdvnam=AMSJob::gethead()->gettdvc(i);
//      cout<<" TDV-name="<<p2tdvnam<<" at "<<i<<endl;
//    }
//
    cout<<endl;
    cout<<"=========> DAQECBlock::buildOnbPed: NewRun/FirstPedBlock call, run:"<<runn<<endl;
    cout<<"           Good PedBlocks in prev.run :"<<_GoodPedBlks
                                            <<", total found:"<<_FoundPedBlks<<endl;
    _FirstPedBlk=false;
    _PrevRunNum=runn;
    ECPedCalib::BRun()=runn;
    ECPedCalib::resetb();//reset histograms, cumulative vars(GoodPedBlks,FoundPedBlks,NReqEdrs,PedBlkCrat[],...)
//
//these codes replace ones which are 4 lines below (thank to onboard s/w developers !)
    for(ic=0;ic<ECRT;ic++){
      for(isl=0;isl<ECEDRS;isl++){//sequential slots(EDRs only)
	_PedBlkCrat[ic][isl]+=1;//mark requested EDRs
	_NReqEdrs+=1;
      }
    }
//
//    for(ic=0;ic<ECRT;ic++){
//      for(isl=0;isl<ECEDRS;isl++){//sequential slots(EDRs only)
//        portid=DAQECBlock::getportid(ic);//>=0 if exist(12,13 here)
//	for(isd=0;isd<2;isd++){//sides(a/b)
//	  crdid=AMSECIds::sl2crdid(isd,isl);
//          pcreq=(pdaq && portid>=0 && crdid>=0
//	            && pdaq->CalibRequested(uinteger(portid),uinteger(crdid)));//ped-cal requested for given crate/slot ?
//          if(pcreq){
//	    _PedBlkCrat[ic][isl]+=1;//mark requested EDRs
//	    _NReqEdrs+=1;
//	  }
//	}
//	if(_PedBlkCrat[ic][isl]>0){//error: both sides requested for given crat/slot
//	  cout<<"      <--- Warning: found that both sides are requested for crt/slt="<<ic<<" "<<isl<<endl;
//	  _PedBlkCrat[ic][isl]=0;//preset by hands
//	  sidedoubled=true;
//	}
//      }
//    }
    cout<<"           Total EDRs requested:"<<_NReqEdrs<<endl;
    if(!sidedoubled && _NReqEdrs<(ECRT*ECEDRS)){
      cout<<"      <--- DAQECBlock::buildonbP:Warning-> not all EDRs requested ?!"<<endl;
//      bad=true;
    }
    else if(sidedoubled){
      cout<<"<--------- Error: Some sides are doubled, NrequestedEDRs="<<_NReqEdrs<<endl;
      bad=true;
    }
  }//--->endof 1st PedBlk(new run)
//
  if(bad)goto Exit;
//--------
  p=p-1;//to follow VC-convention
  jleng=int16u(leng&(0xFFFFL));//fragment's 1st word(block length) call value
  jblid=*(p+jleng);// JINF fragment's last word: Status+slaveID(its id)
//
  dataf=((jblid&(0x8000))>0);//data-fragment
  crcer=((jblid&(0x4000))>0);//CRC-error
  asser=((jblid&(0x2000))>0);//assembly-error
  amswer=((jblid&(0x1000))>0);//amsw-error   
  timoer=((jblid&(0x0800))>0);//timeout-error   
  fpower=((jblid&(0x0400))>0);//FEpower-error   
  seqer=((jblid&(0x0400))>0);//sequencer-error
  cdpnod=((jblid&(0x0020))>0);//CDP-node(like EDR2-node with no futher fragmentation)
  jaddr=(jblid&(0x001F));//slaveID(="NodeAddr"=JINFaddr here)(one of 4 permitted)
//
  if(ECREFFKEY.reprtf[2]>1){//debug
    cout<<"  <---- In DAQECBlock::builOnbP: JINF_leng(incall)="<<*p<<"("<<jleng<<") slave_id:"<<jaddr<<endl;
    if(ECREFFKEY.reprtf[2]>2)EventBitDump(leng,p,"Event-by-event:");
  }
//
  node2crs(jaddr,crat);//get crate#(1-2, =0,if notfound)
  csid=1;//here is dummy(no redundancy for JINFs), later it is defined from card_id(EDR(a,b),ETRG(a,b))
cout<<"      crate="<<crat<<endl;
  if(jleng>1 && crat>0)EcalJobStat::daqs1(1);//<=== count non-empty, valid JINF-fragments
  else{
    if(crat==0)EcalJobStat::daqs1(30);//badExit caused by JINF badID
    if(jleng<=1)EcalJobStat::daqs1(31);//badExit caused by JINF empty
    goto Exit;
  }
//
  if(!dataf){
    if(crcer)EcalJobStat::daqs1(10+7*(crat-1));
    if(asser)EcalJobStat::daqs1(11+7*(crat-1));
    if(amswer)EcalJobStat::daqs1(12+7*(crat-1));
    if(timoer)EcalJobStat::daqs1(13+7*(crat-1));
    if(fpower)EcalJobStat::daqs1(14+7*(crat-1));
    if(seqer)EcalJobStat::daqs1(15+7*(crat-1));
    if(cdpnod)EcalJobStat::daqs1(16+7*(crat-1));
  }
  else{
    EcalJobStat::daqs1(2+crat-1);//<=== count JINF's not segments
  }
//
  noerr=(!dataf && !crcer && !asser && !amswer 
                                       && !timoer && !fpower && !seqer && !cdpnod);
cout<<"<--- JINF: dataf="<<dataf<<endl;
  if(!noerr)EcalJobStat::daqs1(4+crat-1);//<=== count JINF-reply status OK     
  else goto Exit;
//-----------
  jbias=1;
  while(jbias<jleng){//<---- JINF-words loop
    eleng=*(p+jbias);//deeper level (EDR2/ETRG) fragment's length
    eblid=*(p+jbias+eleng);//deeper level (EDR2/ETRG) fragment's Status+slaveID(EDR/ETRG id) 
    eaddr=(eblid&(0x001F));//slaveID(="NodeAddr"=ERD/ETRGaddr here)(one of 7/side permitted)
    datyp=((eblid&(0x00C0))>>6);//0,1,2(only raw or compr)
    if(datyp==0 || datyp==1)setrawf();//or OnBoardPeds
    if(datyp==2)setcomf();
    if(datyp==3)setmixf();
    badtyp=(datyp>0);//not OnBoardPeds format 
    empty=(eleng==1);
    formt=getformat();//0/1/2->raw(OnBpeds)/compr/mix flag for current EDR/ETRG
    if(ECREFFKEY.reprtf[2]>1)cout<<" <--- XDR_length/addr="<<eleng<<" "<<eaddr<<"  datyp="<<datyp<<
                                                                           " formt="<<formt<<endl;
    EcalJobStat::daqs2(crat-1,formt);//<--- entries per crate/format
    slots=AMSECIds::crdid2sl(csid,eaddr);//get seq.slot#(0-5 =>EDRs; =6 =>ETRG;-1==>not_found) ans side
    csid=csid+1;//1/2->active EDR/ETRG side
    if(ECREFFKEY.reprtf[2]>1)cout<<"      SeqSlot(0-6)="<<slots<<endl;
    if(ECREFFKEY.reprtf[2]>1)EventBitDump(eleng,p+jbias,"---> Block-by-block EvDump");
    if(slots<0){//not found in the list of ids
#ifdef __AMSDEBUG__
      cout<<"   <--- ECBlock::Error:illegal CardID, crate/side/fmt/id="<<crat<<" "<<csid<<" "<<formt
                                                                <<" "<<hex<<eaddr<<dec<<endl;
#endif
      EcalJobStat::daqs2(crat-1,3+formt);//illegal CardId
      goto NextBlock;    
    }
    slot=int16u(slots);//0-5,6
    EcalJobStat::daqs3(crat-1,slot,formt);//entries per crate/slot vs fmt
//edr/etrg status-bits:
    dataf=((eblid&(0x8000))>0);//data-fragment(=1 for OnBoardPed Table as for normal data)
    crcer=((eblid&(0x4000))>0);//CRC-error
    asser=((eblid&(0x2000))>0);//assembly-error
    amswer=((eblid&(0x1000))>0);//amsw-error   
    timoer=((eblid&(0x0800))>0);//timeout-error   
    fpower=((eblid&(0x0400))>0);//FEpower-error   
    seqer=((eblid&(0x0400))>0);//sequencer-error
    cdpnod=((eblid&(0x0020))>0);//CDP-node(like EDR2/ETRG-node with no futher fragmentation)
cout<<"   EDRslot:"<<slot<<"  dataf="<<dataf<<" datyp(fmt)="<<datyp<<endl; 
//
    if(dataf && !badtyp){
      if(crcer)EcalJobStat::daqs3(crat-1,slot,7+12*formt);
      if(asser)EcalJobStat::daqs3(crat-1,slot,8+12*formt);
      if(amswer)EcalJobStat::daqs3(crat-1,slot,9+12*formt);
      if(timoer)EcalJobStat::daqs3(crat-1,slot,10+12*formt);
      if(fpower)EcalJobStat::daqs3(crat-1,slot,11+12*formt);
      if(seqer)EcalJobStat::daqs3(crat-1,slot,12+12*formt);
      if(empty)EcalJobStat::daqs3(crat-1,slot,13+12*formt);
      if(cdpnod)EcalJobStat::daqs3(crat-1,slot,14+12*formt);
    }
    else if(!dataf && !badtyp){//format ok, but notData(not OnBoardPeds)
      EcalJobStat::daqs3(crat-1,slot,3);
      goto NextBlock;// skip block
    }//--->endof datatype-ok, but nonData(peds ???)
//
    else if(badtyp){//not requested(implied) format 
      EcalJobStat::daqs3(crat-1,slot,6);//<=== bad slot-format(not raw=OnBped) - take next block
      goto NextBlock;
    }
//    
    noerr=(dataf && !crcer && !asser && !amswer 
                                       && !timoer && !fpower && !seqer && cdpnod);
    if(noerr)EcalJobStat::daqs3(crat-1,slot,15+12*formt);//"EDR/ETRG-reply status OK" entries per crate/format
    else goto NextBlock;
//
    ebias=1;
//------
    if(slot<=5){//<===== EDR-block processing
//-------
      bad=false;
      _FoundPedBlks+=1;
      PedBlkOK=false;
      calstat=*(p+jbias+1);
      if(eleng==(reflen+1+1)){//<-------- PedTable length OK(extra +1 due to calstat word)
	EcalJobStat::daqs3(crat-1,slot,5);//PedTable entrie with length OK
        PedBlkOK=(_PedBlkCrat[crat-1][slot]==0);//true if requested and leng-ok 
        while(ebias<(ECEDRC+1)){//<---- EDR-words loop (243 ADC-values)
          word=*(p+jbias+1+ebias);//ped, ADC-value(multiplied by 16 in DSP)(+1 to bypass calstat word)
          if(dpedin)nword=*(p+jbias+1+ebias+ECEDRC);//dped
          else nword=0;
          bias1=0;//add.bias to thresh-section
          if(dpedin)bias1+=ECEDRC;
          if(thrsin)nnword=*(p+jbias+1+ebias+ECEDRC+bias1);//thrs
          else nnword=0;
          bias2=bias1;//add.bias to sig-section
          if(thrsin)bias2+=ECEDRC; 
          nnnword=*(p+jbias+1+ebias+ECEDRC+bias2);//sig
	  rdch=(ebias-1);//0-242
	  AMSECIds ecid(crat-1,csid-1,slot,rdch);//create ecid-obj
	  swid=ecid.getswid();//long sw_id=LTTPG
          ped=geant(word&0xFFFF)/16;//tempor
          dped=geant(nword&0xFFFF)/16;
          thr=geant(nnword&0xFFFF)/16;
          sig=geant(nnnword&0xFFFF)/16;
	  ECPedCalib::filltb(swid, ped, dped, thr, sig);//tempor
	  ebias+=1;
	}//--->endof EDR-words loop(PedTable)
        if(PedBlkOK){
	  _PedBlkCrat[crat-1][slot]=1;//mark processed(no errrors) crate/edr
          _GoodPedBlks+=1;//counts tot# of requested&processed PedBlocks
	}
//               call DB- and pedfile-writing if last :
        if(_GoodPedBlks==_NReqEdrs 
	                   ||  DAQEvent::CalibDone(4)
	                   || _FoundPedBlks >= _NReqEdrs){//<---last(from requested) ped-block processed
        if(ECREFFKEY.reprtf[2]>0 && !DAQEvent::CalibDone(4))
                cout<<"<---- ECOnbPedCalib Error: Vitaly's CalDone-flag not set when sequence is over !!!?"<<endl;
          nblkok=0;
          for(i=0;i<ECRT;i++)for(j=0;j<ECEDRS;j++)if(_PedBlkCrat[i][j]==1)nblkok+=1;
          if(nblkok==_NReqEdrs){// complete set of good blocks - call output
	    ECPedCalib::outptb((CALIB.SubDetInCalib%10));//1/2/3->write2db/hist_only/write2file+hist
	  }
	  if(ECREFFKEY.reprtf[2]>0)
	    cout<<"   <--- EcalOnbPedCalib sequence is over: blocks found/requested/good:"<<_FoundPedBlks<<
	                                              " "<<_NReqEdrs<<" "<<_GoodPedBlks<<endl<<endl;
        }//---<endof "last PedBlock processed"
      }//--->endof PedTable length check
      else bad=true;//wrong length
//
      if(bad)goto Exit;//wrong length ==> stop any further processing(skip the rest of given JINF)
    }//--->endof "slot<=5"(EDR) check
//----
NextBlock:
    jbias+=eleng+1;//"+1" to include eleng-word itself
  }//---->endof JINF-level loop
//-------
  badexit=false;
Exit:
  if(badexit)EcalJobStat::daqs1(ECJSTA-1);//count rejected JINF-entries(segments)
//    
}//--->endof critical
//
  return;  
}
//------------------------------------
void DAQECBlock::EventBitDump(integer leng, int16u *p, char * message){
  int16u blid,len,naddr,datyp;
  len=int16u(leng&(0xFFFFL));//fragment's length in 16b-words(not including length word itself)
  blid=*(p+len);// fragment's last word: Status+slaveID
  bool dataf=((blid&(0x8000))>0);//data-fragment
  bool crcer=((blid&(0x4000))>0);//CRC-error
  bool asser=((blid&(0x2000))>0);//assembly-error
  bool amswer=((blid&(0x1000))>0);//amsw-error   
  bool timoer=((blid&(0x0800))>0);//timeout-error   
  bool fpower=((blid&(0x0400))>0);//FEpower-error   
  bool seqer=((blid&(0x0400))>0);//sequencer-error
  bool cdpnod=((blid&(0x0020))>0);//CDP-node(like SDR2-node with no futher fragmentation)
  bool noerr;
  naddr=blid&(0x001F);//slaveID(="NodeAddr"=SDR_link#)
  datyp=(blid&(0x00C0))>>6;//0,1,2,3 (if dataf=0, datyp=2->OnbPed)
  if(dataf && datyp>0)noerr=(dataf && !crcer && !asser && !amswer 
                                       && !timoer && !fpower && !seqer && cdpnod);
  if(!dataf)noerr=(!dataf && !crcer && !asser && !amswer 
                                       && !timoer && !fpower && !seqer && cdpnod);
  else noerr=false;
  cout<<"----> DAQECBlock::"<<message<<" for event:"<<AMSEvent::gethead()->getid()<<endl;
  cout<<" SlaveStatWord="<<hex<<blid<<dec<<" NoAsseblyErr="<<noerr<<endl; 
  cout<<" Slave_addr(link#)="<<naddr<<" data_type(fmt)="<<datyp<<" BlLength(in this call)="<<len<<endl;
//
  cout<<"  Block hex/binary dump follows :"<<endl<<endl;
  int16u tstw,tstb;
  char simbl[3]="I|";
  char simb;
  for(int i=0;i<len+1;i++){
    tstw=*(p+i);
    cout<<hex<<setw(4)<<tstw<<"  I"<<dec;
    for(int j=0;j<16;j++){
      if(((j+1)%4)==0)simb=simbl[0];
      else simb=simbl[1];
      tstb=(1<<(15-j));
      if((tstw&tstb)!=0)cout<<"x"<<simb;
      else cout<<" "<<simb;
    }
    cout<<endl;
  }
  cout<<"-----------------------------------------------------------"<<endl;
  cout<<dec<<endl<<endl;
}//
//------------------------------------
integer DAQECBlock::getmaxblocks(){return 2;}//only one JINF per crate is implied(no redundancy)
//---
integer DAQECBlock::calcblocklength(integer ibl){
//calc. JINF-block length from MC-data
//cout<<"------> In calcblocklength, block="<<ibl<<endl;
  int i,j,k;
  integer id,idd;
  integer nqwords(0),nwords(0);
  int16u nwslot[ECEDRS];
  int16u isl,pmt,pix,gain,crate,slot,stat;
  geant padc[3],adcd;
  AMSEcalRawEvent * ptr=(AMSEcalRawEvent*)AMSEvent::gethead()->
                                     getheadC("AMSEcalRawEvent",ibl,0);
  for(i=0;i<ECEDRS;i++)nwslot[i]=0;
//
  bool newdataf(false);
  if(DAQCFFKEY.DAQVersion==1)newdataf=true;
  else newdataf=false;
  if(!AMSJob::gethead()->isRealData())newdataf=true;//tempor fix
//count anodes:
  while(ptr){ // <--- RawEvent-hits loop in crate ibl:
    isl=ptr->getslay();//suplayer:0,...8 
    id=ptr->getid();//LTTP
    idd=id/10;
    pix=id%10-1;//Pixel(0-3) for anodes
    pmt=idd%100-1;//pmTube(0-...35)
    ptr->getpadc(padc);
    if(padc[0]>0 || padc[1]>0){
      gain=0;//slot# is the same for gain=0/1 channels
      AMSECIds ecid(isl,pmt,pix,gain,0);
      slot=ecid.getslot();//0-5 -> EDRs
      if(padc[0]>0)nwslot[slot]+=1;//pix hi-gain
      if(padc[1]>0)nwslot[slot]+=1;//pix low-gain
    }
    ptr=ptr->next();  
  } // ---> end of RawEvent-hits loop in crate
//count dynodes:
  for(isl=0;isl<ECSLMX;isl++){//SupLayers loop
    for(pmt=0;pmt<ECPMMX;pmt++){
      adcd=AMSEcalRawEvent::getadcd(isl,pmt);//extract Dynode
      if(adcd>0){
        gain=0;
        pix=4;
        AMSECIds ecid(isl,pmt,pix,gain,0);
	crate=ecid.getcrate();//0,1
	slot=ecid.getslot();//0-5 for edrs
	if(crate==ibl)nwslot[slot]+=1;
      }
    }
  }
//cout<<"words/slot:"<<endl;
//for(i=0;i<ECEDRS;i++)cout<<nwslot[i]<<" ";
//cout<<endl;
//
  for(isl=0;isl<ECEDRS;isl++){//count EDR's words
    if(nwslot[isl]>0){
      nwords+=2*nwslot[isl];//adc-ch# + adc-value words
      nwords+=2;//nwords word + edr-status
    }
  }
//
  nwords+=9;//add etrg-block words (7data + leng +status)
  nwords+=1;//JINF status
  if(newdataf)nwords+=2;//add new format 2 cdpmsk words (in front of JINF status)
//
  return -nwords;//JINJ-readout
}
//---
void DAQECBlock::buildblock(integer ibl, integer len, int16u *p){
//create JINF-block from MC-data
//cout<<"------> In buildblock, block="<<ibl<<endl;
  int i,j,k;
  integer sta,id,idd;
  int16u isl,pmt,pix,gain,crate,slot,chan,stat;
  geant padc[3];
  integer swid;
  geant peda,pedd,adca,adcd;
  int16u trpatt[6][3];//dyn.trig.patt([6]=>suplayers:2-7; [3]=>3x16bits used for triggered dynodes)
  AMSEcalRawEvent * ptr;
  int16u edrbuf[ECEDRS][ECEDRC];
  int16u nwslot[ECEDRS];
//
  bool newdataf(false);
  if(DAQCFFKEY.DAQVersion==1)newdataf=true;
  else newdataf=false;
  if(!AMSJob::gethead()->isRealData())newdataf=true;//tempor fix
//cout<<"====> newdataf="<<newdataf<<" DAQVersion="<<DAQCFFKEY.DAQVersion<<endl;
//
  ptr=(AMSEcalRawEvent*)AMSEvent::gethead()->
                       getheadC("AMSEcalRawEvent",ibl,0);
  for(i=0;i<ECEDRS;i++){// 6 EDRs(slots)
    nwslot[i]=0;
    for(j=0;j<ECEDRC;j++){//243 channels/EDR
      edrbuf[i][j]=0;
    }
  }
//--->extract anodes:

  while(ptr){ // <--- RawEvent-hits loop in crate:
    isl=ptr->getslay();//suplayer:0,...8
    id=ptr->getid();//LTTP
    idd=id/10;
    pix=id%10-1;//Pixel(0-3)
    pmt=idd%100-1;//pmTube(0-...35)
    ptr->getpadc(padc);
    sta=ptr->getstatus();
    if(padc[0]>0){//pix hi-gain
      gain=0;
      AMSECIds ecid(isl,pmt,pix,gain,0);
      chan=ecid.getrdch();//0-242
      slot=ecid.getslot();//0-5 -> EDRs, 6->ETRG
      crate=ecid.getcrate();
      peda=ECPMPeds::pmpeds[isl][pmt].ped(pix,gain);//current Aped
//      edrbuf[slot][chan]=int16u(floor((padc[0]+peda-0.5)*16));
      edrbuf[slot][chan]=int16u(floor((padc[0])*16));
      nwslot[slot]+=1;
    }
    if(padc[1]>0){//pix low-gain
      gain=1;
      AMSECIds ecid(isl,pmt,pix,gain,0);
      chan=ecid.getrdch();//0-242
      slot=ecid.getslot();//0-5 -> EDRs, 6->ETRG
      crate=ecid.getcrate();
      peda=ECPMPeds::pmpeds[isl][pmt].ped(pix,gain);//current Aped
//      edrbuf[slot][chan]=int16u(floor((padc[1]+peda-0.5)*16));
      edrbuf[slot][chan]=int16u(floor((padc[1])*16));
      nwslot[slot]+=1;
    }
    ptr=ptr->next();  
  } // ---> end of RawEvent-hits loop in crate
//--->extract dynodes:
  for(isl=0;isl<ECSLMX;isl++){//SupLayer loop
    for(pmt=0;pmt<ECPMMX;pmt++){
      adcd=AMSEcalRawEvent::getadcd(isl,pmt);//extract Dynode
      if(adcd>0){
        gain=0;
        pix=4;
        AMSECIds ecid(isl,pmt,pix,gain,0);
        chan=ecid.getrdch();//0-242
        slot=ecid.getslot();//0-5 -> EDRs, 6->ETRG
        crate=ecid.getcrate();//0,1
        if(crate==ibl){
          nwslot[slot]+=1;
          pedd=ECPMPeds::pmpeds[isl][pmt].pedd();//current Dped
//          edrbuf[slot][chan]=int16u(floor((adcd+pedd-0.5)*16));
          edrbuf[slot][chan]=int16u(floor((adcd)*16));
        }
      }
    }
  }
//cout<<"----> BuilBlock: found Anode&Dynode Hits/slot"<<endl;
//for(i=0;i<ECEDRS;i++)cout<<nwslot[i]<<" ";
//cout<<endl;
//--->extract ETRG info:
  int16u val16,ibit,slay,slid,trppmt;
  int16u trpbuf[7];
//
  crate=ibl+1;//1-2
  for(int16u iwd=0;iwd<7;iwd++){// 7 ETRG-words loop
    val16=0;//
    for(int16u ibt=0;ibt<16;ibt++){
      ibit=16*iwd+ibt;//0-111(really used 0-107(3sl*36pm),108(XA),109(XF))
      slay=ibit/36;//0-2
      if(crate==1)slay=2*slay;//trig.layers counting: 0,2,4(X-proj,Face B(D))
      if(crate==2)slay=2*slay+1;//trig.layers counting: 1,3,5(Y-proj,Face A(C))
      pmt=ibit%36;//0-35
      if(AMSEcalRawEvent::gettrpbit(slay,pmt)==1)val16|=(1<<ibt);//set bit
      if(ibit==107)break;
    }
    if(iwd==6){//this last word contains Fast/Lev1(angle) decision bits, let's add them
//add FTE bits:
      if(crate==1)trppmt=38;//X-pr
      if(crate==2)trppmt=39;//Y-pr
      if(AMSEcalRawEvent::gettrpbit(5,trppmt)==1)val16|=(0x8000);//add XF(YF) bit    
//add LVL1 bits:
      if(crate==1)trppmt=36;//X-pr
      if(crate==2)trppmt=37;//Y-pr
      if(AMSEcalRawEvent::gettrpbit(5,trppmt)==1)val16|=(0x4000);//add XA(YA) bit    
    } 
    trpbuf[iwd]=val16;
  }//--->endof ETRG-words loop
//
//--->start to move buffers to outp:
//->EDRs:
  integer nwrite(0);
  for(slot=0;slot<ECEDRS;slot++){//EDR(slot) loop (0-5)
    if(nwslot[slot]>0){
      *(p+nwrite)=2*nwslot[slot]+1;//+1 for EDR status word
      nwrite+=1;
      for(chan=0;chan<ECEDRC;chan++){
        if(edrbuf[slot][chan]>0){
	  *(p+nwrite)=chan;
          nwrite+=1;
	  *(p+nwrite)=edrbuf[slot][chan];
          nwrite+=1;
	}
      }
      slid=AMSECIds::sl2crdid(0,slot);//0=side, slid=-1 if wrong slot(not the case here)
      stat=(0x8000);//data bit
      stat|=(0x20);//no further fragmentation
      stat|=(0x80);//compr.fmt
      stat|=(slid&(0x1F));//add EDR id
      *(p+nwrite)=stat;//status word
      nwrite+=1;
    }
  }
//cout<<"    wrote EDRs 16bwords:"<<nwrite<<endl;
//->ETRG:
  *(p+nwrite)=7+1;//block length (+1 for status)
  nwrite+=1;
  for(int16u iwd=0;iwd<7;iwd++){// 7 ETRG-words loop
    *(p+nwrite)=trpbuf[iwd];
    nwrite+=1;
  }
  slot=6;//etrg seq.slot#
  slid=AMSECIds::sl2crdid(0,slot);//0=side
  stat=(0x8000);//data bit
  stat|=(0x20);//no further fragmentation
  stat|=(0x80);//compr.fmt
  stat|=(slid&(0x1F));//add ETRG id (22)
  *(p+nwrite)=stat;//status word
  nwrite+=1;
//cout<<"    wrote EDRs+ETRG 16bwords:"<<nwrite<<endl;
// add 2 new cdpmsk words in front of JINF status word
  if(newdataf){
    *(p+nwrite)=0;//all cdp's ok
    nwrite+=1;
    *(p+nwrite)=0;//all cdp's ok
    nwrite+=1;
  } 
//->JINF stat.word
  integer portid=DAQECBlock::getportid(int16u(ibl));//12,13
  slid=int16u(portid&(0x001FL));
  stat=(0x8000);//data bit
  stat|=(0x80);//compr.fmt
  stat|=slid;//add JINF port id
  *(p+nwrite)=stat;//status word
  nwrite+=1;
//
//  cout<<"<--- exit: call/internal length="<<len<<" "<<nwrite<<endl;
  if(nwrite!=len){
    cout<<"<--- DAQECBlock::buildblock:length mismatch, call/intern length="<<len<<" "<<nwrite<<endl;
//    exit(10);
  }
//
}
