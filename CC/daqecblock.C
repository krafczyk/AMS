// AMS02 version 16.11.06 by E.Choumilov
#include "typedefs.h"
#include <iostream.h>
#include "link.h"
#include <fstream.h>
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
int16u DAQECBlock::format=0; // default format (raw)
//
int16u DAQECBlock::nodeids[ecalconst::ECRT]=//valid EC_nodes(JINFs) id(link#)(2 JINF => 2crates)  
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
//
integer DAQECBlock::checkblockid(int16u blid){//JINF's ids as Ports("VC"'s blid)
  int valid(0);
  char sstr[128];
  for(int i=0;i<ecalconst::ECRT;i++){
    sprintf(sstr,"JINFE%X",i);
    valid=DAQEvent::ismynode(blid,sstr)?(i+1):0;
  }
//  cout<<"----> In DAQECBlock::checkblockid, blid(hex)="<<hex<<blid<<",addr="<<dec<<(blid&(0x001F))<<",valid="<<valid<<endl;
  return valid;
}
//----
integer DAQECBlock::checkblockidP(int16u blid){//EDR's and JINF's ids as Nodes("VC"'s blid)
  int valid(0);
  char sstr[128];
  char side[5]="ABPS";
  char str[2];
//  cout<<"---> In DAQS2Block::checkblockid, blid(hex)="<<hex<<blid<<",addr:"<<dec<<(blid&(0x001F))<<endl;
  for(int i=0;i<ecalconst::ECRT;i++){//for EDRs
    for(int j=0;j<3;j++){
      str[0]=side[j];
      str[1]='\0';
      sprintf(sstr,"EDR%X%X%s",i,j,str);
      if(DAQEvent::ismynode(blid,sstr)){
        valid=10*(i+1)+j+1;
//cout<<"<--- valid="<<valid<<endl;
        return valid;
      }
    } 
  }
  if(valid==0){
    for(int i=0;i<ecalconst::ECRT;i++){//for JINFs
    for(int j=0;j<4;j++){
      str[0]=side[j];
      str[1]='\0';
      sprintf(sstr,"JF-E%X%s",i,str);
      if(DAQEvent::ismynode(blid,sstr)){
        valid=10*(i+1)+j+1;
//cout<<"<--- valid="<<valid<<endl;
        return valid;
      }
    } 
    }
  }
  return 0;
}
//-------------------------------------------------------
void DAQECBlock::buildraw(integer leng, int16u *p){
// it is implied that event_fragment is EC-JINF's one (validity is checked in calling procedure)
//           *p=pointer_to_beggining_of_block_data(i.e. NEXT to len-word !!!)
// this event fragment may keep EDR2,ETRG blocks in arbitrary order;
// Length counting does not include length-word itself !!!
//
  integer i,j,ic,icn;
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
// for class/DownScaled ped-cal
  bool PedCal(false);//means PedCal job, using event-by-event in RawFMT or DownScaled 
  bool DownScal(false);
  static int FirstDScalBlk(0);
  int16u PedSubt[ECSLOTS]={1,1,1,1,1,1,1};//0/1->no/yes PedSubtr at RawEvent creation
// for PedCalTable(onboard calib)
  static integer FirstPedBlk(0);
  static integer TotPedBlks(0);
  static integer PedBlkCrat[ECRT][ECEDRS]={0,0,0,0,0,0,0,0,0,0,0,0};
  bool PedBlkOK(false);
  geant ped,sig;
  int16u sts,nblkok;
//
  uinteger swvbuf[ECRCMX];
// keep:       ADC-values   
  integer  swibuf[ECRCMX],hwibuf[ECRCMX];
//keep:        swids=LTTPG         hwids=CSSRRR   
// these buffers will be filled with Ecal sw-channels hits in current JINF(crate) as 
// arrays of sequential sw-channel index 
// Also filled at EcalRawEvent objects creation the dynode static arrays:
// AMSEcalRawEvent::dynadc[ECSLMX][ECPMSMX];//Dunode hits vs (SL,PMT)
//
  for(ic=0;ic<ECRCMX;ic++){//clear buffs
    swibuf[ic]=0;
    hwibuf[ic]=0;
    swvbuf[ic]=0;
  }
//
  EcalJobStat::daqs1(0);//count entries
  p=p-1;//to follow VC-convention
  jleng=int16u(leng&(0xFFFFL));//fragment's 1st word(block length) call value
  jblid=*(p+jleng);// JINF fragment's last word: Status+slaveID(its id)
//
  bool dataf=((jblid&(0x8000))>0);//data-fragment
  bool crcer=((jblid&(0x4000))>0);//CRC-error
  bool asser=((jblid&(0x2000))>0);//assembly-error
  bool amswer=((jblid&(0x1000))>0);//amsw-error   
  bool timoer=((jblid&(0x0800))>0);//timeout-error   
  bool fpower=((jblid&(0x0400))>0);//FEpower-error   
  bool seqer=((jblid&(0x0400))>0);//sequencer-error
  bool cdpnod=((jblid&(0x0020))>0);//CDP-node(like EDR2-node with no futher fragmentation)
  bool noerr;
  jaddr=(jblid&(0x001F));//slaveID(="NodeAddr"=JINFaddr here)(one of 4 permitted)
//
  if(ECREFFKEY.reprtf[2]>1){//debug
    cout<<"====> In DAQECBlock::buildraw: JINF_leng(incall)="<<*p<<"("<<jleng<<") slave_id:"<<jaddr<<endl;
    if(ECREFFKEY.reprtf[2]>1)EventBitDump(leng,p,"Event-by-event:");
  }
//
  node2crs(jaddr,crat);//get crate#(1-2, =0,if notfound)
  csid=1;//here is dummy(no redundancy for JINFs), later it is defined from card_id(EDR(a,b),ETRG(a,b))
//  cout<<"      crate="<<crat<<endl;
  if(jleng>1 && crat>0)EcalJobStat::daqs1(1);//<=== count non-empty, valid JINF-fragments
  else goto BadExit;
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
  else goto BadExit;
//-----------
  if(ECREFFKEY.relogic[1]==4)PedCal=true;//tempor(use later info about presence of ClassPedData from header ?)
  if(!PedCal && ECREFFKEY.relogic[1]==4){
    cout<<"DAQECBlock::buildraw-W-Not ClassicPedCalibData when classic PedCal job is requested !!!"<<endl;
    return;
  }
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
    formt=getformat();//0/1->raw/compr flag for current EDR/ETRG
//    cout<<"    XDR_length/addr="<<eleng<<" "<<eaddr<<"  datyp="<<datyp<<endl;
    if(eleng>1 && datyp>0)EcalJobStat::daqs2(crat-1,formt);//entries per crate/format
    else goto NextBlock;
//edr/etrg status-bits:
    dataf=((eblid&(0x8000))>0);//data-fragment
    crcer=((eblid&(0x4000))>0);//CRC-error
    asser=((eblid&(0x2000))>0);//assembly-error
    amswer=((eblid&(0x1000))>0);//amsw-error   
    timoer=((eblid&(0x0800))>0);//timeout-error   
    fpower=((eblid&(0x0400))>0);//FEpower-error   
    seqer=((eblid&(0x0400))>0);//sequencer-error
    cdpnod=((eblid&(0x0020))>0);//CDP-node(like EDR2/ETRG-node with no futher fragmentation)
    noerr=(dataf && !crcer && !asser && !amswer 
                                       && !timoer && !fpower && !seqer && cdpnod);
//    if(noerr)cout<<" status-bits OK..."<<endl;
    if(noerr)EcalJobStat::daqs2(crat-1,2+formt);//"EDR/ETRG-reply status OK" entries per crate/format
    else goto NextBlock;
//
    slots=AMSECIds::crdid2sl(csid,eaddr);//get seq.slot#(0-5 =>EDRs; =6 =>ETRG;-1==>not_found) ans side
    csid=csid+1;//1/2->active EDR/ETRG side
//    cout<<"    SeqSlot="<<slots<<endl;
    if(slots<0){
#ifdef __AMSDEBUG__
      cout<<"ECBlock::Error:illegal CardID, crate/side/fmt/id="<<crat<<" "<<csid<<" "<<formt
                                                                <<" "<<hex<<eaddr<<dec<<endl;
#endif
      EcalJobStat::daqs2(crat-1,4+formt);//illegal CardId
      goto NextBlock;    
    }
    slot=int16u(slots);
    EcalJobStat::daqs3(crat-1,slot,0);//entries per crate/slot
    ebias=1;
//------
    if(slot==6){//<===== ETRG-block processing(modifying ebias)
      if(eleng==8)EcalJobStat::daqs3(crat-1,slot,2);//lengthOK
      else goto BadExit;
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
    }
//------
    if(slot<=5){//<===== EDR-block processing
//--
      if(formt==0){//<===================== RawFormat
//-------
	if(eleng==(3*ECEDRC+1)){//<-------- PedTable found
	  EcalJobStat::daqs3(crat-1,slot,3);//PedTable entrie with length OK
	  if(ECREFFKEY.relogic[1]==6){//PedTable was requested ==> processing...
            if(FirstPedBlk==0){
              ECPedCalib::BTime()=AMSEvent::gethead()->gettime();
              ECPedCalib::BRun()=AMSEvent::gethead()->getrun();
              ECPedCalib::resetb();
            }
            PedBlkOK=true;
            while(ebias<eleng){//<---- EDR-words loop (3*243 ADC-values)
              word=*(p+jbias+ebias);//ped, ADC-value(multiplied by 16 in DSP)
              nword=*(p+jbias+ebias+1);//sig, ADC-value(multiplied by 16 in DSP)
              nnword=*(p+jbias+ebias+2);//stat, x16 ???
	      rdch=(ebias-1)/3;//0-242
	      AMSECIds ecid(crat-1,csid-1,slot,rdch);//create ecid-obj
	      swid=ecid.getswid();//long sw_id=LTTPG
              ped=geant(word&0xFFFF)/16;//tempor
              sig=geant(nword&0xFFFF)/16;//tempor
              sts=nnword;//tempor 1/0->bad(empty)/ok
	      ECPedCalib::filltb(swid, ped, sig, sts);//tempor
	      ebias+=3;
	    }//--->endof EDR-words loop(PedTable)
            if(PedBlkOK)PedBlkCrat[crat-1][slot]=1;//mark good processed crate
            TotPedBlks+=1;//counts tot# of processed PedBlocks
            FirstPedBlk=1;
//               call DB- and pedfile-writing if last :
            if(TotPedBlks==(ECRT*ECEDRS)){//<---last(12th) ped-block processed
              nblkok=0;
              for(i=0;i<ECRT;i++)for(j=0;j<ECEDRS;j++)if(PedBlkCrat[i][j]==1)nblkok+=1;
              if(nblkok==(ECRT*ECEDRS)){// all found blocks OK
	        ECPedCalib::outptb(ECCAFFKEY.pedoutf);//0/1/2->noactions(only_histos)/write2db+file/write2file
	      }
              TotPedBlks=0;//be ready for next calib.blocks sequence
              FirstPedBlk=0;
              for(i=0;i<ECRT;i++)for(j=0;j<ECEDRS;j++)PedBlkCrat[i][j]=0;
            }//---<endof "last PedBlock processed"
	  }//--->endof PedTable processing
          goto NextBlock;//(if any)
	}//--->endof PedTable presence check
//-------
        else if(eleng==(ECEDRC+1))EcalJobStat::daqs3(crat-1,slot,1);//true RawSegment & length OK
        else goto BadExit;//wrong length ==> stop any further processing
//cout<<"    RawFMT,EDR_length OK"<<endl;
//             <======================== Normal RawFormat :
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
        if(PedCal)PedSubt[slot]=0;//my internal flag to subtr(1)/not(0) peds at RawEvent-creation
        DownScal=(ECREFFKEY.relogic[1]==5);//consider EDR-block as "downscaled",i.e no "0" suppression
//
	if(DownScal && PedCal){//"downscaled" block found (just because it is requested !)
          if(FirstDScalBlk==0){
            ECPedCalib::BTime()=AMSEvent::gethead()->gettime();//store Begin-time
	    FirstDScalBlk=1;
          }
	}//--->endof "DownScaled" block ped-processing
      }
//-------
      else{//<======================== ComprFormat : 
        if(eleng<=(2*ECEDRC+1) && (eleng%2==1))EcalJobStat::daqs3(crat-1,slot,2);//lengthOK
        else goto BadExit;
//cout<<"    ComprFMT,EDR_length OK"<<endl;
        while(ebias<eleng){//<---- EDR-words loop (max 2*243 (rdch# + ADC-valie))
	  rdch=*(p+jbias+ebias);//rdch(0-242)
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
	  ebias+=2;
//
	}//--->endof EDR-words loop(comprF)
      }//--->endof ComprFMT
    }
//----
NextBlock:
    jbias+=eleng+1;//"+1" to include eleng-word itself
  }//---->endof JINF-level loop
