//  $Id$
// 1.0 version 2.07.97 E.Choumilov
// AMS02 version 7.11.06 by E.Choumilov : TOF/ANTI RawFormat preliminary decoding is provided
// 
#include "typedefs.h"
#include "link.h"
#include "cern.h"
#include "commons.h"
#include "event.h"
#include <time.h>
#include "timeid.h"
#include "tofdbc02.h"
#include "tofid.h"
#include "daqs2block.h"
#include "tofrec02.h"
#include "tofsim02.h"
#include "antidbc02.h"
#include "antirec02.h"
#include "tofcalib02.h"
#include "anticalib02.h"
//
using namespace TOF2GC;
using namespace ANTI2C;
//
 uinteger DAQS2Block::_PrevRunNum(0);
 integer DAQS2Block::_NReqEdrs(0);
 bool DAQS2Block::_FirstPedBlk(true);
 integer DAQS2Block::_GoodPedBlks(0);
 integer DAQS2Block::_FoundPedBlks(0);
 integer DAQS2Block::_PedBlkCrat[TOF2GC::SCCRAT]={-1,-1,-1,-1};
 bool DAQS2Block::_CalFirstSeq(true);//is set inside particular calib-job routines
//
int16u DAQS2Block::format=0; // default format (raw)
//
//int16u DAQS2Block::nodeids[2*TOF2GC::SCCRAT]=//Data SC_node(SDR) ids(link#)(*2 due to 2 halfs) 
//  {
//    6,4,18,20, //  a-sides, crates 1->4 
//    7,5,19,21  //  b-sides, ...........
//  };
//
//int16u DAQS2Block::nodeidsP[4*TOF2GC::SCCRAT]=//OnBrdPeds SC_node(SDR) ids(*4 due to 2 halfs +prim+sec) 
//  {
//    266,270,274,278, //  a-sides, crates 1-4 
//    267,271,275,279,  //  b-sides, ........
//    268,272,276,280,  //  primary, ........
//    269,273,277,281  //  secondary ........
//  };
//
integer DAQS2Block::totbll=0;
//------------------------------
// functions for S-blocks class:
//
void DAQS2Block::node2crs(int16u nodeid, int16u &crat, int16u &sid){//called by me only
//  on input: slaveid=(blid&0x001F)
//  on output: crate=1-4, =0 if ID is not found; side=(1,2) -> (a,b)
  crat=atoi(DAQEvent::getportname(nodeid)+4)+1;//1-4, no validity check -> all is verified during VC's call to checkblockid
  switch(*(DAQEvent::getportname(nodeid)+5)){
    case 'A':
      sid=1;
      break;
    case 'B':
      sid=2;
      break;
    default:
      cout<<" ID-problem in DAQS2Block::node2crs, id="<<nodeid<<" crat="<<crat<<" A(B)="<<*(DAQEvent::getportname(nodeid)+5)<<endl;
      crat=0;
  }
}
//----
void DAQS2Block::node2crsP(int16u nodeid, int16u &crat, int16u &sid){//called by me only
//  on input: nodeid=(((VC's line)>>5)&(0x1FF)) !!! 
//  on output: crate=1-4, =0 if ID is not found; side=(1,2,3,4) -> (a,b,primary,secondary)
  crat=atoi(DAQEvent::getnodename(nodeid)+4)+1;//1-4, no validity check -> all is verified during VC's call to checkblockid
  switch(*(DAQEvent::getnodename(nodeid)+5)){
    case 'A':
      sid=1;
      break;
    case 'B':
      sid=2;
      break;
    case 'P':
      sid=3;
      break;
    case 'S':
      sid=4;
      break;
    default:
      cout<<" ID-problem in DAQS2Block::node2crsP, id="<<nodeid<<" crat="<<crat<<" A(B)="<<*(DAQEvent::getnodename(nodeid)+5)<<endl;
      //exit(1);
      return;
  }
}
//----
integer DAQS2Block::checkblockid(int16u blid){//SDR's ids as Ports
  int valid(0);
  char sstr[128];
  char side[3]="AB";
  char str[2];
//  cout<<"---> In DAQS2Block::checkblockid, blid(hex)="<<hex<<blid<<",addr:"<<dec<<(blid&(0x001F))<<endl;
  for(int i=0;i<TOF2GC::SCCRAT;i++){
    for(int j=0;j<2;j++){
      str[0]=side[j];
      str[1]='\0';
      sprintf(sstr,"SDR-%X%s",i,str);
      if(DAQEvent::ismynode(blid,sstr)){
        valid=10*(i+1)+j+1;
//cout<<"<--- valid="<<valid<<endl;
        return valid;
      }
    } 
  }
  return 0;
}
//----
integer DAQS2Block::getportid(int16u crat, int16u sdrs){
// on input crat=0,1,2,3;SDRside=0,1(A,B); return portid >=0, -1 if not found
  integer valid(-1);
  char sstr[128];
  char side[3]="AB";
  char str[2];
  if(crat<4 && sdrs<2){
    str[0]=side[sdrs];
    str[1]='\0';
    for(int16u i=0;i<32;i++){
      sprintf(sstr,"SDR-%X%s",crat,str);
      valid=DAQEvent::ismynode(i,sstr)?i:-1;
      if(valid>=0)break;
    }
  }
  return valid;
}
//----
integer DAQS2Block::checkblockidP(int16u blid){//SDR's ids as Nodes
// here blid is VC's raw "node+type" line from header !!!
  int valid(0);
  char sstr[128];
  char side[5]="ABPS";
  char str[2];
//  cout<<"---> In DAQS2Block::checkblockidP, blid(hex)="<<hex<<blid<<",addr:"<<dec<<((blid>>5)&(0x1FF))<<endl;
  for(int i=0;i<TOF2GC::SCCRAT;i++){
    for(int j=0;j<4;j++){
      str[0]=side[j];
      str[1]='\0';
      sprintf(sstr,"SDR-%X%s",i,str);
      if(DAQEvent::ismynode(blid,sstr)){
        valid=10*(i+1)+j+1;
//cout<<"<--- valid="<<valid<<endl;
        return valid;
      }
    } 
  }
  return 0;
}
//----
integer DAQS2Block::getnodeid(int16u crat, int16u sdrs){
// on input crat=0,1,2,3;SDRside=0,1,2,3(A,B,P,S); return nodeid >=0, -1 if not found
  int valid(-1);
  char sstr[128];
  char side[5]="ABPS";
  char str[2];
  if(crat<4 && sdrs<4){
    str[0]=side[sdrs];
    str[1]='\0';
    for(int16u i=0;i<512;i++){
      sprintf(sstr,"SDR-%X%s",crat,str);
      valid=DAQEvent::ismynode(i,sstr)?i:-1;
      if(valid>=0)break;
    }
  }
  return valid;
}
//-------------------------------------------------------
void DAQS2Block::buildraw(integer leng, int16u *p){
// it is implied that event_fragment is SDR2's one (checked in calling procedure)
// on input: len=tot_block_length as given at the beginning of block
//           *p=pointer_to_beggining_of_block_data(i.e. NEXT to len-word !!!) 
// Length counting does not includes length-word itself !!! 
//
  integer i,j,il,ib,is,ic,icn,nh,irl1=0,irl2=0,irs1=0,irs2=0;
  int swid,swidn=0,pmmx,hwid,hwidc,hwidt,hwidq[4],hwidtc;
  int16u sswid,sswidn,shwid;
  int16u pmt,mtyp,swch,bpnt,rdch,inch,slot=0,crat,slid,csid,val16,tsens,sslot;
  int16u blid,dtyp=0,naddr,datyp,len,lenraw(0),lencom(0),formt;
  int16u word,nword,nnword,bias,dlink,plink;
  int16u osbit,wtyp;
  int16u qchmsk,qlowchf;
  int16u biasmx;
  int16u n16wrds;
  int16u nhitmx;
  int16u crsd;
//
  int16u ltmoutf(0);//SFET/SFEA/SFEC(link) time-out flags (from raw-eos or from compressed-part)
  int16u sptcmdh(0);//spt_cmd_h (from raw-eos or from compressed-part)
  int16u wcount(0);//sequencer Word-counter(raw)
  int16u mwcount(0);//sequencer Word-counter(mix)
  int16u svermsk(0);//stat.verification mask in TrPatt/Stat-block of ComprFMT 
  geant ped,sig;
  int16u sta;
  static integer firstevs(0);
// for classic ped-run events or for DownScaled events
  bool TofPedCal(false);//Separate TofPedCal-job(ev-by-ev) using RawFMT(class/DownScaled mode)  
  bool AccPedCal(false);//Separate AccPedCal-job(ev-by-ev) using RawFMT(only fmt for AccQ)(class/DownScaled mode)  
  bool subtpedTof(false),subtpedAcc(false);
  bool DownScal(false);//tempor:  how to recognize it ???
// some static vars for debug:
  static integer fsterr1[SCFETA]={0,0,0,0,0};
  static integer err1(0);
//
  bool dataf,crcer,asser,amswer,timoer,fpower,seqer,cdpnod,noerr;
//
  int16u tdcbfn[SCFETA];//buff. counters for each TDC(link# 1-5)
  int16u tdcbfo[SCFETA];//TDC-buff OVFL FLAGS
  uinteger tdcbfh[SCFETA][8*SCTHMX2+4];//Raw-fmt TDC-buff,"+4" to count temper,header,error and trailer words
  uinteger wds2tdc,wttem,wthed,wterr,wttrl,tem1,tem2,htime;
  int16u nwtdcb,ntimbh;
//
  int16u *pr;
  integer bufpnt(0),lbbs;
  uinteger val32;
  geant temp,charge=0;
//
  bool tmout;
  int16u eoslenr(10);//length of end-of-segment record for raw format
  int16u sptcmdt;//SPT-reading command-type (i.e. format of prig-patt block, =0(err)/1/2/3)
  bool sptgen;//SPT test-generator bit setting
//
  uinteger swcbuf[2*SCRCMX][SCTHMX2];
// keep: 1st half->LT/FT/HT-time+ampl values for raw/compr. fmt; 2nd->for raw if mixed fmt   
  integer  swnbuf[2*SCRCMX],swibuf[2*SCRCMX],hwibuf[2*SCRCMX];
//keep:     LT/FT/HT hits#      swids=LBBSPM         hwids=CSSRR   for raw format
// these buffers will be filled with TOF/ANTI sw-channels hits in current crate as 
// arrays of sequential sw-channel (number of hits in each channel will be ovfl-protected !!!).
// static Tof2JobStat temperature-array is also filled;
// Also filled at TOF(ANTI)RawEvent objects creation the following static arrays:
// TOF2RawSide::FTtime[SCCRAT][SCFETA][SCTHMX1];//FT-channel time-hits(incl.ANTI)
// TOF2RawSide::FThits[SCCRAT][SCFETA];//number of FT-channel hits ....... 
// TOF2RawSide::SumHTt[SCCRAT][SCFETA-1][SCTHMX2];//TOF SumHT-channel time-hits
// TOF2RawSide::SumHTh[SCCRAT][SCFETA-1];// number of SumHT-channel time-hits 
// TOF2RawSide::SumSHTt[SCCRAT][SCFETA-1][TOF2GC::SCTHMX2];//TOF SumSHT-channel time-hits
// TOF2RawSide::SumSHTh[SCCRAT][SCFETA-1];// number of SumSHT-channel time-hits
//-----
  TOF2JobStat::daqsfr(0);//count entries
  p=p-1;//to go from VC-convention to my !!! Now it point to Segm.length word 
//  len=*p;//fragment's 1st word(length in bytes, not including length word itself)
  len=int16u(leng&(0xFFFFL));//fragment's length in 16b-words(not including length word itself)
  int icca=(leng>>30)&1;
//
  crsd=int16u((leng>>16)&(0x3FFFL));//CS(c=1-4,s=1-4=>a,b,p,s) as return by my "checkblockid"-1
  crat=(crsd+1)/10;
  csid=(crsd+1)%10;
// cout<<"<--- <<crat/side="<<crat<<" "<<csid<<" leng="<<hex<<leng<<dec<<endl;
  if(crat<=0 || crat>4 || csid<=0 || csid>4){//illegal crate/side
    TOF2JobStat::daqsfr(105);//count bad cr/sd entries
    if(err1++<100)cerr<<" <---- DAQS2Block::buildraw -E- Wrong Crate/Side="<<crat<<"/"<<csid<<endl;
    if(TFREFFKEY.reprtf[3]>0
                            && TFREFFKEY.reprtf[4]>0
                                                    ){//debug
      cout<<"DAQS2Block: -E- invalid crate|side, crate/side="<<crat<<" "<<csid<<" leng="<<hex<<leng<<dec<<endl;
    }
    goto BadExit;
  }
//
  if(csid==3)csid=1;//tempor
  else if(csid==4)csid=2;
  if(csid==1)TOF2JobStat::daqsfr(1+crat);//entries/crate, side=1
  if(csid==2)TOF2JobStat::daqsfr(100+crat);//entries/crate, side=2
//
  leng=leng &( (1<<30)-1);//just for later usage in bit-dump calls
  blid=*(p+len);// fragment's last word: Status+slaveID
//  cout<<"    blid="<<hex<<blid<<dec<<endl;
  dataf=((blid&(0x8000))>0);//data-fragment
  crcer=((blid&(0x4000))>0);//CRC-error
  asser=((blid&(0x2000))>0);//assembly-error
  amswer=((blid&(0x1000))>0);//amsw-error   
  timoer=((blid&(0x0800))>0);//timeout-error   
  fpower=((blid&(0x0400))>0);//FEpower-error   
  seqer=((blid&(0x0200))>0);//sequencer-error
  cdpnod=((blid&(0x0020))>0);//CDP-node(like SDR2-node with no futher fragmentation)
  naddr=(blid&(0x001F));//slaveID(="NodeAddr"=SDR_link#)
  datyp=((blid&(0x00C0))>>6);//(0-should not be),1,2,3
//
//
  if(TFREFFKEY.reprtf[3]>0 && TFREFFKEY.reprtf[4]>0){//debug
    cout<<"====> In DAQS2Block::buildraw: BlLen="<<*p<<" data-fmt:"<<datyp<<" slave_id:"<<naddr<<endl;
    cout<<"      BlLeng(in call)="<<(leng&(0xFFFFL))<<" data/crc_er/ass_er/amsw_er/tmout/FEpow_er/seq_er/eoffr/="<<
    dataf<<" "<<crcer<<" "<<asser<<" "<<amswer<<" "<<timoer<<" "<<fpower<<" "<<seqer<<" "<<cdpnod<<endl;
  }
//
  if(datyp==1)setrawf();
  else if(datyp==2)setcomf();
  else if(datyp==3)setmixf();
  formt=getformat();//0/1/2/3->raw/compr/mixt/onboard_pedcal_table
  TOF2JobStat::daqsfr(34+datyp);//<=== entries/dataformat(illeg,raw,com,mix) 
//
  if(datyp==0 || len==1){
    TOF2JobStat::daqsfr(5+crat);//counts illeg.fmt or empty
    if(TFREFFKEY.reprtf[3]>0)EventBitDump(leng,p,"Bad DataFormat | EmptyBlock bitDump:");//debug
    goto BadExit;
  }
//
  TOF2JobStat::daqsfr(9+crat);//<==== datyp+len_OK/crate
//
  if(dataf){
    if(crcer)TOF2JobStat::daqsfr(40+8*(crat-1));
    if(asser)TOF2JobStat::daqsfr(41+8*(crat-1));
    if(amswer)TOF2JobStat::daqsfr(42+8*(crat-1));
    if(timoer)TOF2JobStat::daqsfr(43+8*(crat-1));
    if(fpower)TOF2JobStat::daqsfr(44+8*(crat-1));
    if(seqer)TOF2JobStat::daqsfr(45+8*(crat-1));
    if(cdpnod)TOF2JobStat::daqsfr(46+8*(crat-1));
  }
  else{
    TOF2JobStat::daqsfr(21+crat);//count nonData
  }
//---------
  noerr=(dataf 
             && !crcer 
	               && !asser 
		                && !amswer 
                                          && !timoer 
					            && !fpower 
						              && !seqer 
							               && cdpnod);
  if(noerr){
    TOF2JobStat::daqsfr(29+crat);//<=== no errors 
  }     
  else{
    TOF2JobStat::daqsfr(25+crat);
    goto BadExit;
  }
//---------
  if(TFREFFKEY.relogic[0]==5 || TFREFFKEY.relogic[0]==6)TofPedCal=true;//TofPedCal-job(Class/DownScaled) requested
  if(ATREFFKEY.relogic==2 || ATREFFKEY.relogic==3)AccPedCal=true;//AccPedCal-job(Class/DownSc) requested 
  if((TofPedCal && formt>0) || (AccPedCal && formt>0)){//tempor
    if(firstevs==0)cout<<"<====== DAQS2Block::buildraw: Unproper format when class/ondata PedCal-job is requested !!!"<<endl;
    firstevs=1;
    return;
  }
//
//---------
  if(formt<=1)TOF2JobStat::daqscr(formt,crat-1,0);//count crate-entries with stand-alone  raw/comp format
  else if(formt==2){//mix.form
    TOF2JobStat::daqscr(0,crat-1,0);//count crate-entries with raw format
    TOF2JobStat::daqscr(1,crat-1,0);//count crate-entries with compr. format
  }
  else TOF2JobStat::daqscr(2,crat-1,0);//=3 -> count crate-entries with pedcal_table format
#pragma omp critical (tofbll)
  totbll+=len;//summing to have event(scint) data length
//
//-----
// reset buffers (here SCRCMX is used as max. number(even higher) of sw-channels)
  for(i=0;i<2*SCRCMX;i++){
    swnbuf[i]=0;
    swibuf[i]=0;
    hwibuf[i]=0;
    for(j=0;j<SCTHMX2;j++)swcbuf[i][j]=0;
  }
//
  if(formt==2)bufpnt=SCRCMX;//pointer to 2nd half of decoded buffer to store Raw format
// data if mixed format; if non-mixed format - decoded data are stored in 1st half of the buffer
//
  if(TFREFFKEY.reprtf[3]>0 && TFREFFKEY.reprtf[4]>0)EventBitDump(leng,p,"Event-by-event:");//debug
//-----------------------------------------------------------------------------------------------------
// !!!!!! Warning: Switching-Off of PedSuppression(for classic PedRun or DownScalled events) 
//  does not(?) automatically means usage of RAW-format (Format may still be Compressed(addr+value)???)
//-----------------------------------------------------------------------------------------------------
//
  if(formt==0 || formt==2){// <=========================  RawFMT is present - process it:
//bias_min=1 (pr+bias_min always points to 1st word of raw (sub-)segment) (i.e. 1st word of Q-block !!!)
// pr+bias_max always points to wcount-word (last word of raw (sub-)segment) 
    if(formt==0){//(0) raw alone
      pr=p;//points to length-word (then next word is beginning of Q-blk)
      biasmx=len-1;//bias_max (-1 to exclude blid words)
    }
    else{//(2) raw in mixt
      pr=p+1;//points to lenraw-word (next word is beginning of Q-blk)
      lenraw=*pr;
      biasmx=lenraw;//bias_max
//cout<<" MixtFMT:RawSubSegm length="<<lenraw<<endl;
    }
//cout<<" decoded RawSubSegm data will be put in glob.buffer starting from location="<<bufpnt<<endl; 
    wcount=*(pr+biasmx);//raw-subsegm words counter 
//cout<<" RawSubSegm_WordCounter(hex)="<<hex<<wcount<<dec<<" biasmx="<<biasmx<<endl;
    if((wcount&(0x8000))!=0){//raw-subsegm. is truncated
      TOF2JobStat::daqscr(0,crat-1,1);// RawForm max.length ovfl
    }
    wcount=(wcount&(0x7FF));//counts raw-subseg length=lenraw-evnum-itself(ovfl-bit removed)
//    cout<<"    wcount(dec)="<<wcount<<endl;  
    if(biasmx!=(wcount+1)){//<---length mismatch check,"+1" stay because wcount not incl. itself
      TOF2JobStat::daqscr(0,crat-1,2);// RawForm length error
      goto BadExit;    
    }
//
    for(i=0;i<SCFETA;i++){
      tdcbfn[i]=0;//clear TDC-buff counters
      tdcbfo[i]=0;//clear TDC-buff ovfl flags
    }
//
//-------------> start raw-subsegment processing:
//
    bias=1;//bias for RAW-pointer(i.e. to current rawdata-word, now 1st raw-DATA word (next to evnum))
    while(bias<=biasmx){//<--- loop over raw-subsegment words(excl. lenraw,evnum words)
      word=*(pr+bias);
//      cout<<" --> word/bias="<<hex<<word<<dec<<" "<<bias<<endl;
//-----   
      if(bias<=90){//<================= charge-words reading(incl the ones in SFET/SFEA !!!)
	dlink=(word&(0xF000))>>12;//decoded link# (0,1,..,8)
	slid=dlink;
	plink=(bias-1)%9;//"in block" position defined link# (0,1,...8)
	if(dlink!=plink || slid>8){
          if(TFREFFKEY.reprtf[3]>0){//err-debug
	    cout<<"DAQS2Block::RawFmtError:link<->position mismatch, crate/dlink/plink="<<crat<<" "<<dlink<<" "<<plink<<endl;
          }
          TOF2JobStat::daqscr(0,crat-1,3);//charge_link <-> position mismatch
	  goto BadExit;    
	}
	val16=word&(0x0FFF);// charge value (=0, if link problem)
	slot=AMSSCIds::crdid2sl(crat-1,slid)+1;//slot-id to abs.slot-number(solid,sequential, 1,...,11)
	if(slot<=0 || slot==1 || slot==4 || slot>11){//check slot# validity
          if(TFREFFKEY.reprtf[3]>0){//err-debug
	    cout<<"DAQS2Block::RawFmtError:NotChargeSlotNumber , crat/slot_id/slot="<<crat<<" "<<slid<<" "<<slot<<endl;
	  }
	  TOF2JobStat::daqscr(0,crat-1,4);//invalid slot number
	  goto BadExit;    
	}
	TOF2JobStat::daqssl(0,crat-1,slot-1,0);//count legal charge-slots
	inch=int16u(floor(float(bias-1)/9)+1);// slot's input channel(1,...10)
	sslot=AMSSCIds::sl2qsid(slot-1);//seq.numbering of Q-measuting slots(1-9=>,4xSFET,1xSFEA,4xSFEC)
        mtyp=1;//charge
	rdch=AMSSCIds::ich2rdch(crat-1,slot-1,inch-1,mtyp);//outp.ch:0(if empty),1,2...
	if(rdch>0 && val16>0){//<-- valid, not empty channel
//	  TOF2JobStat::daqsch(0,crat-1,slot-1,rdch-1,0);//count charge channels entries
	  TOF2JobStat::daqsch(0,crat-1,slot-1,inch-1,0);//count charge channels entries
          AMSSCIds scinid(crat-1,slot-1,rdch-1);
//          mtyp=scinid.getmtyp();//0->LTtime, 1->Q, 2->FT, 3->sumHT, 4->sumSHT
//          pmt=scinid.getpmt();//0->anode, 1-3->dynode#
          swch=scinid.getswch();//seq. sw-channel (0,1,...)
	  swid=scinid.getswid();
	  hwid=scinid.gethwid();
	  bpnt=bufpnt+swch;//swch true address in the buffer(buff.half directed)
	  if(swnbuf[bpnt]<1){
	    swcbuf[bpnt][swnbuf[bpnt]]=val16;//store value
	    swibuf[bpnt]=swid;
	    hwidc=1000*crat+100*sslot+inch;//non-standard hwid !!!(Crat(1-4)|Sslot(1-9)|Ich(1-10))
	    hwibuf[bpnt]=hwidc;
	    swnbuf[bpnt]+=1;//increase counter of given swch
	  }
	  else{//should not happen, normally 1hit/channel
            if(TFREFFKEY.reprtf[3]>0){//err-debug
	      cout<<"DAQS2Block::RawFmtError:ChargeHits > 1 for hwid/swid="<<hwid<<" "<<swid<<endl;
            }
	  }
	}//--->endof empty chan. check
	bias+=1;
      }//--->endof charge words reading
//-----
      else if(bias>90 && bias<=94){//<===== trig.patt words reading
//        cout<<"  TP-decoding:"<<endl;
        ltmoutf=*(pr+biasmx-eoslenr+1);//links time_out_flags word from end_of_segment(EOS) block
	tmout=((ltmoutf&(0x0004))!=0);//if true -> found timeout
        sptcmdh=*(pr+biasmx-eoslenr+2);//SPT_CMD_H word from EOS
	if((sptcmdh&(0x1000))!=(0x1000))sptcmdt=0;//error(not read-mode !)
	else if((sptcmdh&(0xFFF))==(0x400))sptcmdt=1;//not masked
	else if((sptcmdh&(0xFFF))==(0x405))sptcmdt=2;//CP-masked
	else if((sptcmdh&(0xFFF))==(0x40A))sptcmdt=3;//CT-masked (rubbish)
	else sptcmdt=0;//error
        for(i=0;i<4;i++){//check presence of TrPat-type info
          word=*(pr+bias+i);
	  if((word&(0xE000))!=0){//3 msb should be 0
	    TOF2JobStat::daqscr(0,crat-1,5);//notTrPat word in TrPat-area: fatal
            goto BadExit;
	  }
	}
	if(tmout || sptcmdt==0){
	  TOF2JobStat::daqscr(0,crat-1,6);//time-out or wrong format according to EOS-block
	  goto SkipTPpr;//skip TP-processing
	}
//decode 1st pair of trpatt-words(HT)"
        word=*(pr+bias);//msbits
        nword=*(pr+bias+1);//lsbits
	val32=uinteger(nword&(0x03FF));//HT-trigpatt least sign.bits
	val32|=(uinteger(word&(0x03FF))<<10);//add HT-trigpatt most sign.bits
//	cout<<"   1st 16bits paire was combined into:"<<hex<<val32<<dec<<endl;
	for(i=0;i<20;i++){//HT-trigpatt bits loop
	  lbbs=AMSSCIds::gettpbas(crat-1,i);
	  if(lbbs>0){//valid lbbs for given bit
	    is=lbbs%10;//plane side(1,2)
	    ib=(lbbs/10)%100;//paddle(1-8(10))
	    il=lbbs/1000;//plane(1-4)
	    if(i<=9){//use any of 10 lsbits to save il/is of "1st" layer(side) contributing to given crate
	      irl1=il;//these L/S corresponds to "nword"(lsbits)
	      irs1=is;
	    } 
	    if(i>=10){//use any of 10 msbits to save il/is of "2nd" layer(side) contributing to given crate
	      irl2=il;//these L/S corresponds to "word"(msbits)
	      irs2=is;
	    }
	    if((val32&(1<<i))>0)
	      TOF2RawSide::addtpb(il-1,ib-1,is-1);//set bit in RawSide's TPs(reseted in retof2initevent() !
	  }
	}
//set s1/s2 32-bits of TP[ilay] for "1st" layer(side), i.e. the one extracted from "nword"=>lsbits:
	sptgen=((nword&(0x0400))!=0);//true, if generator bit was set
	if(sptgen)TOF2RawSide::addtpb(irl1-1,14,irs1-1);//set as fictitious paddle-15
	osbit=(nword&(0x0800));//other card-side CP-bit(11) of 1st layer(side)(ICP0)
	if(osbit>0)TOF2RawSide::addtpb(irl1-1,13,irs1-1);//set as fictitious paddle-14
	osbit=(nword&(0x1000));//other card-side CT-bit(12) of 1st layer(side)(ICT0)(OR of CP0-bits if masked fmt)
	if(osbit>0)TOF2RawSide::addtpb(irl1-1,12,irs1-1);//set as fictitious paddle-13
        if(sptcmdt>0)TOF2RawSide::addtpb(irl1-1,11,irs1-1);//if masked, set as fictitious paddle-12
	
//set s1/s2 32-bits of TP[ilay] for "2nd" layer(side), i.e. the one extracted from "word"=>msbits:	
	sptgen=((word&(0x0400))!=0);//true, if generator bit was set
	if(sptgen)TOF2RawSide::addtpb(irl2-1,14,irs2-1);//set as fictitious paddle-15
	osbit=(word&(0x0800));//other card-side CP-bit of 2nd layer(side)(ICP1)
	if(osbit>0)TOF2RawSide::addtpb(irl2-1,13,irs2-1);//set as fictitious paddle-14
	osbit=(word&(0x1000));//other card-side CT-bit of 2nd layer(side)(ICT1)(OR of CP1-bits if masked fmt)
	if(osbit>0)TOF2RawSide::addtpb(irl2-1,12,irs2-1);//set as fictitious paddle-13
        if(sptcmdt>0)TOF2RawSide::addtpb(irl2-1,11,irs2-1);//if masked, set as fictitious paddle-12
//decode 2nd pair of trpatt-words(SHT):
        word=*(pr+bias+2);
        nword=*(pr+bias+3);
	val32=uinteger(nword&(0x03FF));//SHT-trigpatt least sign.bits
	val32|=(uinteger(word&(0x03FF))<<10);//add SHT-trigpatt most sign.bits
//	cout<<"   2nd 16bits paire was combined into:"<<hex<<val32<<dec<<endl;
	for(i=0;i<20;i++){//SHT-trigpatt bits loop
	  lbbs=AMSSCIds::gettpbas(crat-1,i);
	  if(lbbs>0){
	    is=lbbs%10;//plane side(1,2)
	    ib=(lbbs/10)%100;//paddle(1-8(10))
	    il=lbbs/1000;//plane(1-4)
	    if(i<=9){//save il/is of 1st layer(side) contributing to given crate
	      irl1=il;//these L/S corresponds to "nword"(lsbits)
	      irs1=is;
	    } 
	    if(i>=10){//save il/is of 2nd layer(side) contributing to given crate
	      irl2=il;//these L/S corresponds to "word"(msbits)
	      irs2=is;
	    }
	    if((val32&(1<<i))>0)TOF2RawSide::addtpzb(il-1,ib-1,is-1);//SHT trigpatt bits setting
	  }
	}
//set s1/s2 32-bits of TP[ilay] for "1st" layer(side), i.e. the one extracted from "nword"=>lsbits:
	sptgen=((nword&(0x0400))!=0);//true, if generator bit was set
	if(sptgen)TOF2RawSide::addtpzb(irl1-1,14,irs1-1);//set as fictitious paddle-15
	osbit=(nword&(0x0800));//other card-side BZ-bit(11) of 1st layer(side)(IBZ0)
	if(osbit>0)TOF2RawSide::addtpb(irl1-1,13,irs1-1);//set as fictitious paddle-14
	osbit=(nword&(0x1000));//THIS card-side BZ-bits OR of 1st layer(side)(BZ0, if masked fmt)
	if(osbit>0)TOF2RawSide::addtpzb(irl1-1,12,irs1-1);//set as fictitious paddle-13
        if(sptcmdt>0)TOF2RawSide::addtpzb(irl1-1,11,irs1-1);//if masked, set as fictitious paddle-12
	
//set s1/s2 32-bits of TP[ilay] for "2nd" layer(side), i.e. the one extracted from "word"=>msbits:	
	sptgen=((word&(0x0400))!=0);//true, if generator bit was set
	if(sptgen)TOF2RawSide::addtpzb(irl2-1,14,irs2-1);//set as fictitious paddle-15
	osbit=(word&(0x0800));//other card-side BZ-bit of 2nd layer(side)(IBZ1)
	if(osbit>0)TOF2RawSide::addtpb(irl2-1,13,irs2-1);//set as fictitious paddle-14
	osbit=(word&(0x1000));//THIS card-side BZ-bits OR of 2nd layer(side)(BZ1, if masked fmt)
	if(osbit>0)TOF2RawSide::addtpzb(irl2-1,12,irs2-1);//set as fictitious paddle-13
        if(sptcmdt>0)TOF2RawSide::addtpzb(irl2-1,11,irs2-1);//if masked, set as fictitious paddle-12
//
SkipTPpr:
        bias+=4;
      }//--->endof tr.pat.words reading
//-----
      else if(bias>94 && bias<=(biasmx-eoslenr)){//<===== time(+temper) words reading
//        cout<<"  Temp+Time-decoding:"<<endl;
        nword=*(pr+bias+1);
	dlink=(word&(0xF00))>>8;//decoded link# (0,...4)
	wtyp=((word&(0xF000))>>12);
	slid=dlink;//0,1,2,3,4 => SFET_0,_1,_2,_3,SFEA
	if(slid>4){
          if(TFREFFKEY.reprtf[3]>0){//err-debug
	    cout<<"DAQS2Block::RawFmt_TimeBlErr: invalid link, crate/dlink="<<crat<<" "<<dlink<<endl;
          }
          TOF2JobStat::daqscr(0,crat-1,7);//invalid link number
	  goto BadExit;    
        }
	slot=AMSSCIds::crdid2sl(crat-1,slid)+1;//slot-id to slot-number(solid,sequential, 1,...,11)
	if(slot<=0 || slot==1 || slot==4 || slot>7){//check slot# validity
          if(TFREFFKEY.reprtf[3]>0){//err-debug
	    cout<<"DAQS2Block::RawFmt_TimeBlErr: invalid slot, crat/slot_id/slot="<<crat<<" "<<slid<<" "<<slot<<endl;
          }
	  TOF2JobStat::daqscr(0,crat-1,8);//invalid slot number
	  goto BadExit;    
	}
	if(wtyp==8 || wtyp==2 || wtyp==3 || wtyp==6 ||wtyp==4){//time(+temper) wtype ?
	  TOF2JobStat::daqssl(0,crat-1,slot-1,1);//count TDC-buff entries(pairs of 16bits words)
//if(crat==3 && slid==4)cout<<" -----> FillBuf:cr3sfea..."<<" nwbuf="<<tdcbfn[slid]<<" w="<<hex<<word<<" "<<
//                                                                    nword<<dec<<endl;
	  if(tdcbfn[slid]<(8*SCTHMX2+4)){//fill TDC-buffer
//if(crat==3 && slid==4)cout<<"    Filled Wtyp="<<wtyp<<endl;
	    val32=(uinteger(word)<<16);//msb
	    val32|=uinteger(nword);//lsb
	    tdcbfh[slid][tdcbfn[slid]]=val32;//store 32-bits word in TDC-buff
	    tdcbfn[slid]+=1;
	  }
	  else{//TDC-buff overflow
            if(TFREFFKEY.reprtf[3]>0){//err-debug
	      cout<<"DAQS2Block::RawFmt_TimeErr: internal TDC-buff Ovfl, crate/link="<<crat<<" "<<slid<<endl;
              cout<<" ovffl="<<tdcbfo[slid]<<" cr/sl/bufcounter="<<crat<<" "<<slid<<" "<<tdcbfn[slid]<<endl;
            }
	    if(tdcbfo[slid]==0)TOF2JobStat::daqssl(0,crat-1,slot-1,2);//counts Events with TDC-buff ovfl
	    tdcbfo[slid]=1;
	  }
	}
	else{
          if(TFREFFKEY.reprtf[3]>0){//err-debug
	    cout<<"DAQS2Block::RawFmt_TimeErr: InvalidWordType, crate/link/wtyp="<<crat<<" "<<slid<<" "<<wtyp<<endl;
          }
	  TOF2JobStat::daqssl(0,crat-1,slot-1,3);//count  invalid wtyp
	  goto BadExit;    
	}
	bias+=2;
      }//--->endof time(+temper)-words reading block
//-----
      else{//<-- SeqStatusBlock words reading
        ltmoutf=word;//link time-out flags
//cout<<"RawSubSegm:SeqStatusBlock: timeout_flags word="<<hex<<ltmoutf<<" wcount_word="
//                                  <<*(pr+bias+eoslenr-1)<<dec<<" bias="<<bias<<endl;
        bias+=eoslenr;//to skip the rest of eos-record;
      }
//------
    }//--->endof raw-subsegment words reading loop
//-------------> now start TDC-buffers inspection and decoding:
    for(slid=1;slid<=SCFETA;slid++){//<---loop over TDC-buffers
      wttem=0;
      wthed=0;
      wterr=0;
      wttrl=0;
      wds2tdc=0;
      tdcbfo[slid-1]=0;
      nwtdcb=tdcbfn[slid-1];//number of filled 32b-words in TDC(slid)
//  if(crat==3 && slid==5)cout<<" -->ReadBuf: n32w="<<nwtdcb<<endl;
      if(nwtdcb==0)continue;//empty buffer->skip it
      wttrl=((tdcbfh[slid-1][nwtdcb-1]&(0xF0000000L))>>28);//wtyp of last word
      if(wttrl!=3){
        TOF2JobStat::daqssl(0,crat-1,slot-1,4);//count links with no trailer - bad
	continue;//next slot
      }
      if(nwtdcb==1){
        TOF2JobStat::daqssl(0,crat-1,slot-1,5);//trailer alone - bad
	continue;//next slot
      }
// now we have situation: trailer + something (i.e. >= 2words):
      wttem=((tdcbfh[slid-1][0]&(0xF0000000L))>>28);//wtyp of 1st word
      wthed=((tdcbfh[slid-1][1]&(0xF0000000L))>>28);//wtyp of 2nd word
      wterr=((tdcbfh[slid-1][nwtdcb-2]&(0xF0000000L))>>28);//wtyp of prev. to last word
      wds2tdc=(tdcbfh[slid-1][nwtdcb-1]&(0xFFFL));//nwords given by trailer(last word)
//if(crat==3 && slid==5)cout<<" wttem="<<wttem<<" wthed="<<wthed<<" wterr="<<wterr<<"nw_fr_trl="<<wds2tdc<<endl;  
      if(slid<=2)tmout=((ltmoutf&(1<<(slid-1)))>0);//time-out flag from eos
      else tmout=((ltmoutf&(1<<slid))>0);//to skip already checked SPT-timeout bit (bit2 starting from 0)
      slot=AMSSCIds::crdid2sl(crat-1,slid-1)+1;//slot-id to slot-number(solid,sequential, 1,...,11)
      sslot=AMSSCIds::sl2tsid(slot-1);//seq.numbering of T-measuting slots(1-5=>,4xSFET,1xSFEA)
      if(wttem!=8 || wthed!=2 || tmout || nwtdcb!=(wds2tdc+1)){//trailer present, but broken structure
#ifdef __AMSDEBUG__
        if(crat==3 && slid==5)cout<<"slid="<<slid<<" wttem/wthed="<<wttem<<" "<<wthed<<" tmout="<<tmout
	                 <<" wterr="<<wterr<<" nwtdcb/wds2tdc="<<nwtdcb<<" "<<wds2tdc<<endl;
#endif
        TOF2JobStat::daqssl(0,crat-1,slot-1,6);//count links with broken struct
	if(wttem!=8)TOF2JobStat::daqssl(0,crat-1,slot-1,7);//case1:no temp
	if(wthed!=2)TOF2JobStat::daqssl(0,crat-1,slot-1,8);//case2:no Head
	if(tmout)TOF2JobStat::daqssl(0,crat-1,slot-1,9);//case3:TimeOut
        if(wterr==6)TOF2JobStat::daqssl(0,crat-1,slot-1,10);//case4: Err
#pragma omp critical (daqs2block)
{
	if(fsterr1[slot-1]==0){
	  fsterr1[slot-1]=1;
          if(TFREFFKEY.reprtf[3]>0){//err-debug
	    cout<<"  ---> TofDecoding:BadTimeBlockStructure/timeout in Run/evnt="<<AMSEvent::gethead()->getrun()
	                                                 <<" "<<AMSEvent::gethead()->getid()<<endl;
	    cout<<"       cr/sl:"<<crat<<" "<<sslot<<" wttem/wthed/wterr(8/2/6)="<<wttem
	                           <<" "<<wthed<<" "<<wterr<<" wds/trwds="<<nwtdcb<<" "<<wds2tdc<<endl;
	    cout<<"---------------------------------------------------------"<<endl;
	    EventBitDump(leng,p,"BadTimeBlockStructure/timeout !!!");
	  }
	}
}
	continue;//skip link(TDC) with broken structure (or time-out)
      }
      TOF2JobStat::daqssl(0,crat-1,slot-1,11);//count good links
//
      val32=tdcbfh[slid-1][0];//lst word(temperature)
      tem1=((val32&(0xFFF000L))>>12);
      tem2=(val32&(0xFFFL));
      temp=0;
      if(tem2>0)temp=235-400*geant(tem1)/geant(tem2);//Cels.degree
      tsens=AMSSCIds::sl2tsid(slot-1);//seq.slot#->temp.sensor#; 1,2,3,4,5
      TOF2JobStat::puttemp(crat-1,tsens-1,temp);
//
      val32=tdcbfh[slid-1][1];//2nd word(header)
      val32=tdcbfh[slid-1][nwtdcb-1];//last word(trailer)
      ntimbh=nwtdcb-3;//number of time-hits(all channels in current TDC)
      for(int ih=0;ih<ntimbh;ih++){//<---loop over all time-hits
        val32=tdcbfh[slid-1][ih+2];//time-word
	inch=int16u((val32&(0xE00000L))>>21);//inp.channel#(0-7)
	htime=((val32&(0x7FFFFL))<<2);//19 msb
	htime|=((val32&(0x180000L))>>19);//add 2 lsb
	mtyp=0;//"0" for ich2rdch-routine means "any Time-ch like LT,FT,sHT,sSHT"
	rdch=AMSSCIds::ich2rdch(crat-1,slot-1,inch,mtyp);//outp.ch:0(if empty),1,2...
	if(rdch>0){//<-- not empty channel
          AMSSCIds scinid(crat-1,slot-1,rdch-1);//
//	  TOF2JobStat::daqsch(0,crat-1,slot-1,rdch-1,1);//counts time outp.channels entries
	  TOF2JobStat::daqsch(0,crat-1,slot-1,inch,1);//counts time inp.channels entries
          swch=scinid.getswch();//seq. sw-channel (0,1,...)
	  swid=scinid.getswid();//LBBSPM
          mtyp=swid%10;//phys.bars Time/Charge:0/1, fict.extra.bars FT/sumHT/sumSHT-times:2/3/4
	  if(mtyp==0)nhitmx=SCTHMX3;
	  else if(mtyp==1)nhitmx=1;
	  else if(mtyp==2)nhitmx=SCTHMX1;
	  else nhitmx=SCTHMX2;
	  bpnt=bufpnt+swch;//swch true address in the buffer(buff.half directed)
	  hwid=scinid.gethwid();
	  if(swnbuf[bpnt]<nhitmx){
	    swcbuf[bpnt][swnbuf[bpnt]]=htime;//store time-value
	    swibuf[bpnt]=swid;
	    hwidc=1000*crat+100*sslot+inch+1;//non-standard hwid !!!(Crat(1-4)|Sslot(1-5)|Ich(1-8))
	    hwibuf[bpnt]=hwidc;
	    swnbuf[bpnt]+=1;//increase counter of given swch
	  }
	  else{
	    if(tdcbfo[slid-1]==0){//message on the 1st overflow-hit only
              if(TFREFFKEY.reprtf[3]>0){//err-debug
	        cout<<"DAQS2Block::RawFmt_Error:SWCH_Buf_Ovfl, hwid/swid="<<hwid<<" "<<swid<<
		                   " mtyp/nhits: "<<mtyp<<" "<<swnbuf[bpnt]<<" InpCh="<<inch<<endl;
	      }
	    }
	    tdcbfo[slid-1]=1;
//	    TOF2JobStat::daqsch(0,crat-1,slot-1,rdch-1,2);//count Nhit overflows in time channels
	    TOF2JobStat::daqsch(0,crat-1,slot-1,inch,2);//count Nhit overflows in time channels
	  }
	}//--->endof "rdch>0" check
      }//--->endof time-hits-loop
    }//--->endof TDC-buffers loop
//------
//cout<<"*******> endof RawFMT-decoding !"<<endl;
  }//=========================> endof "RawFMT presence check/processing
//
//
//============================> ComprFMT processing (if present) :
//                     (always 1st half of the glob.buffer will be filled)
//
  if(formt>0){//<========== Mixt/ComprFMT check(pedcalFormat(3) is already processed)
//
    int16u *pc;
    int16u *pss;//sub-sect pointer (pss+bias always points2 1st word (when bias=1)of sub-sector)
    int16u nnzch,nthts,tlhead,tlerrf(0);
    int16u nqwrds(0),ntwrds(0);
//
    if(formt==2){//<============= ComprFMT is inside of MixtFMT
      TOF2JobStat::daqscr(1,crat-1,1);//ComprFMT entries inside MixedFMT(after RawFMT ok)
      pc=p+1+lenraw;//points to last raw-subsegm. word(wcount)
      if((len-2-lenraw)<=0){//this is compr-subsection length 
        TOF2JobStat::daqscr(1,crat-1,2);//count truncated_segments cases
	goto BadExit;    
      }
      lencom=len-2-lenraw;//tot.length of compr.subgegment("-2" to excl. lenraw,blid words)
      nqwrds=*(pc+8)+1;//q-group words, "+1"->nwords-word itself
      ntwrds=(*(pc+8+nqwrds)&(0x3FFF))+1;//t-group wordfs,......
      if(lencom!=(7+nqwrds+ntwrds)){
        if(TFREFFKEY.reprtf[3]>0){//err-debug
          cout<<" -----> CompInMixFMT::LengthMismatch: lenraw/lencom="<<lenraw<<" "<<lencom
                                                       <<" nQwrds/nTwrds="<<nqwrds<<" "<<ntwrds<<endl;
	}
        TOF2JobStat::daqscr(1,crat-1,4);//count length mismatch inside ComprSection(inside mixFMT
	goto BadExit;    
      }
    }
    else{//<=============== ComprFMT is stand-alone
      pc=p;//points to 1st word of stand-alone ComprFMT segm (segm.length)
      lencom=len-1;//tot.length of compressed subsegment(excl. (Stat+SlaveId)- word)
      nqwrds=*(pc+8)+1;//q-group words, "+1"->nwords-word itself
      ntwrds=(*(pc+8+nqwrds)&(0x3FFF))+1;//t-group words,......
      if(lencom!=(7+nqwrds+ntwrds)){
        if(TFREFFKEY.reprtf[3]>0){//err-debug
          cout<<" -----> CompAloneFMT::LengthMismatch: lencom="<<lencom
                                                <<" nQwrds/nTwrds="<<nqwrds<<" "<<ntwrds<<endl;
	}
        TOF2JobStat::daqscr(1,crat-1,5);//count length mismatch inside ComprFMT
	goto BadExit;    
      }
    }
//                                                                        
//
//<==================== TrPatt/Status section:
if(TFREFFKEY.reprtf[3]>0 && TFREFFKEY.reprtf[4]>0)cout<<"   ---> ComprSegment::TrPatt/Status-decoding:"<<endl;
        pss=pc;
	bias=1;//pss+bias points2 1st word of TrPatt-section
        ltmoutf=*(pss+bias+4);//links time_out_flags word from Kunin's Status sub-section
	tmout=((ltmoutf&(0x0004))!=0);//if true-> found timeout
        for(i=0;i<4;i++){//check presence of TrPat-type info
          word=*(pss+bias+i);
	  sptcmdt=(word&(0xE000));
	  if(sptcmdt!=0 && sptcmdt!=(0x4000) && sptcmdt!=(0x8000)){//3 msb should be 0 or 0x4000 or 0x8000
	    TOF2JobStat::daqscr(1,crat-1,6);//notTrPat word in TrPat-area: fatal
            goto BadExit;
	  }
	}
	sptcmdt=((*(pss+bias))&(0xC000))>>14;//0/1/2->Not/CP(BZ)_masked/CT(BZ)_masked
	if(tmout){
	  TOF2JobStat::daqscr(1,crat-1,7);//time-out
	  goto SkipTPpr1;//skip TP-processing
	}
//decode 1st pair of trpatt-words(HT)"
        word=*(pss+bias);//msbits
        nword=*(pss+bias+1);//lsbits
	val32=uinteger(nword&(0x03FF));//HT-trigpatt least sign.bits
	val32|=(uinteger(word&(0x03FF))<<10);//add HT-trigpatt most sign.bits
if(TFREFFKEY.reprtf[3]>0 && TFREFFKEY.reprtf[4]>0)cout<<"   1st 16bits paire was combined into:"<<hex<<val32<<dec<<endl;
	for(i=0;i<20;i++){//HT-trigpatt bits loop
	  lbbs=AMSSCIds::gettpbas(crat-1,i);
	  if(lbbs>0){//valid lbbs for given bit
	    is=lbbs%10;//plane side(1,2)
	    ib=(lbbs/10)%100;//paddle(1-8(10))
	    il=lbbs/1000;//plane(1-4)
	    if(i<=9){//use any of 10 lsbits to save il/is of "1st" layer(side) contributing to given crate
	      irl1=il;//these L/S corresponds to "nword"(lsbits)
	      irs1=is;
	    } 
	    if(i>=10){//use any of 10 msbits to save il/is of "2nd" layer(side) contributing to given crate
	      irl2=il;//these L/S corresponds to "word"(msbits)
	      irs2=is;
	    }
	    if((val32&(1<<i))>0)
	      TOF2RawSide::addtpb(il-1,ib-1,is-1);//set bit in RawSide's TPs(reseted in retof2initevent() !
	  }
	}
//set s1/s2 32-bits of TP[ilay] for "1st" layer(side), i.e. the one extracted from "nword"=>lsbits:
	sptgen=((nword&(0x0400))!=0);//true, if generator bit was set
	if(sptgen)TOF2RawSide::addtpb(irl1-1,14,irs1-1);//set as fictitious paddle-15
	osbit=(nword&(0x0800));//other card-side CP-bit of 1st layer(side)(ICP0)
	if(osbit>0)TOF2RawSide::addtpb(irl1-1,13,irs1-1);//set as fictitious paddle-14
	osbit=(nword&(0x1000));//other card-side CT-bit of 1st layer(side)(ICT0)(OR of CP0-bits if masked fmt)
	if(osbit>0)TOF2RawSide::addtpb(irl1-1,12,irs1-1);//set as fictitious paddle-13
        if(sptcmdt>0)TOF2RawSide::addtpb(irl1-1,11,irs1-1);//if masked, set as fictitious paddle-12
	
//set s1/s2 32-bits of TP[ilay] for "2nd" layer(side), i.e. the one extracted from "word"=>msbits:	
	sptgen=((word&(0x0400))!=0);//true, if generator bit was set
	if(sptgen)TOF2RawSide::addtpb(irl2-1,14,irs2-1);//set as fictitious paddle-15
	osbit=(word&(0x0800));//other card-side CP-bit of 2nd layer(side)(ICP1)
	if(osbit>0)TOF2RawSide::addtpb(irl2-1,13,irs2-1);//set as fictitious paddle-14
	osbit=(word&(0x1000));//other card-side CT-bit of 2nd layer(side)(ICT1)(OR of CP1-bits if masked fmt)
	if(osbit>0)TOF2RawSide::addtpb(irl2-1,12,irs2-1);//set as fictitious paddle-13
        if(sptcmdt>0)TOF2RawSide::addtpb(irl2-1,11,irs2-1);//if masked, set as fictitious paddle-12
//decode 2nd pair of trpatt-words(SHT):
        word=*(pss+bias+2);
        nword=*(pss+bias+3);
	val32=uinteger(nword&(0x03FF));//SHT-trigpatt least sign.bits
	val32|=(uinteger(word&(0x03FF))<<10);//add SHT-trigpatt most sign.bits
if(TFREFFKEY.reprtf[3]>0 && TFREFFKEY.reprtf[4]>0)cout<<"   2nd 16bits paire was combined into:"<<hex<<val32<<dec<<endl;
	for(i=0;i<20;i++){//SHT-trigpatt bits loop
	  lbbs=AMSSCIds::gettpbas(crat-1,i);
	  if(lbbs>0){
	    is=lbbs%10;//plane side(1,2)
	    ib=(lbbs/10)%100;//paddle(1-8(10))
	    il=lbbs/1000;//plane(1-4)
	    if(i<=9){//save il/is of 1st layer(side) contributing to given crate
	      irl1=il;//these L/S corresponds to "nword"(lsbits)
	      irs1=is;
	    } 
	    if(i>=10){//save il/is of 2nd layer(side) contributing to given crate
	      irl2=il;//these L/S corresponds to "word"(msbits)
	      irs2=is;
	    }
	    if((val32&(1<<i))>0)TOF2RawSide::addtpzb(il-1,ib-1,is-1);//SHT trigpatt bits setting
	  }
	}
//set s1/s2 32-bits of TP[ilay] for "1st" layer(side), i.e. the one extracted from "nword"=>lsbits:
	sptgen=((nword&(0x0400))!=0);//true, if generator bit was set
	if(sptgen)TOF2RawSide::addtpzb(irl1-1,14,irs1-1);//set as fictitious paddle-15
	osbit=(nword&(0x0800));//other card-side BZ-bit(11) of 1st layer(side)(IBZ0)
	if(osbit>0)TOF2RawSide::addtpb(irl1-1,13,irs1-1);//set as fictitious paddle-14
	osbit=(nword&(0x1000));//THIS card-side BZ-bits OR of 1st layer(side)(BZ0, if masked fmt)
	if(osbit>0)TOF2RawSide::addtpzb(irl1-1,12,irs1-1);//set as fictitious paddle-13
        if(sptcmdt>0)TOF2RawSide::addtpzb(irl1-1,11,irs1-1);//if masked, set as fictitious paddle-12
	
//set s1/s2 32-bits of TP[ilay] for "2nd" layer(side), i.e. the one extracted from "word"=>msbits:	
	sptgen=((word&(0x0400))!=0);//true, if generator bit was set
	if(sptgen)TOF2RawSide::addtpzb(irl2-1,14,irs2-1);//set as fictitious paddle-15
	osbit=(word&(0x0800));//other card-side BZ-bit of 2nd layer(side)(IBZ1)
	if(osbit>0)TOF2RawSide::addtpb(irl2-1,13,irs2-1);//set as fictitious paddle-14
	osbit=(word&(0x1000));//THIS card-side BZ-bits OR of 2nd layer(side)(BZ1, if masked fmt)
	if(osbit>0)TOF2RawSide::addtpzb(irl2-1,12,irs2-1);//set as fictitious paddle-13
        if(sptcmdt>0)TOF2RawSide::addtpzb(irl2-1,11,irs2-1);//if masked, set as fictitious paddle-12
//
SkipTPpr1:
// decode status part:
        ltmoutf=*(pss+bias+4);//link's time-out flags(sequencer Timout flag as in rawFMT)
	mwcount=*(pss+bias+5);//sequencer Counter (as in rawFMT)
        if((mwcount&(0x8000))!=0){//mix-subsegm. is truncated
          TOF2JobStat::daqscr(1,crat-1,3);// subsegment truncated
        }
        mwcount=(mwcount&(0x7FF));//counts raw-subseg length=lenraw-evnum-itself(ovfl-bit removed)
	svermsk=*(pss+bias+6);//status verif.mask
	if((svermsk&(0x3FF))>0){//problems in st.verif.mask
	  TOF2JobStat::daqscr(1,crat-1,8);//StatVerifMask problems
//          goto BadExit;//tempor
	}
//
        pss+=7;//shift by 7 words, now pss points to last(7th) word of TrPatt-section
//=========>endof TrPatt/Status section
//
//<====================== Charge-section:
if(TFREFFKEY.reprtf[3]>0 && TFREFFKEY.reprtf[4]>0)cout<<"  ---> ComprSegment::Qsection-decoding:"<<endl;
      bias=1;
// !!! here pss+bias points to Qwords word
      while(bias<nqwrds){//q-block words loop(nqwrds=1 if Kunin's nwords=0
	word=*(pss+bias+1);// current link header(+1 to bypass Qnwords word)
	slid=(word&0x000F);//0,..,8
	qlowchf=0;
	if((word&(0x4000))>0)qlowchf=1;//set "negative (adc-ped)" presence flag ??? latestKunin:==0
//cout<<"  bias="<<bias<<" word="<<hex<<word<<dec<<" slid="<<slid<<endl;
	if((word&(0x8000))>0 && slid<9){//header's marker,link# OK
	  slot=AMSSCIds::crdid2sl(crat-1,slid)+1;//slot-id to abs.slot-number(solid,sequential, 1,...,11)
	  if(slot<=0 || slot==1 || slot==4 || slot>11){//check slot# validity
	    TOF2JobStat::daqscr(1,crat-1,10);//invalid slot number
            if(TFREFFKEY.reprtf[3]>0){
	      cout<<"  <-- CompFmt:Q-section Error: invalid slot: crat="<<crat<<" link="<<slid<<"  aslot="<<slot<<endl;
	    }
	    goto BadExit;
	  }    
	  TOF2JobStat::daqssl(1,crat-1,slot-1,0);//count legal charge-slot entries
	  sslot=AMSSCIds::sl2qsid(slot-1);//seq.numbering of Q-measuting slots(1-9=>,4xSFET,1xSFEA,4xSFEC)
	  if(qlowchf)TOF2JobStat::daqssl(1,crat-1,slot-1,1);//count charge-slots with (adc-ped)<=-3
	  qchmsk=((word&(0x3FF0))>>4);//mask of "above-ped" channels
	  nnzch=0;
	  for(inch=0;inch<10;inch++){//<--- current_link input channels loop
	    if((qchmsk&(1<<inch))>0){//non-empty inp.channel
// now fill global buffer:
              mtyp=1;//charge
	      rdch=AMSSCIds::ich2rdch(crat-1,slot-1,inch,mtyp);//outp.ch:0(if empty),1,2...
	      val16=*(pss+bias+2+nnzch);//ped-subtracted ADC-value multiplied by 8
	      if(rdch>0 && (val16&(0x7FFF))>0){//<-- valid, not empty channel
	        TOF2JobStat::daqsch(1,crat-1,slot-1,inch,0);//count charge channels entries
                AMSSCIds scinid(crat-1,slot-1,rdch-1);
//          mtyp=scinid.getmtyp();//0->LTtime, 1->Q, 2->FT, 3->sumHT, 4->sumSHT
//          pmt=scinid.getpmt();//0->anode, 1-3->dynode#
                swch=scinid.getswch();//seq. sw-channel (0,1,...)
	        swid=scinid.getswid();
	        hwid=scinid.gethwid();
	        bpnt=swch;//swch true address in the buffer(1st buff.half)
	        if(swnbuf[bpnt]<1){
	          swcbuf[bpnt][swnbuf[bpnt]]=(val16&(0x7FFF));
	          swibuf[bpnt]=swid;
	          hwidc=1000*crat+100*sslot+inch+1;//non-standard hwid !!!(Crat(1-4)|Sslot(1-9)|Ich(1-10))
	          hwibuf[bpnt]=hwidc;
	          swnbuf[bpnt]+=1;//increase counter of given swch
	        }
	        else{//should not happen, normally 1hit/channel
                  if(TFREFFKEY.reprtf[3]>0)cout<<"  <-- Error: NChargeHits > 1 for hwid/swid="<<hwid<<" "<<swid<<endl;
	        }
	      }//--->endof empty chan. check
              nnzch+=1;//counts hits
            }//--->endof "non-empty" inp.channel check
	  }//--->endof inp.channels loop
	  bias+=(nnzch+1);//+1 for link-header itself
	}//--->endof "link-header OK"
	else{
	  TOF2JobStat::daqscr(1,crat-1,9);//wrong link-header in Q-section
          if(TFREFFKEY.reprtf[3]>0)cout<<" <-- CompFmt::Error: wrong link-header in Q-section !"<<endl;
	  goto BadExit;    
	}
      }//--->endof q-block words loop
//
      pss+=nqwrds;//shift by Qwords, now pss points to last word of Q-section 
//============>endof Charge-section
//
//<====================== TempTime-section:
//
if(TFREFFKEY.reprtf[3]>0 && TFREFFKEY.reprtf[4]>0)cout<<"  --->  ComprSegment::Tsection-decoding:"<<endl;
      bias=1;
      word=*(pss+bias);//NTwords word
      if((word&(0x8000))>0 || (word&(0x4000))>0){
	TOF2JobStat::daqscr(1,crat-1,11);//was link# problems during Kunin's raw->comp conversion 
      }
      while(bias<ntwrds){//<---temp/time-block words loop
	tlhead=*(pss+bias+1);//current time-link-header
	slid=(tlhead&(0x0007));//slot_id(link#) 0,1,2,3,4 => SFET_0,_1,_2,_3,SFEA
	nthts=((tlhead&(0x0FF0))>>4);//time-hits in current link
	if((tlhead&(0x2000))>0)nthts+=1;//consider temper. as additional(1st) time-hit
        n16wrds=nthts+(nthts+nthts%2)/2;//numb.of 16bins-words for given link
	if((tlhead&(0x1000))>0)n16wrds+=1;//add error-word is present
//cout<<" lhead="<<hex<<tlhead<<dec<<" slid/nthts="<<slid<<" "<<nthts<<" n16wrds="<<n16wrds<<endl;
	if((tlhead&(0x8000))==0 || (tlhead&(0x4000))==0 || (tlhead&(0x0008))>0){//
	  TOF2JobStat::daqscr(1,crat-1,12+slid);//err while raw->comp (no header|trailer or probl with evn/wcount)
	  bias+=(n16wrds+1);//to point to next link-header(+1 for current link header)
          if(TFREFFKEY.reprtf[3]>0){
	    cout<<" <-- CompFmt-Error in Event:"<<AMSEvent::gethead()->getid()<<" T-block: HTSbits-problem,crat/link="<<crat<<" "<<slid<<"H:"
                         <<((tlhead&(0x8000))>>15)<<"T:"<<((tlhead&(0x4000))>>14)<<"S:"<<((tlhead&(0x0008))>>3)<<endl;
	  }
	  continue;//---> skip bad link info
	  goto BadExit; 
	}
        slot=AMSSCIds::crdid2sl(crat-1,slid)+1;//slot-id to slot-number(solid,sequential, 1,...,11)
//cout<<" abs.slot="<<slot<<endl;
	if(slot<=0 || slot==1 || slot==4 || slot>7){//check slot# validity
	  TOF2JobStat::daqscr(1,crat-1,17);//invalid slot number
	  bias+=(n16wrds+1);//to point to next link-header
          if(TFREFFKEY.reprtf[3]>0){
//            cout<<" <-- CompFmt::T-block Error: InvalidSlot, crat/link="<<crat<<" "<<slid<<" slot="<<slot<<endl;
	  }
	  continue;//---> skip bad link info
	  goto BadExit;    
	}
	TOF2JobStat::daqssl(1,crat-1,slot-1,2);//count legal time-slot entries("T-entries")
        sslot=AMSSCIds::sl2tsid(slot-1);//seq.numbering of T-measuting slots(1-5=>,4xSFET,1xSFEA)
//cout<<" sslot="<<sslot<<endl;
//
	bias+=1;//to point to 1st tempr-word(if present), or to 1st word of the 1st time-hit otherwise
	if((tlhead&(0x2000))==0)TOF2JobStat::daqssl(1,crat-1,slot-1,3);//count missing temperature cases
//cout<<"   new nthts="<<nthts<<endl;
//
	for(nh=0;nh<nthts;nh++){//<---hits loop
	  word=*(pss+bias+1);
	  nword=*(pss+bias+2);
	  nnword=*(pss+bias+3);
	  if(nh%2==0){//1st,3rd,...hit
	    if(nh==0 && (tlhead&(0x2000))>0){//1st hit is the temperature - decode it
	      val32=(uinteger(word)<<8);//16 msb
	      val32|=(uinteger(nword)>>8);//8 lsb
              tem1=((val32&(0xFFF000L))>>12);
              tem2=(val32&(0xFFFL));
              temp=0;
              if(tem2>0)temp=235-400*geant(tem1)/geant(tem2);//
              tsens=AMSSCIds::sl2tsid(slot-1);//seq.slot#->temp.sensor#; 1,2,3,4,5
              TOF2JobStat::puttemp(crat-1,tsens-1,temp);
	      continue;//temp-hit is not stored in swcbuf, so skip storing(i.e. take next hit) 
	    }
	    else{
	      inch=((word&(0xE000))>>13);//inp.ch# 0-7
	      val32=uinteger((nword&(0xFF00))>>8);// lsb
	      val32|=(uinteger(word&(0x1FFF))<<8);// msb
	    }
	  }
	  else{//2nd,4th,...hit
	    inch=((nword&(0x00E0))>>5);//inp.ch# 0-7
	    val32=uinteger(nnword);// lsb
	    val32|=(uinteger(nword&(0x001F))<<16);// msb
	    bias+=3;//now points to next "3x16bits" or to err-patt word(if any) or to next link-header
	  }
// now fill global buffer:
	  mtyp=0;//"0" for ich2rdch-routine means "any Time-ch like LT,FT,sHT,sSHT"
	  rdch=AMSSCIds::ich2rdch(crat-1,slot-1,inch,mtyp);//outp.ch:0(if empty),1,2...
	  if(rdch>0){//<-- not empty channel
            AMSSCIds scinid(crat-1,slot-1,rdch-1);//
//	    TOF2JobStat::daqsch(1,crat-1,slot-1,rdch-1,1);//counts time rd-channels entries
	    TOF2JobStat::daqsch(1,crat-1,slot-1,inch,1);//counts time inp-channels entries
            swch=scinid.getswch();//seq. sw-channel (0,1,...)
	    swid=scinid.getswid();//LBBSPM
            mtyp=swid%10;//phys.bars Time/Charge:0/1, fict.extra.bars FT/sumHT/sumSHT-times:2/3/4
	    if(mtyp==0)nhitmx=SCTHMX3;
	    else if(mtyp==1)nhitmx=1;
	    else if(mtyp==2)nhitmx=SCTHMX1;
	    else nhitmx=SCTHMX2;
	    bpnt=swch;//swch true address in the buffer(1st buff.half)
	    hwid=scinid.gethwid();
	    htime=((val32&(0x7FFFFL))<<2);//19 msb
	    htime|=((val32&(0x180000L))>>19);//add 2 lsb
	    if(swnbuf[bpnt]<nhitmx){
	      swcbuf[bpnt][swnbuf[bpnt]]=htime;//store time-value
	      swibuf[bpnt]=swid;
	      hwidc=1000*crat+100*sslot+inch+1;//non-standard hwid !!!(Crat(1-4)|Sslot(1-5)|Ich(1-8))
	      hwibuf[bpnt]=hwidc;
	      swnbuf[bpnt]+=1;//increase counter of given swch
	    }
	    else{
//	      TOF2JobStat::daqsch(1,crat-1,slot-1,rdch-1,2);//count time-channels hits ovfls
	      TOF2JobStat::daqsch(1,crat-1,slot-1,inch,2);//count time-channels hits ovfls
	    }
	  }//--->endof "rdch>0" check
//
	}//--->endof hits loop
//
	if((nthts%2)==1)bias+=2;//now points to err-patt word(if any) or to next link-header
	if((tlhead&(0x1000))>0){//error-word is present
	  tlerrf=*(pss+bias+1);
          if(TFREFFKEY.reprtf[3]>0){
	    cout<<"  <-- CompFmt::Tblock: ErrorWord found: LinkHeader/ErrWord="<<hex<<tlhead<<" "<<tlerrf<<dec<<endl;
	  }
	  TOF2JobStat::daqssl(1,crat-1,slot-1,4);//count time-slots with error
	  bias+=1;//now poits to to next link-header
	}
      }//--->endof while(temp+time)-block words loop
//
//===========>endof TempTime-section
if(TFREFFKEY.reprtf[3]>0 && TFREFFKEY.reprtf[4]>0)cout<<"<----- endof ComprFMT-decoding !"<<endl;
  }//-->endof "Compr or Mixt"-datatype check
//
//<=========================== Endof CompressedFormat processing
//
//===========================> compare Raw/Comp-halfs of the decoded buffer(if mixed format):
  if(formt==2){
    TOF2JobStat::daqscr(3,crat-1,0);//count crate-entries with mixt-format(raw/com-subsegm OK)
    if(TFREFFKEY.reprtf[3]>0 && TFREFFKEY.reprtf[4]>0){    
      cout<<endl;
      cout<<"----> DAQS2Block:: MixFMT processed: Compare Raw- and Compr-subsegments decoding results:"<<endl;
    }
    integer ihr,ihc,nhraw,nhcom,swidr,swidc,hwidr,nhitsc(0),nhitsr(0);
    integer vmism,err1c(0),err2c(0);
    uinteger valr,valc;
    for(ic=0;ic<SCRCMX;ic++){//compr half-buf. loop (all swch)
      nhcom=swnbuf[ic];
      nhraw=swnbuf[ic+bufpnt];
      if(nhcom>0){
        swidc=swibuf[ic];
	hwidc=hwibuf[ic];
        swidr=swibuf[ic+bufpnt];
	hwidr=hwibuf[ic+bufpnt];
	if(nhraw<nhcom || swidc!=swidr || hwidc!=hwidr){
          if(TFREFFKEY.reprtf[3]>0){
	    cout<<"<--- RawCompFmtCompar:Error_1: nhits/id mismatch for hwidc/swidc="<<hwidc<<" "<<swidc<<endl;
	    cout<<"     nhcom/nhraw="<<nhcom<<" "<<nhraw<<" hwidr/swidr="<<hwidr<<" "<<swidr<<" swch="<<ic<<endl;
          }
	  err1c+=1;
          TOF2JobStat::daqscr(3,crat-1,1);//count Err-1
	}
	vmism=1;
        for(ihc=0;ihc<nhcom;ihc++){//<--- loop over compFMT-hits to find confirmation in rawFMT
	  valc=swcbuf[ic][ihc];
	  for(ihr=0;ihr<nhraw;ihr++){
	    valr=swcbuf[ic+bufpnt][ihr];
	    if(valc==valr)vmism=0;//OK: compFMT-hit confirmed in rawFMT 
	  }
	  if(vmism==1){
	    err2c+=1;//count mism.hits
            TOF2JobStat::daqscr(3,crat-1,2);//count Err-2
            if(TFREFFKEY.reprtf[3]>0){
              cout<<"<--- RawCompFmtCompar:Error_2: unconfirmed CompFMT-hit="<<valc<<
	                                                             " for swid/hwid="<<swidc<<" "<<hwidc<<endl;
            }
	  }
        }//--->endof confirm.loop
	nhitsc+=nhcom;//count compFMT-hits
      }//-->endof nhcom>0 check
//
      if(nhraw>0){
	nhitsr+=nhraw;//count rawFMT-hits
      }
    }//-->endof buff-loop
//
    if(err1c==0 && err2c==0)TOF2JobStat::daqscr(3,crat-1,3);//count errorless crate-entries
if(TFREFFKEY.reprtf[3]>0 && TFREFFKEY.reprtf[4]>0){    
  cout<<"<---- MixFMT check completed:total Raw/CompFMT Q+T(LT,FT,sHT,sSHT)-hits :"<<nhitsr<<" "<<nhitsc<<endl;
  cout<<"       Total Nhit/swid/hwid-mismatch errors:"<<err1c<<endl;
  cout<<"       Total unconfirmed CompFMT-hits:"<<err2c<<endl;
  cout<<endl;
} 
  }
//<=========================== End of Comparison section
//
// Now scan buffers and create tof/anti raw-event obj.:
//
  integer nftdc;         // number of FT-tdc hits
  integer ftdc[SCTHMX1]; // FT-tdc" hits (in TDC channels)
  integer nstdc;         // number of LT-tdc hits
  integer stdc[SCTHMX3]; // LT-tdc" hit (in TDC channels)
  geant adca; // Anode-channel ADC hit (ADC-counts, float)
  integer nadcd;         // number of NONZERO Dynode-channels(max PMTSMX)
  geant adcd[PMTSMX]; // ALL Dynodes ADC hits(ADC-counts, positional, float)
  int16u sslt;
  integer nsumh,nsumsh,sumht[TOF2GC::SCTHMX2],sumsht[TOF2GC::SCTHMX2];
  geant athr,dthr,anthr;
  athr=TOF2Varp::tofvpar.daqthr(3);//tof daq readout thr(ped sigmas) for anode
  dthr=TOF2Varp::tofvpar.daqthr(4);//tof daq-thr. for dynode
  temp=999;//for AntiRawEvent-obj, 999 mean undefined value (real/default one will be set at validation stage)
  geant temp1,temp2,temp3;
  temp1=999;//for TofRawSide-obj(SFET/SFEC/PMT-temper), 999 mean undefined values  
//                            (real/default ones will be set during validation stage)
  temp2=999;
  temp3=999;
//
  if((getformat()==0 || DownScal) && !TofPedCal)subtpedTof=true;//RawFmt/DownScaled Tof/Acc-data are in crate
  if((getformat()==0 || DownScal) && !AccPedCal)subtpedAcc=true;
//
//for(ic=0;ic<SCRCMX;ic++){
//  if(swibuf[ic]>0){
//    cout<<"ic="<<ic<<" swid/hwid="<<swibuf[ic]<<" "<<hwibuf[ic]<<" nhits="<<swnbuf[ic]<<" Hits:";
//    for(i=0;i<swnbuf[ic];i++)cout<<" "<<swcbuf[ic][i];
//    cout<<endl;
//  }
//}
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
  hwidt=0;
  for(i=0;i<PMTSMX+1;i++)hwidq[i]=0;
  for(ic=0;ic<SCRCMX;ic++){//1-pass scan
    swid=swibuf[ic];//LBBSPM
    if(swid>0){//!=0 LBBSPM found
      sswid=swid/100;//LBBS ("short swid")
      for(icn=ic+1;icn<SCRCMX;icn++){//find next !=0 LBBSPM
        swidn=swibuf[icn];
        if(swidn>0)break;
      }
      if(swidn>0)sswidn=swidn/100;//next LBBS
      else sswidn=9999;//means all icn>ic are "0"
      il=swid/100000;
      mtyp=swid%10;//phys.bars Time/Charge:0/1, fict.extra.bars FT/sumHT/sumSHT-times:2/3/4
      if(il==0)dtyp=2;//anti
      else{
        dtyp=1;//tof
        il-=1;
      }
      ib=(swid%100000)/1000-1;
      is=(swid%1000)/100-1;
      pmt=(swid%100)/10;
      if(mtyp<2){
        if(dtyp==1)pmmx=TOF2DBc::npmtps(il,ib);
        if(dtyp==2)pmmx=1;
      }
      else pmmx=0;//for fict.bars, not used really
      nh=swnbuf[ic];//#of hits for given sw-chan(in accordance with mtyp)
      hwid=hwibuf[ic];//NonStandard hwid: Crat(1-4)|SeqSlot(1-9 i.e. 4xSFET,1xSFEA,4xSFEC)|IInpch(1-10)
      shwid=hwid/100;//Crat|SeqSlot
      sslt=(shwid%10);//SeqSlot# (1-9)
      if(hwidt==0)hwidt=10000*shwid;//CS0000
//      if(mtyp!=1 && hwidt==0)hwidt=10000*shwid;//CS0000
      if(mtyp>=2)assert(sslt>0 && sslt<=SCFETA);//SFET+SFEA seq.slot# is used for FT/sHT/sSHT storing
//
      switch(mtyp){//fill RawEvent arrays
        case 0:
	  hwidt+=1000*(hwid%100);//store LTime InpCh#(1-9) in time-hwid(CS|Ich|Ift|Isht|Issht, 6 digits)
	  for(i=0;i<nh;i++)stdc[i]=swcbuf[ic][i];
	  nstdc=nh;
	  break;
        case 1:
	  hwidq[pmt]=hwid;//store anode/dynodes Q-InpCh# in Q-hwid(CSII, II=1-10) array(SII's are different)
	  if(pmt>pmmx){
            if(TFREFFKEY.reprtf[3]>0)cout<<"GlobBufferScan: Error - Npm>Max in ADC-hit, swid="<<swid<<endl;
          }
	  else{
	    if(pmt==0){//anode
	      if(formt==0)adca=geant(swcbuf[ic][0])+0.5;//"+0.5" to be at ADC-bin center
	      else adca=geant(swcbuf[ic][0])/8;//in comp.fmt Tof adc=int16u((adc-ped)*8)
	      if(dtyp==1){//tof
	        ped=TOFBPeds::scbrped[il][ib].apeda(is);
	        sig=TOFBPeds::scbrped[il][ib].asiga(is);
//cout<<"---> TofAnode:L/B/S="<<il+1<<" "<<ib+1<<" "<<is+1<<" ped/sig="<<ped<<" "<<sig<<endl;
		if(subtpedTof){
		  if((adca-ped)>athr*sig)adca-=ped;
		  else adca=0;
		}
	      }
	      if(dtyp==2 ){//anti
                ped=ANTIPeds::anscped[ib].apeda(is);
                sig=ANTIPeds::anscped[ib].asiga(is);
//cout<<"---> AccAnode:B/S="<<ib+1<<" "<<is+1<<" ped/sig="<<ped<<" "<<sig<<endl;
                anthr=ANTI2SPcal::antispcal[ib].getdqthr();//tempor is not side-individual !!!
		if(subtpedAcc){
		  if((adca-ped)>anthr*sig)adca-=ped;
		  else adca=0;
		}
	      }    
	    }
	    else{//dynode
	      if(formt==0)adcd[pmt-1]=geant(swcbuf[ic][0])+0.5;
	      else adcd[pmt-1]=geant(swcbuf[ic][0])/8;//in comp.fmt Tof adc=int16u((adc-ped)*8)
	      if(subtpedTof){
	        ped=TOFBPeds::scbrped[il][ib].apedd(is,pmt-1);
	        sig=TOFBPeds::scbrped[il][ib].asigd(is,pmt-1);
		if((adcd[pmt-1]-ped)>dthr*sig){
		  adcd[pmt-1]-=ped;
	          nadcd+=1;
		}
		else adcd[pmt-1]=0;
	      }
	      else nadcd+=1;
	    }
	  }
	  break;
        case 2:
	  hwidtc=100*(hwid%100);//store FTime InpCh#(1-9) in time-hwid(CS|Ich|Ift|Isht|Issht)
	  TOF2RawSide::FThits[crat-1][sslt-1]=nh;
	  for(i=0;i<nh;i++)TOF2RawSide::FTtime[crat-1][sslt-1][i]=swcbuf[ic][i];
	  TOF2RawSide::FTSchan[crat-1][sslt-1]+=hwidtc;//store Ich number(6)
	  break;
	case 3:
	  hwidtc=10*(hwid%100);//store sumHTime InpCh#(1-9) in time-hwid(CS|Ich|Ift|Isht|Issht)
          TOF2RawSide::SumHTh[crat-1][sslt-1]=nh; 
          for(i=0;i<nh;i++)TOF2RawSide::SumHTt[crat-1][sslt-1][i]=swcbuf[ic][i];
	  TOF2RawSide::FTSchan[crat-1][sslt-1]+=hwidtc;//store Ich number(7)
	  break;
	case 4:
	  hwidtc=(hwid%100);//store sumSHTime InpCh#(1-9) in time-hwid(CS|Ich|Ift|Isht|Issht)
	  hwidt+=hwidtc;
          TOF2RawSide::SumSHTh[crat-1][sslt-1]=nh; 
          for(i=0;i<nh;i++)TOF2RawSide::SumSHTt[crat-1][sslt-1][i]=swcbuf[ic][i];
	  TOF2RawSide::FTSchan[crat-1][sslt-1]+=hwidtc;//store Ich number(8)
	  break;
        default:
	  if(TFREFFKEY.reprtf[3]>0)cout<<"Buf-scan:unknown measurement type ! swid="<<swid<<endl;
      }//-->endof switch
    }//-->endof "!=0 LBBSPM found"
//
    if(sswid!=sswidn){//new/last LBBS found -> create RawEvent-obj for current LBBS
// (after 1st swid>0 sswid is = last filled LBBS, sswidn is = LBBS of next nonempty channel or =9999)
//  at this stage temp is not defined, will be redefined at validation-stage using static job-store or DB)
// here FT/sHT/sSHT Inp.ch# in hwidt are still not defined(will be added during validate-stage from static arr)	  
      if(dtyp==1){//TOF
	if(nstdc>0 || adca>0 || nadcd>0){//create tof-raw-side obj
	  if(subtpedTof || formt>0)sta=0;//ok(normal TOF2RawSide object with subtracted ped)
	  else sta=1;//for the moment it is a flag for Validate-stage that Peds was not subtracted !!!
	  nftdc=0;//dummy(filled later at valid. stage from [cr][sl] static stores)
	  nsumh=0;
	  nsumsh=0;
//hwidt's FT/sHT/sSHT inp.ch# info is added at validation stage from SumSHTh[crat-1][sslt-1] array
          if(TFREFFKEY.reprtf[3]>0 && TFREFFKEY.reprtf[4]>0){//<---debug
	    cout<<endl;
	    cout<<"    ==> Create TOFRawSide: short swid/hwidt="<<sswid<<" "<<hwidt<<endl;
	    cout<<"                           hwidq="<<hwidq[0]<<" "<<hwidq[1]<<" "<<hwidq[2]<<" "<<hwidq[3]<<endl;
	    cout<<"    nLT-hits="<<nstdc;
	    for(i=0;i<nstdc;i++)cout<<" "<<stdc[i];
	    cout<<endl;
	    cout<<"    adca="<<adca<<" nadcd="<<nadcd<<"  dynh="<<adcd[0]<<" "<<adcd[1]<<" "<<adcd[2]<<endl;
          }
// for the moment i do not use hwidq (no Q-linearity coorr foreseen now !??)
          AMSEvent::gethead()->addnext(AMSID("TOF2RawSide",icca),
                 new TOF2RawSide(sswid,hwidt,hwidq,sta,charge,temp1,temp2,temp3,
		                                                   nftdc,ftdc,nstdc,stdc,
		                                                   nsumh,sumht,
								   nsumsh,sumsht,
                                                                   adca,
			                                           nadcd,adcd));
	}
      }
      else{//ANTI 
	if(nstdc>0 || adca>0){
	  if(subtpedAcc || formt>0)sta=0;//ok(normal Anti2RawEvent object with subtracted ped)
	  else sta=1;//for the moment it is a flag for Validate-stage that Peds was not subtracted !!!
	  nftdc=0;//dummy(filled later at valid. stage from [cr][sl] static stores)
          if(TFREFFKEY.reprtf[3]>0 && TFREFFKEY.reprtf[4]>0){//<---debug
	    cout<<"   ==> Create AntiRawEvent: swid="<<sswid<<endl;
	    cout<<"    nLT-hits="<<nstdc;
	    for(i=0;i<nstdc;i++)cout<<" "<<stdc[i];
	    cout<<endl;
	    cout<<"    adca="<<adca<<endl;
	    cout<<endl;
          }
//ACC de-swapping if needed:
	  if(AMSJob::gethead()->isRealData() && AMSEvent::gethead()->getrun()<1211886677
	                                     && !AccPedCal){//de-swapping
// my
	    if(sswid==22)sswid=12;
	    else if(sswid==42)sswid=22;
	    else if(sswid==12)sswid=42;
	    
	    else if(sswid==62)sswid=52;
	    else if(sswid==82)sswid=62;
	    else if(sswid==52)sswid=82;
	    
	    else if(sswid==31)sswid=21;
	    else if(sswid==41)sswid=31;
	    else if(sswid==21)sswid=41;
	    
	    else if(sswid==71)sswid=61;
	    else if(sswid==81)sswid=71;
	    else if(sswid==61)sswid=81;
	  }
//
          AMSEvent::gethead()->addnext(AMSID("Anti2RawEvent",0),
                                    new Anti2RawEvent(sswid,sta,temp,adca,nftdc,ftdc,nstdc,stdc));
	}
      }
      adca=0;//reset RawEvent buffer
      nftdc=0;
      nstdc=0;
      nadcd=0;
      hwidt=0;
      for(i=0;i<PMTSMX+1;i++)hwidq[i]=0;
      for(i=0;i<PMTSMX;i++){
        adcd[i]=0;
      }
    }//-->endof next/last LBBS check
  }//-->endof scan
//
  if(TFREFFKEY.reprtf[3]>0 && TFREFFKEY.reprtf[4]>0){//<---debug
  cout<<"<---- DAQS2Block::buildraw: FT/sumHT/sumSHT report for crate="<<crat<<endl;
  cout<<"    FT-time hits report:"<<endl;
  for(int isla=0;isla<5;isla++){
    cout<<"SFETA-slot="<<isla+1<<" hits:"<<endl;
    cout<<" nhits="<<TOF2RawSide::FThits[crat-1][isla];
    for(ic=0;ic<TOF2RawSide::FThits[crat-1][isla];ic++)cout<<" "<<TOF2RawSide::FTtime[crat-1][isla][ic];
    cout<<endl;
  }
  cout<<"    sumHT-time hits report:"<<endl;
  for(int isla=0;isla<4;isla++){
    cout<<"SFETA-slot="<<isla+1<<" hits:"<<endl;
    cout<<" nhits="<<TOF2RawSide::SumHTh[crat-1][isla];
    for(ic=0;ic<TOF2RawSide::SumHTh[crat-1][isla];ic++)cout<<" "<<TOF2RawSide::SumHTt[crat-1][isla][ic];
    cout<<endl;
  }
  cout<<"    sumSHT-time hits report:"<<endl;
  for(int isla=0;isla<4;isla++){
    cout<<"SFETA-slot="<<isla+1<<" hits:"<<endl;
    cout<<" nhits="<<TOF2RawSide::SumSHTh[crat-1][isla];
    for(ic=0;ic<TOF2RawSide::SumSHTh[crat-1][isla];ic++)cout<<" "<<TOF2RawSide::SumSHTt[crat-1][isla][ic];
    cout<<endl;
  }
//
  cout<<"Temperature report:"<<endl;
  for(int its=0;its<5;its++){
    cout<<"  SFETA-slot:"<<its+1<<" temper:"<<TOF2JobStat::gettemp(crat-1,its)<<endl;
  }
//
  integer trpat[TOF2GC::SCLRS],trpatz[TOF2GC::SCLRS];
  TOF2RawSide::getpatt(trpat);
  cout<<"TrigPatt report(HT):"<<endl;
  for(il=0;il<TOF2GC::SCLRS;il++){// pattern histogr
    cout<<"   layer="<<il+1<<endl;
    cout<<"     trpatt="<<hex<<trpat[il]<<dec<<endl;
    cout<<"      side1-pads:";
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      if((trpat[il]&(1<<ib))>0)cout<<" "<<ib+1;
    }
    cout<<endl;
    if((trpat[il]&(1<<12))>0)cout<<"       other-SPTside CPbit-on";
    if((trpat[il]&(1<<13))>0)cout<<"       other-SPTside CTbit-on";
    cout<<endl;
    cout<<"      side2-pads:";
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      if((trpat[il]&(1<<(16+ib)))>0)cout<<" "<<ib+1;
    }
    cout<<endl;
    if((trpat[il]&(1<<28))>0)cout<<"       other-SPTside CPbit-on";
    if((trpat[il]&(1<<29))>0)cout<<"       other-SPTside CTbit-on";
    cout<<endl;
  }
  TOF2RawSide::getpattz(trpatz);
  cout<<"TrigPatt report(SHT):"<<endl;
  for(il=0;il<TOF2GC::SCLRS;il++){// pattern histogr
    cout<<"   layer="<<il+1<<endl;
    cout<<"     trpatt="<<hex<<trpatz[il]<<dec<<endl;
    cout<<"      side1-pads:";
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      if((trpatz[il]&(1<<ib))>0)cout<<" "<<ib+1;
    }
    cout<<endl;
    if((trpat[il]&(1<<12))>0)cout<<"       other-SPTside BZbit-on";
    cout<<endl;
    cout<<"      side2-pads:";
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      if((trpatz[il]&(1<<(16+ib)))>0)cout<<" "<<ib+1;
    }
    cout<<endl;
    if((trpat[il]&(1<<28))>0)cout<<"       other-SPTside BZbit-on";
    cout<<endl;
  }
  }//--->endof debug