//
//----------------------------> scan swch-buffers and create RawEvent-Objects:
//
  integer sswid,swidn,sswidn,shwid,subtrped,sta;
  geant peda,siga,pedd,sigd,athr,dthr,padc[3],adca,adcd;
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
      sigd=ECPMPeds::pmpeds[slay-1][pmt-1].pedd();//current Dsig
//
      switch(gain){//fill RawEvent arrays
        case 1:
	  if(pix<=4){//anode-adc(hi)
	    adca=geant(swvbuf[ic])/16+0.5;//"16" to go back to true ADC-value
	    if(subtrped){
	      if((adca-peda)>athr*siga)padc[0]=adca-peda;//subtr.ped and apply DAQ-threshold
	      else padc[0]=0;
	    }
	    else padc[0]=adca;
	  }
	  else{//dynode(pix=5)
	    adcd=geant(swvbuf[ic])/16+0.5;
	    if(subtrped){
	      if((adcd-pedd)>dthr*sigd)padc[2]=adcd-pedd;//subtr.ped and apply DAQ-threshold
	      else padc[2]=0;
	    }
	    else padc[2]=adcd;
	    AMSEcalRawEvent::setadcd(slay-1,pmt-1,padc[2]);
	  }
	  break;
//
        case 2:
	  adca=geant(swvbuf[ic])/16+0.5;//anode-adc(low)
	  if(subtrped){
	    if((adca-peda)>athr*siga)padc[1]=adca-peda;//subtr.ped and apply DAQ-threshold
	    else padc[1]=0;
	  }
	  else padc[1]=adca;
	  break;
//
        default:
	  cout<<"EcalDAQBuf-scan:unknown measurement type(gain) ! swid="<<swid<<endl;
      }//-->endof switch
    }//-->endof "!=0 LTTPG found"
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
	if(PedSubt[aslt-1])sta=0;//ok(normal RawEvent object with subtracted ped)
	else sta=1;//NOW it is flag for Validate-stage that Peds was not subtracted !!!
        padc[2]=0;//no valid Dyn-info in RawEvent for the moment(will be added in Validation)
        if(AMSEvent::gethead()->addnext(AMSID("AMSEcalRawEvent",crat-1),
                 new AMSEcalRawEvent(sswid,sta,csid,padc)))crsta=1;
      }
      for(i=0;i<3;i++){//reset RawEvent adc-buffer
        padc[i]=0;
      }
    }//-->endof next/last LTTP check
  }//-->endof scan
//
  if(ECREFFKEY.reprtf[2]>1){//debug-prints
    cout<<"===> Fired Dynodes map after processing of crate="<<crat<<":"<<endl;
    cout<<"           (reading direction: pm=1--->pm=36)"<<endl<<endl;
    for(slay=0;slay<ECSLMX;slay++){
      for(pmt=0;pmt<ECPMMX;pmt++){
        cout<<AMSEcalRawEvent::getadcd(slay,pmt)<<" ";
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
void DAQECBlock::buildonbP(integer leng, int16u *p){
// it is implied that event_fragment is EC-JINF's one (validity is checked in calling procedure)
//           *p=pointer_to_beggining_of_block_data(i.e. NEXT to len-word !!!)
// this event fragment may keep EDR2,ETRG blocks in arbitrary order;
// Length counting does not include length-word itself !!!
//
  integer i,j,ic,icn;
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
// for PedCalTable(onboard calib)
  static integer FirstPedBlk(0);
  static integer TotPedBlks(0);
  static integer PedBlkCrat[ECRT][ECEDRS]={0,0,0,0,0,0,0,0,0,0,0,0};
  bool PedBlkOK(false);
  geant ped,sig;
  int16u sts,nblkok;
//
  EcalJobStat::daqs1(0);//count entries
  p=p-1;//to follow VC-convention
  jleng=int16u(leng&(0xFFFFL));//fragment's 1st word(block length) call value
  jblid=*(p+jleng);// JINF fragment's last word: Status+slaveID(its id)
//
  bool dataf=((jblid&(0x8000))>0);//data-fragment
  bool crcer=((jblid&(0x4000))>0);//CRC-error
  bool asser=((jblid&(0x2000))>0);//assembly-error
  bool amswer=((jblid&(0x1000))>0);//amsw-error   
  bool timoer=((jblid&(0x0800))>0);//timeout-error   
  bool fpower=((jblid&(0x0400))>0);//FEpower-error   
  bool seqer=((jblid&(0x0400))>0);//sequencer-error
  bool cdpnod=((jblid&(0x0020))>0);//CDP-node(like EDR2-node with no futher fragmentation)
  bool noerr;
  jaddr=(jblid&(0x001F));//slaveID(="NodeAddr"=JINFaddr here)(one of 4 permitted)
//
  if(ECREFFKEY.reprtf[2]>1){//debug
    cout<<"====> In DAQECBlock::buildraw: JINF_leng(incall)="<<*p<<"("<<jleng<<") slave_id:"<<jaddr<<endl;
    if(ECREFFKEY.reprtf[2]>1)EventBitDump(leng,p,"Event-by-event:");
  }
//
  node2crs(jaddr,crat);//get crate#(1-2, =0,if notfound)
  csid=1;//here is dummy(no redundancy for JINFs), later it is defined from card_id(EDR(a,b),ETRG(a,b))
//  cout<<"      crate="<<crat<<endl;
  if(jleng>1 && crat>0)EcalJobStat::daqs1(1);//<=== count non-empty, valid JINF-fragments
  else goto BadExit;
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
  else goto BadExit;
//-----------
  if(ECREFFKEY.relogic[1]!=6){
    cout<<"DAQECBlock::buildonbP: Called while OnBoardPedCal job is not requested !!!"<<endl;
    return;
  }
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
    formt=getformat();//0/1->raw/compr flag for current EDR/ETRG
    if(ECREFFKEY.relogic[1]==6)formt=2;
//    cout<<"    XDR_length/addr="<<eleng<<" "<<eaddr<<"  datyp="<<datyp<<endl;
    if(eleng>1 && datyp>0)EcalJobStat::daqs2(crat-1,formt);//entries per crate/format
    else goto NextBlock;
//edr/etrg status-bits:
    dataf=((eblid&(0x8000))>0);//data-fragment
    crcer=((eblid&(0x4000))>0);//CRC-error
    asser=((eblid&(0x2000))>0);//assembly-error
    amswer=((eblid&(0x1000))>0);//amsw-error   
    timoer=((eblid&(0x0800))>0);//timeout-error   
    fpower=((eblid&(0x0400))>0);//FEpower-error   
    seqer=((eblid&(0x0400))>0);//sequencer-error
    cdpnod=((eblid&(0x0020))>0);//CDP-node(like EDR2/ETRG-node with no futher fragmentation)
    noerr=(dataf && !crcer && !asser && !amswer 
                                       && !timoer && !fpower && !seqer && cdpnod);
//    if(noerr)cout<<" status-bits OK..."<<endl;
    if(noerr)EcalJobStat::daqs2(crat-1,3+formt);//"EDR/ETRG-reply status OK" entries per crate/format
    else goto NextBlock;
//
    slots=AMSECIds::crdid2sl(csid,eaddr);//get seq.slot#(0-5 =>EDRs; =6 =>ETRG;-1==>not_found) ans side
    csid=csid+1;//1/2->active EDR/ETRG side
//    cout<<"    SeqSlot="<<slots<<endl;
    if(slots<0){
#ifdef __AMSDEBUG__
      cout<<"ECBlock::Error:illegal CardID, crate/side/fmt/id="<<crat<<" "<<csid<<" "<<formt
                                                                <<" "<<hex<<eaddr<<dec<<endl;
#endif
      EcalJobStat::daqs2(crat-1,6+formt);//illegal CardId
      goto NextBlock;    
    }
    slot=int16u(slots);
    EcalJobStat::daqs3(crat-1,slot,0);//entries per crate/slot
    ebias=1;
//------
    if(slot<=5){//<===== EDR-block processing
//-------
      if(eleng==(3*ECEDRC+1)){//<-------- PedTable found
	EcalJobStat::daqs3(crat-1,slot,3);//PedTable entrie with length OK
        if(FirstPedBlk==0){
          ECPedCalib::BTime()=AMSEvent::gethead()->gettime();
          ECPedCalib::BRun()=AMSEvent::gethead()->getrun();
          ECPedCalib::resetb();
        }
        PedBlkOK=true;
        while(ebias<eleng){//<---- EDR-words loop (3*243 ADC-values)
          word=*(p+jbias+ebias);//ped, ADC-value(multiplied by 16 in DSP)
          nword=*(p+jbias+ebias+1);//sig, ADC-value(multiplied by 16 in DSP)
          nnword=*(p+jbias+ebias+2);//stat, x16 ???
	  rdch=(ebias-1)/3;//0-242
	  AMSECIds ecid(crat-1,csid-1,slot,rdch);//create ecid-obj
	  swid=ecid.getswid();//long sw_id=LTTPG
          ped=geant(word&0xFFFF)/16;//tempor
          sig=geant(nword&0xFFFF)/16;//tempor
          sts=nnword;//tempor 1/0->bad(empty)/ok
	  ECPedCalib::filltb(swid, ped, sig, sts);//tempor
	  ebias+=3;
	}//--->endof EDR-words loop(PedTable)
        if(PedBlkOK)PedBlkCrat[crat-1][slot]=1;//mark good processed crate
        TotPedBlks+=1;//counts tot# of processed PedBlocks
        FirstPedBlk=1;
//               call DB- and pedfile-writing if last :
        if(TotPedBlks==(ECRT*ECEDRS)){//<---last(12th) ped-block processed
          nblkok=0;
          for(i=0;i<ECRT;i++)for(j=0;j<ECEDRS;j++)if(PedBlkCrat[i][j]==1)nblkok+=1;
          if(nblkok==(ECRT*ECEDRS)){// all found blocks OK
	    ECPedCalib::outptb(ECCAFFKEY.pedoutf);//0/1/2->noactions(only_histos)/write2db+file/write2file
	  }
          TotPedBlks=0;//be ready for next calib.blocks sequence
          FirstPedBlk=0;
          for(i=0;i<ECRT;i++)for(j=0;j<ECEDRS;j++)PedBlkCrat[i][j]=0;
        }//---<endof "last PedBlock processed"
        goto NextBlock;//(if any)
      }//--->endof PedTable presence check
      else goto BadExit;//wrong length ==> stop any further processing
    }//--->endof "slot<=5"(EDR) check
//----
NextBlock:
    jbias+=eleng+1;//"+1" to include eleng-word itself
  }//---->endof JINF-level loop
//-------
  return;
BadExit:
  EcalJobStat::daqs1(ECJSTA-1);//count rejected JINF-entries(segments)    
//  
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
  datyp=(blid&(0x00C0))>>6;//0,1,2,3
  noerr=(dataf && !crcer && !asser && !amswer 
                                       && !timoer && !fpower && !seqer && cdpnod);
  cout<<"----> DAQECBlock::"<<message<<" for event:"<<AMSEvent::gethead()->getid()<<endl;
  cout<<" Segment_id="<<hex<<blid<<dec<<" NoAsseblyErr="<<noerr<<endl; 
  cout<<" node_addr(link#)="<<naddr<<" data_type(fmt)="<<datyp<<" block_length="<<len<<endl;
//
  cout<<"  Block hex/binary dump follows :"<<endl<<endl;
  int16u tstw,tstb;
  for(int i=0;i<len+1;i++){
    tstw=*(p+i);
    cout<<hex<<setw(4)<<tstw<<"  |"<<dec;
    for(int j=0;j<16;j++){
      tstb=(1<<(15-j));
      if((tstw&tstb)!=0)cout<<"x"<<"|";
      else cout<<" "<<"|";
    }
    cout<<endl;
  }
  cout<<"-----------------------------------------------------------"<<endl;
  cout<<dec<<endl<<endl;
}//
//------------------------------------
integer DAQECBlock::getmaxblocks(){return 2;}//only one JINF per crate is implied(no redundancy)
integer DAQECBlock::calcblocklength(integer ibl){return 0;}
void DAQECBlock::buildblock(integer ibl, integer len, int16u *p){
  int leng=*p;
}