//--------
  return;
BadExit:
  TOF2JobStat::daqsfr(1);//count rejected entries(segments)    
//  
}
//----------------------------------------------------
void DAQS2Block::EventBitDump(integer leng, int16u *p, char * message){
  int16u blid,len,naddr,datyp;
  len=int16u(leng&(0xFFFFL));//fragment's length in 16b-words(not including length word itself)
  blid=*(p+len);// fragment's last word: Status+slaveID
  bool dataf=((blid&(0x8000))>0);//data-fragment(not calib)
  bool crcer=((blid&(0x4000))>0);//CRC-error
  bool asser=((blid&(0x2000))>0);//assembly-error
  bool amswer=((blid&(0x1000))>0);//amsw-error   
  bool timoer=((blid&(0x0800))>0);//timeout-error   
  bool fpower=((blid&(0x0400))>0);//FEpower-error   
  bool seqer=((blid&(0x0400))>0);//sequencer-error
  bool cdpnod=((blid&(0x0020))>0);//CDP-node(like SDR2-node with no futher fragmentation)
  bool noerr;
  noerr=(dataf 
             && !crcer 
	               && !asser 
		                && !amswer 
                                          && !timoer 
					            && !fpower 
						              && !seqer 
							               && cdpnod);
  naddr=blid&(0x001F);//slaveID(="NodeAddr"=SDR_link#)
  datyp=(blid&(0x00C0))>>6;//0,1,2,3 (if dataf=0, datyp=2->OnbPed)
  if(dataf && datyp>0)noerr=(dataf && !crcer && !asser && !amswer 
                                       && !timoer && !fpower && !seqer && cdpnod);
  if(!dataf)noerr=(!dataf && !crcer && !asser && !amswer 
                                       && !timoer && !fpower && !seqer && cdpnod);
  else noerr=false;
  cout<<"----> DAQS2Block::"<<message<<" for event:"<<AMSEvent::gethead()->getid()<<endl;
  cout<<" SlaveStatWord="<<hex<<blid<<dec<<" NoAsseblyErr="<<noerr<<endl; 
  cout<<" node_addr(link#)="<<naddr<<" DataFormat="<<datyp<<" BlLength(in this call)="<<len<<endl;
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
      if((tstw&tstb)!=0)cout<<"+"<<simb;
      else cout<<" "<<simb;
    }
    cout<<endl;
  }
  cout<<"-----------------------------------------------------------"<<endl;
  cout<<dec<<endl<<endl;
}//
//-------------------------------------------------------
void DAQS2Block::buildonbP(integer leng, int16u *p){
//used for OnBoard PedTables processing (and writing to DB)
// it is implied that event_fragment is SDR2's one (checked in calling procedure)
// on input: len=>2 ms-bytes(node_addr)+2 ls-bytes(length as given at the beginning of block)
//           *p=pointer_to_beggining_of_block_data(i.e. NEXT to len-word !!!) 
// Length counting does not includes length-word itself !!! 
//
  integer i,il,ib,is;
  int swid;
  int16u pmt,mtyp,rdch,inch,slot,crat,slid,csid;
  int16u blid,dtyp,naddr,len;
  int16u word,nword,nnword,nnnword,bias;
  int16u bias1,bias2;
  int16u calstat(0);//calib.status (1st word after block length word)
  int16u crsd;
  int16u headw1;
//
// for onboard ped-cal tables:
  uinteger runn;
  bool PedBlkReqw(false);
  bool ONBpedblk(true);//because buildOnbP() is called only for that case
  bool newrun;
  geant ped,sig;
  geant dped,thr;
  int16u nblkok;
  bool sidedoubled(false);
  integer spatt=TFCAFFKEY.onbpedspat;//bit-patt for onb.ped-table sections (bit set if section is present)
  bool dpedin=((spatt&16)==16);//dynam.peds-section present(90 words)
  bool ptrwin=((spatt&8)==8);//pretrigwords ............(4 ...)
  bool stawin=((spatt&4)==4);//statwords ...............(10...)
  bool thrsin=((spatt&2)==2);//thresholds ..............(90...)
  integer reflen=90+90;//ped&width-sections always present
  if(dpedin)reflen+=90;//in reality absent
  if(ptrwin)reflen+=4;
  if(stawin)reflen+=10;
  if(thrsin)reflen+=90;
  reflen+=2;//add calstat. and slavestat words : 286=1+3*90+4+10+1
//                       totdatalen=286+1(CRC at the very end)=287
//  leng(in call)=totdatalen+2(2 header words)=289
// All this calculationS are for the mode when JMDC directly requests caldata from SDR (SDR-addr is in 1st header word)
  bool tofout,accout;
  int outflg; 
//
//
  bool dataf;
  bool crcer;
  bool asser;
  bool amswer;   
  bool timoer;   
  bool fpower;   
  bool seqer;
  bool cdpnod;
  bool noerr;
  bool badexit;
//-----
#pragma omp critical (tfac_pedc_onb)
{
  badexit=true;
  if((CALIB.SubDetInCalib/1000)%10!=1){
    if(_CalFirstSeq){//CalFirstSeq is set to true only once in initjob(initb)
      cout<<endl<<"<======= DAQS2Block::buildOnbPed: Warning - OnBoardPed DB-writing is blocked !!!"<<endl<<endl;
//      _CalFirstSeq is set to false after the 1st good sequence (was call to outb(flg))
    }
  }
//
  TOF2JobStat::daqsfr(60);//count entries
  (void)AMSEvent::gethead()->getheadC("DAQEvent",6);
  runn=AMSEvent::gethead()->getrun();
  newrun=(_PrevRunNum!=runn);
//
  if(newrun || _FirstPedBlk){//count/mark requested SDRs on 1st call or new run
//    cout<<"  Ntdv="<<AMSJob::gethead()->gettdvn()<<endl;
//    for(i=0;i<AMSJob::gethead()->gettdvn();i++){     
//      p2tdvnam=AMSJob::gethead()->gettdvc(i);
//      cout<<" TDV-name="<<p2tdvnam<<" at "<<i<<endl;
//    }
//
    cout<<endl;
    cout<<"=========> DAQS2Block::buildOnbPed: NewRun/FirstPedBlock call, run:"<<runn<<endl;
    cout<<"           Good PedBlocks in prev.run :"<<_GoodPedBlks
                                            <<", total found:"<<_FoundPedBlks<<endl;
    _FirstPedBlk=false;
    _PrevRunNum=runn;
    TOFPedCalib::BRun()=runn;
    ANTPedCalib::BRun()=runn;
    TOFPedCalib::resetb();//reset histograms, cumulative vars(GoodPedBlks,FoundPedBlks,NReqEdrs,PedBlkCrat[],...)
    ANTPedCalib::resetb();//reset histograms
//these codes replace ones which are 4 lines below (thank to onboard s/w developers !)
    for(crat=0;crat<SCCRAT;crat++){//imply that all crates are pres.
      _PedBlkCrat[crat]+=1;//mark requested SDRs(finally have to be 0 because set to -1 in init)
      _NReqEdrs+=1;
    } 
//
//    for(crat=0;crat<SCCRAT;crat++){//imply that all crates are 
//	for(csid=0;csid<2;csid++){//sides(a/b)
//          portid=DAQS2Block::getportid(crat,csid);//>=0 if exist(4,5,6,7,18,19,20,21)
//	  crdid=0;
//          pcreq=(pdaq && portid>=0 && crdid>=0
//	            && pdaq->CalibRequested(uinteger(portid),uinteger(crdid)));//ped-cal requested for given crate/slot ?
//          if(pcreq){
//	    _PedBlkCrat[crat]+=1;//mark requested SDRs
//	    _NReqEdrs+=1;
//	  }
//	}
//	if(_PedBlkCrat[crat]>0){//error: both sides requested for given crat/slot
//	  cout<<"      <--- buildOnbP:Warning: found that multiple sides are requested for crt="<<ic<<endl;
//	  _PedBlkCrat[crat]=0;//preset by hands
//	  sidedoubled=true;
//	}
//    }
    cout<<"           Total SDRs requested:"<<_NReqEdrs<<endl;
    if(!sidedoubled && _NReqEdrs<(SCCRAT)){
      cout<<"      <--- buildOnbP:Warning-> not all SDRs requested ?!"<<endl;
//      bad=true;
    }
    else if(sidedoubled){
      cout<<"<--------- BuildOnbPed:Error: Some sides are doubled, NrequestedSDRs="<<_NReqEdrs<<endl;
      goto Exit;
    }
  }//--->endof 1st PedBlk(new run)
//
//--------
  headw1=*(p-2);
  calstat=*p;
//  cout<<"  hw1="<<hex<<*(p-2)<<"  hw2="<<*(p-1)<<"  calstat="<<*p<<dec<<endl;
  naddr=((headw1>>5)&0x1FF);//node addr from 1st header word
  len=int16u(leng&(0xFFFFL));//fragment's length in 16b-words(not including length word itself)
  crsd=int16u((leng>>16)&(0xFFFFL));//CS(c=1-4,s=1-4=>a,b,p,s) as return by my "checkblockidP"-1
  crat=(crsd+1)/10;
  csid=(crsd+1)%10;
  if(csid==3)csid=1;//tempor
  else if(csid==4)csid=2;
//  node2crsP(naddr,crat,csid);//get crate#(1-4,=0 when wrong addr)),card_side(1-4<->a,b,p,s)
  if(TFREFFKEY.reprtf[3]>0 && TFREFFKEY.reprtf[4]>0){//debug
    cout<<"<--- <<crat/side="<<crat<<" "<<csid<<" naddr="<<naddr<<endl;
    cout<<"=======> In DAQS2Block::builOnbPed: BlLength="<<len<<" SDR_node:"<<naddr<<endl;
    EventBitDump(reflen-1,p,"OnbPed: Event-by-Event bitDump:");//debug
  }
  if(crat<=0){//illegal node-address
    goto Exit;
  }
  TOF2JobStat::daqscr(2,crat-1,0);// entries/crate
//---
  if((calstat&(0x4000))==0){
    if(TFREFFKEY.reprtf[3]>0){
      cout<<"<------ In BuilOnbPed:Bad CalStat,crat="<<crat<<endl;
    }
    TOF2JobStat::daqscr(2,crat-1,1);// bad cal.status entries
    goto Exit;
  }
//---
  if((calstat&(0xF))>0){
    TOF2JobStat::daqscr(2,crat-1,2);//crazy bit entries
  }
//---
  if((len-reflen-3)!=0){
    if(TFREFFKEY.reprtf[3]>0){
      cout<<"<------ In BuilOnbPed:Error in length,crat="<<crat<<" len/(reflen+3)="<<len<<" "<<(reflen+3)<<endl;
    }
    TOF2JobStat::daqscr(2,crat-1,3);//PedCal length error
    goto Exit;
  }
  TOF2JobStat::daqscr(2,crat-1,4);//PedBlockLengOK/crate
//---
  blid=*(p+reflen-1);//slave status word
//  cout<<"   slavestat="<<hex<<blid<<dec<<endl;
  dataf=((blid&(0x8000))>0);//data-fragment(sci) if >0, otherwise OnbCalib
  crcer=((blid&(0x4000))>0);//CRC-error
  asser=((blid&(0x2000))>0);//assembly-error
  amswer=((blid&(0x1000))>0);//amsw-error   
  timoer=((blid&(0x0800))>0);//timeout-error   
  fpower=((blid&(0x0400))>0);//FEpower-error   
  seqer=((blid&(0x0200))>0);//sequencer-error
  cdpnod=((blid&(0x0020))>0);//CDP-node(like SDR2-node with no futher fragmentation)
//
  if(TFREFFKEY.reprtf[3]>0 && TFREFFKEY.reprtf[4]>0){//debug
    cout<<"      data/crc_er/ass_er/amsw_er/tmout/FEpow_er/seq_er/lastlevel/="<<
    dataf<<" "<<crcer<<" "<<asser<<" "<<amswer<<" "<<timoer<<" "<<fpower<<" "<<seqer<<" "<<cdpnod<<endl;
  }
//---
  ONBpedblk=(dataf==0);//ignore datyp for ped-calibr.
  if(!ONBpedblk){
    if(TFREFFKEY.reprtf[3]>0)EventBitDump(reflen-1,p,"OnbPed: Bad DataType/Format bitDump:");//debug
    goto Exit;
  }
  TOF2JobStat::daqscr(2,crat-1,5);// PedDataTypeBlocks/crate
//--
  if(crcer)TOF2JobStat::daqscr(2,crat-1,6);
  if(asser)TOF2JobStat::daqscr(2,crat-1,7);
  if(amswer)TOF2JobStat::daqscr(2,crat-1,8);
  if(timoer)TOF2JobStat::daqscr(2,crat-1,9);
  if(fpower)TOF2JobStat::daqscr(2,crat-1,10);
  if(seqer)TOF2JobStat::daqscr(2,crat-1,11);
  if(cdpnod)TOF2JobStat::daqscr(2,crat-1,12);
//---------
  noerr=(!dataf 
             && !crcer 
	               && !asser 
		                && !amswer 
                                          && !timoer 
					            && !fpower 
						              && !seqer 
							               && cdpnod);
  if(noerr){
    TOF2JobStat::daqscr(2,crat-1,13);//<=== no errors in slave stat.word 
  }     
  else{
    goto Exit;
  }
//---------
  totbll+=len;//summing to have event(scint) data length
//
//
//---------> table processing (tof+acc):
//
//
  bias=1;
  _FoundPedBlks+=1;
//
  PedBlkReqw=(_PedBlkCrat[crat-1]==0);//true if requested 
  if(!PedBlkReqw){
    goto Exit;    
  }
  TOF2JobStat::daqscr(2,crat-1,14);// block requested
  while(bias<=90){//<---loop over ped-section words(=90,always present)
    word=*(p+bias);//ped 
    if(dpedin)nword=*(p+bias+90);//dped
    else nword=0;
    bias1=0;//add.bias to thresh-section
    if(dpedin)bias1+=90;
    if(ptrwin)bias1+=4;
    if(stawin)bias1+=10;
    if(thrsin)nnword=*(p+bias+90+bias1);//thrs
    else nnword=0;
    bias2=bias1;//add.bias to sig-section
    if(thrsin)bias2+=90; 
    nnnword=*(p+bias+90+bias2);//sig
    slid=(bias-1)%9;//position(in block),  define link_id(addr) (0,1,...8)
    slot=AMSSCIds::crdid2sl(crat-1,slid)+1;//slot-id to abs.slot-number(solid,sequential, 1,...,11)
    if(slot<=0 || slot==1 || slot==4 || slot>11){//check slot# validity
      if(TFREFFKEY.reprtf[3]>0){
        cout<<" <--- OnbPed: Error: InvalidSlot , crat/slot_id/slot="<<crat<<" "<<slid<<" "<<slot<<endl;
      }
      TOF2JobStat::daqscr(2,crat-1,15);//invalid slot number
      goto Exit;    
    }
    inch=int16u(floor(float(bias-1)/9)+1);// slot's input channel(1,...10)
    mtyp=1;//for subr. ich2rdch only mtyp=0/1 has meaning(==>time/charge measuring channel)
    rdch=AMSSCIds::ich2rdch(crat-1,slot-1,inch-1,mtyp);//outp(=readout) ch:0(if empty),1,2...
    ped=geant(word&0xFFFF)/8;//according to Kunine
    dped=geant(nword&0xFFFF)/8;
    thr=geant(nnword&0xFFFF)/8;
    sig=geant(nnnword&0xFFFF)/8;
    if(rdch>0){//<-- valid, not empty channel
      AMSSCIds scinid(crat-1,slot-1,rdch-1);
      swid=scinid.getswid();
      mtyp=scinid.getmtyp();//0->LTtime, 1->Q, 2->FT, 3->sumHT, 4->sumSHT
      pmt=scinid.getpmt();//0->anode, 1-3->dynode#
      il=swid/100000;//0,1,2,3,4
      mtyp=swid%10;
      if(il==0)dtyp=2;//anti
      else{
        dtyp=1;//tof
        il-=1;
      }
      ib=(swid%100000)/1000-1;//0,1,..
      is=(swid%1000)/100-1;//0,1
//---
      if(dtyp==1)TOFPedCalib::filltb(il, ib, is, pmt, ped, dped, thr, sig);//store values
      if(dtyp==2)ANTPedCalib::filltb(ib, is, ped, dped, thr, sig);//store values
    }//--->endof "rdch>0" check
    bias+=1; 
  }//--->endof block words loop
//
  TOF2JobStat::daqscr(2,crat-1,16);//requested+processed blocks(crates)
//---
  _PedBlkCrat[crat-1]=1;//mark processed(no errors) crate
  _GoodPedBlks+=1;//counts tot# of requested&processed PedBlocks per calib.sequence(=run ?)
// --->   call DB- and pedfile-writing if last block:
  if(_GoodPedBlks==_NReqEdrs 
                             || DAQEvent::CalibDone(2)  
	                     || _FoundPedBlks >= _NReqEdrs){//<---last(from requested) ped-block processed
    if(TFREFFKEY.reprtf[3]>0 && !DAQEvent::CalibDone(2))
                cout<<"<---- TOFOnbPedCalib Error: Vitaly's CalDone-flag not set when sequence is over !!!?"<<endl;
    nblkok=0;
    for(i=0;i<SCCRAT;i++)if(_PedBlkCrat[i]==1)nblkok+=1;
    if(nblkok==_NReqEdrs){// complete set of good blocks - call output
      outflg=((CALIB.SubDetInCalib/1000)%10);//->1/2/3->write2db/hist_only/write2file+hist
      tofout=((outflg==1) || (outflg>1 && TFREFFKEY.relogic[0]==7));
      accout=((outflg==1) || (outflg>1 && ATREFFKEY.relogic==4));
      if(tofout)TOFPedCalib::outptb(outflg);
      if(accout)ANTPedCalib::outptb(outflg);
      _CalFirstSeq=false;
    }
    if(TFREFFKEY.reprtf[3]>0 && TFREFFKEY.reprtf[4]>0){
      cout<<"<========= OnbPedCalib sequence is over: blocks found/requested/good:"<<_FoundPedBlks<<
	                                              " "<<_NReqEdrs<<" "<<_GoodPedBlks<<endl<<endl;
    }
  }//---<endof "last PedBlock processed"
//-------
  badexit=false;
Exit:
  if(badexit)TOF2JobStat::daqsfr(61);//count rejected entries(segments)
//    
}//--->endof critical
//
  return;  
}
//----------------------------------------------------
integer DAQS2Block::calcblocklength(integer ibl){
//imply compressed format for Tof and Acc !!!
  integer i,icr,isl,nadcd,iqm,totl(0);
  geant adca,adcd[TOF2GC::PMTSMX];
  integer tdch[TOF2GC::SCTHMX];
  integer hwidt;//CSIIII->Cr(1-4)|SeqSlot(1-5)|Inpch(1-5)LT||Inpch(6)FT|Inpch(7)SumHT|Inpch(8)SumSHT
  integer hwidq[4];//Q_hwid(A,D1,D2,D3 each coded as CSII(C=1-4, S=1-9(SFET(A,C)seq.slot#), II=1-10)
  integer qhpersl[9]={0,0,0,0,0,0,0,0,0};//q-hits per slot(9 seq.slots(links))
  integer thpersl[5]={0,0,0,0,0};//Time(LT,sumHT,sumSHT-hits per slot(5 seg.slots(link))
  TOF2RawSide *ptrt;
  Anti2RawEvent *ptra;
  ptrt=(TOF2RawSide*)AMSEvent::gethead()->getheadC("TOF2RawSide",0);//contains ped-subtracted data
  ptra=(Anti2RawEvent*)AMSEvent::gethead()->getheadC("Anti2RawEvent",0);//contains ped-subtracted data
//cout<<"-----> In CalcBlockLength:"<<endl;
//------>collect TOF-info:
  while(ptrt){// <--- loop over TOF RawSide hits
    hwidt=ptrt->gethidt();//CSIIII
    icr=(hwidt/100000);//1-4
    if((ibl+1)!=icr)goto NextTFObj;//skip other than ibl crates
    for(i=0;i<TOF2GC::PMTSMX+1;i++)hwidq[i]=ptrt->gethidq(i);//each =CSII
//q-hits:
    adca=ptrt->getadca();
    ptrt->getadcd(adcd);
    nadcd=ptrt->getnadcd();
    for(iqm=0;iqm<4;iqm++){//a,d1,d2,d3 loop
      isl=((hwidq[iqm]%1000)/100)-1;//seq.slot(link)# 0-8
      if(iqm==0 && adca>0){
        qhpersl[isl]+=1;
      }
      if(nadcd==0)break;
      if(iqm>0 && adcd[iqm-1]>0){
        qhpersl[isl]+=1;
      }
    }
//LTtime-hits:
    isl=((hwidt%100000)/10000)-1;//seq.slot(link)# 0-8
    thpersl[isl]+=(ptrt->getnstdc());//LT
NextTFObj:
    ptrt=ptrt->next();// take next RawSide hit
  }//  ---- end of RawSide hits loop ------->
/*
cout<<"<--- Calcblocklength:"<<endl;
cout<<"TofQhits/slot:"<<endl;
for(isl=0;isl<9;isl++)cout<<qhpersl[isl]<<" ";
cout<<endl;
cout<<"TofThits/slot:"<<endl;
for(isl=0;isl<TOF2GC::SCFETA;isl++)cout<<thpersl[isl]<<" ";
cout<<endl;
*/
//---------  
//--->FT-hits from static array(tof+anti):
  for(isl=0;isl<TOF2GC::SCFETA;isl++)thpersl[isl]+=TOF2RawSide::FThits[ibl][isl];//number of FT-hits
//--->sumHT-hits from static array(tof):
  for(isl=0;isl<TOF2GC::SCFETA-1;isl++)thpersl[isl]+=TOF2RawSide::SumHTh[ibl][isl];//number of sumHT-hits
//--->sumSHT-hits from static array(tof):
  for(isl=0;isl<TOF2GC::SCFETA-1;isl++)thpersl[isl]+=TOF2RawSide::SumSHTh[ibl][isl];//number of sumHT-hits
/*
cout<<"TofThits with FT/sumHT/sumSHT:"<<endl;
for(isl=0;isl<TOF2GC::SCFETA;isl++)cout<<thpersl[isl]<<" ";
cout<<endl;
*/
//------>collect ATC-info:
  int16u id;
  integer sector,isid;
  int16u mtyp(0),otyp(0);
//
  while(ptra){// <--- loop over TOF RawSide hits
    id=ptra->getid();// BBarSide
    sector=id/10-1;//bar 0-7
    isid=id%10-1;//0-1 (bot/top)
    mtyp=0;
    otyp=0;
    AMSSCIds antid(sector,isid,otyp,mtyp);//otyp=0(anode),mtyp=0(LT-time)
    icr=antid.getcrate();//0-3
    isl=antid.gettempsn();//sequential temper.sensor#(1,2,...,5, for ATC only =5)(=link !!!)
    isl-=1;//0-4
    if(ibl!=icr)goto NextACObj;//skip other than ibl crates
//q-hits:
    adca=ptra->getadca();
    if(adca>0)qhpersl[isl]+=1;
//LTtime-hits:
    thpersl[isl]+=(ptra->gettdct(tdch));
NextACObj:
    ptra=ptra->next();// take next RawEvent hit
  }//  ---- end of RawSide hits loop ------->
/*
cout<<"Tof+AccQhits/slot:"<<endl;
for(isl=0;isl<9;isl++)cout<<qhpersl[isl]<<" ";
cout<<endl;
cout<<"Tof+AccThits/slot:"<<endl;
for(isl=0;isl<TOF2GC::SCFETA;isl++)cout<<thpersl[isl]<<" ";
cout<<endl;
*/
//---------
  integer tlsl16,nh,qlen(0),tlen(0);
  for(isl=0;isl<9;isl++){
    if(qhpersl[isl]>0)qlen+=(1+qhpersl[isl]);//1 stay for link-header
  }
  qlen+=1;//add q-section header(q-nwords) - always present
//
  for(isl=0;isl<TOF2GC::SCFETA;isl++){
    tlsl16=0;//link-length in 16bits words
    nh=thpersl[isl]+1;//imply temperature as 1 additional hit
    tlsl16=nh+(nh+nh%2)/2;//each 2hits->3words; 1hit->2words !!!
    tlen+=(tlsl16+1);//+1 for link-header(always present as temperature)
  }
  tlen+=1;//add t-section header(t-nwords)
  totl=qlen+tlen+7+1;//7 stay for 4trpatt+3status; 1 stay for last status-word(slave_id+...)
//cout<<"TotLen="<<totl<<" qlen/tlen="<<qlen<<" "<<tlen<<endl<<endl;  
  return -totl;
}
//----------------------------------------------------
integer DAQS2Block::getmaxblocks(){//"2" to include a-b sides(may be present both)
  if(AMSJob::gethead()->isSimulation())return TOF2GC::SCCRAT;//only 1 side (a) in MC-daq
  else return 2*TOF2GC::SCCRAT;
}
//----------------------------------------------------
void DAQS2Block::buildblock(integer ibl, integer len, int16u *p){
//build Tof+Acc DAQ-block from MC TOF2RawSide/Anti2RawEvent-objects
//imply compressed format for Tof and Acc !!!
//
// on input: len=tot_block_length as was defined by call to calcblocklength
//           *p=pointer_to_beginning_of_block_data (word next to length)
//
  integer i,j,il,ib,is,icr,isl,ich,ichmx,iht,nadcd,iqm,lbbs;
  integer hwidt;//CSIIII->Cr(1-4)|SeqSlot(1-5)|Inpch(1-5)LT||Inpch(6)FT|Inpch(7)SumHT|Inpch(8)SumSHT
  integer hwidq[4];//Q_hwid(A,D1,D2,D3 each coded as CSII(C=1-4, S=1-9(SFET(A,C)seq.slot#), II=1-10)
  int16u sptpat[4]={0,0,0,0};
  int16u sptpator[4]={0,0,0,0};
  geant adca,adcd[TOF2GC::PMTSMX];
  integer nhts,nqhits(0),nthits(0);
  geant qdata[9][10];//9 links(slots, SFET(A,C)), 10 inp.channels
  integer tdata[TOF2GC::SCFETA][TOF2GC::SCTDCCH][TOF2GC::SCTHMX];//SCFETA(5)links,SCTDCCH(8)chan,SCTHMX(16)hits
  integer ntdata[TOF2GC::SCFETA][TOF2GC::SCTDCCH];//.............number of hits
  integer temdat[TOF2GC::SCFETA];//TDC-temperature in slots (coded as t1(12 msb)|t2(12 lsb))
  integer nftdc,nstdc,nsumh,nsumsh;
  integer tdch[TOF2GC::SCTHMX];
  geant temp,trat;
  integer t1,t2;
//
//cout<<"-----> In BuilBlock for crate="<<ibl+1<<endl;
  for(isl=0;isl<9;isl++){//SFET(A,C) seq.slot#(=link#)
    for(ich=0;ich<10;ich++)qdata[isl][ich]=0;
  }
  for(isl=0;isl<TOF2GC::SCFETA;isl++){
    for(ich=0;ich<TOF2GC::SCTDCCH;ich++){
      ntdata[isl][ich]=0;
      for(iht=0;iht<TOF2GC::SCTHMX;iht++)tdata[isl][ich][iht]=0;
    }
  }
//
  TOF2RawSide *ptrt;
  Anti2RawEvent *ptra;
  ptrt=(TOF2RawSide*)AMSEvent::gethead()->getheadC("TOF2RawSide",0);
  ptra=(Anti2RawEvent*)AMSEvent::gethead()->getheadC("Anti2RawEvent",0);
// 
//------>collect TOF-info:
  while(ptrt){// <--- loop over TOF RawSide hits
    hwidt=ptrt->gethidt();//CSIIII
    icr=(hwidt/100000);//1-4
    if((ibl+1)!=icr)goto NextTFObj;//skip other than ibl crates
    for(i=0;i<TOF2GC::PMTSMX+1;i++)hwidq[i]=ptrt->gethidq(i);
//q-hits:
    adca=ptrt->getadca();
    ptrt->getadcd(adcd);
    nadcd=ptrt->getnadcd();
    for(iqm=0;iqm<4;iqm++){//a,d1,d2,d3 loop
      isl=((hwidq[iqm]%1000)/100);//seq.slot(link)# 1-9
      ich=(hwidq[iqm]%100);//1-10
      if(iqm==0 && adca>0){
        qdata[isl-1][ich-1]=adca;
	nqhits+=1;
      }
      if(nadcd==0)break;
      if(iqm>0 && adcd[iqm-1]>0){
        qdata[isl-1][ich-1]=adcd[iqm-1];
	nqhits+=1;
      }
    }
//LTtime-hits:
    isl=((hwidt%100000)/10000);
    nstdc=ptrt->getnstdc();//LT
    if(nstdc>0){
      ich=((hwidt%10000)/1000);
      ptrt->getstdc(tdch);
      for(iht=0;iht<nstdc;iht++)tdata[isl-1][ich-1][iht]=tdch[iht];
      ntdata[isl-1][ich-1]=nstdc;
      nthits+=nstdc;
    }
NextTFObj:
    ptrt=ptrt->next();// take next RawSide hit
  }//  ---- end of RawSide hits loop ------->
//----
//--->FT-hits from static array(tof+anti):
  for(isl=0;isl<TOF2GC::SCFETA;isl++){
    nftdc=TOF2RawSide::FThits[ibl][isl];//number of FT-channel hits
    ich=5;//imply FT always sitting in ch-5(counting from 0)
    if(nftdc>0){
      for(iht=0;iht<nftdc;iht++)tdata[isl][ich][iht]=TOF2RawSide::FTtime[ibl][isl][iht];
      ntdata[isl][ich]=nftdc;
      nthits+=nftdc;
    }
    temp=TOF2Varp::tofvpar.Tdtemp();//default fast temper(celsium degr) from data card
    trat=(235-temp)/400;
    t2=2048;//lsb, take in the middle of range (12bits)
    t1=floor(trat*t2+0.5);//msb, +0.5 to round
    temdat[isl]=(t2&(0xFFFL));//12 lsb
    temdat[isl]|=((t1&(0xFFFL))<<12);//12 msb, now encoded
  }
//--->sumHT-hits from static array(tof):
  for(isl=0;isl<TOF2GC::SCFETA-1;isl++){
    nsumh=TOF2RawSide::SumHTh[ibl][isl];//number of sumHT-channel hits
    ich=6;//imply sumHT always sitting in ch-6(counting from 0)
    if(nsumh>0){
      for(iht=0;iht<nsumh;iht++)tdata[isl][ich][iht]=TOF2RawSide::SumHTt[ibl][isl][iht];
      ntdata[isl][ich]=nsumh;
      nthits+=nsumh;
    }
  }
//--->sumSHT-hits from static array(tof):
  for(isl=0;isl<TOF2GC::SCFETA-1;isl++){
    nsumsh=TOF2RawSide::SumSHTh[ibl][isl];//number of sumHT-channel hits
    ich=7;//imply sumSHT always sitting in ch-7(counting from 0)
    if(nsumsh>0){
      for(iht=0;iht<nsumsh;iht++)tdata[isl][ich][iht]=TOF2RawSide::SumSHTt[ibl][isl][iht];
      ntdata[isl][ich]=nsumsh;
      nthits+=nsumsh;
    }
  }
//
//--->SPT trig.patterns:
//
  integer trpat[TOF2GC::SCLRS],trpatz[TOF2GC::SCLRS];
  TOF2RawSide::getpatt(trpat);
  TOF2RawSide::getpattz(trpatz);
//cout<<"     Encoding MyTrPatt: trpat="<<hex<<trpat[0]<<" "<<trpat[1]<<" "<<trpat[2]<<" "<<trpat[3]<<dec<<endl;
//cout<<"                       trpatz="<<hex<<trpatz[0]<<" "<<trpatz[1]<<" "<<trpatz[2]<<" "<<trpatz[3]<<dec<<endl;
  for(integer bit=0;bit<20;bit++){
    lbbs=AMSSCIds::gettpbas(ibl,bit);//LBBS<->bit
    if(lbbs==0)continue;
    il=(lbbs/1000)-1;//0-3
    ib=((lbbs%1000)/10)-1;//0-9
    is=(lbbs%10)-1;//0-1
    if(bit<10){//fill 2nd/4th patt.words
      if((trpat[il]&(1<<(ib+16*is)))>0){
        sptpat[1]|=(1<<bit);
	sptpator[1]=1;
      }
      if((trpatz[il]&(1<<(ib+16*is)))>0){
        sptpat[3]|=(1<<bit);
	sptpator[3]=1;
      }
    }
    else{//fill 1st/3rd patt.word
      if((trpat[il]&(1<<(ib+16*is)))>0){
        sptpat[0]|=(1<<(bit-10));
	sptpator[0]=1;
      }
      if((trpatz[il]&(1<<(ib+16*is)))>0){
        sptpat[2]|=(1<<(bit-10));
	sptpator[2]=1;
      }
    }
  }
  if(TFMCFFKEY.trlogic[0]==1){
    for(i=0;i<4;i++){
      sptpat[i]|=(1<<14);//add masking(format) bit
      if(sptpator[i]==1)sptpat[i]|=(1<<12);//add "this card-side" CP(BZ)-bits OR 
    }
  } 
//cout<<"   SPT2pat="<<hex<<sptpat[0]<<" "<<sptpat[1]<<" "<<sptpat[2]<<" "<<sptpat[3]<<dec<<endl;
//-----------------------
//------>collect ATC-info:
  int16u id;
  integer sector,isid;
  int16u mtyp(0),otyp(0);
//
  while(ptra){// <--- loop over TOF RawSide hits
    id=ptra->getid();// BBarSide
    sector=id/10-1;//bar 0-7
    isid=id%10-1;//0-1 (bot/top)
    mtyp=0;
    otyp=0;
    AMSSCIds antid(sector,isid,otyp,mtyp);//otyp=0(anode),mtyp=0(LT-time)
    icr=antid.getcrate();//0-3
    isl=antid.gettempsn();//sequential temper.sensor#(1,2,...,5, for ATC only =5)(=link !!!)
    isl-=1;//0-4
    if(ibl!=icr)goto NextACObj;//skip other than ibl crates
    ich=antid.getinpch();//0-9(really for ACC 0-3)
//q-hits:
    adca=ptra->getadca();
    if(adca>0){
      qdata[isl][ich]=adca;
      nqhits+=1;
    }
//LTtime-hits:
    nstdc=ptra->gettdct(tdch);
    if(nstdc>0){
      for(iht=0;iht<nstdc;iht++)tdata[isl][ich][iht]=tdch[iht];
      ntdata[isl][ich]=nstdc;
      nthits+=nstdc;
    }
NextACObj:
    ptra=ptra->next();// take next RawEvent hit
  }//  ---- end of RawSide hits loop ------->
//
//(FT-hits and temperatures are already filled in TOF-section from common arrays)
//-----------------------
//---> now encode/write SC-block:
  int nwtot(0),nwtq(0),nwq(0),nwt(0);//daq(int16u) words 
  int16u adc,qbuf[11],mask(0),rrr;
  number adcf;
  integer adci;
// 
//--->Patterns:
  for(i=0;i<4;i++)*(p+i)=sptpat[i];
//2 statuses:
  *(p+4)=0;
//  *(p+5)=0;//event size +trunc.bit, set at the end
  *(p+6)=0;
  nwtot+=7;
//-------------  
//--->charge data:
  for(isl=0;isl<9;isl++){
    mask=0;
    nwq=0;
    for(i=0;i<11;i++)qbuf[i]=0;
    if(isl<TOF2GC::SCFETA)ichmx=5;//SFET(A) q-inp.channels
    else ichmx=10;//SFEC q-inp.channels
    for(ich=0;ich<ichmx;ich++){
      if(qdata[isl][ich]>0){
        adcf=8*qdata[isl][ich];
	adci=floor(adcf);
        adc=int16u(adci&0x7FFFL);//float_adc -> daq_adc
	qbuf[nwq+1]=adc;
	mask|=(1<<ich);
	nwq+=1;
      }
    }
    if(nwq>0){
      qbuf[0]=0x8000;//bit 15
      qbuf[0]|=int16u(isl);
      qbuf[0]|=(mask<<4);
      nwq+=1;//add link-header as 1st word of the buff
      for(j=0;j<nwq;j++)*(p+nwtot+1+nwtq+j)=qbuf[j];//buf->output(+1 reserv. for q-section header(Nqwtot) 
      nwtq+=nwq;//current q-links tot.length(incl. link-headers)
    }
//    cout<<"      link/qwords="<<isl<<" "<<nwq<<" nwtq="<<nwtq<<endl;
  }
  *(p+nwtot)=nwtq;//Qsection Nwords to output(always present)
  nwtq+=1;//+1 for Nwords-word itself
  nwtot+=nwtq;//
//cout<<"  <--- Qsection words written: "<<nwtq<<" totlen="<<nwtot<<endl<<endl;
//-------------
//--->time data:
  int16u nwtt(0);//tot.words(16bits) in T-section (excluding itself)
  integer thit,ithit;
  int16u word16[3] = {0};
  integer lbuf32[1+8*TOF2GC::SCTHMX];//link_buff(temp+8ch*nhitmx)=max 129 32bits-words if nhitmx=16
  int16u nlb32;//nwords in it
  int16u lhead(0);
//
  for(isl=0;isl<TOF2GC::SCFETA;isl++){//link loop
    lhead=0;
    ichmx=8;//SFET(A) max t-inp.channels
    if(isl==(TOF2GC::SCFETA-1))ichmx=6;
    lhead|=(0xC000);//add H/T-bits(in MC imply header+trailer always exist and correct)
    nwt=0;
    nlb32=0;
    lbuf32[nlb32]=temdat[isl];//put temper
    nlb32+=1;
    lhead|=(0x2000);//add temp.bit
    lhead|=int16u(isl);//add link#
    for(ich=0;ich<ichmx;ich++){//chan.loop to create daq-fmt 32bits time words in lbuf32
      nhts=ntdata[isl][ich];
      if(nhts==0)continue;//skip empty chan.
      for(iht=0;iht<nhts;iht++){
        thit=tdata[isl][ich][iht];//21 lsb only useful
	ithit=((thit&0x1FFFFCL)>>2);//19 msb
	ithit|=((thit&0x3L)<<19);//2 lsb of expantion
	ithit|=((ich&0x7L)<<21);//add ch# 3bits, now 24bits ithit is inverted as in TDC-format + ch#
	lbuf32[nlb32]=ithit;//lbuf32[i] REALLY KEEP 24 BITS  INFO in its lsb
	nlb32+=1;
      }
    }//--->endof chan.loop
//complete link-header:
    lhead|=(((nlb32-1)&0xFF)<<4);//add link's nhits (-1 for temper)
    *(p+nwtot+1+nwtt)=lhead;//lhead to output(+1 reservation for section header(N timewords)
    nwtt+=1;//counts link-header itself(always present)
    nwt+=1;
//
    for(int ibf=0;ibf<nlb32;ibf++){//lbuf32 words loop
      if(ibf%2==0){//1st,3rd,...32bits words in buf32
	word16[0]=int16u((lbuf32[ibf]&0xFFFF00L)>>8);//next 16 msb of buf32-word in 1st word16
        word16[1]=(int16u(lbuf32[ibf]&0xFFL)<<8);//8 lsb of buf32-word in 2nd word16(8msb)
      }
      else{//2nd,4th,...32bits words in buf32
        word16[2]=int16u(lbuf32[ibf]&0xFFFFL);//16 lsb of buf32-word in 3rd word16
	word16[1]|=int16u(((lbuf32[ibf]&0xFF0000L)>>16));//next 8 msb of buf32-word in 2nd word16(8lsb)
        *(p+nwtot+1+nwtt)=word16[0];//3 16bits time-words buffer to output on each 2nd hit(lbuf32 word)
        *(p+nwtot+1+nwtt+1)=word16[1];
        *(p+nwtot+1+nwtt+2)=word16[2];
//cout<<" 3w to outp:"<<hex<<word16[0]<<" "<<word16[1]<<" "<<word16[2]<<dec<<" nwt="<<nwt<<" nwtt="<<nwtt<<endl;
        nwtt+=3;
        nwt+=3;
      }
    }//--->endof lbuf32 words loop
//
    if(nlb32%2==1){//flush partially encoded 3-words buffer (only word[0-1])
      *(p+nwtot+1+nwtt)=word16[0];
      *(p+nwtot+1+nwtt+1)=word16[1];
//cout<<" flush2w to buf:"<<hex<<word16[0]<<" "<<word16[1]<<dec<<" nwt="<<nwt<<" nwtt="<<nwtt<<endl;
      nwtt+=2;
      nwt+=2;
    }
//    cout<<"        encoded in 16bits words:"<<nwt<<" nwtt="<<nwtt<<endl;
  }//--->endof links loop
  *(p+nwtot)=nwtt;//T-section Nwords to output(always present, T,L bits =0 in MC))
  nwtt+=1;//+1 fot Nwords-word itself
  nwtot+=nwtt;
//cout<<"  <--- Tsection words written: "<<nwtt<<" totlen="<<nwtot+1<<endl<<endl;
//--->seq_counter word:
  *(p+5)=nwtot;//tempor event size +trunc.bit
//======================> Add status word:
  rrr=int16u(getportid(int16u(ibl),0));//board side-A port(slave)id as seen by JINJ
  rrr|=(1<<5);//cdpnode(end of fragmentation)
  rrr|=(1<<7);//compr.fmt
  rrr|=(1<<15);//data-fragment
  *(p+nwtot)=rrr;
  nwtot+=1;//count block-id word
//cout<<"<----- Final length="<<nwtot<<endl<<endl;
//-------------  
  if(len != nwtot){
    cout<<"<----- DAQS2Block::buildblock: length-mismatch !!! for block "<<ibl<<endl;
    cout<<"       OnCall length="<<len<<" but really "<<nwtot<<" words was written "<<endl<<endl;
    exit(1);
  }
//---------------
  if(ibl==(TOF2GC::SCCRAT-1))  //clear RawEvent/Hit container after last block processed
  {
#ifdef __AMSDEBUG__

    AMSContainer *ptr1=AMSEvent::gethead()->getC("TOF2RawSide",0);
    if(ptr1)ptr1->eraseC();
//
    AMSContainer *ptr2=AMSEvent::gethead()->getC("Anti2RawEvent",0);
    if(ptr2)ptr2->eraseC();
//
//
#endif
  }
//--------------
}
