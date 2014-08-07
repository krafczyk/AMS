//  $Id$
// Simple version 9.06.1997 by E.Choumilov
// deep modifications Nov.2005 by E.Choumilov
// decoding tools added dec.2006 by E.Choumilov
//
#include "commons.h" 
#include "trigger102.h"
#include "event.h"
#include "user.h"
#include "mccluster.h"
#include "tofdbc02.h"
#include "antidbc02.h"
#include "ecaldbc.h"
#include "tofrec02.h"
#include "tofsim02.h"
#include "antirec02.h"
#include "ecalrec.h"
#include "ntuple.h"
#include <time.h>
#include "timeid.h"
using namespace std;
using namespace ecalconst;
//
 Trigger2LVL1::Scalers Trigger2LVL1::_scaler;
 Lvl1TrigConfig Trigger2LVL1::l1trigconf;
 ScalerMon Trigger2LVL1::scalmon;
 integer Trigger2LVL1::PhysBranchCount[8]={0,0,0,0,0,0,0,0};
 integer TGL1JobStat::countev[20];
 integer TGL1JobStat::daqc1[80];
 int16u Trigger2LVL1::nodeids[2]={14,15};//node addr for side_a/_b
 bool Trigger2LVL1::SetupIsChanged(false);
 bool Trigger2LVL1::ScalerIsChanged(false);
//
void Trigger2LVL1::build(){//called by sitrigevent() AND retrigevent()
//build lvl1-obj for MC; Complete(+rebuild) lvl1 for RealData.
// 
  integer PhysBPatt(0);//Lvl1 phys.branches pattern(e,p,gamma,unbiased,...,external; 8 in total)
  integer JMembPatt(0);//Lvl1 joined members pattern(FTC,FTZ,FTE,ACC0,...,BZ,ECAL-F_and,...; 16 in total)
  integer auxtrpat(0);
  int i,ibmx,ns1,ns2;
  integer tofpatt1[TOF2GC::SCLRS]={0,0,0,0};//all sides,FTC(z>=1)
  integer tofpatt2[TOF2GC::SCLRS]={0,0,0,0};//all sides,BZ(z>=2)
  geant rates[19]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};//TrigCompRates(Hz)
  integer tofcpcode(0),tofbzflag(0);
  integer toftrcode1(-1),toftrcode2(-1);//0->4planes,(1-4)->3plns,(5-8)->2plns, <0->none
  integer ftpatt;
  integer nanti=0;
  integer antipatt=0;
  integer ectrigfl=0;
  integer ectrconf=0;
  int16u ectrpatt[6][3];
  geant gmaglat,gmagphi;
  geant ectrsum(0);
  geant livetime(1),ratemx;
  uinteger trtime[5]={0,0,0,0,0};
  bool toftcp1(0);//tof.centr.area flag(1/0->centre/not),FTCP1(lut1)
  bool toftcp2(0);//tof.centr.area flag(1/0->centre/not),FTCP2(lut2)
  uinteger cbt,lsbit(1);
  int antismx[2];
  char mnames[16][4];
//
  TGL1JobStat::addev(0);
//-----------------------------------------------------------------------------------------
//=====> Reconstruction(MCdaq or RDdaq):
  if(AMSJob::gethead()->isReconstruction()){// <===== add some info to existing DAQ-created lvl1-obj
    Trigger2LVL1 *ptr=(Trigger2LVL1*)AMSEvent::gethead()->getheadC("TriggerLVL1",0);
    if(ptr){
      TGL1JobStat::addev(15);
      TOF2RawSide::getpatt(tofpatt1);//extract tof-patt
      TOF2RawSide::getpattz(tofpatt2);
      AMSEcalRawEvent::gettrpatt(ectrpatt);//masked trpatt[6][3]: max 3x16bits in 6 "trigger" sup.layers
      ectrsum=AMSEcalRawEvent::gettrsum();
      ptr->settofpat1(tofpatt1);//copy tof-patt into lev1-obj
      ptr->settofpat2(tofpatt2);
      ptr->setecpat(ectrpatt);
      ptr->setectrs(ectrsum);
      antipatt=ptr->getantipatt();//extract antipatt from lvl1-obj
      for(i=0;i<8;i++)if((antipatt & (1<<i))>0)nanti+=1;//count nsectors
      Anti2RawEvent::setpatt(uinteger(antipatt));//copy antipatt/nanti into AntiRawEvent-obj
      Anti2RawEvent::setncoinc(nanti);
      TGL1JobStat::addev(16);
//---
      if(SetupIsChanged){
#pragma omp critical (print_longsetup)
{ 
        TGL1JobStat::addev(17);
        cout<<"============================================================================"<<endl;
        cout<<"|      TrigConditions changed for Run/Event="<<AMSEvent::gethead()->getrun()<<" "<<
                                                                AMSEvent::gethead()->getid()<<endl;
        cout<<"                New settings are :"<<endl;
        cout<<"      Anabled ATC-sectors/sides:";
        for(i=0;i<ANTI2C::MAXANTI;i++)cout<<l1trigconf.antinmask(i)<<" ";
        cout<<endl;
        cout<<"              ATC-sectors side_OR(1)/AND(0) flags:";
        for(i=0;i<ANTI2C::MAXANTI;i++)cout<<l1trigconf.antoamask(i)<<" ";
        cout<<endl;
      
        cout<<"      Anabled ECAL [FTE(x|y)|ANG(x|y)]:"<<l1trigconf.ecprjmask()<<endl;
        cout<<"              ECAL-proj OR/AND(->1/2):"<<l1trigconf.ecorand()<<endl;
      
        cout<<"      TOF-Layers Sides_OR(1)/AND(0) flags(CP/BZ):"<<" ";
        for(i=0;i<TOF2GC::SCLRS;i++)cout<<l1trigconf.tofoamask(i)<<" ";
        cout<<" "; 
        for(i=0;i<TOF2GC::SCLRS;i++)cout<<l1trigconf.tofoazmask(i)<<" ";
        cout<<endl; 
      
        cout<<"      Anabled globFT-members pattern(FTE|FTZ|FTC<-):"<<l1trigconf.globftmask()<<endl;
        bool ftztbor=(l1trigconf.toflcsz()/10==1);
        bool ftz12or=((l1trigconf.toflcsz()%10)==2 || (l1trigconf.toflcsz()%10)==3); 
        bool ftz34or=((l1trigconf.toflcsz()%10)==1 || (l1trigconf.toflcsz()%10)==3); 
        cout<<"      FTZ-setting(OR/AND->1/0): Top/Bot:"<<ftztbor<<" L12:"<<ftz12or<<" L34:"<<ftz34or<<endl;
      
        cout<<"      TOF LUT1/LUT2/LUTBZ-settings:"<<l1trigconf.toflut1()<<"/"<<l1trigconf.toflut2()
                                                                              <<"/"<<l1trigconf.toflutbz()<<endl;
									    
        geant pwextt=geant(20*(1+(l1trigconf.tofextwid()&31)));//ext.width for top-coinc. signal
        geant pwextb=geant(20*(1+((l1trigconf.tofextwid()&(31<<5))>>5)));//ext.width for bot-coins. signal
        cout<<"      TOF-FTZ top/bot pulse ext.width(ns):"<<pwextt<<" "<<pwextb<<endl<<endl;
      
        cout<<"      All 8 Lev1MembersPatterns :"<<endl; 
        cout<<"|FTP0|FTP1|FTT0|FTT1| FTZ| FTE|ACC0|ACC1|  BZ|ECFA|ECFO|ECAA|ECAO|EXG0|EXG1|"<<endl;
        for(i=0;i<8;i++){
          for(int j=0;j<15;j++){
	    if((l1trigconf.physbrmemb(i)&1<<j)>0)cout<<"  X  ";
            else cout<<"  0  ";
          }
          cout<<endl;
        }
      
        cout<<"      Anabled Lev1PhysBranchesPatterns :"<<endl;
        cout<<"|  FTC| Z>=1| Z>=2|Z>=2s|Elect|Gamma|  FTE|Extrn|"<<endl;  
        for(i=0;i<8;i++){
          if((l1trigconf.globl1mask()&1<<i)>0)cout<<"   X  ";
          else cout<<"   0  ";
        }
        cout<<endl;
        cout<<"      with Prescale-factors :"<<endl;
        for(i=0;i<8;i++)if((l1trigconf.globl1mask()&1<<i)>0)cout<<l1trigconf.phbrprescf(i)<<" ";
        cout<<endl;
      
        cout<<"|     ACC NsectThresholds(accept Ns<=thr):"<<l1trigconf.antsectmx(0)<<"(equ) "
                                                       <<l1trigconf.antsectmx(1)<<"(pol)"<<endl; 
        cout<<"============================================================================"<<endl;
//---
        if(TGL1FFKEY.Lvl1ConfSave>0)l1trigconf.saveRD(TGL1FFKEY.Lvl1ConfSave);//save setup
//        SetupIsChanged=false;//reset flag (now reset in initevent)
}//---> endof pragma critical 
      }//--->endof "setup changed" check
    }//--->endof "trig.obj exist ?" check
//-------------------------------------------
    return;
  }
//----------------------------------------------------------------------------------------
//=====> Simulation:
  else{//<===== create lvl1-obj using subdet.simulated RawEvent objects
//
    strcpy(mnames[0],"FTC");
    strcpy(mnames[1],"CP0");
    strcpy(mnames[2],"CP1");
    strcpy(mnames[3],"CT0");
    strcpy(mnames[4],"CT1");
    strcpy(mnames[5],"FTZ");
    strcpy(mnames[6],"FTE");
    strcpy(mnames[7],"AC0");
    strcpy(mnames[8],"AC1");
    strcpy(mnames[9],"BZ ");
    strcpy(mnames[10],"EFa");
    strcpy(mnames[11],"EFo");
    strcpy(mnames[12],"EAa");
    strcpy(mnames[13],"EAo");
    strcpy(mnames[14],"EX0");
    strcpy(mnames[15],"EX1");
//
    antismx[0]=Trigger2LVL1::l1trigconf.antsectmx(0);//from DB/File(already redeined by DC if was requested)
    antismx[1]=Trigger2LVL1::l1trigconf.antsectmx(1);//..........................
//set some dummy rates(just for debug):
    for(i=0;i<5;i++)rates[i]=geant(scalmon.FTtrig(i));
    for(i=0;i<9;i++)rates[5+i]=geant(scalmon.LVL1trig(i));
    for(i=0;i<5;i++)rates[14+i]=geant(scalmon.DetMaxRate(i));
    ratemx=rates[5];//tempor LVL1-rate
//
//-->TOF:
    toftrcode1=TOF2RawSide::gettrcode();//<0 ->noFTC(z>=1), >=0 ->OK, masked
    toftrcode2=TOF2RawSide::gettrcodez();//<0 ->noBZ(z>=2), >=0 ->OK, masked
    ftpatt=TOF2RawSide::getftpatt();//GlobFT-patt (already masked during creation in  TofRawSide::mcbuild) !!!
    tofcpcode=TOF2RawSide::getcpcode();//IJ->lut2|lut1  (i(j)=1/0->used/not)
    tofbzflag=TOF2RawSide::getbzflag();//Logically:No BZ-flag if FTC missing
    TOF2RawSide::getpatt(tofpatt1);
    TOF2RawSide::getpattz(tofpatt2);
    if((TGL1FFKEY.printfl%10)>0){
#pragma omp critical (hf1)
{
      HF1(1290,geant(toftrcode1),1.);
      HF1(1291,geant(toftrcode2),1.);
}
    }
    if((TGL1FFKEY.printfl/10)>=3){
      cout<<endl;
      cout<<"===> In TrigLev1Build: FTpatt="<<ftpatt<<" toftrcode/toftrcodez="<<toftrcode1<<" "<<toftrcode2<<endl;
      cout<<"     tofcpcode="<<tofcpcode<<" bzflag="<<tofbzflag<<endl;
    }
//
//-->ECAL:
    ectrigfl=AMSEcalRawEvent::gettrfl();//Proj-masked(i.e. active proj based) TrigFlag MN : 
// M=0/1/2/3->FTE(Ener)Flag=No/NoFTE_when_1prj@2prj_req/FTE&1proj(or)/FTE&2proj(and),
// N=0/1/2/3->LVL1(Angle)Flag=Undef(noFTE)/0prj@FTEfound/OrLVL1/AndLVL1;
//                                         created in EcalRawEvent::mc_build for convenience
    ectrconf=AMSEcalRawEvent::gettrconf();//Proj-masked OR/AND-conf MN->Energy(FTE)|Angle(LVL1) :
//                                                                   M(N)=2/1/0=> AND/OR/No of proj
//                                         created in EcalRawEvent::mc_build for convenience
    AMSEcalRawEvent::gettrpatt(ectrpatt);//masked trpatt[6][3]: max 3x16bits in 6 "trigger" sup.layers
    ectrsum=AMSEcalRawEvent::gettrsum();
//   
//-->ANTI:
//    
    antipatt=Anti2RawEvent::getpatt();//bits (1-8)->sectors in coinc.with FT
    nanti=Anti2RawEvent::getncoinc();
//
//-->Event: latitude
//
    AMSEvent::gethead()->getmag(gmaglat,gmagphi);//(latt(thet,shirota),long(phi,dolgota))
//
    if((TGL1FFKEY.printfl/10)>=3){
      cout<<"     EC TrigDecisionFlag(M(fte)|N(lv1):"<<ectrigfl<<endl;
      cout<<"       M=0/1/2/3->FTE(Energy)Flag=No/NoFTE_when1prj@2prj_req/FTE&1prj(or)/FTE&2prj(and)"<<endl;
      cout<<"       N=0/1/2/3->LVL1(Angle)Flag=Undef(noFTE)/0prj@FTEfound/OrLVL1/AndLVL1"<<endl<<endl;
      cout<<"     EC ProjConfigFlag(MN->FTE|LVL1(1/2=or/and):"<<ectrconf<<endl;
      cout<<"     NumbAntiSectors/SectorsBitPatt="<<nanti<<" "<<hex<<antipatt<<dec<<endl;
      cout<<"     MagLat/Phi="<<gmaglat<<" "<<gmagphi<<endl;
    }
//------
    
    if(ftpatt==0 || !AMSmceventg::CalcLiveTime(0))return;// no global(FTC/FTZ/F
    
    TGL1JobStat::addev(1);
//--> create event's LVL1 trig-time:
    uinteger EventTime=AMSEvent::gethead()->gettime()-mktime(&AMSmceventg::Orbit.Begin);
    uinteger utime=AMSEvent::gethead()->getutime();
    trtime[1]=0;//reset counter (24-bits coarse time register)
    number fttime=TOF2RawSide::gettrtime();//FTtime in Geant-ns(0.5ns accur)
    uint64 trtime64=uint64(EventTime)*1000000000+utime+fttime;
    uint64 ifttime=trtime64/640;//in 0.64mks-bin time_tap_counter units

    trtime[2]=(ifttime & 0xFFFFFFFF);//32 lsb
    trtime[3]=(ifttime>>32);//8 msb
    trtime[0]=1562500;//1sec in 0.64mks units (calibration counter)
//
    if(ftpatt&(1)){//was FTC
      TGL1JobStat::addev(2);//was FTC-member
      if(tofbzflag==1)TGL1JobStat::addev(3);//was FTC+BZ
    }
//
    if((ftpatt&(1<<1))>0)TGL1JobStat::addev(4);//was FTZ-member
    if((ftpatt&(1<<1))>0 && (ftpatt&1)==0)TGL1JobStat::addev(5);//was FTZ-member when FTC missed
    if((ftpatt&(1<<2))>0){//was FTE-member
      TGL1JobStat::addev(6);
      if((TGL1FFKEY.printfl%10)>0){
#pragma omp critical (hf1)
{
        HF1(1097,geant(ectrconf),1.);
        if((ftpatt&3)==1)HF1(1096,geant(ectrconf),1.);
}
      }
    }
    if((ftpatt&(1<<2))>0 && (ftpatt&3)==0)TGL1JobStat::addev(7);//was FTE-member fhen TOF-FT missed
    if((ftpatt&(1<<3))>0)TGL1JobStat::addev(8);//was EXT-member
//---> create "central trigger"-like flag FTCT1(lut1)(exists in Lin's paper, useless):    
    if((ftpatt&1)>0){
      if(tofcpcode%10>0){//lut1
        integer nsc=0;
        ns1=0;
        ns2=0;
        for(int il=0;il<TOF2DBc::getnplns();il++){   // <-------- loop over layers
	  if(il!=2)continue;//outer Pads (1,10) are excluded in L3 only
          ibmx=TOF2DBc::getbppl(il);
          cbt=lsbit<<(ibmx-1);//last(in current il) paddle bit
          if((tofpatt1[il] & 1)>0 || (tofpatt1[il] & cbt)>0)ns1+=1;//s1 fired   
          if((tofpatt1[il] & 1<<16)>0 || (tofpatt1[il] & cbt<<16)>0)ns2+=1;//s2 fired
        }
	nsc=(tofpatt1[2]&0x1FE01FEL);//check central counters in L3 
        toftcp1=(nsc>0 || toftrcode1==3);//(L3 missing or at least central counters present in layer-trig)==central trig OK 
      }
//   
      if(tofcpcode/10>0){//lut2
        integer nsc=0;
        ns1=0;
        ns2=0;
        for(int il=0;il<TOF2DBc::getnplns();il++){   // <-------- loop over layers
	  if(il!=2)continue;//outer Pads (1,10) are excluded in L3 only
          ibmx=TOF2DBc::getbppl(il);
          cbt=lsbit<<(ibmx-1);//last(in current il) paddle bit
          if((tofpatt1[il] & 1)>0 || (tofpatt1[il] & cbt)>0)ns1+=1;//s1 fired   
          if((tofpatt1[il] & 1<<16)>0 || (tofpatt1[il] & cbt<<16)>0)ns2+=1;//s2 fired
        }
	nsc=(tofpatt1[2]&0x1FE01FEL);//check central counters in L3 
        toftcp2=(nsc>0 || toftrcode1==3);//(L3 missing or at least central counters present in layer-trig)==central trig OK 
      }
    }
//
//-->Make lvl1 JoinedMembersPattern(JMembPatt includes FTC, i.e all 16 bits):
//
    if((ftpatt&1)>0)JMembPatt|=1;//FTC
    if(tofcpcode%10>0)JMembPatt|=(1<<1);//FTCP0
    if(tofcpcode/10>0)JMembPatt|=(1<<2);//FTCP1
    if(toftcp1)JMembPatt|=(1<<3);//FTCT0 
    if(toftcp2)JMembPatt|=(1<<4);//FTCT1
    if((ftpatt&(1<<1)) > 0)JMembPatt|=(1<<5);//FTZ
    if((ftpatt&(1<<2)) > 0)JMembPatt|=(1<<6);//FTE
    if(nanti<=antismx[0])JMembPatt|=(1<<7);//ACC0(<1)
    if(nanti<=antismx[1])JMembPatt|=(1<<8);//ACC1(<5sectors)
    if(tofbzflag==1)JMembPatt|=(1<<9);//BZ
    if(ectrconf/10==2){
      JMembPatt|=(1<<10);//ECAL=F_and
      JMembPatt|=(1<<11);//set also ECAL=F_or when "and"
    }
    if(ectrconf/10==1)JMembPatt|=(1<<11);//ECAL=F_or
    if(ectrconf%10==2)JMembPatt|=(1<<12);//ECAL=A_and
    if(ectrconf%10==1)JMembPatt|=(1<<13);//ECAL=A_or
    if((ftpatt&(1<<3)) > 0)JMembPatt|=(1<<14);//Ext0
//
    if((TGL1FFKEY.printfl/10)>=3){
      cout<<"     JMembPatt(lsb->):"<<endl;
      for(i=0;i<16;i++){
        if((JMembPatt&(1<<i))>0)cout<<mnames[i]<<"|";
        else cout<<"000|";
      }
      cout<<endl;
    } 
//------
//
//    geant sumsc=_scaler.getsum(AMSEvent::gethead()->gettime());
//    geant livetime=_scaler.getlifetime(AMSEvent::gethead()->gettime());
//    integer tm=int(floor(TOF2Varp::tofvpar.getmeantoftemp(0)));   
//    if(livetime>1. && !MISCFFKEY.BeamTest && AMSJob::gethead()->isRealData() )
//                                                     AMSEvent::gethead()->seterror();
//------
//
    bool comtrok(0),BranchOK[8]={0,0,0,0,0,0,0,0};
//  if(fabs(gmaglat)<TGL1FFKEY.TheMagCut && nanti <= antismx[0])antiok=1;//equat.region
//  if(fabs(gmaglat)>TGL1FFKEY.TheMagCut && nanti <= antismx[1])antiok=1;//polar region
//
    integer brand[8];//ANDs of current and preset patterns of phys.branches
    integer brreq[8]; 
    for(i=0;i<8;i++){
      brreq[i]=Trigger2LVL1::l1trigconf.physbrmemb(i);//16bits patt for each phys.branch
      brand[i]=((JMembPatt>>1) & brreq[i]);//">>1" due to extra FTC-bit0 in JMembPatt wrt Request 
    }
//
//===> It is implied that globFT is always present, so below requirements are complementary !!!
// BranchOK[i](i=0,7) means that fired 16bits patt matched with ref.one for given phys.branch "i"
    BranchOK[0]=(brreq[0]>0 && brreq[0]==brand[0]);//unb_tof(ftc)                                
    BranchOK[1]=(brreq[1]>0 && brreq[1]==brand[1]);//Z>=1(ftc, anti)  
    BranchOK[2]=(brreq[2]>0 && brreq[2]==brand[2]);//Z>=2(ftc, bz)                              
    BranchOK[3]=(brreq[3]>0 && brreq[3]==brand[3]);//SlowZ>=2(ftz)
    BranchOK[4]=(brreq[4]>0 && brreq[4]==brand[4]);//e(ftc,fte, ecFand)
    BranchOK[5]=(brreq[5]>0 && brreq[5]==brand[5]);//gamma(fte, ecFand, ecAand)
    BranchOK[6]=(brreq[6]>0 && brreq[6]==brand[6]);//unb_ec(fte) 
    BranchOK[7]=(brreq[7]>0 && brreq[7]==brand[7]);//ext0
//
    char subln[8][7];
    strcpy(subln[0],"unbTOF");
    strcpy(subln[1],"Z>=1  ");
    strcpy(subln[2],"Z>=2  ");
    strcpy(subln[3],"slZ>=2");
    strcpy(subln[4],"electr");
    strcpy(subln[5],"gamma ");
    strcpy(subln[6],"unbEC ");
    strcpy(subln[7],"ext0  ");
    if((TGL1FFKEY.printfl/10)>=3){
      cout<<"     Fired Lvl1-PhysBranchesPatt(lsb->):"<<endl;
      for(int ibr=0;ibr<8;ibr++){
        if(BranchOK[ibr])cout<<subln[ibr]<<"|";
        else cout<<"000000|";
      }
      cout<<endl;
    }
//
    int nbreq(0);
    integer trtype(0);
    trtype=Trigger2LVL1::l1trigconf.globl1mask();//from DB/File(already corrected by DC if was requested)
    if((TGL1FFKEY.printfl/10)>=3)cout<<"     Requested Lvl1PhysBranchesPatt(hex)="<<hex<<trtype<<dec<<endl;
//
    if((TGL1FFKEY.printfl%10)>0){
#pragma omp critical (hf1)
{
      HF1(1098,0.,1.);
}
    }
    for(i=0;i<8;i++){
      if(BranchOK[i]){
        if((TGL1FFKEY.printfl%10)>0){
#pragma omp critical (hf1)
{
	  HF1(1098,geant(i+1),1.);//fired
}
	}
      }
      if(trtype & 1<<i)nbreq+=1;//count requested branches
    }
//
    if(nbreq==0 || trtype>128){//">128" means (ExtTr + smth else) = wrong
      cout<<"<=== Trigger2LVL1::build Error: unknown trigger type requested ! "<<trtype<<endl;
      exit(10);
    }
//
    if((TGL1FFKEY.printfl%10)>0){
#pragma omp critical (hf1)
{
      for(i=0;i<16;i++){
        if((JMembPatt&(1<<i))>0){
          HF1(1095,geant(i+1),1.);//fired(unmasked)
        }
      }
}
    }
//
//==========================> apply presc.factors:
    integer prescf;
#pragma omp critical (prescf)
{
    for(i=0;i<8;i++)if(BranchOK[i])PhysBranchCount[i]+=1;//count fired phys.branches
    if(!(trtype==1 || trtype==64 || trtype==65
                   || trtype==128 || TGL1FFKEY.NoPrescalingInMC==1)){//disable prescaling if unbiased/ext trig. requested
      for(i=0;i<8;i++){
        if(BranchOK[i]){
 	  prescf=Trigger2LVL1::l1trigconf.phbrprescf(i);
	  if((PhysBranchCount[i]%prescf)!=0)BranchOK[i]=false;//fired but ignored
        }
      }
    }
}
//------------------------------------------------
//
//                         <---- count (requested & fired) phys-branches(request=>trigger_type):
    int nbchk(0);
    if(trtype ==128){
      comtrok=1;
      PhysBPatt=128;
    }
    else{
      for(i=0;i<8;i++){
        if(TGL1FFKEY.NoMaskingInMC==1 && BranchOK[i])PhysBPatt|=(1<<i);//create fired-branch pattern(no masking)
        if((trtype & 1<<i)>0 && BranchOK[i]){
          nbchk+=1;
	  if(TGL1FFKEY.NoMaskingInMC==0)PhysBPatt|=(1<<i);//create (fired&requested=masked)-branch pattern
        }
      }
      if(nbchk>0)comtrok=1;
    }
//
    if(comtrok && AMSmceventg::CalcLiveTime(1)){
//
      livetime=AMSmceventg::LiveTime();
      trtime[4]=AMSmceventg::DT();
      TGL1JobStat::addev(9);
      if((ratemx<TGL1FFKEY.MaxScalersRate && livetime>TGL1FFKEY.MinLifeTime) || (auxtrpat > 0))
      {
//---> lvl1-decision taken:
        TGL1JobStat::addev(10);
//
      if((TGL1FFKEY.printfl%10)>0){
#pragma omp critical (hf1)
{
	HF1(1093,0.,1.);
        for(i=0;i<8;i++){
          if((PhysBPatt&(1<<i))>0)HF1(1093,geant(i+1),1.);//accepted
        }
        for(i=0;i<16;i++){
          if((JMembPatt&(1<<i))>0){
            HF1(1092,geant(i+1),1.);//accepted
          }
        }
}
      }
//
        AMSEvent::gethead()->addnext(AMSID("TriggerLVL1",0),
          new Trigger2LVL1(PhysBPatt,JMembPatt,auxtrpat,toftrcode1,toftrcode2,tofpatt1,tofpatt2,
	                         antipatt,ectrigfl,ectrpatt,ectrsum,livetime,rates,trtime));//create lvl1trig-object
      }
//<---
      else AMSEvent::gethead()->seterror();
    }
  }//--->endof "MC-data"
}
//--------------------
void Trigger2LVL1::init(){
//always called in _sitrig2initjob()/siamsinitjob()/AMSJob::init()
if((TGL1FFKEY.printfl%10)>0){
  if(AMSJob::gethead()->isSimulation()){
    HBOOK1(1290,"LVL1_Simu:TofFlg1",20,-1.,19.,0.);
    HBOOK1(1291,"LVL1_Simu:TofFlg2",20,-1.,19.,0.);
  }
  HBOOK1(1092,"LVL1:TrPatt_accep:ftc,cp0,cp1,ct0,ct1,ftz,fte,ac0,ac1,bz,ecfa,ecfo,ecaa,ecao,ext",20,0.,20.,0.);
  HBOOK1(1093,"LVL1:PhysBranchPatt_Accepted(F&R)(globFT,unbTOF,Z>=1,Z>=2,SlowZ>=2,elec,phot,unbEC,ext",10,0.,10.,0.);
  HBOOK1(1292,"LVL1_Reco:TofFlg1",20,-1.,19.,0.);
  HBOOK1(1293,"LVL1_Reco:TofFlg2",20,-1.,19.,0.);
  if(AMSJob::gethead()->isSimulation()){
   HBOOK1(1095,"LVL1:TrPatt_fired:ftc,cp0,cp1,ct0,ct1,ftz,fte,ac0,ac1,bz,ecfa,ecfo,ecaa,ecao,ext",20,0.,20.,0.);
   HBOOK1(1098,"LVL1:PhysBranchPatt_Fired(globFT,unbTOF,Z>=1,Z>=2,SlowZ>=2,elec,phot,unbEC,ext",10,0.,10.,0.);
   HBOOK1(1096,"LVL1:EC ProjConfig(when FTE&TOF_FT, masked)",30,0.,30.,0.);
   HBOOK1(1097,"LVL1:EC ProjConfig(when FTE, masked, val: M|N=FTE|ANG, M(N)=1/2->or/end_proj)",30,0.,30.,0.);
  }
  HBOOK1(1099,"DeltaEventTime(mksec)",100,0.,2000.,0.);
  HBOOK1(1094,"LiveTime(portion)",100,0.1,1.1,0.);
  HBOOK1(1294,"LVL1:TofPlaneSide-MaxRate(Hz)",100,0.,20000.,0.);
  HBOOK1(1295,"LVL1:FTC-rate(Hz)",100,0.,10000.,0.);
  HBOOK1(1296,"LVL1:FTZ-rate(Hz)",100,0.,500.,0.);
  HBOOK1(1297,"LVL1:FTE-rate(Hz)",100,0.,500.,0.);
  HBOOK1(1298,"LVL1:Lev1-rate(Hz)",100,0.,1000.,0.);
}
  TGL1JobStat::resetstat();
  SetupIsChanged=false;//reset new_setup flag
  ScalerIsChanged=false;//reset new_scalers flag
}
//--------------------
void Lvl1TrigConfig::read(){//read needed Lvl1TrigConfig-params(masks,...) from def.file
  char fname[1024];
  char name[256];
  char datt[3];
  char ext[80];
  int date[2],year,mon,day,hour,min,sec;
  uinteger iutct;
  tm begin;
  time_t utct;
  int endflab(0);
//
//reset:
 _globftmask=0;
 _globl1mask=0;
 _toflut1=0;
 _toflut2=0;
 _toflutbz=0;
 _toflcsz=0;
 for(int il=0;il<TOF2GC::SCLRS;il++){
   _tofoamask[il]=0;//reset mask
   _tofoazmask[il]=0;//reset mask
   for(int ib=0;ib<TOF2GC::SCMXBR;ib++){
     _tofinmask[il][ib]=0;//reset mask
     _tofinzmask[il][ib]=0;//reset mask
   }
 }
//
  strcpy(name,"Lvl1Conf");//generic Lvl1ConfData file name
  if(AMSJob::gethead()->isMCData()){
    cout <<"====> Trigger2LVL1::Init: LVL1TrigConf-data for MC are requested:"<<endl;
    strcpy(datt,"MC");
    sprintf(ext,"%d",TGL1FFKEY.Lvl1ConfMCVers);//MC-versn
  }
  else{
    cout <<"====> Trigger2LVL1::Init: LVL1TrigConf-data for RealData are requested:"<<endl;
    strcpy(datt,"RD");
    sprintf(ext,"%d",TGL1FFKEY.Lvl1ConfRDVers);//RD-utc
  }
  strcat(name,datt);
  strcat(name,".");
  strcat(name,ext);
//
   if(TGL1FFKEY.Lvl1ConfRead/10==0)strcpy(fname,AMSDATADIR.amsdatadir);//M
   if(TGL1FFKEY.Lvl1ConfRead/10==1)strcpy(fname,"");
   strcat(fname,name);
   cout<<"      Opening file : "<<fname<<'\n';
   ifstream tgfile(fname,ios::in); // open file for reading
   if(!tgfile){
     cerr <<"<---- Trigger2LVL1::Init: Missing Lvl1ConfData file "<<fname<<endl;
     exit(1);
   }
//---->>> TOF:
  for(int il=0;il<TOF2DBc::getnplns();il++){
    for(int ib=0;ib<TOF2DBc::getbppl(il);ib++){
      tgfile >> _tofinmask[il][ib];
      if(tgfile.eof()){
        cerr<<"<---- Trigger2LVL1::Init:Unexpected EOF reading TofInTrigMask(z>=1) !!!"<<endl;
	exit(1);
      }
    }
  }
//  
  for(int il=0;il<TOF2DBc::getnplns();il++){
    for(int ib=0;ib<TOF2DBc::getbppl(il);ib++){
      tgfile >> _tofinzmask[il][ib];
      if(tgfile.eof()){
        cerr<<"<---- Trigger2LVL1::Init:Unexpected EOF reading TofInTrigMask(z>=2) !!!"<<endl;
	exit(1);
      }
    }
  }
//  
  for(int il=0;il<TOF2DBc::getnplns();il++){
    tgfile >> _tofoamask[il];
    if(tgfile.eof()){
      cerr<<"<---- Trigger2LVL1::Init:Unexpected EOF reading Tof PlaneSidesOrAndMask(z>=1) !!!"<<endl;
      exit(1);
    }
  }
//  
  for(int il=0;il<TOF2DBc::getnplns();il++){
    tgfile >> _tofoazmask[il];
    if(tgfile.eof()){
      cerr<<"<---- Trigger2LVL1::Init:Unexpected EOF reading Tof PlaneSidesOrAndMask(z>=2) !!!"<<endl;
      exit(1);
    }
  }
//  
  tgfile >> _toflut1;
  if(tgfile.eof()){
    cerr<<"Trigger2LVL1::Init:Unexpected EOF reading TOF LayerConfLut1(z>=1) !!!"<<endl;
    exit(1);
  }
//  
  tgfile >> _toflut2;
  if(tgfile.eof()){
    cerr<<"<---- Trigger2LVL1::Init:Unexpected EOF reading TOF LayerConfLut2(z>=1) !!!"<<endl;
    exit(1);
  }
//
  tgfile >> _toflutbz;
  if(tgfile.eof()){
    cerr<<"<---- Trigger2LVL1::Init:Unexpected EOF reading TOF LayerConfLutbz(z>=2) !!!"<<endl;
    exit(1);
  }
//
  tgfile >> _toflcsz;
  if(tgfile.eof()){
    cerr<<"<---- Trigger2LVL1::Init:Unexpected EOF reading TOF LayerConfSlowZ(z>=2) !!!"<<endl;
    exit(1);
  }
//
  tgfile >> _tofextwid;
  if(tgfile.eof()){
    cerr<<"<---- Trigger2LVL1::Init:Unexpected EOF reading TOF SlowZExtWidth(z>=2) !!!"<<endl;
    exit(1);
  }
//----->>> ANTI:  
  for(int is=0;is<ANTI2C::MAXANTI;is++){
    tgfile >> _antinmask[is];
    if(tgfile.eof()){
      cerr<<"<---- Trigger2LVL1::Init:Unexpected EOF reading Anti InTrigMask !!!"<<endl;
      exit(1);
    }
  }
//-----
  for(int is=0;is<ANTI2C::MAXANTI;is++){
    tgfile >> _antoamask[is];
    if(tgfile.eof()){
      cerr<<"<---- Trigger2LVL1::Init:Unexpected EOF reading Anti OrAndMask !!!"<<endl;
      exit(1);
    }
  }
//-----
  for(int ip=0;ip<2;ip++){
    tgfile >> _antsectmx[ip];
    if(tgfile.eof()){
      cerr<<"Trigger2LVL1::Init:Unexpected EOF reading ANTI-sectors number limits !!!"<<endl;
      exit(1);
    }
  }
//----->>> EcalOrAndFlag and ActiveProjMask:
  tgfile >> _ecorand;
  if(tgfile.eof()){
    cerr<<"<---- Trigger2LVL1::Init:Unexpected EOF reading EcalProjOrAndFlag !!!"<<endl;
    exit(1);
  }
//-----
  tgfile >> _ecprjmask;
  if(tgfile.eof()){
    cerr<<"Trigger2LVL1::Init:Unexpected EOF reading EcalActiveProjMask !!!"<<endl;
    exit(1);
  }
//----->>> globalFastTrig:  
  tgfile >> _globftmask;
  if(tgfile.eof()){
    cerr<<"<---- Trigger2LVL1::Init:Unexpected EOF reading GlobalFTrigMask !!!"<<endl;
    exit(1);
  }
//----->>> globalLVL1:  
  tgfile >> _globl1mask;
  if(tgfile.eof()){
    cerr<<"<---- Trigger2LVL1::Init:Unexpected EOF reading lLvl1SubTrigMask !!!"<<endl;
    exit(1);
  }
//----->>> TrigMembSetting each of 8 PhysBranches:
  for(int ip=0;ip<8;ip++){
    tgfile >> _physbrmemb[ip];
    if(tgfile.eof()){
      cerr<<"Trigger2LVL1::Init:Unexpected EOF reading  !!!"<<endl;
      exit(1);
    }
  }
//----->>> Phys.branches prescale-factors:  
  for(int ip=0;ip<8;ip++){
    tgfile >> _phbrprescf[ip];
    if(tgfile.eof()){
      cerr<<"<---- Trigger2LVL1::Init:Unexpected EOF reading PhysBranchesPrescFactors !!!"<<endl;
      exit(1);
    }
  }
//-----
  if(AMSJob::gethead()->isMCData()){
    tgfile >> date[0];//YYYYMMDD beg.validity of Lvl1ConfMC.ext file
    if(tgfile.eof()){
      cerr<<"<---- Trigger2LVL1::Init:Unexpected EOF reading validity date !!!"<<endl;
      exit(1);
    }
    tgfile >> date[1];//HHMMSS beg.validity of Lvl1ConfMC.ext file
    if(tgfile.eof()){
      cerr<<"<---- Trigger2LVL1::Init:Unexpected EOF reading validity date !!!"<<endl;
      exit(1);
    }
    year=date[0]/10000;//2004->
    mon=(date[0]%10000)/100;//1-12
    day=(date[0]%100);//1-31
    hour=date[1]/10000;//0-23
    min=(date[1]%10000)/100;//0-59
    sec=(date[1]%100);//0-59
    begin.tm_isdst=0;
    begin.tm_sec=sec;
    begin.tm_min=min;
    begin.tm_hour=hour;
    begin.tm_mday=day;
    begin.tm_mon=mon-1;
    begin.tm_year=year-1900;
    utct=mktime(& begin);
    iutct=uinteger(utct);
    cout<<"      Lvl1ConfMC-file begin_date: year:month:day = "<<year<<":"<<mon<<":"<<day<<endl;
    cout<<"                                     hour:min:sec = "<<hour<<":"<<min<<":"<<sec<<endl;
    cout<<"                                         UTC-time = "<<iutct<<endl;
  }
  else{
    utct=time_t(TGL1FFKEY.Lvl1ConfRDVers);
    printf("      Lvl1ConfRD-file begin_date: %s",ctime(&utct)); 
  }
//
  tgfile>>endflab;
//
  tgfile.close();
//
  if(endflab==12345){
    cout<<"<---- Lvl1Config-data are successfully read !"<<endl;
  }
  else{cout<<"<---- Error reading Lvl1Config-data !"<<endl;
    exit(1);
  }
//
}
//--------------------
void Lvl1TrigConfig::redefbydc(){
//called from siamsinitevent(), redefine in memory some trigconfig settings by DataCards
  integer lut1,lut2,toflc,toflcz,toflcsz,cftmask,ftpatreq(0),sorand,ewcode;
  integer i,j,k,l,trtype;
  geant pwextt,pwextb;
//---
  trtype=TGL1FFKEY.trtype;
  if(trtype>=0){
    if(trtype>0 && trtype<=128)Trigger2LVL1::l1trigconf.globl1mask()=trtype;//redef required PhysBrMembers
    else{
      cout<<"<---L1TrigConfig:redefbydc: Error: bad PhysBranches requested by data card !!!"<<endl;
      exit(1);
    }
  }
  if(TGL1FFKEY.toflc>=0){//redef FTC LUTs settings by request from data card
    toflc=TGL1FFKEY.toflc;
    if(toflc==0){//>=3of4
      lut1=(1<<7)+(1<<11)+(1<<13)+(1<<14)+(1<<15);//>=3 layers combinations
      lut2=(1<<15);//4 layers combination
    }
    else if(toflc==1){//4of4
      lut1=(1<<15);//4 layers combination
      lut2=0;//none
    }
    else if(toflc==2){//>=2of4, but top&bot
      lut1=(1<<5)+(1<<6)+(1<<9)+(1<<10)+(1<<7)+(1<<11)+(1<<13)+(1<<14)+(1<<15);//(13+23+14+24)+(>=3of4) comb.
      lut2=0;//
    }
    else if(toflc==3){//any >=2of4
      lut1=(1<<5)+(1<<6)+(1<<9)+(1<<10)+(1<<3)+(1<<12)//2of4(13+23+14+24+12+34) 
                 +(1<<7)+(1<<11)+(1<<13)+(1<<14)+(1<<15);// + >=3of4 comb.
      lut2=0;//
//these 2 lines is special case: LUT2:0, LUT1:"texas"
//      lut2=0;//Texas:
//      lut1=(1<<2)+(1<<6)+(1<<10)+(1<<14);//"no_1" + "yes_2" combinations
    }
    else{
      cout<<"    Lvl1TrigConfig::redefbydc: Error:bad FTC-layers conf. requested by data card !!!"<<endl;
      exit(1);
    }
    Trigger2LVL1::l1trigconf.toflut1()=lut1;
    Trigger2LVL1::l1trigconf.toflut2()=lut2;
  }
//---
  if(TGL1FFKEY.toflcz>=0){//redef BZ LUT setting by request from data card
    toflcz=TGL1FFKEY.toflcz;
    if(toflcz==0){//>=3of4
      lut1=(1<<7)+(1<<11)+(1<<13)+(1<<14)+(1<<15);
    }
    else if(toflcz==1){//4of4
      lut1=(1<<15);//4 layers combination
    }
    else if(toflcz==2){//>=2of4(at least 1top and 1bot)
      lut1=(1<<5)+(1<<6)+(1<<7)+(1<<9)+(1<<10)+(1<<11)+(1<<13)+(1<<14)+(1<<15); 
    }
    else{
      cout<<"    Lvl1TrigConfig::redefbydc: Error:bad Lev1 BZ-layers conf. requested by data card !!!"<<endl;
      exit(1);
    }
    Trigger2LVL1::l1trigconf.toflutbz()=lut1;
  }
//---
  if(TGL1FFKEY.toflcsz>=0){//redef FTZ(slowZ>=2) layers logic by request from DC
    toflcsz=TGL1FFKEY.toflcsz;//IJ(J=0/1/2/3->topAND*botAND/topAND*botOR/topOR*botAND/topOR*botOR;I=0/1=>"*"=AND/OR)
    if((toflcsz/10)<=1 && (toflcsz%10)<=3)Trigger2LVL1::l1trigconf.toflcsz()=toflcsz;
    else{
      cout<<"    Lvl1TrigConfig::redefbydc: Error:bad F-layers conf. requested by data card !!!"<<endl;
      exit(1);
    }
  }
//---
  if(TGL1FFKEY.tofextwid>=0){//redef FTZ(slowZ>=2) layers logic by request from DC
    ewcode=TGL1FFKEY.tofextwid;
    pwextt=geant(20*(1+(ewcode&31)));//ext.width for top-coinc. signal
    pwextb=geant(20*(1+((ewcode&(31<<5))>>5)));//ext.width for bot-coins. signal
    if(pwextt>=260 && pwextb>=260 && pwextt<=640 && pwextb<=640) 
                                                          Trigger2LVL1::l1trigconf.tofextwid()=ewcode;
    else{
      cout<<"    Lvl1TrigConfig::redefbydc: Error:bad FTZ ext.width requested by data card !!!"<<endl;
      exit(1);
    }
  }
//---
  if(TGL1FFKEY.cftmask>=0){//redef active globFTrig  branches
    cftmask=TGL1FFKEY.cftmask;// decimal IJK->FTE|FTZ|FTC
    if(cftmask%10>0)ftpatreq|=1;//create binary analog of cftmask
    if((cftmask%100)/10>0)ftpatreq|=(1<<1);
    if(cftmask/100>0)ftpatreq|=(1<<2);
    if(ftpatreq==0){
      cout<<"    Lvl1TrigConfig::redefbydc: Error:bad glob.FTmemb.mask requested by data card !!!"<<endl;
      exit(1);
    }
    Trigger2LVL1::l1trigconf.globftmask()=cftmask;// decimal IJK->FTE|FTZ|FTC
  }
//---
  if(TGL1FFKEY.tofsc>=0){//redef TofPlaneSidesOrAnd from data-card(FTC,BZ)
    sorand=TGL1FFKEY.tofsc;//MN (m(n)=0/1-> plane two-sides-AND/OR selection, n->FTC, m->BZ)
    if((sorand/10)<=1 && (sorand%10)<=1){
      for(int il=0;il<TOF2GC::SCLRS;il++){
        if((sorand/10)==0)Trigger2LVL1::l1trigconf.tofoamask(il)=0;//and
        else Trigger2LVL1::l1trigconf.tofoamask(il)=1;//or
        if((sorand%10)==0)Trigger2LVL1::l1trigconf.tofoazmask(il)=0;//and
        else Trigger2LVL1::l1trigconf.tofoazmask(il)=1;//or
      }
    }
    else{
      cout<<"    Lvl1TrigConfig::redefbydc: Error:bad TofPlanes SidesOrAnd conf. requested by data card !!!"<<endl;
      exit(1);
    }
  }  
//---
  if(TGL1FFKEY.antismx[0]>=0){//redef ACC multipl.thresh.settings
    if(TGL1FFKEY.antismx[0]<=8)Trigger2LVL1::l1trigconf.antsectmx(0)=TGL1FFKEY.antismx[0];
    else{
      cout<<"    Lvl1TrigConfig::redefbydc: Error:bad ACC mult.threshold1 requested by data card !!!"<<endl;
      exit(1);
    }
    if(TGL1FFKEY.antismx[1]<=8)Trigger2LVL1::l1trigconf.antsectmx(1)=TGL1FFKEY.antismx[1];
    else{
      cout<<"    Lvl1TrigConfig::redefbydc: Error:bad ACC mult.threshold2 requested by data card !!!"<<endl;
      exit(1);
    }
  }
//---
  if(TGL1FFKEY.antisc>=0){//redef ACC sides OR/AND setting
    sorand=TGL1FFKEY.antisc;
    if(sorand<=1){
      for(int is=0;is<ANTI2C::MAXANTI;is++){
        if(sorand==0)Trigger2LVL1::l1trigconf.antoamask(is)=0;//And
        else Trigger2LVL1::l1trigconf.antoamask(is)=1;//Or
      }
    }
    else{
      cout<<"    Lvl1TrigConfig::redefbydc: Error:bad ACC-sides OR/AND requested by data card !!!"<<endl;
      exit(1);
    }
  }
//---
  integer orand;
  if(TGL1FFKEY.ecorand>=0){//redef ECproj or/and
    orand=TGL1FFKEY.ecorand;//proj-or/and(1/2) according to  data-card
    if(orand>=1 && orand<=2)Trigger2LVL1::l1trigconf.ecorand()=orand;
    else{
      cout<<"    LvTrigger2LVL1::l1TrigConfig::redefbydc: Error:bad ECproj OR/AND requested by data card !!!"<<endl;
      exit(1);
    }
  }
  integer prjmsk,ij,kl;
  if(TGL1FFKEY.ecprjmask>=0){//redef active EC-proj mask for FTE/LVL1
    prjmsk=TGL1FFKEY.ecprjmask;//active proj.mask from data-card (lkji)
    ij=(prjmsk%100);//FTE xy
    i=(ij%10);//x
    j=(ij/10);//y
    kl=(prjmsk/100);//LVL1 xy
    k=(kl%10);//x
    l=(kl/10);//y
    if(i<2 && j<2 && k<2 && l<2){
      Trigger2LVL1::l1trigconf.ecprjmask()=prjmsk;//proj.mask(lkji: ij=1/0->XYproj active/disabled in FTE; kl=same for LVL1(angle)
    }
    else{
      cout<<"    Lvl1TrigConfig::redefbydc: Error:bad ECproj active-mask requested by data card !!!"<<endl;
      exit(1);
    }
  }
}
//---------
void Lvl1TrigConfig::saveRD(int flg){//save current Lvl1TrigConfig-params(masks,...) to def.file(+DB)
//
//flg=1/2-> save2file/+DB
  integer endflab(12345);
  char fname[1024];
  char name[256];
  char buf[20];
  static integer runno(-1);
//
  time_t begin=AMSEvent::gethead()->gettime();//begin time = 1st_event_time(filled at 1st "ped-block" arrival)
  uinteger runn=AMSEvent::gethead()->getrun();//1st event run# 
  time_t end,insert;
  char DataDate[30],WrtDate[30];
  strcpy(DataDate,asctime(localtime(&begin)));
  time(&insert);
  strcpy(WrtDate,asctime(localtime(&insert)));
//
  cout<<"====> Lvl1TrigConfig::saveRD: Config is going to be saved in file and DB !!!"<<endl;
  if(runno!=int(runn)){
    cout<<"      New/old Runn="<<runn<<"/"<<runno<<endl;
    if(uinteger(begin)!=runn){
      cout<<"<---- Lvl1TrigConfig::saveRD: Warning - RunN/1stEvtTime mismatch, 1stEvtT:"<<uinteger(begin)<<endl;
    }
    runno=runn;
  }
  else {
    cout<<"<---- Lvl1TrigConfig::saveRD: Warning - repeated run number - ignore !"<<endl;
    return;
  }
//
// ---> prepare update of DB :
  if(flg==2){//Update DB "on flight"
    AMSTimeID *ptdv;
    ptdv = AMSJob::gethead()->gettimestructure(AMSID("Lvl1Config",AMSJob::gethead()->isRealData()));
    ptdv->UpdateMe()=1;
    ptdv->UpdCRC();
    time(&insert);
    end=begin+86400*30;//30 days ???
    ptdv->SetTime(insert,begin,end);
//
    if(AMSFFKEY.Update==2 ){
      AMSTimeID * offspring = 
         (AMSTimeID*)((AMSJob::gethead()->gettimestructure())->down());//get 1st timeid instance
      while(offspring){
        if(offspring->UpdateMe())cout << "         Start update Lvl1Config DB "<<*offspring; 
        if(offspring->UpdateMe() && !offspring->write(AMSDATADIR.amsdatabase))
        cerr <<"<---- Problem To Update Lvl1Config in DB"<<*offspring;
        offspring=(AMSTimeID*)offspring->next();//get one-by-one
      }
    }
  }
// ---> write Lvl1Cinfig to file:
  if(flg==1 || flg==2){
    strcpy(name,"Lvl1ConfRD.");//
    sprintf(buf,"%d",runn);
    strcat(name,buf);
    strcpy(fname,name);//creat in current dir.
    cout<<"       Opening file for writing : "<<fname<<'\n';
    cout<<"       Date of the first used event : "<<DataDate<<endl;
    cout<<"       Date of the file writing   : "<<WrtDate<<endl;
    cout<<"       RunN="<<runn<<endl;
    ofstream icfile(fname,ios::out|ios::trunc); // open config-file for writing
    if(!icfile){
      cerr <<"<---- Problems to write new Lvl1ConfRD-file !!? "<<fname<<endl;
      exit(1);
    }
    icfile.setf(ios::fixed);
//
//---->>> TOF:
    for(int il=0;il<TOF2DBc::getnplns();il++){
      for(int ib=0;ib<TOF2DBc::getbppl(il);ib++){
        icfile << _tofinmask[il][ib] <<" ";
      }
      icfile << endl;
    }
    icfile << endl;
//  
    for(int il=0;il<TOF2DBc::getnplns();il++){
      for(int ib=0;ib<TOF2DBc::getbppl(il);ib++){
        icfile << _tofinzmask[il][ib] <<" ";
      }
      icfile << endl;
    }
    icfile << endl;
//  
    for(int il=0;il<TOF2DBc::getnplns();il++){
      icfile << _tofoamask[il] <<" ";
    }
    icfile << endl;
    icfile << endl;
//  
    for(int il=0;il<TOF2DBc::getnplns();il++){
      icfile << _tofoazmask[il] <<" ";
    }
    icfile << endl;
    icfile << endl;
//  
    icfile << _toflut1;
    icfile << endl;
    icfile << endl;
//  
    icfile << _toflut2;
    icfile << endl;
    icfile << endl;
//
    icfile << _toflutbz;
    icfile << endl;
    icfile << endl;
//
    icfile << _toflcsz;
    icfile << endl;
    icfile << endl;
//
    icfile << _tofextwid;
    icfile << endl;
    icfile << endl;
//----->>> ANTI:  
    for(int is=0;is<ANTI2C::MAXANTI;is++){
      icfile << _antinmask[is] <<" ";
    }
    icfile << endl;
    icfile << endl;
//-----
    for(int is=0;is<ANTI2C::MAXANTI;is++){
      icfile << _antoamask[is] <<" ";
    }
    icfile << endl;
    icfile << endl;
//-----
    for(int ip=0;ip<2;ip++){
      icfile << _antsectmx[ip] <<" ";
    }
    icfile << endl;
    icfile << endl;
//----->>> EcalOrAndFlag and ActiveProjMask:
    icfile << _ecorand;
    icfile << endl;
    icfile << endl;
//-----
    icfile << _ecprjmask;
    icfile << endl;
    icfile << endl;
//----->>> globalFastTrig:  
    icfile << _globftmask;
    icfile << endl;
    icfile << endl;
//----->>> globalLVL1:  
    icfile << _globl1mask;
    icfile << endl;
    icfile << endl;
//----->>> TrigMembSetting each of 8 PhysBranches:
    for(int ip=0;ip<8;ip++){
      icfile << _physbrmemb[ip];
      icfile << endl;
    }
    icfile << endl;
//----->>> Phys.branches prescale-factors:  
    for(int ip=0;ip<8;ip++){
      icfile << _phbrprescf[ip] <<" ";
    }
    icfile << endl;
    icfile << endl;
//
    icfile << endflab;
    icfile << endl;
//
    icfile << endl<<"======================================================"<<endl;
    icfile << endl<<" Date of the 1st used event : "<<DataDate<<endl;
    icfile << endl<<" Date of the file writing   : "<<WrtDate<<endl;
    icfile.close();
//
    cout<<"<---- Real Lvl1TrigConfig data are successfully saved !!!"<<endl<<endl;
  }//--->endof file writing 
}
//---------
void ScalerMon::setdefs(){
// called by sitrig2initjob(for real data also)
//
  if(AMSJob::gethead()->isMCData()){//arbitrary mc-values(just to debugg)
    _FTtrig[0]=10;
    _FTtrig[1]=10;
    _FTtrig[2]=10;
    _FTtrig[3]=10;
    _FTtrig[4]=0;
    _LVL1trig[0]=1;
    _LVL1trig[1]=1;
    _LVL1trig[2]=1;
    _LVL1trig[3]=1;
    _LVL1trig[4]=1;
    _LVL1trig[5]=1;
    _LVL1trig[6]=1;
    _LVL1trig[7]=1;
    _LVL1trig[8]=0;
    _LiveTime[0]=1;
    _LiveTime[1]=1;
    _DetMaxRate[0]=1;
    _DetMaxRate[1]=1;
    _DetMaxRate[2]=1;
    _DetMaxRate[3]=1;
    _DetMaxRate[4]=1;
    _TimeCalib=0;
    _tgatelt=0;
    _tgatetr=0;
    _tgatesc=0;
/*
    for(int i=0;i<5;i++)_SPtrig[i]=0;
    for(int i=0;i<4;i++){
      _CPside1[i]=100;
      _CPside2[i]=100;
      _CTside1[i]=50;
      _CTside2[i]=50;
      _BZside1[i]=15;
      _BZside2[i]=15;
    }
    for(int i=0;i<8;i++){
      _AntiBot[i]=20;
      _AntiTop[i]=20;
    }
    _ECftProj[0]=60;
    _ECftProj[1]=60;
    _ECl1Proj[0]=30;
    _ECl1Proj[1]=30;
    _TrigTimeT=9999;
    _ScalTimeT=7777;
    _TrigFPGAid=11;
    _ScalFPGAid=9;
*/
  }
  else if(AMSJob::gethead()->isRealData()){//set "-1" as "undefined"
    for(int i=0;i<5;i++)_FTtrig[i]=-1;
    for(int i=0;i<9;i++)_LVL1trig[i]=-1;
    for(int i=0;i<5;i++)_DetMaxRate[i]=-1;    
    _TimeCalib=0;
    _tgatelt=0;
    _tgatetr=0;
    _tgatesc=0;
/*
    for(int i=0;i<5;i++)_SPtrig[i]=-1;
    for(int i=0;i<4;i++){
      _CPside1[i]=-1;
      _CPside2[i]=-1;
      _CTside1[i]=-1;
      _CTside2[i]=-1;
      _BZside1[i]=-1;
      _BZside2[i]=-1;
    }
    for(int i=0;i<8;i++){
      _AntiBot[i]=-1;
      _AntiTop[i]=-1;
    }
    _ECftProj[0]=-1;
    _ECftProj[1]=-1;
    _ECl1Proj[0]=-1;
    _ECl1Proj[1]=-1;
    _TrigTimeT=-1;
    _ScalTimeT=-1;
    _LiveTime[0]=1;
    _LiveTime[1]=1;
    _TrigFPGAid=0;
    _ScalFPGAid=0;
*/
  }
}  
//---------
  void TGL1JobStat::resetstat(){
    for(int i=0;i<20;i++)countev[i]=0;
    for(int i=0;i<80;i++)daqc1[i]=0;
  }
//
  void TGL1JobStat::addev(int i){
    assert(i>=0 && i< 20);
#pragma omp critical (l1_addev) 
    countev[i]+=1;
  }
//
  void TGL1JobStat::daqs1(integer info){
#ifdef __AMSDEBUG__
      assert(info>=0 && info<70 );
#endif
#pragma omp critical (l1_daqs1) 
    daqc1[info]+=1;
  }
//
void TGL1JobStat::printstat(){
if((TGL1FFKEY.printfl%10)>0){
  if(AMSJob::gethead()->isSimulation()){
    HPRINT(1290);
    HPRINT(1291);
    HPRINT(1097);
    HPRINT(1096);
    HPRINT(1098);
    HPRINT(1095);
  }
  HPRINT(1292);
  HPRINT(1293);
  HPRINT(1099);
  HPRINT(1092);
  HPRINT(1093);
  HPRINT(1094);
  HPRINT(1294);
  HPRINT(1295);
  HPRINT(1296);
  HPRINT(1297);
  HPRINT(1298);
}
  printf("\n");
  printf("    ============ LVL1-PreDecoding statistics =============\n");
  printf("\n");
  printf(" LVL1-segment entries                : % 8d\n",daqc1[40]);
  if(daqc1[40]>0){
    printf(" LVL1-segments non empty             : % 8d\n",daqc1[41]);
    printf(" ............. from A-side           : % 8d\n",daqc1[42]);
    printf(" ............. from B-side           : % 8d\n",daqc1[43]);
    printf(" ... with format(unkn/raw/comp/mix   : %8d %8d %8d %8d\n",daqc1[44],daqc1[45],daqc1[46],daqc1[47]);
    printf(" NoAssemblyErrors                    : % 8d\n",daqc1[48]);
    printf(" RawFmt time-calib found             : % 8d\n",daqc1[51]);
    printf(" RawFmt LiveTime info found          : % 8d\n",daqc1[52]);
    printf(" RawFMT LiveTime-1 > 1               : % 8d\n",daqc1[53]);
    printf(" RawFMT LiveTime-2 > 1               : % 8d\n",daqc1[54]);
    printf(" Bad LVL1-blocks length              : % 8d\n",daqc1[55]);
    printf(" TrigSetup block found               : % 8d\n",daqc1[56]);
    printf(" TrigSetup block updates             : % 8d\n",daqc1[57]);
    printf(" Scalers block found                 : % 8d\n",daqc1[58]);
    printf(" Scalers block updates(sw-changes)   : % 8d\n",daqc1[65]);
    printf(" CompFmt time-calib info updates     : % 8d\n",daqc1[59]);
    printf(" CompFmt LiveTime info updates       : % 8d\n",daqc1[60]);
    printf(" CompFmt LiveTime-1 > 1              : % 8d\n",daqc1[61]);
    printf(" CompFMT LiveTime-2 > 1              : % 8d\n",daqc1[62]);
    printf(" CompFMT FT/LVL1-rates info updates  : % 8d\n",daqc1[63]);
    printf(" CompFMT SubDet-rates info updates   : % 8d\n",daqc1[64]);
    printf(" Total bad structure LVL1-segments   : % 8d\n",daqc1[50]);
  }
//
  printf("\n");
  printf("    ============ LVL1-Decoding statistics =============\n");
  printf("\n");
  printf(" LVL1-segment entries                : % 8d\n",daqc1[0]);
  if(daqc1[0]>0){
    printf(" LVL1-segments non empty             : % 8d\n",daqc1[1]);
    printf(" ............. from A-side           : % 8d\n",daqc1[2]);
    printf(" ............. from B-side           : % 8d\n",daqc1[3]);
    printf(" ... with format(unkn/raw/comp/mix   : %8d %8d %8d %8d\n",daqc1[20],daqc1[21],daqc1[22],daqc1[23]);
    printf(" ... NonDATA segments( ????? )         : % 8d\n",daqc1[24]);
    printf(" ReplyStatus:  CRCerr  ASSMerr  AMSWerr  TimeOut FEPOWerr   SEQerr  CDPnode:\n");
   printf("             %8d %8d %8d %8d %8d %8d %8d\n",
                                 daqc1[25],daqc1[26],daqc1[27],daqc1[28],daqc1[29],daqc1[30],daqc1[31]); 
    printf(" Segments with reply-status bits OK  : % 8d\n",daqc1[4]);
    printf(" ............. bad length            : % 8d\n",daqc1[10]);
    printf(" TrigPatternBlock entries            : % 8d\n",daqc1[5]);
    printf(" LiveTimeBlock entries               : % 8d\n",daqc1[7]);
    printf("            W:LTime1>1               : % 8d\n",daqc1[6]);
    printf("            W:LTime2>1               : % 8d\n",daqc1[8]);
    printf(" TrigSetupBlock entries              : % 8d\n",daqc1[9]);
    printf(" TrigSetupBlock Changes              : % 8d\n",daqc1[16]);
    printf(" ScalersBlock entries                : % 8d\n",daqc1[11]);
    printf(" ScalersBlock Changes                : % 8d\n",daqc1[12]);
    printf(" Total bad structure LVL1-segments   : % 8d\n",daqc1[13]);
    printf(" At least 1 FT-memb in LVL1          : % 8d\n",daqc1[35]);
    printf(" Events with JMemb&PhysBrMem_patt >0 : % 8d\n",daqc1[36]);
    printf(" Events with 0-JMembPatt             : % 8d\n",daqc1[32]);
    printf(" Events with 0-PhysBranchPatt        : % 8d\n",daqc1[33]);
    printf(" Events with AuxTrigPatt>0           : % 8d\n",daqc1[34]);
    printf(" Events with TOF-rates/LiveTime OK   : % 8d\n",daqc1[37]);
    printf(" Events with TOF&ECAL-FT(in JmembPat): % 8d\n",daqc1[38]);
    printf(" Created LVL1-objects                : % 8d\n",daqc1[14]);
    printf(" LVL1data NonFatalErrors(obj created): % 8d\n",daqc1[17]);
    printf(" Missing MembPatts(no obj created)   : % 8d\n",daqc1[15]);
  }
//
  printf("\n");
  printf("    ============ LVL1-RECO statistics =============\n");
  printf("\n");
  printf(" entries(MC/RD)                      : % 6d\n",countev[0]);
  if(AMSJob::gethead()->isReconstruction()){
    printf(" RD/MC: DAQ-created lvl1-obj found   : % 6d\n",countev[15]);
    printf("        missing patterns added       : % 6d\n",countev[16]);
    printf("        Setup changes                : % 6d\n",countev[17]);
  }
  else{
    printf(" MC: globFTriger(incl ext) OK      : % 6d\n",countev[1]);
    printf(" MC: with FTC(tofZ>=1) included    : % 6d\n",countev[2]);
    printf(" MC: with FTC&BZ(Z>=2) included    : % 6d\n",countev[3]);
    printf(" MC: with FTZ(tofSlowZ>=2) included: % 6d\n",countev[4]);
    printf(" MC: with FTZ at missing FTC       : % 6d\n",countev[5]);
    printf(" MC: with FTE(EcalEnergy) included : % 6d\n",countev[6]);
    printf(" MC: with FTE at missing TOF-FT    : % 6d\n",countev[7]);
    printf(" MC: with EXT(Ext.trig) included   : % 6d\n",countev[8]);
    printf(" MC: LVL1 conditions satisfied     : % 6d\n",countev[9]);
    printf(" MC:   + ScalerRate/LiveTime OK    : % 6d\n",countev[10]);
  }
  printf("   ================================================\n");
}
//------
integer Trigger2LVL1::checktofpattor(integer tof, integer paddle){
#ifdef __AMSDEBUG__
 assert(tof >=0 && tof <TOF2GC::SCLRS);
#endif
 return ((_tofpatt1[tof])&(1 << paddle))||((_tofpatt1[tof])&(1 << (16+paddle)));
}


integer Trigger2LVL1::checktofpattand(integer tof, integer paddle){
#ifdef __AMSDEBUG__
 assert(tof >=0 && tof <TOF2GC::SCLRS);
#endif
 return ((_tofpatt1[tof])&(1 << paddle))&&((_tofpatt1[tof])&(1 << (16+paddle)));
}


void Trigger2LVL1::_writeEl(){
  // p2memb for Root
// Fill the ntuple
#ifdef __WRITEROOT__
  AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif
/*
  lvl1N->PhysBPatt[lvl1N->Nlvl1]=_PhysBPatt;
  lvl1N->JMembPatt[lvl1N->Nlvl1]=_JMembPatt;
  lvl1N->LiveTime[lvl1N->Nlvl1]=_LiveTime;
  lvl1N->TOFlag1[lvl1N->Nlvl1]=_tofflag1;
  lvl1N->TOFlag2[lvl1N->Nlvl1]=_tofflag2;
  int i,j;
  for(i=0;i<4;i++)lvl1N->TOFPatt1[lvl1N->Nlvl1][i]=_tofpatt1[i];
  for(i=0;i<4;i++)lvl1N->TOFPatt2[lvl1N->Nlvl1][i]=_tofpatt2[i];
  lvl1N->AntiPatt[lvl1N->Nlvl1]=_antipatt;
  lvl1N->ECALflag[lvl1N->Nlvl1]=_ecalflag;
  for(i=0;i<6;i++)for(j=0;j<3;j++)lvl1N->ECALpatt[lvl1N->Nlvl1][i][j]=_ectrpatt[i][j];
  lvl1N->ECALtrsum[lvl1N->Nlvl1]=_ectrsum;
  for(i=0;i<19;i++)lvl1N->TrigRates[lvl1N->Nlvl1][i]=_TrigRates[i];
  for(i=0;i<4;i++)lvl1N->TrigTime[lvl1N->Nlvl1][i]=_TrigTime[i];
  lvl1N->Nlvl1++;
*/
}



void Trigger2LVL1::builddaq(integer ibl, integer n, int16u *p){
// on input p points to 1st word of the block (after length)
  int i,nwords(0);
  integer val;
  int16u inpat[15]={15,14,13,11,7,5,9,6,10,3,12,1,2,4,8};//CPpatt vs tofflag(0-14)
  uinteger ltim(0);
  geant tgate(1);//default gate 1sec for live time measur.
  int16u rrr,rrr1,rrr2;
  int16u setupbs(15),livetbs(47);
  Trigger2LVL1 *ptr=(Trigger2LVL1*)AMSEvent::gethead()->getheadC("TriggerLVL1",0);
// 
  if(ptr){
    if((TGL1FFKEY.printfl/10)>=3)cout<<"====> In Trigger2LVL1::builddaq:blk#/len="<<ibl<<" "<<n<<endl;
//==================>TrigPatterns:
    *p = int16u(ptr->_JMembPatt);//LVL1-members
    rrr=int16u(ptr->_PhysBPatt);//Phys.branches
    rrr=(rrr|(1<<11));//triggered by DSP
    *(p+1) = rrr;
    nwords+=2;
//
    *(p+2) = int16u(ptr->_antipatt);
    nwords+=1;
//CP/CT/BZ inp.patt:
    rrr=0;
    if(ptr->_tofflag1>=0)rrr=inpat[int16u(ptr->_tofflag1)];//CP inp.patt
    rrr1=(rrr<<4);//CT=CP(for simplicity, because i do not use CT at al)
    rrr=(rrr|rrr1);
    rrr1=0;
    if(ptr->_tofflag2>=0)rrr1=inpat[int16u(ptr->_tofflag2)];//BZ inp.patt
    rrr2=(rrr1<<8);//BZ
    rrr=(rrr|rrr2);
    *(p+3) = rrr;//
    nwords+=1;
//EventTime counters:
    *(p+4) = int16u(ptr->_TrigTime[2]&0xFFFFL);//16 lsb
    *(p+5) = int16u((ptr->_TrigTime[2]>>16)&0xFFFFL);//+ higher 16 lsb
    rrr=int16u(ptr->_TrigTime[3]&0xFFL);//8 msb
    rrr1=int16u(ptr->_TrigTime[1]&0xFFL);//8 lsb of trst counter
    *(p+6) = (rrr|(rrr1<<8));//    
    *(p+7) = int16u((ptr->_TrigTime[1]&0xFFFF00L)>>8);//16 msb of trst counter
    nwords+=4;
//Buzy:
    *(p+8) = 0;//busy err/patt_lsb
    *(p+9) = 0;//busy  
    *(p+10) = 0;//busy
    nwords+=3;
//time-calib reg:
    *(p+11) = int16u(ptr->_TrigTime[0]&0xFFFFL);//16lsb time calibration
    *(p+12) = int16u((ptr->_TrigTime[0]&0xFFFF0000L)>>16);//16msb ...........
    nwords+=2;
    *(p+13) = 0;//2 reserved words
    *(p+14) = 0;
    nwords+=2;
//now have 15 words in total for ev-by-ev info      
//
//=================================> TrigSetup:
//
    *(p+setupbs) = 0;//ACC mask, all chan. anabled(is in principal derived from antinmask)
    *(p+setupbs+1) = 0;//CP/CT mask, all chan.enabled(is in principal derived from tofinmask)
    rrr=0;
    rrr1=l1trigconf.ecprjmask();//proj.mask(lkji: ij=1/0->XYproj active/disabled in FTE; kl=same for LVL1(angle)
    if((rrr1%10)==0)rrr|=(1<<8);//FT_Xproj disabled   
    if(((rrr1%100)/10)==0)rrr|=(1<<9);//FT_Yproj disabled   
    if(((rrr1%1000)/100)==0)rrr|=(1<<10);//LV1(angl)_Xproj disabled   
    if(((rrr1%10000)/1000)==0)rrr|=(1<<11);//LV1(angl)_Yproj disabled   
    *(p+setupbs+2) = rrr;//BZ/EC_F/A/EXT mask, all BZ-chan.enabled(is in principal derived from tofinzmask)   
    *(p+setupbs+3) = 0;//TRST/buzy masks, all anabled   
    *(p+setupbs+4) = 0;//TRST/buzy masks, all anabled   
    *(p+setupbs+5) = 0;//TRST/buzy masks, all anabled
    nwords+=6;
//tofoamask and tofoazmask:
    rrr=0;   
    for(i=0;i<TOF2GC::SCLRS;i++){// tofoamask and tofoazmask
      if(l1trigconf.tofoamask(i)==1){
        rrr=(rrr|(1<<i));//OR for CP-sides(FTC)
	rrr=(rrr|(1<<(i+4)));//OR for CT-sides(FTC) as above
      }
      if(l1trigconf.tofoazmask(i)==1)rrr=(rrr|(1<<(i+8)));//OR for BZ-sides(FTC)
    }
    *(p+setupbs+6)=rrr;
    nwords+=1;
//antioamask:
    rrr=0;
    for(i=0;i<ANTI2C::MAXANTI;i++){//antioamask
      if(l1trigconf.antoamask(i)==1)rrr=(rrr|(1<<i));//OR
    }
    *(p+setupbs+7)=rrr;
    nwords+=1;
//
    *(p+setupbs+8)=(1<<15);//int.trigger setting (disabled)
    nwords+=1;
//
    rrr=0;
    rrr|=3;//2bits, LA-trig.disabled
    rrr|=(3<<4);//LA-trig.outp disabled
    rrr|=(1<<6);//Ext-trig.outp disabled
    rrr|=(1<<7);//Fast-trig. to RICH disabled
    rrr|=(1<<8);//LA-trig outp -> LVL1(=0 for FT)
    rrr|=(1<<9);//Ext-trig outp -> LVL1(=0 for FT)
    *(p+setupbs+9)=rrr;
    nwords+=1;
//FT-settings:
    rrr=0;
    rrr1=int16u(l1trigconf.globftmask());//dec IJK -> FTE|FTZ|FTC, I(J,K)=1->enabled,0->disabled
    if(rrr1%10==0)rrr|=1;//FTC,1->disabled (following to Lin) 
    if((rrr1%100)/10==0)rrr|=(1<<1);//FTZ
    if(rrr1/100==0)rrr|=(1<<2);//FTE
    rrr1=int16u(l1trigconf.toflcsz());//FTZ-layers logic MN,M=0/1->and/or of top and bot logic, N->top(bot)logic
    if((rrr1%10==1) || (rrr1%10==3))rrr|=(1<<6);//bot=OR(top=AND/OR)
    if((rrr1%10==2) || (rrr1%10==3))rrr|=(1<<5);//top=OR(bot=AND/OR)
// if(rrr1%10==0) both top and bot =AND, i.e. nothing to add to rrr
    if(rrr1/10==1)rrr|=(1<<7);// top and bot =OR
    rrr1=int16u(l1trigconf.ecorand());//1/2->or/and of 2 FTE-projections
    if(rrr1==1)rrr|=(1<<8);//FTE-projections OR
    *(p+setupbs+10)=rrr;
    nwords+=1;
//LUTs-setting:
    *(p+setupbs+11)=int16u(l1trigconf.toflut1());//FTC lut0
    *(p+setupbs+12)=int16u(l1trigconf.toflut2());//FTC lut1
    *(p+setupbs+13)=int16u(l1trigconf.toflutbz());//BZ lut
    nwords+=3;
//TOF BZ p.width extention codes:
    rrr1=int16u(l1trigconf.tofextwid());//5 lsb->topTOF ext.w.code, next 5bits->botTOF ext.w.code
    rrr=(rrr1&0x1F);
    rrr|=((rrr1&0x3E0)<<3);
    *(p+setupbs+14)=rrr;
    nwords+=1;
//LVL1 PhysBrMask + 2 ACC thresh. on #sect
    rrr1=int16u(l1trigconf.globl1mask());
    rrr=0;
    for(i=0;i<8;i++){
      if((rrr1&(1<<i))==0)rrr|=(1<<i);//going to Lin-convention(1-> PhysBranch is disabled)
    }
    rrr1=int16u(l1trigconf.antsectmx(0));//max.cectors in Equat-region (1)
    rrr|=(rrr1<<8);
    rrr1=int16u(l1trigconf.antsectmx(1));//max.cectors in Polar-region (0)
    rrr|=(rrr1<<12);
    *(p+setupbs+15)=rrr;
    nwords+=1;
//LVL1-masks for 8 phys.branches:
    for(i=0;i<8;i++){
      rrr1=(int16u(l1trigconf.physbrmemb(i))&0x7FFF);
      *(p+setupbs+16+i)=((~rrr1)&0x7FFF);//going back to Lin's convention(1->disabled)
      nwords+=1;
    }
//PrescaleFact for phys.branches:
    for(i=0;i<8;i++){//branch#
      rrr=(0x3FF);//def.code(->1)
      val=l1trigconf.phbrprescf(i);//presc.factor value
      rrr1=prescfval2code(val);
      if(rrr1!=0)rrr=rrr1;
      *(p+setupbs+16+8+i)=rrr;
      nwords+=1;
    }
//now we have 32 setup words
//=====================> Add LiveTime registers all_buzy/fe_buzy:
    ltim=uinteger(tgate/(2.e-8))*AMSmceventg::LiveTime();//ref.counter value for 20ns period and tgate
    *(p+livetbs) = int16u(ltim&(0xFFFFL));//16 lsb of All_buzy LiveTime
    rrr=int16u((ltim&(0x7FF0000L))>>16);//11 msb
    rrr|=(2<<12);//time gate code (2->1sec)
    *(p+livetbs+1) = rrr;
    *(p+livetbs+2) = int16u(ltim&(0xFFFFL));//16 lsb of Fe_buzy LiveTime
    rrr=int16u((ltim&(0x7FF0000L))>>16);//11 msb
    rrr|=(2<<12);//time gate code (2->1sec)
    *(p+livetbs+3) = rrr;
    nwords+=4;
//======================> Add status word:
    rrr=getdaqid(0);//board side-A node(slave)id (14)
    rrr|=(1<<5);//cdpnode(end of fragmentation)
    rrr|=(1<<6);//raw.fmt
    rrr|=(1<<15);//data-fragment
    *(p+livetbs+4) = rrr;
    nwords+=1;
    if((TGL1FFKEY.printfl/10)>=3)cout<<"<==== Block was build for DAQ portid="<<rrr<<endl<<endl;
  }
  if(nwords!=n){
    cout<<"<==== Trigger2LVL1::builddaq:Error - wrong length, call="<<n<<" assembled="<<nwords<<endl;
    exit(10);//tempor
  }   
}

//----------------------------------------------------------------------------------
void Trigger2LVL1::buildraw(integer len, int16u *p){
//
// on input: len=tot_block_length as given at the beginning of block
//           *p=pointer_to_beggining_of_block_data (next to the length word)
//
  {
   AMSContainer *ptr=AMSEvent::gethead()->getC("TriggerLVL1",0);
   if(ptr)ptr->eraseC();
   else cerr <<"<---- Trigger2LVL1::buildraw-S-NoContainer"<<endl;

  }
//
  integer lencalc(0);
  integer JMembPatt(0),PhysBPatt(0),TofFlag1(-1),TofFlag2(-1),AntiPatt(0),EcalFlag(0);
  geant LiveTime[2]={0,0};
  geant livetm[2];
  geant TrigRates[19]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  static number evtprev(-1);
  number evtcurr(0),delevt;
  uinteger ntrst(0),timcal(0);
  uinteger time[2]={0,0};
  uinteger trtime[5]={0,0,0,0,0};
  uinteger busypat[2]={0,0};//1st word->bits 0-31, 2nd word-> bits 32-39 of 40-bits busy patt.word
#ifdef _OPENMP
  int16u nw3;
#endif
  bool busyerr(0);
  bool PreAssRD=(AMSJob::gethead()->isRealData()
                 && (AMSEvent::gethead()->getrun() < 1213470000)) ;//flag to identify RD of preassembly period
//
  integer tofpat1[4]={0,0,0,0};//will be added to ready lev1-object later(after decoding of TOF-data)
  integer tofpat2[4]={0,0,0,0};//..................................................
  int16u ecpat[6][3]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};//.....(after decoding of EC-data)
  geant ectrs=0;//........................................................................
//
  integer i,j,ib;
  int16u bit;
  int16u word,wordi,wordf;
  uinteger lword;
  uinteger ltim(0);
  geant tgate(0);
  uinteger timgid;
//
  static integer err(0),err1(0),err2(0),err3(0),err4(0);
//  
  int16u datyp(0),formt(0);
  int16u jblid,jleng,jaddr,csid;
  integer auxtrpat(0);
//
  int16u rstatw1(0),rstatw2(0),rstatw3(0);
  int16u nrdow1(0),nrdow2(0),nrdow3(0);
//
  int ltimbias;//tempor bias to live-time data(in "trig"-block)
  int trgsbias;//bias to trig.setup/status block
  int pattbias;//bias to trig.patt sub-block
  int scalbias;//bias to scalers sub-block
//
  TGL1JobStat::daqs1(0);//count entries
  p=p-1;//to follow VC-convention(here points to length-word)
  jleng=int16u(len&(0xFFFFL));
  csid=int16u((len>>16)&(0xFFFFL))+1;//S(s=1-4=>a,b,p,s) as return by my "checkblockid"
  jblid=*(p+jleng);// JLV1 fragment's last word: Status+slaveID(its id)
//
  bool dataf=((jblid&(0x8000))>0);//data-fragment
  bool crcer=((jblid&(0x4000))>0);//CRC-error
  bool asser=((jblid&(0x2000))>0);//assembly-error
  asser=false;
  bool amswer=((jblid&(0x1000))>0);//amsw-error   
  bool timoer=((jblid&(0x0800))>0);//timeout-error   
  bool fpower=((jblid&(0x0400))>0);//FEpower-error   
  bool seqer=((jblid&(0x0400))>0);//sequencer-error
  bool cdpnod=((jblid&(0x0020))>0);//CDP-node(like EDR2-node with no futher fragmentation)
  bool noerr;
  bool LTwinv=false;//invert order of 2 LiveTime word
//
  jaddr=(jblid&(0x001F));//slaveID(="NodeAddr"=JLV1addr here)(one of 2 permitted(sides a/b))
  datyp=((jblid&(0x00C0))>>6);//(0-should not be),1,2,3(raw/compr/mix)
//
  if((TGL1FFKEY.printfl/10)>=3){
    cout<<endl;
    cout<<"======> In Trigger2LVL1::buildraw: JLV1_length(in call)="<<*p<<"("<<jleng<<"), slave_id="<<jaddr
                                                                       <<" data-type="<<datyp<<endl<<endl;
    if((TGL1FFKEY.printfl/10)>=4)EventBitDump(jleng,p,"Dump Event-by-Event:");//debug
    
  }
  bool siderr(false);

  if(jleng>1)TGL1JobStat::daqs1(1);//<=== count non-empty fragments
  else goto BadExit;
//
  if(csid>0 && csid<=4){
    if(csid==3)csid=1;//tempor
    else if(csid==4)csid=2;
    TGL1JobStat::daqs1(2+csid-1);//sides sharing
  }
  else{
    siderr=true;
    csid=1;
//    goto BadExit;//unknown side(ignore here, status bit will be set at lvl1-obj creation)
  }
//
  TGL1JobStat::daqs1(20+datyp);//<=== count lvl1's format-types
  if(datyp==0){
    if(err2++<100)cerr<<" <---- TriggerLVL1::buildraw-E-Wrong format-type(!=raw|comp) !"<<endl;
//    goto BadExit;//unknown format-type//tempor commented:dangerous fix of datyp=0 case
  }
//
  if(!dataf){
    TGL1JobStat::daqs1(24);//<=== count lvl1's notData segments
    if(err3++<100)cerr<<" <---- TriggerLVL1::buildraw-E- Not data block !"<<endl;
    //goto BadExit;//not remove here: set later bit in lvl1-status word(event may be still ok ???)
  }
//
  if(crcer)TGL1JobStat::daqs1(25);
  if(asser)TGL1JobStat::daqs1(26);
  if(amswer)TGL1JobStat::daqs1(27);
  if(timoer)TGL1JobStat::daqs1(28);
  if(fpower)TGL1JobStat::daqs1(29);
  if(seqer)TGL1JobStat::daqs1(30);
  if(cdpnod)TGL1JobStat::daqs1(31);
//
  noerr=(!crcer 
//               && !asser
	                && !amswer 
			          && !timoer 
				            && !fpower 
					              && !seqer
						               && cdpnod);
  if(noerr)TGL1JobStat::daqs1(4);//<=== count no_ass_errors JLV1-fragments for given DATA-type     
  else{
    if(err4++<1000)cerr<<" <---- TriggerLVL1::buildraw-E- Assembly-errors block !"<<endl;
//    goto BadExit;//other errors(status bit will be set at lvl1-obj creation)
  }
//
  if(jleng==52)//tempor dangerous fix of datyp=0 case
//  if((PreAssRD && jleng==52 && datyp==2)//RD PreAssPeriod(RawFmt but datyp=2!!!; jleng=52 due to 2 empty-words)
//    || (jleng==52 && datyp==1))//MC/newRD(in RawFmt with datyp=1)
  {
    pattbias=0;//bias to patt. sub-block
    trgsbias=15;//bias to trig-setup sub-block(points to PREVIOUS to the 1st sub-block word)
    ltimbias=48;//bias to time sub-block
    formt=1;//means real raw fmt
    if(PreAssRD && jleng==52 && datyp==2)LTwinv=true;//invert for 2008 data
  }
  else if(AMSJob::gethead()->isRealData() && (datyp==2 || datyp==0)){//tempor dangerous fix of datyp=0 case
//  else if(AMSJob::gethead()->isRealData() && datyp==2){//futur RD in true compr.format
    pattbias=0;//bias to patterns sub-block
    trgsbias=12;//bias to trig-setup sub-block(points to 1st readout-status word)
    formt=2;//means real compr fmt
  } 
  else {
      TGL1JobStat::daqs1(10);//wrong segment length or format
      if((TGL1FFKEY.printfl/10)>=1)cout<<"<---- Trigger2LVL1::buildraw: length/fmt error, len="
                                                   <<jleng<<" addr="<<jaddr<<" datyp="<<datyp<<endl;
      goto BadExit;
  }
//=====================================================================
  if(formt==1 || formt==2){//<------- "Event-by-event" trig-info(patterns,..)
    TGL1JobStat::daqs1(5);//"TrigPatternsBlock" entries    
    word=*(p+1+pattbias);//JMembPatt(FTC,FTCP0,...FTZ,...EXT-GATE1
    JMembPatt=integer(word);
    word=*(p+2+pattbias);//Lev1PhysMembPatt
    PhysBPatt=integer(word&0x00FF);
    auxtrpat=((word&0x1F00)>>8);//aux.trig.patt(LA-0,LA-1,DSP,intTR)
    word=*(p+3+pattbias);//AntiPatt
    AntiPatt=integer(word&0x00FF);
//
    word=*(p+4+pattbias);//Tof CP,CT,BZ layers input pattern
//---> RD Tof hw-problem(L3<->L4) treatment for CP inp.patt:
    wordi=(word&0x000F);
    if(AMSJob::gethead()->isRealData()){
      wordf=(wordi&0x0003);
      if((wordi&(1<<2))>0)wordf|=(1<<3);
      if((wordi&(1<<3))>0)wordf|=(1<<2);
    }
    else{
      wordf=wordi;
    }
//
    if(wordf==0x000F)TofFlag1=0;//all4
    else if(wordf==0x000E)TofFlag1=1;//2,3,4(miss1)
    else if(wordf==0x000D)TofFlag1=2;//1,3,4(miss2)
    else if(wordf==0x000B)TofFlag1=3;//1,2,4(miss3)
    else if(wordf==0x0007)TofFlag1=4;//1,2,3(miss4)
    else if(wordf==0x0005)TofFlag1=5;//1,3
    else if(wordf==0x0009)TofFlag1=6;//1,4
    else if(wordf==0x0006)TofFlag1=7;//2,3
    else if(wordf==0x000A)TofFlag1=8;//2,4
    else if(wordf==0x0003)TofFlag1=9;//1,2(miss3,4)
    else if(wordf==0x000C)TofFlag1=10;//3,4(miss1,2)
    else if(wordf==0x0001)TofFlag1=11;//1
    else if(wordf==0x0002)TofFlag1=12;//2
    else if(wordf==0x0004)TofFlag1=13;//3
    else if(wordf==0x0008)TofFlag1=14;//4
    else{
      TofFlag1=-1;
#ifdef __AMSDEBUG__
      cerr<<"   <-- Trigger2LVL1::buildraw:Wrong CPinpPatt word="<<hex<<word<<dec<<endl;
#endif
    }
//
//RD Tof hw-problem treatment for BZ inp.patt:
    wordi=(word&0x0F00);//select BZ(ignore CT for the moment)
    wordi=(wordi>>8);
    if(AMSJob::gethead()->isRealData()){
      wordf=(wordi&0x0003);
      if((wordi&(1<<2))>0)wordf|=(1<<3);
      if((wordi&(1<<3))>0)wordf|=(1<<2);
    }
    else wordf=wordi;
//
    if(wordf==0x000F)TofFlag2=0;//all4
    else if(wordf==0x000E)TofFlag2=1;//2,3,4(miss1)
    else if(wordf==0x000D)TofFlag2=2;//1,3,4(miss2)
    else if(wordf==0x000B)TofFlag2=3;//1,2,4(miss3)
    else if(wordf==0x0007)TofFlag2=4;//1,2,3(miss4)
    else if(wordf==0x0005)TofFlag2=5;//1,3
    else if(wordf==0x0009)TofFlag2=6;//1,4
    else if(wordf==0x0006)TofFlag2=7;//2,3
    else if(wordf==0x000A)TofFlag2=8;//2,4
    else if(wordf==0x0003)TofFlag2=9;//1,2(miss3,4)
    else if(wordf==0x000C)TofFlag2=10;//3,4(miss1,2)
    else if(wordf==0x0001)TofFlag2=11;//1
    else if(wordf==0x0002)TofFlag2=12;//2
    else if(wordf==0x0004)TofFlag2=13;//3
    else if(wordf==0x0008)TofFlag2=14;//4
    else {
      TofFlag2=-1;
#ifdef __AMSDEBUG__
      cerr<<"   <-- Trigger2LVL1::buildraw:Wrong BZinpPatt word="<<hex<<word<<dec<<endl;
#endif
    }
    if((TGL1FFKEY.printfl%10)>0){
#pragma omp critical (hf1)
{
      HF1(1292,geant(TofFlag1),1.);
      HF1(1293,geant(TofFlag2),1.);
}
    }
//cout<<"Lev1Reco:Event="<<(AMSEvent::gethead()->getid())<<" TofFlag1/2="<<TofFlag1<<" "<<TofFlag2<<endl;
//-----> create EcalFlag (FTE/LVL1 decisions):
    EcalFlag=0;
    bool FTEok=((JMembPatt&(1<<6))>0);
    if(FTEok && (JMembPatt&(1<<10))>0)EcalFlag=30;//FTE + ProjAnd(2prj)
    else if(FTEok && (JMembPatt&(1<<11))>0)EcalFlag=20;//FTE + ProjOR(1prj)
    else if(FTEok)EcalFlag=10;//FTE when no proj-bits (crazy situation)
    if((JMembPatt&(1<<12))>0)EcalFlag+=3;//Lvl1(Small angle in both proj(AngleAnd)) 
    else if((JMembPatt&(1<<13))>0)EcalFlag+=2;//Lvl1(Small angle at least in one proj(AngleOr))
    else{//no LVL1-AngleBits
      if(FTEok)EcalFlag+=1;//No ECLev1(Big angle in both proj, but FTE ok)
    }
//-----> trig.patt histos: 
    if((TGL1FFKEY.printfl%10)>0){
#pragma omp critical (hf1)
{
      for(i=0;i<16;i++){
        if((JMembPatt&(1<<i))>0){
          HF1(1092,geant(i+1),1.);
        }
      }
      HF1(1093,0.,1.);
      for(i=0;i<8;i++){
        if((PhysBPatt&(1<<i))>0)HF1(1093,geant(i+1),1.);
      }
}
    }
//-----> event(trigger) time info: 
    time[0]=0;
    time[1]=0;
    word=*(p+5+pattbias);//1st 16bits of time
    time[0]|=uinteger(word);
    lword=uinteger(*(p+6+pattbias));//2nd 16bits of time
    time[0]|=(lword<<16);
    word=*(p+7+pattbias);//last 8bits of time +1st 8bits of ntrst
    time[1]=uinteger((word&0x00FF));
    ntrst|=uinteger((word&0xFF00)>>8);
    lword=uinteger(*(p+8+pattbias));//last 16bits of ntrst
    ntrst|=(lword<<8);
    trtime[1]=ntrst;
    trtime[2]=time[0];//lsb
    trtime[3]=time[1];//msb
//
    if(ntrst>0)evtcurr=1000000.*(ntrst-1)+time[0]*0.64+time[1]*pow(2.,32)*0.64;//mksec
    else evtcurr=time[0]*0.64+time[1]*pow(2.,32)*0.64;//tempor
    if(evtprev>0){
      delevt=evtcurr-evtprev;
      evtprev=evtcurr;
      if(delevt>(0xFFFF))trtime[4]=uinteger(0xFFFF);
      else trtime[4]=uinteger(floor(delevt));
    }
    else evtprev=evtcurr;
#ifdef _OPENMP
 trtime[4]=DAQEvent::gethead()->trigtime();
#else
 if (int(trtime[4])!=DAQEvent::gethead()->trigtime()){
  cerr<<"TriggerLVL1-E-TrigTimeDiff NotMatch "<<trtime[4]<<" "<<DAQEvent::gethead()->trigtime()<<endl;
 }
#endif
    if((TGL1FFKEY.printfl%10)>0){
#pragma omp critical (hf1)
{
      HF1(1099,geant(trtime[4]),1.);
}
    }
    
//
    word=*(p+9+pattbias);//busy-patt,err
    busyerr=((word&1)>0);
    if(!busyerr){
      busypat[0]|=uinteger((word&0xFF00)>>8);//1st 8bits of patt
      lword=uinteger(*(p+10+pattbias));//next 16bits of patt
      busypat[0]|=(lword<<8);
      lword=uinteger(*(p+11+pattbias));//last 16bits of patt
      busypat[0]|=(lword<<24);
      busypat[1]|=(lword>>8);
    }
    lencalc+=11;//tot.leng of "event-by-event" block
//---> event time-calib counter (when TRST asserted)
    if(formt==1){//RD PreAssPeriod OR RawFmt(RD/MC)
// attention:  i suppose that 0x0D(p+14) and 0x0E(p+15) words exist but empty
      timcal=0;
      word=*(p+12+pattbias);//1st 16bits of time-calib word
      timcal|=uinteger(word);
      lword=uinteger(*(p+13+pattbias));//last 16bits of time-calib word
      timcal|=(lword<<16);
      lencalc+=4;//add time-calib and 2 empty words for RawFmt
    }
//---> print event-by-event (patterns) info:
    if((TGL1FFKEY.printfl/10)>=2){
#pragma omp critical (evpatterns)
{ 
      cout<<"  -------> Event-by-event(instant) LVL1-info (patterns,time-counters,...):"<<endl<<endl;
      cout<<"      Triggered by (hex_patt="<<auxtrpat<<") :";
      if((auxtrpat&1)>0)cout<<"LA-0"<<endl;
      if((auxtrpat&2)>0)cout<<"LA-1"<<endl;
      if((auxtrpat&4)>0)cout<<"????"<<endl;
      if((auxtrpat&8)>0)cout<<"DSP"<<endl;
      if((auxtrpat&16)>0)cout<<"IntTrig"<<endl;
      cout<<endl;
      
      cout<<"      Instant Lev1MembersPattern :"<<endl; 
      cout<<"| FTC|FTP0|FTP1|FTT0|FTT1| FTZ| FTE|ACC0|ACC1|  BZ|ECFA|ECFO|ECAA|ECAO|EXG0|EXG1|"<<endl;
      for(j=0;j<16;j++){
	if((JMembPatt&1<<j)>0)cout<<"  X  ";
	else cout<<"  0  ";
      }
      cout<<endl<<endl;
      
      cout<<"      Instant Lev1PhysBranchesPattern(masked ?) :"<<endl;
      cout<<"|  FTC| Z>=1| Z>=2|Z>=2s|Elect|Gamma|  FTE|Extrn|"<<endl;  
      for(i=0;i<8;i++){
	  if((PhysBPatt&(1<<i))>0)cout<<"   X  ";
	  else {cout<<"   0  ";}
      }
      cout<<endl<<endl;
      
      cout<<"      ECAL TrigFlag(FTE/LVL1 decisions) MN="<<EcalFlag<<endl;
      cout<<"       M=0/1/2/3->FTE(Energy)Flag=No/NoFTE_when1prj@2prj_req/FTE&1prj(or)/FTE&2prj(and)"<<endl;
      cout<<"       N=0/1/2/3->LVL1(Angle)Flag=Undef(noFTE)/0prj@FTEfound/OrLVL1/AndLVL1"<<endl<<endl;
      
      cout<<"      Instant ATC-sectors pattern:";
      for(i=0;i<ANTI2C::MAXANTI;i++){
        if((AntiPatt&(1<<i))>0)cout<<"1"<<" ";
	else {cout<<"0"<<" ";}
      }
      cout<<endl<<endl;
      
      cout<<"      Instant CP_LayerPatt(myFTCflg):"<<TofFlag1<<", BZ_LayerPatt(myBZflg):"<<TofFlag2<<endl<<endl;
      
      cout<<"      FineTimeCounter value(8msb|32lsb):"<<time[1]<<"|"<<time[0]<<", CoarseTimeCounter:"<<ntrst<<endl;
      number tmksec;
      if(ntrst>0){     
        tmksec=time[0]*0.64+time[1]*pow(2.,32)*0.64+1000000.*(ntrst-1);
        cout<<"      TimeTag(mksec):"<<tmksec<<"  delta="<<trtime[4]<<endl;
      }
      else{
        tmksec=time[0]*0.64+time[1]*pow(2.,32)*0.64;
        cout<<"      TimeTag-problem (CoarseTimeCounter=0 !!!)"<<" tmksec/delta="<<tmksec<<" "<<trtime[4]<<endl;
      }
      if(!busyerr)cout<<"      BusyPattern(msb|lsb)(hex):"<<hex<<busypat[1]<<"|"<<busypat[0]<<dec<<endl;
      else cout<<"      BusyError found !!!"<<endl;
      cout<<"      TimeCalibrationCounter:"<<timcal<<endl<<endl;
}//--->endof pragma
    }//--->endof "need print ?" check
  }//------>endof "TrigPatt"(ev-by-ev) block
//======================================================================
  if(formt==1){// <------- LiveTime info (present in every event for raw fmt case)
// warning: 1st and 2nd words are interchanged in each livetime pair for 2008 data
    TGL1JobStat::daqs1(7);//"LiveTimeBlock" entries
    for(j=0;j<2;j++){// lt1/lt2
      if(LTwinv)word=*(p+ltimbias+1+2*j);//1st 16bits of live-time
      else word=*(p+ltimbias+2*j);
      ltim=uinteger(word);
      if(LTwinv)lword=uinteger(*(p+ltimbias+2*j));//last 11bits of live_time + time_gate.id
      else lword=uinteger(*(p+ltimbias+1+2*j));
      ltim|=((lword&0x07FFL)<<16);
      timgid=((lword&0x3000L)>>12);//2bits of time_gate.id
      if(timgid==3)tgate=2;//time-gate(sec)
      else if(timgid==0)tgate=0.25;
      else if(timgid==1)tgate=0.5;
      else tgate=1;
      livetm[j]=geant(number(ltim)*(2.e-8))/tgate;//livetime fraction(imply 20ns pulses period)
      if(livetm[j]>1){
      if((TGL1FFKEY.printfl/10)>=1)cout<<" <---- Trigger2LVL1::buildraw:W - LiveTime1>1!, tg/lt="<<
	                        tgate<<" "<<ltim<<" type="<<j+1<<" LiveTime="<<livetm[j]<<endl;
//        LiveTime[j]=1; 
        TGL1JobStat::daqs1(6+2*j);//counts  LTime>1
      }
//
    }//--->endof lt1/2 loop
    lencalc+=4;//add LiveTime-block leng in RawFmt 
  }//------->endof "LiveTime" block(RawFmt only)
//================================================================================================
//========> check the presence/length of TrigSetup/Scaler blocks:
  if(formt==1){//raw fmt
    rstatw1=(0xFFFF);//all bits set(complete set of 1-16 setup words)
    rstatw2=(0xFFFF);//all bits set(complete set of 17-32 setup words)
    rstatw3=0;//all scalers info missing in raw fmt
    nrdow1=16;
    nrdow2=16;
    nrdow3=0;
    lencalc+=(nrdow1+nrdow2);//add Setup-block leng. in RawFmt
  }
  else{//ComprFormat
    nrdow1=0;
    nrdow2=0;
    nrdow3=0;
    rstatw1=*(p+trgsbias);//1st readout-status word
    for(ib=0;ib<16;ib++){
      bit=(1<<ib);
      if((rstatw1&bit)>0)nrdow1+=1;
    }
    rstatw2=*(p+trgsbias+nrdow1+1);//2nd readout-status word
    for(ib=0;ib<16;ib++){
      bit=(1<<ib);
      if((rstatw2&bit)>0)nrdow2+=1;
    }
    lencalc+=nrdow1+nrdow2+2;//add Setup-block leng. in CompFmt(+2 for 2 readoutstat-words)
//calc.scalers-block length
    scalbias=trgsbias+nrdow1+nrdow2+2;
    rstatw3=*(p+scalbias);//section's readout-members bit-pattern
    for(ib=0;ib<16;ib++){
      bit=(1<<ib);
      if((rstatw3&bit)>0){
        if(ib<=2)nrdow3+=1;
	if(ib==12)nrdow3+=2;
	if(ib==13)nrdow3+=4;
	if(ib==14)nrdow3+=14;
	if(ib==15)nrdow3+=5;
      }
    }
    lencalc+=nrdow3+1;//add Scalers-blocks length in CompFmt(+1 for stat-word)
  }
//========>
  if((lencalc+1)!=jleng){
    TGL1JobStat::daqs1(10);//wrong segment length
    if((TGL1FFKEY.printfl/10)>=1)cout<<"<---- Trigger2LVL1::buildraw: length error, LengInCall="
                                              <<jleng<<" CalcLeng="<<lencalc<<" datyp="<<datyp<<endl;
    goto BadExit;
  }
//================================> TrigSetup block decoding(RawFmt- every event,CompFmt- beg.of.run):
//  CP/CT/BZ-inp-masks are not stored in Lvl1TrigConfig-object for the moment, so the related 
//       decoded info is not used now, and the rest is decoded in EarlyDecoding !!!!!!
//
  if(nrdow1>0 || nrdow2>0){
  TGL1JobStat::daqs1(9);//"TrigSetupBlock" non-empty entries
//------
/*
  nw1=0;
  if(nrdow1>0){//<---check readout-bits word1
//
    for(ib=0;ib<16;ib++){//<--- bit loop
    bit=(1<<ib);
    if((rstatw1&bit)>0){
      nw1+=1;
      word=*(p+trgsbias+nw1);
//--
      if(ib==0){//<---Anti-mask
        for(i=0;i<ANTI2C::MAXANTI;i++){//update antinmask
          if((word&1<<(8+i))==0 && (word&1<<i)==0)antinmask[i]=1;//s1&s2 "in"
          else if((word&1<<(8+i))==0)antinmask[i]=2;//s1(n) "in"
          else if((word&1<<i)==0)antinmask[i]=3;//s2(p) "in"
          else antinmask[i]=0;
        }
      }
//--
      if(ib==1){//<---CP/CT mask
//CP-mask is not really used - i have pad-individual masks tofinmask(il,ib), CT-mask is ignored.
        cpmask=(word&0x00FF);//cp-mask(original, not convinient bits order)
        ctmask=((word&0xFF00)>>8);//ct-mask(original, not convinient bits order)
        for(i=0;i<TOF2GC::SCLRS;i++){           	    
	  if((cpmask&1<<(4+i))==0 && (cpmask&1<<i)==0)cpinmask[i]=1;//s1&s2 "in"
          else if((cpmask&1<<(4+i))==0)cpinmask[i]=2;//s1(n) "in"
          else if((cpmask&1<<i)==0)cpinmask[i]=3;//s2(p) "in"
          else cpinmask[i]=0;
        }
        if(cpinmask[0]==2)cpinmask[0]=3;//L1 special treatment(p <-> n, thank to Lin)
        else if(cpinmask[0]==3)cpinmask[0]=2;
//
        if(AMSJob::gethead()->isRealData()){//RD L3<->L3 problem treatment
	  rrr=cpinmask[2];
	  cpinmask[2]=cpinmask[3];
	  cpinmask[3]=rrr;
        }
      }
//--
      if(ib==2){//<---BZ/ECAL masks
//BZ-mask is not really used - i have pad-individual masks tofinzmask(il,ib))
        bzmask=(word&0x00FF);//bz-mask(original, not convinient bits order)
        for(i=0;i<TOF2GC::SCLRS;i++){
          if((bzmask&1<<(4+i))==0 && (bzmask&1<<i)==0)bzinmask[i]=1;//s1&s2 "in"
          else if((bzmask&1<<(4+i))==0)bzinmask[i]=2;//s1(n) "in"
          else if((bzmask&1<<i)==0)bzinmask[i]=3;//s2(p) "in"
          else bzinmask[i]=0;
        }
        if(bzinmask[0]==2)bzinmask[0]=3;//L1 special treatment(p <-> n, thank to Lin)
        else if(bzinmask[0]==3)bzinmask[0]=2;
//
        if(AMSJob::gethead()->isRealData()){//RD L3<->L3 problem treatment
	  rrr=bzinmask[2];
	  bzinmask[2]=bzinmask[3];
	  bzinmask[3]=rrr;
        }
//
        wvar=0;
        if((word&1<<8)==0)wvar+=1;//ECft x-proj active(ababled)
        if((word&1<<9)==0)wvar+=10;//ECft y-proj active
        if((word&1<<10)==0)wvar+=100;//ECangl x-proj active
        if((word&1<<11)==0)wvar+=1000;//ECangl y-proj active
        ecprjmask=wvar;//update ecprjmask
      }
//--
      if(ib==3){//<---   TRST/FEbusy-masks are not used for the moment(just stored)
        trstmsk=(word&0xFF);//raw TRST-mask(bits(0-7) positions unchanged)
        febusymsk[0]|=((word&0xFF00)>>8);//FE-busy bits 0-7
      }
//--
      if(ib==4){
        febusymsk[0]|=(word<<8);//add FE-busy bits 8-23
      }
      if(ib==5){
        febusymsk[1]=uinteger(word);//last 16 FEbusy bits(24-39) are stored as lsbits of febusymsk[1]
      }
//--
      if(ib==6){//<--- Tof CP/CT/BZ sides OR/AND setting
        for(i=0;i<TOF2GC::SCLRS;i++){//update tofoamask and tofoazmask
          if((word&(1<<i))>0)tofoamask[i]=1;//or for CP-sides(FTC)
          else tofoamask[i]=0;//and
          if((word&(1<<(i+8)))>0)tofoazmask[i]=1;//or for BZ-sides
          else tofoazmask[i]=0;//and
        }
        if(AMSJob::gethead()->isRealData()){//RD L3<->L3 problem treatment
	  rrr=tofoamask[2];
	  tofoamask[2]=tofoamask[3];
	  tofoamask[3]=rrr;
	  rrr=tofoazmask[2];
	  tofoazmask[2]=tofoazmask[3];
	  tofoazmask[3]=rrr;
        }
      }
//--
      if(ib==7){//<--- ACC sides OR/AND setting
        for(i=0;i<ANTI2C::MAXANTI;i++){//update antioamask
          if((word&(1<<i))>0)antoamask[i]=1;//or
          else antoamask[i]=0;//and
        }
      }
//--
      if(ib==8){//<--- Settings for internal trigger
//ignore for the moment
      }
//      
      if(ib==9){//<--- Settings for external/LA trigger
//ignore for the moment
      }
//--
      if(ib==10){//<--- settings for FT
        if((word&1)==0)gftmsk+=1;
        if((word&2)==0)gftmsk+=10;
        if((word&4)==0)gftmsk+=100;
        globftmask=gftmsk;//update globFTmask(FTE|FTZ|FTC)
        if((word&(1<<7))>0)ftzlmsk+=10;//OR of top/bot in FTZ-trig
        if((word&(1<<5))>0 & (word&(1<<6))>0)ftzlmsk+=3;//(1|2)+(3|4)
        else if((word&(1<<5))>0 & (word&(1<<6))==0)ftzlmsk+=2;//(1|2)+(3&4)
        else if((word&(1<<5))==0 & (word&(1<<6))>0)ftzlmsk+=1;//(1&2)+(3|4)
        toflcsz=ftzlmsk;//update FTZ layers or/and mask
        if((word&(1<<8))>0)ecorand=1;//update FTE-proj or/and(1/2->or/and) requir.
        else ecorand=2;
      }
//--
      if(ib>=11 && ib<=13){ 
        luti=(word&0xFFFE);
	if(AMSJob::gethead()->isRealData())lutf=lutbswap(luti);//RD L3<->L4 problem treatment for LUTs
	else lutf=luti;
      }
      if(ib==11){
        toflut1=integer(lutf);//<--- LUT-FT0 setting
      }
//        
      if(ib==12){
        toflut2=integer(lutf);//<--- LUT-FT1 setting
      }
//        
      if(ib==13){
        toflutbz=integer(lutf);//<--- LUT-BZ setting
      }
//--
      if(ib==14){//<--- settings for FTZ top/bot-signal's extention width        
        ftzwdcode=(word&0x1F);//topTOF p.width extention code
        ftzwdcode|=((word&0x1F00)>>3);//botTOF p.width extention code
        tofextwid=integer(ftzwdcode);
      }
//
      if(ib==15){//<--- settings for Lev1-phys.branches mask and Acc0/1-thresholds
        globl1mask=integer((~(word&0xFF))&0xFF);//update Lev1-phys.branches mask
        antsectmx[0]=integer((word&0xF00)>>8);//Max. Anti-sectors(equat. region)=1 
        antsectmx[1]=integer((word&0xF000)>>12);//Max. Anti-sectors(polar region)=0
      } 
//
    }//--->endof bit-set check
//
    }//--->endof bits loop
//      
  }//--->endof readout-bits word1 check
  if(formt==2)nrdow1+=1;//for compr.fmt count readout-bits word1 itself
//------------------------------------------- 
  if(nrdow2>0){//<---check readout-bits word2
//
    nw2=0;
    for(ib=0;ib<16;ib++){//<--- bit loop
    bit=(1<<ib);
    if((rstatw2&bit)>0){
      nw2+=1;
      word=*(p+trgsbias+nrdow1+nw2);
//
      if(ib>=0 && ib<8){//<--- LVL1-memb.settings for 8 physical branches
        physbrmemb[ib]=integer((~(word&0x7FFF))&0x7FFF);//Lin(disable)->Me(anable),except bit15 !
      }
//
      if(ib>=8 && ib<16){//<--- Presc.factors settings for 8 physical branches
        psfcode=(word&0x3FF);//presc.factor code
	l1trigconf.phbrprescf(ib-8)=prescfcode2val(psfcode);//presc.factor value
      }
//
    }//--->endof bit-set check
//
    }//--->endof bits loop
//      
  }//--->endof readout-bits word2 check
  if(formt==2)nrdow2+=1;//for compr.fmt count readout-bits word2 itself
*/
//------
  }//---> endof SetupBlock decoding
//--------------------------------------------------------------------------------------------------------
  if(SetupIsChanged){
    TGL1JobStat::daqs1(16);//"TrigSetupBlock" changes
    if((TGL1FFKEY.printfl%10)>=2){//print brief setup info (on change and request):
#pragma omp critical (print_shortsetup)
{ 
    cout<<"===================================================="<<endl;
    cout<<"|      TrigSetup changed for Run/Event="<<AMSEvent::gethead()->getrun()<<" "<<
                                                                AMSEvent::gethead()->getid()<<endl;
    cout<<"|        New SetupWordsPatterns(hex): "<<hex<<rstatw1<<" "<<rstatw2<<dec<<endl;
    cout<<"===================================================="<<endl;
}//--->endof pragma
    }//---> endof "PrintSetupInfo"
  }
// Don't save here to file/DB - tofinmask,tofinzmask may be not yet decoded, so save later in Trigger2LVL1::build
//---------------------------------------------------------------------------------------------------
#ifdef _OPENMP
// tempor debugging: decode scalers here and compare with latest ones in static arr(done in Early(Pre)Decoding): 
//
//==============================> Scalers Block decoding(CompFmt only, incl.LiveTime)
//  RECO is already done in BuildEarly !!! Here just counting for control !!!
//
  if(formt==2 && nrdow3>0){//<--- Compr.fmt scalers ("from-time-to-time") block found
    TGL1JobStat::daqs1(11);//"ScalersBlock" entries
    scalbias=trgsbias+nrdow1+nrdow2+2;//+2 to counts nw1/2 itself
    rstatw3=*(p+scalbias);//section's readout-members bit-pattern
    nw3=0;
//---
    bit=1;
    if((rstatw3&bit)>0){//<---time-gate for LiveTime
      word=*(p+scalbias+1+nw3);
//      tgid=(word&0x3);//2bits of time_gate.id(already extracted/shifted by Kounin)
//      if(tgid==3)tgatelt=2;//time-gate(sec)
//      else if(tgid==0)tgatelt=0.25;
//      else if(tgid==1)tgatelt=0.5;
//      else tgatelt=1;
      nw3+=1;
    }
//---
    bit=(1<<1);
    if((rstatw3&bit)>0){//<--- time-gate for scalers in trigger FPGA
      word=*(p+scalbias+1+nw3);
//      tgid=(word&0x3);//time-gate for trigger-scalers
//      if(tgid==3)tgatetr=2;//time-gate(sec)
//      else if(tgid==0)tgatetr=0.25;
//      else if(tgid==1)tgatetr=0.5;
//      else tgatetr=1;
      nw3+=1;
    }
//---
    bit=(1<<2);
    if((rstatw3&bit)>0){//<--- time-gate for scalers in scalers FPGA
      word=*(p+scalbias+1+nw3);
//      tgid=(word&0x3);//time-gate for scaler-scalers
//      if(tgid==3)tgatesc=2;//time-gate(sec)
//      else if(tgid==0)tgatesc=0.25;
//      else if(tgid==1)tgatesc=0.5;
//      else tgatesc=1;
      nw3+=1;
    }
//---
    bit=(1<<12);
    if((rstatw3&bit)>0){//<--- time-calibration
      word=*(p+scalbias+1+nw3);
//      nword=*(p+scalbias+1+nw3+1);
//      timcal=0;
//      timcal|=uinteger(word);
//      lword=uinteger(nword);//last 16bits of time-calib word
//      timcal|=(lword<<16);
      nw3+=2;
    }
//---
    bit=(1<<13);
    if((rstatw3&bit)>0){//<--- LiveTime1/2
      TGL1JobStat::daqs1(7);//"LiveTimeBlock" entries
      for(j=0;j<2;j++){// lt1/lt2
//        word=*(p+scalbias+1+nw3+1);//last 11bits + time-gate.id, No inversion for compr.format
//        nword=*(p+scalbias+1+nw3);//first 16bits
//        ltim=uinteger(nword);
//        lword=uinteger(word);//last 11bits of live_time + time_gate.id
//        ltim|=((lword&0x07FFL)<<16);
//        timgid=((lword&0x3000L)>>12);//2bits of time_gate.id
//        if(timgid==3)tgate=2;//time-gate(sec)
//        else if(timgid==0)tgate=0.25;
//        else if(timgid==1)tgate=0.5;
//        else tgate=1;
//        ltimec[j]=ltim;
//        ltimeg[j]=tgate;
//        if(tgate>0)livetm[j]=geant(number(ltim)*(2.e-8))/tgate;//livetime fraction(imply 20ns pulses period)
//	  else livetm[j]=0;
//        if(livetm[j]>1 || livetm[j]==0){
//          if((TGL1FFKEY.printfl/10)>=1){
//	    cout<<" <---- Trigger2LVL1::buildraw:W - Wrong LiveTime, tg/lt="<<
//	                tgate<<" LiveTime="<<livetm[j]<<" static="<<scalmon.LiveTime(j)<<endl;
//          }
//          TGL1JobStat::daqs1(6+2*j);//counts  LTime>1,=0
//        } 
        nw3+=2;
      }
    }
//---
    bit=(1<<14);
    if((rstatw3&bit)>0){//<--- FT/LVL1-rates
//      for(j=0;j<5;j++){//FT-rates(FT,FTC,FTZ,FTE,NonPhys) 
//        word=*(p+scalbias+1+nw3+j);
//	  scrate=number(word)/tgatetr;
//	  tgrates[j]=scrate;
//      }
      nw3+=5;
//-
//      for(j=0;j<9;j++){//<--- Lev1,subLev1 rates
//        word=*(p+scalbias+1+nw3+j);
//	  scrate=number(word)/tgatetr;
//	  tgrates[5+j]=scrate;
//      }
      nw3+=9;
    }
//---
    bit=(1<<15);
    if((rstatw3&bit)>0){//<--- Detector-rates(SideMax for Tof(CP)/Tof(BZ)/Acc/ECf(FTE)/ECa(L1))
//      for(j=0;j<5;j++){
//        word=*(p+scalbias+1+nw3+j);
//	  scrate=number(word)/tgatesc;
//	  dtrates[j]=scrate;
//      }
      nw3+=5;
    }
//-------
//    if(livetm[0]!=scalmon.LiveTime(0) || livetm[1]!=scalmon.LiveTime(1)){
//      cerr<<"TriggerLVL1-E-Event/Static LiveTimes Mismatch "<<livetm[0]<<" "<<scalmon.LiveTime(0)<<" "
//          <<livetm[1]<<" "<<scalmon.LiveTime(1)<<endl;
//    }
  }//--->endof Scalers Block decoding
#endif
//---------------
  if(ScalerIsChanged){//print brief scalers info (on change and request):
    TGL1JobStat::daqs1(12);//"ScalersBlock" changes
    if((TGL1FFKEY.printfl%10)>=2){
#pragma omp critical (print_scaler)
{ 
      cout<<"=============================================================="<<endl;
      cout<<"|  ScalersBlock params changed, Run/Event="<<AMSEvent::gethead()->getrun()<<" "<<
                                                                  AMSEvent::gethead()->getid()<<endl;
      cout<<"|        New ScalersBlockWordsPatterns(hex): "<<hex<<rstatw3<<dec<<endl;
   if((rstatw3&0xc000)!=0){
      cout<<"| Rates: Lvl1="<<scalmon.LVL1trig(0)<<"  FT="<<scalmon.FTtrig(0)<<" FTC="<<scalmon.FTtrig(1)<<endl;
      cout<<"|        TofLSideMX="<<scalmon.DetMaxRate(0)<<" AccMX="<<scalmon.DetMaxRate(2)<<" EcMX="<<scalmon.DetMaxRate(3)<<endl;
   }
   if((rstatw3&0x2000)!=0){
      cout<<"| LiveTime="<<scalmon.LiveTime(0)<<" "<<scalmon.LiveTime(1)<<endl;
   }
   if((rstatw3&0x1000)!=0){
      cout<<"| TimeCalib="<<scalmon.TimeCalib()<<endl;
   }
      cout<<"=============================================================="<<endl;
}//--->endof pragma
    }//---> endof "PrintSetupInfo"
//    ScalerIsChanged=false;//reset
  }//---> endof "ScalerIsChanged"
//-------------------------------------------------------------------------------------------
//
//===================================> create Lev1-object:
//
//---> take latest updates on some non "event-by-event" variables (important Scalers-block data):
//
  if(formt==2){//CompFmt: take TimeCalib/LiveTime from static arrs (done in Early(Pre)Decoding)
    LiveTime[0]=scalmon.LiveTime(0); 
    LiveTime[1]=scalmon.LiveTime(1);
    trtime[0]=scalmon.TimeCalib();
  }
  else if(formt==1){//RawFmt: above info exists in every events 
    LiveTime[0]=livetm[0]; 
    LiveTime[1]=livetm[1];
    trtime[0]=timcal;
  }
  if((TGL1FFKEY.printfl%10)>0){
#pragma omp critical (hf1)
{
    HF1(1094,geant(LiveTime[0]),1.);
}
  } 
// 
  for(j=0;j<5;j++){//FT-rates(FT,FTC,FTZ,FTE,NonPhys)
    TrigRates[j]=scalmon.FTtrig(j);
  } 
  for(j=0;j<9;j++){//<--- Lev1,subLev1 rates
    TrigRates[j+5]=scalmon.LVL1trig(j);
  }
  for(j=0;j<5;j++){//SubDet rates
    TrigRates[j+14]=scalmon.DetMaxRate(j);
  }
//--- some rec0-statistics:
  if(TofFlag1>=0 || TofFlag2>=0 || (EcalFlag/10)>=2)TGL1JobStat::daqs1(35);//FT in LVL1 ok
  if(JMembPatt>0 && PhysBPatt>0)TGL1JobStat::daqs1(36);//
  if(JMembPatt==0)TGL1JobStat::daqs1(32);//count empty TrigPatt entries
  if(PhysBPatt==0)TGL1JobStat::daqs1(33);//count empty TrigPatt entries
  if(auxtrpat>0)TGL1JobStat::daqs1(34);//count non-empty AuxTrPatt
  if(((JMembPatt&1)>0 || (JMembPatt&(1<<5))>0) && (JMembPatt&(1<<6))>0)TGL1JobStat::daqs1(38);
//
//
  if(TrigRates[5]<TGL1FFKEY.MaxScalersRate && LiveTime[0]>=TGL1FFKEY.MinLifeTime)TGL1JobStat::daqs1(37);
//     
  if(
       (auxtrpat > 0)                                                               //aux.tr
                     || (JMembPatt>0 || PhysBPatt>0)
                                                    ){
    AMSEvent::gethead()->addnext(AMSID("TriggerLVL1",0),
    new Trigger2LVL1(PhysBPatt,JMembPatt,auxtrpat,TofFlag1,TofFlag2,tofpat1,tofpat2,
                                AntiPatt,EcalFlag,ecpat,ectrs,LiveTime[0],TrigRates,trtime));
    TGL1JobStat::daqs1(14);//count created LVL1-objects(good event)    
    Trigger2LVL1 *ptr=(Trigger2LVL1*)AMSEvent::gethead()->getheadC("TriggerLVL1",0);
    if(!dataf)ptr->setstatus(AMSDBc::TGL1NONDATA);
    if(!noerr)ptr->setstatus(AMSDBc::TGL1ASSERR);
    if(siderr)ptr->setstatus(AMSDBc::TGL1SIDERR);
    if(!dataf || !noerr || siderr)TGL1JobStat::daqs1(17);
  }
  else{
    TGL1JobStat::daqs1(15);//count errored entries    
    AMSEvent::gethead()->seterror();
  }

  return;
//
BadExit:
  if(err++<1000){
    cerr<<" TriggerLVL1::buildraw-E- Was fatal decoding error (rejected) ! "<<endl;
    cerr<<"              ErrCounts:"<<err1<<" "<<err2<<" "<<err3<<" "<<err4<<endl;
  }
  TGL1JobStat::daqs1(13);//count rejected LVL1-entries(segments)

}




//------------------------------------------------------------------------
integer Trigger2LVL1::buildrawearly(integer len, int16u *p){
//called in pdaq->read where the events order is not affected by MP.
//this is important for those trigger variables which are appeared/updated
// not in every event and the order is important for calculation of ev-by-ev
// differencies, partial updates, some info from 1st event of run(job), etc
//
// on input: len=tot_block_length as given at the beginning of block
//           *p=pointer_to_beggining_of_block_data (next to the length word)
//
//
  integer lenoncall,lencalc(0);
  static number evtprev(-1);
  static int16u parbitpatt[3]={0,0,0};
  number evtcurr(0),delevt;
  uinteger ntrst(0),timcal(0);
  uinteger time[2]={0,0};
  uinteger trtime[5]={0,0,0,0,0};
//
  bool PreAssRD=(AMSJob::gethead()->isRealData()
          && (DAQEvent::gethead()->runno()<1213470000)) ;//flag to identify RD of preassembly period
//
  integer i,j,ib,wvar;
  int16u bit,rrr;
  int16u word,nword,luti,lutf=0;
  uinteger lword;
  uinteger ltim(0);
  geant tgate(0);
  geant tgatelt(0),tgatetr(0),tgatesc(0);
  uinteger timgid;
  integer gftmsk(0),ftzlmsk(0);
  int16u ftzwdcode(0);
  geant LiveTime[2]={0,0};
//  
  int16u datyp(0),formt(0);
  int16u jblid,jleng,csid,psfcode;
//
  int16u rstatw1(0),rstatw2(0),rstatw3(0);
  int16u nrdow1(0),nrdow2(0),nrdow3(0);
  int16u nw1,nw2,nw3;
//
  int ltimbias;//tempor bias to live-time data(in "trig"-block)
  int trgsbias;//bias to trig.setup/status block
  int pattbias;//bias to trig.patt sub-block
  int scalbias=0;//bias to scalers sub-block
//
  lenoncall=(len&(0xFFFFL));
  TGL1JobStat::daqs1(40);//count entries
//  
  SetupIsChanged=false;// reset for each event (treadprivate val)
  ScalerIsChanged=false;// reset for each event (treadprivate val)
//
  p=p-1;//to follow VC-convention
  jleng=int16u(len&(0xFFFFL));
  csid=int16u((len>>16)&(0xFFFFL))+1;//S(s=1-4=>a,b,p,s) as return by my "checkblockid"
  jblid=*(p+jleng);// JLV1 fragment's last word: Status+slaveID(present if not JMDC request)
//
  //bool dataf=((jblid&(0x8000))>0);//data-fragment
  bool crcer=((jblid&(0x4000))>0);//CRC-error
//  asser=false;
  bool amswer=((jblid&(0x1000))>0);//amsw-error   
  bool timoer=((jblid&(0x0800))>0);//timeout-error   
  bool fpower=((jblid&(0x0400))>0);//FEpower-error   
  bool seqer=((jblid&(0x0400))>0);//sequencer-error
  bool cdpnod=((jblid&(0x0020))>0);//CDP-node(i.e. with no futher fragmentation)
  bool noerr;
  bool LTwinv=false;
//
  uinteger runn=DAQEvent::gethead()->runno();
  time_t evtime=DAQEvent::gethead()->time();
//
  if(AMSUser::PreviousRunN()!=runn){
    if(AMSUser::PreviousRunN()==0){
      AMSUser::JobFirstRunN()=runn;
      AMSUser::PreviousRunN()=runn;
      AMSUser::JobFirstEventT()=evtime;
      cout<<"--->Trigger2LVL1::buildrawearly:NewJob:Run/EvTime="<<AMSUser::JobFirstRunN()<<" "<<AMSUser::JobFirstEventT()<<endl;
    }
    AMSUser::RunFirstEventT()=evtime;
    cout<<"--->Trigger2LVL1::buildrawearly:NewRun:Run/EvTime="<<runn<<" "<<AMSUser::RunFirstEventT()<<endl;
    AMSUser::PreviousRunN()=runn;
  }
//
  datyp=((jblid&(0x00C0))>>6);//(0-should not be),1,2,3(raw/compr/mix)
//    
  if(jleng>1){
    TGL1JobStat::daqs1(41);//<=== count non-empty fragments
  }
  else goto BadExit;
//
  if(csid>0 && csid<=4){
    if(csid==3)csid=1;//tempor
    else if(csid==4)csid=2;
    TGL1JobStat::daqs1(42+csid-1);//sides sharing
  }
  else{
    csid=1;
//    goto BadExit;//unknown side(ignore here, status-bit will be set in buildraw)
  }
//
  TGL1JobStat::daqs1(44+datyp);//<=== count lvl1's format-types
  if(datyp==0){
    goto BadExit;//unknown format
  }
//
//  if(!dataf){// do not check here: check later in buildraw and set status "bad" if !dataf
//    goto BadExit;//not data
//  }
//
//
  noerr=(!crcer 
//               && !asser
	                && !amswer 
			          && !timoer 
				            && !fpower 
					              && !seqer
						               && cdpnod);
  if(noerr){
    TGL1JobStat::daqs1(48);//<=== count no-errors fragments
  }
//  else goto BadExit;//ignore here,status bit will be set at lvl1-obj creation time !!!)
//
  if((PreAssRD && jleng==52 && datyp==2)//RD PreAssPeriod(RawFmt but datyp=2!!!; jleng=52 due to 2 empty-words)
    || (jleng==52 && datyp==1))//MC/newRD(in RawFmt with datyp=1)
  {
    pattbias=0;//bias to patt. sub-block
    trgsbias=15;//bias to trig-setup sub-block(points to PREVIOUS to the 1st sub-block word)
    ltimbias=48;//bias to time sub-block
    formt=1;//means real raw fmt
    if(PreAssRD && jleng==52 && datyp==2)LTwinv=true;//invert for 2008 data
  }
  else if(AMSJob::gethead()->isRealData() && datyp==2){//futur RD in true compr.format
    pattbias=0;//bias to patterns sub-block
    trgsbias=12;//bias to trig-setup sub-block(p+trgsbias points to 1st readout-status word)
    formt=2;//means real compr fmt
  } 
  else {
      goto BadExit;
  }
//
  TGL1JobStat::daqs1(49);//<=== entries to decoding
//
//----------------> fill(update) here Lev1 static objects l1trigconf and scalmon :
//
//--->extract TimCal info for RawFmt(always present, but may be not h/w-updated):
//
  lencalc=3+4+4;//add always present but not decoded here 3 busy-patt words, 4 trigpatt words 
//                                                          and decoded at the end 4 EventTime(tr.time) words
  if(formt==1){//RD PreAssPeriod OR RawFmt
    TGL1JobStat::daqs1(51);// count RawFmt TimeCalib updates
// attention:  i suppose that 0x0D(p+14) and 0x0E(p+15) words exist but empty
    timcal=0;
    word=*(p+12+pattbias);//1st 16bits of time-calib word
    timcal|=uinteger(word);
    lword=uinteger(*(p+13+pattbias));//last 16bits of time-calib word
    timcal|=(lword<<16);
    lencalc+=4;//add time-calib and 2 empty words for RawFmt
    scalmon.TimeCalib()=timcal;
  }
//
//----> extract LiveTime info for RawFmt(always present, but may be not h/w-updated):
//
  if(formt==1){
    TGL1JobStat::daqs1(52);// count RawFmt LiveTime updates
// warning: 1st and 2nd words are interchanged in each livetime pair
    for(j=0;j<2;j++){// lt1/lt2
      if(LTwinv)word=*(p+ltimbias+1+2*j);//1st 16bits of live-time, 
      else word=*(p+ltimbias+2*j);
      ltim=uinteger(word);
      if(LTwinv)lword=uinteger(*(p+ltimbias+2*j));//last 11bits of live_time + time_gate.id
      else lword=uinteger(*(p+ltimbias+1+2*j));
      ltim|=((lword&0x07FFL)<<16);
      timgid=((lword&0x3000L)>>12);//2bits of time_gate.id
      if(timgid==3)tgate=2;//time-gate(sec)
      else if(timgid==0)tgate=0.25;
      else if(timgid==1)tgate=0.5;
      else tgate=1;
      LiveTime[j]=geant(number(ltim)*(2.e-8))/tgate;//livetime fraction(imply 20ns pulses period)
      if(LiveTime[j]>1){
//        LiveTime[j]=1; 
        TGL1JobStat::daqs1(53+j);//counts  LTime>1
      }
//
      scalmon.LiveTime(j)=LiveTime[j]; 
    }//--->endof lt1/2 loop
    lencalc+=4;//add LiveTime-block leng in RawFmt
  }//------->endof "LiveTime" block(RawFmt only)
//
//========> check the presence/length of TrigSetup/Scaler blocks:
//
  if(formt==1){//raw fmt
    rstatw1=(0xFFFF);//all bits set(complete set of 1-16 setup words)
    rstatw2=(0xFFFF);//all bits set(complete set of 17-32 setup words)
    rstatw3=0;//scalers info always missing
    nrdow1=16;
    nrdow2=16;
    nrdow3=0;
    lencalc+=(nrdow1+nrdow2);//add Setup-block leng. in RawFmt
  }
  else{//ComprFormat
    nrdow1=0;
    nrdow2=0;
    nrdow3=0;
    rstatw1=*(p+trgsbias);//1st readout-status word
    for(ib=0;ib<16;ib++){
      bit=(1<<ib);
      if((rstatw1&bit)>0)nrdow1+=1;
    }
    rstatw2=*(p+trgsbias+nrdow1+1);//2nd readout-status word
    for(ib=0;ib<16;ib++){
      bit=(1<<ib);
      if((rstatw2&bit)>0)nrdow2+=1;
    }
    lencalc+=nrdow1+nrdow2+2;//add Setup-block leng. in CompFmt(+2 for 2 readoutstat-words)
//calc.scalers-block length
    scalbias=trgsbias+nrdow1+nrdow2+2;
    rstatw3=*(p+scalbias);//section's readout-members bit-pattern
    for(ib=0;ib<16;ib++){
      bit=(1<<ib);
      if((rstatw3&bit)>0){
        if(ib<=2)nrdow3+=1;
	if(ib==12)nrdow3+=2;
	if(ib==13)nrdow3+=4;
	if(ib==14)nrdow3+=14;
	if(ib==15)nrdow3+=5;
      }
    }
    lencalc+=nrdow3+1;//add Scalers-blocks length in CompFmt 
  }
//========>
  if((lencalc+1)!=lenoncall){
    TGL1JobStat::daqs1(55);//wrong segment length
    if((TGL1FFKEY.printfl/10)>=1)cout<<"<---- Trigger2LVL1::BuildRawEarly: length error, LengInCall="
                                          <<lenoncall<<" CalcLeng="<<lencalc<<" datyp="<<datyp<<endl;
    goto BadExit;
  }
//=============================> TrigSetup block decoding:
// This section updates TrigConfig parameters of current Lvl1TrigConfig-object(static l1trigconf) in memory
//  CP/CT/BZinp-masks are not stored in Lvl1TrigConfig-object for the moment, so the related 
//       decoded info is not used now !!!!!!
//
  if(nrdow1>0 || nrdow2>0){
  TGL1JobStat::daqs1(56);//"TrigSetupBlock" non-empty entries
//
  nw1=0;
  if(nrdow1>0){//<---check readout-bits word1
//
    for(ib=0;ib<16;ib++){//<--- bit loop
    bit=(1<<ib);
    if((rstatw1&bit)>0){
      nw1+=1;
      word=*(p+trgsbias+nw1);
//--
      if(ib==0){//<---Anti-mask
        for(i=0;i<ANTI2C::MAXANTI;i++){//update antinmask
          if((word&1<<(8+i))==0 && (word&1<<i)==0)l1trigconf.antinmask(i)=1;//s1&s2 "in"
          else if((word&1<<(8+i))==0)l1trigconf.antinmask(i)=2;//s1(n) "in"
          else if((word&1<<i)==0)l1trigconf.antinmask(i)=3;//s2(p) "in"
          else l1trigconf.antinmask(i)=0;
        }
      }
//--
      if(ib==1){//<---CP/CT-inp mask
//CP/CT-mask are ignored - i have pad-individual masks tofinmask(il,ib))
      }
//--
      if(ib==2){//<---BZ/ECAL masks
//BZ-mask is ignored - i have pad-individual masks tofinzmask(il,ib))
//
        wvar=0;
        if((word&1<<8)==0)wvar+=1;//ECft x-proj active(ababled)
        if((word&1<<9)==0)wvar+=10;//ECft y-proj active
        if((word&1<<10)==0)wvar+=100;//ECangl x-proj active
        if((word&1<<11)==0)wvar+=1000;//ECangl y-proj active
        l1trigconf.ecprjmask()=wvar;//update ecprjmask
      }
//--
      if(ib==3){//<---   TRST/FEbusy-masks 
// ignored
      }
//--
      if(ib==4){
// ignored
      }
      if(ib==5){
// ignored
      }
//--
      if(ib==6){//<--- Tof CP/CT/BZ sides OR/AND setting
        for(i=0;i<TOF2GC::SCLRS;i++){//update tofoamask and tofoazmask
          if((word&(1<<i))>0)l1trigconf.tofoamask(i)=1;//or for CP-sides(FTC)
          else l1trigconf.tofoamask(i)=0;//and
          if((word&(1<<(i+8)))>0)l1trigconf.tofoazmask(i)=1;//or for BZ-sides
          else l1trigconf.tofoazmask(i)=0;//and
        }
        if(AMSJob::gethead()->isRealData()){//RD L3<->L4 problem treatment
	  rrr=l1trigconf.tofoamask(2);
	  l1trigconf.tofoamask(2)=l1trigconf.tofoamask(3);
	  l1trigconf.tofoamask(3)=rrr;
	  rrr=l1trigconf.tofoazmask(2);
	  l1trigconf.tofoazmask(2)=l1trigconf.tofoazmask(3);
	  l1trigconf.tofoazmask(3)=rrr;
        }
      }
//--
      if(ib==7){//<--- ACC sides OR/AND setting
        for(i=0;i<ANTI2C::MAXANTI;i++){//update antioamask
          if((word&(1<<i))>0)l1trigconf.antoamask(i)=1;//or
          else l1trigconf.antoamask(i)=0;//and
        }
      }
//--
      if(ib==8){//<--- Settings for internal trigger
//ignore for the moment
      }
//      
      if(ib==9){//<--- Settings for external/LA trigger
//ignore for the moment
      }
//--
      if(ib==10){//<--- settings for FT
        if((word&1)==0)gftmsk+=1;
        if((word&2)==0)gftmsk+=10;
        if((word&4)==0)gftmsk+=100;
        l1trigconf.globftmask()=gftmsk;//update globFTmask(FTE|FTZ|FTC)
        if((word&(1<<7))>0)ftzlmsk+=10;//OR of top/bot in FTZ-trig
        if((word&(1<<5))>0 && (word&(1<<6))>0)ftzlmsk+=3;//(1|2)+(3|4)
        else if((word&(1<<5))>0 && (word&(1<<6))==0)ftzlmsk+=2;//(1|2)+(3&4)
        else if((word&(1<<5))==0 && (word&(1<<6))>0)ftzlmsk+=1;//(1&2)+(3|4)
        l1trigconf.toflcsz()=ftzlmsk;//update FTZ layers or/and mask
        if((word&(1<<8))>0)l1trigconf.ecorand()=1;//update FTE-proj or/and(1/2->or/and) requir.
        else l1trigconf.ecorand()=2;
      }
//--
      if(ib>=11 && ib<=13){ 
        luti=(word&0xFFFE);
	if(AMSJob::gethead()->isRealData())lutf=lutbswap(luti);//RD L3<->L4 problem treatment for LUTs
	else lutf=luti;
      }
      if(ib==11)l1trigconf.toflut1()=integer(lutf);//<--- LUT-FT0 setting
//        
      if(ib==12)l1trigconf.toflut2()=integer(lutf);//<--- LUT-FT1 setting
//        
      if(ib==13)l1trigconf.toflutbz()=integer(lutf);//<--- LUT-BZ setting
//--
      if(ib==14){//<--- settings for FTZ top/bot-signal's extention width        
        ftzwdcode=(word&0x1F);//topTOF p.width extention code
        ftzwdcode|=((word&0x1F00)>>3);//botTOF p.width extention code
        l1trigconf.tofextwid()=integer(ftzwdcode);
      }
//
      if(ib==15){//<--- settings for Lev1-phys.branches mask and Acc0/1-thresholds
        l1trigconf.globl1mask()=integer((~(word&0xFF))&0xFF);//update Lev1-phys.branches mask
        l1trigconf.antsectmx(0)=integer((word&0xF00)>>8);//Max. Anti-sectors(equat. region)=1 
        l1trigconf.antsectmx(1)=integer((word&0xF000)>>12);//Max. Anti-sectors(polar region)=0
      } 
//
    }//--->endof bit-set check
//
    }//--->endof bits loop
//      
  }//--->endof readout-bits word1 check
  if(formt==2)nrdow1+=1;//for compr.fmt count readout-bits word1 itself
//------------------------------------------- 
  if(nrdow2>0){//<---check readout-bits word2
//
    nw2=0;
    for(ib=0;ib<16;ib++){//<--- bit loop
    bit=(1<<ib);
    if((rstatw2&bit)>0){
      nw2+=1;
      word=*(p+trgsbias+nrdow1+nw2);
//
      if(ib>=0 && ib<8){//<--- LVL1-memb.settings for 8 physical branches
        l1trigconf.physbrmemb(ib)=integer((~(word&0x7FFF))&0x7FFF);//Lin(disable)->Me(anable),except bit15 !
      }
//
      if(ib>=8 && ib<16){//<--- Presc.factors settings for 8 physical branches
        psfcode=(word&0x3FF);//presc.factor code
	l1trigconf.phbrprescf(ib-8)=prescfcode2val(psfcode);//presc.factor value
      }
//
    }//--->endof bit-set check
//
    }//--->endof bits loop
//      
  }//--->endof readout-bits word2 check
  if(formt==2)nrdow2+=1;//for compr.fmt count readout-bits word2 itself
  }//------> endof SetupBlock decoding
//----
//----------> check change of TrigSetup
//
  if((rstatw1!=0 && rstatw1!=parbitpatt[0]) || (rstatw2!=0 && rstatw2!=parbitpatt[1])){
    TGL1JobStat::daqs1(57);//TrigSetup changes
    parbitpatt[0]=rstatw1;
    parbitpatt[1]=rstatw2;
    SetupIsChanged=true;
  }
//--------------------------------------------------------------------------------------------------------
//=========================> Scalers Block decoding(CompFmt only, incl.LiveTime)
  int16u tgid;
  number scrate;
  if(formt==2 && nrdow3>0){//<--- scalers(+live_time+gate_width) "from-time-to-time" block
    TGL1JobStat::daqs1(58);//"ScalersBlock" entries
    rstatw3=*(p+scalbias);//section's readout-members bit-pattern
    nw3=0;
//---
    bit=1;
    if((rstatw3&bit)>0){//<---time-gate for LiveTime
      word=*(p+scalbias+1+nw3);
      tgid=(word&0x3);//2bits of time_gate.id(already extracted/shifted by Kounin)
      if(tgid==3)tgatelt=2;//time-gate(sec)
      else if(tgid==0)tgatelt=0.25;
      else if(tgid==1)tgatelt=0.5;
      else tgatelt=1;
      nw3+=1;
      scalmon.TGateLT()=tgatelt;//update LiveTime-scaler gate in static var.
    }
//---
    bit=(1<<1);
    if((rstatw3&bit)>0){//<--- time-gate for scalers in trigger FPGA
      word=*(p+scalbias+1+nw3);
      tgid=(word&0x3);//time-gate for trigger-scalers
      if(tgid==3)tgatetr=2;//time-gate(sec)
      else if(tgid==0)tgatetr=0.25;
      else if(tgid==1)tgatetr=0.5;
      else tgatetr=1;
      nw3+=1;
      scalmon.TGateTR()=tgatetr;//update in static var.
    }
//---
    bit=(1<<2);
    if((rstatw3&bit)>0){//<--- time-gate for scalers in scalers FPGA
      word=*(p+scalbias+1+nw3);
      tgid=(word&0x3);//time-gate for scaler-scalers
      if(tgid==3)tgatesc=2;//time-gate(sec)
      else if(tgid==0)tgatesc=0.25;
      else if(tgid==1)tgatesc=0.5;
      else tgatesc=1;
      nw3+=1;
      scalmon.TGateSC()=tgatesc;//update in static var.
    }
//---
    bit=(1<<12);
    if((rstatw3&bit)>0){//<--- time-calibration
      TGL1JobStat::daqs1(59);//"time-calib" entries
      word=*(p+scalbias+1+nw3);
      nword=*(p+scalbias+1+nw3+1);
      timcal=0;
      timcal|=uinteger(word);
      lword=uinteger(nword);//last 16bits of time-calib word
      timcal|=(lword<<16);
      scalmon.TimeCalib()=timcal;
      nw3+=2;
    }
//---
    bit=(1<<13);
    if((rstatw3&bit)>0){//<--- LiveTime1/2
      TGL1JobStat::daqs1(60);//"LiveTimeBlock" entries
      for(j=0;j<2;j++){// lt1/lt2
        word=*(p+scalbias+1+nw3+1);//last 11bits + time-gate.id
        nword=*(p+scalbias+1+nw3);//first 16bits
        ltim=uinteger(nword);
        lword=uinteger(word);//last 11bits of live_time + time_gate.id
        ltim|=((lword&0x07FFL)<<16);
        timgid=((lword&0x3000L)>>12);//2bits of time_gate.id
        if(timgid==3)tgate=2;//time-gate(sec)
        else if(timgid==0)tgate=0.25;
        else if(timgid==1)tgate=0.5;
        else tgate=1;
	if(tgate!=scalmon.TGateLT()){//just check Kunin
	  if((TGL1FFKEY.printfl/10)>=1)cout<<" <---- Trigger2LVL1::BuildRawEarly:W - Event/static LiveTime-gate mismatch: "<<
	                                             tgate<<"/"<<scalmon.TGateLT()<<endl;
	  if(tgate==0)tgate=scalmon.TGateLT();
	}
        if(tgate>0)LiveTime[j]=geant(number(ltim)*(2.e-8))/tgate;//livetime fraction(imply 20ns pulses period)
	else LiveTime[j]=0;
        if(LiveTime[j]>1 || LiveTime[j]==0){
          if((TGL1FFKEY.printfl/10)>=1)cout<<" <---- Trigger2LVL1::BuildRawEarly:W - Wrong LiveTime, tgate="<<
	                 tgate<<" static="<<scalmon.TGateLT()<<" type="<<j+1<<" LiveTime="<<LiveTime[j]<<endl;
//           LiveTime[j]=1; 
          TGL1JobStat::daqs1(61+j);//counts  LTime>1
        } 
        scalmon.LiveTime(j)=LiveTime[j];//copy to scalmon
        nw3+=2;
      }
      if((TGL1FFKEY.printfl%10)>0){//for compr.fmt fill here(for raw - in BuildRaw because in have it each event)
//#pragma omp critical (hf1)
//{
//        HF1(1094,geant(scalmon.LiveTime(0)),1.);
//}
      } 
    }
//---
    bit=(1<<14);
    if((rstatw3&bit)>0){//<--- FT/LVL1-rates
      TGL1JobStat::daqs1(63);//"FT/LVL1-rates" entries
      for(j=0;j<5;j++){//FT-rates(FT,FTC,FTZ,FTE,NonPhys) 
        word=*(p+scalbias+1+nw3+j);
	if(scalmon.TGateTR()>0)scrate=number(word)/scalmon.TGateTR();
	else{
	  scrate=0;
	  if((TGL1FFKEY.printfl/10)>0){
	    cout<<"<---- Trigger2LVL1::BuildRawEarly:W - TrigScallersGate=0 !!!"<<endl;
	  }
	}
        scalmon.FTtrig(j)=scrate;//set FTs rates
        if(j>0 && j<4 && (TGL1FFKEY.printfl%10)>0){//for compr.fmt
#pragma omp critical (hf1)
{
          HF1(1294+j,geant(scalmon.FTtrig(j)),1.);
}
        } 
      }
      nw3+=5;
//-
      for(j=0;j<9;j++){//<--- Lev1,subLev1 rates
        word=*(p+scalbias+1+nw3+j);
	if(scalmon.TGateTR()>0)scrate=number(word)/scalmon.TGateTR();
	else{
	  scrate=0;
	}
        scalmon.LVL1trig(j)=scrate;
        if(j<1 && (TGL1FFKEY.printfl%10)>0){//for compr.fmt
#pragma omp critical (hf1)
{
          HF1(1298+j,geant(scalmon.LVL1trig(j)),1.);//FTC/FTZ/FTE
}
        } 
      }
      nw3+=9;
    }
//---
    bit=(1<<15);
    if((rstatw3&bit)>0){//<--- Detector-rates(SideMax for Tof(cp)/Tof(bz)/Acc/ECf/ECa)
      TGL1JobStat::daqs1(64);//"SubDet-rates" entries
      for(j=0;j<5;j++){
        word=*(p+scalbias+1+nw3+j);
	if(scalmon.TGateSC()>0)scrate=number(word)/scalmon.TGateSC();
	else{
	  scrate=0;
	  if((TGL1FFKEY.printfl/10)>0){
	    cout<<"<---- Trigger2LVL1::BuildRawEarly:W - SubdScallersGate=0 !!!"<<endl;
	  }
	}
        scalmon.DetMaxRate(j)=scrate;
        if(j==0 && (TGL1FFKEY.printfl%10)>0){//for compr.fmt
#pragma omp critical (hf1)
{
          HF1(1294+j,geant(scalmon.DetMaxRate(j)),1.);//TofPlaneSide-MaxRate
}
        } 
      }
      nw3+=5;
    }
  }//--->endof Scalers Block decoding
//----
//----------> check change of any scaler-block parameter
//
//  if(rstatw3!=0 && rstatw3!=parbitpatt[2]){
  if(rstatw3!=0){
    TGL1JobStat::daqs1(65);//ScalersBlock changes
    if((TGL1FFKEY.printfl/10)>1)
       cout<<"<--- InBuildEarly :ScalersChanged:rstatw3/static="<<hex<<rstatw3<<" "<<parbitpatt[2]<<dec<<endl; 
    parbitpatt[2]=rstatw3;
    ScalerIsChanged=true;
  }
//========================> TrigTime info decoding (to calc. delta time):
  if(formt==1 || formt==2){
//-----> event(trigger) time info(always sitting at fixed bias 5->) : 
    time[0]=0;
    time[1]=0;
    word=*(p+5+pattbias);//1st 16bits of time
    time[0]|=uinteger(word);
    lword=uinteger(*(p+6+pattbias));//2nd 16bits of time
    time[0]|=(lword<<16);
    word=*(p+7+pattbias);//last 8bits of time +1st 8bits of ntrst
    time[1]=uinteger((word&0x00FF));
    ntrst|=uinteger((word&0xFF00)>>8);
    lword=uinteger(*(p+8+pattbias));//last 16bits of ntrst
    ntrst|=(lword<<8);
    trtime[1]=ntrst;
    trtime[2]=time[0];//lsb
    trtime[3]=time[1];//msb
//
    if(ntrst>0)evtcurr=1000000.*(ntrst-1)+time[0]*0.64+time[1]*pow(2.,32)*0.64;//mksec
    else evtcurr=time[0]*0.64+time[1]*pow(2.,32)*0.64;//tempor
    if(evtprev>0){
      delevt=evtcurr-evtprev;
      evtprev=evtcurr;
      if(delevt>(0xFFFF))trtime[4]=uinteger(0xFFFF);
      else trtime[4]=uinteger(floor(delevt));
    }
    else evtprev=evtcurr;
    return trtime[4];
  }//----> endof TigTime decoding
//
BadExit:
  TGL1JobStat::daqs1(50);//<=== bad exits
  static int err1=0;
  if(err1++<1000)cerr<<" TriggerLVL1::buildrawEarly-E-BadLvl1BlockRejected "<<endl;
  return -1;
}



//------------------------------------------------------------

uinteger Trigger2LVL1::Scalers::_GetIndex(time_t time ){
static integer k=0;
// first check k; then either k+1 or run binary search

  if(time>=_Tls[0][k] && time < (k<int(_Nentries)-1?_Tls[0][k+1]:time+1)){
   return k;
  }
  else if(k++<int(_Nentries)-1 ){
   if(time>=_Tls[0][k] && time < (k<int(_Nentries)-1?_Tls[0][k+1]:time+1)){
   return k;
   }
  }
  k=abs(AMSbins(_Tls[0], uinteger(time), _Nentries))-1;   
  if(k<0){
   cerr<<" Trigger2LVL1::Scalers::_GetIndex-S-IndexLessZero "<<k<<" "<<time<<endl;
   k=0;
   AMSEvent::gethead()->seterror();   
  }   
return k;
}
//------------- DAQ interface: --------------
integer Trigger2LVL1::checkdaqid(int16u blid){
  int valid(0);
  char sstr[128];
  char side[5]="ABPS";
  char str[2];
  for(int j=0;j<4;j++){//sides loop
    str[0]=side[j];
    str[1]='\0';
    sprintf(sstr,"JLV1-%s",str);
    if(DAQEvent::ismynode(blid,sstr)){
      valid=j+1;//Active Side
//cout<<"<--- Found block:"<<blid<<" "<<hex<<blid<<dec<<" name:"<<sstr<<" Side="<<valid<<endl;
      return valid;
    }
  }
  return valid;
}
//---
void Trigger2LVL1::node2side(int16u nodeid, int16u &side){
//  on input: slaveid=(blid&0x001F)
//  on output: side=(1,2) -> (a,b), or 0 if nodeid is missing
  side=0;
  for(int i=0;i<2;i++)if(nodeid == nodeids[i])side=i+1;
}
integer Trigger2LVL1::calcdaqlength(int i){//imply only raw format(but no evnum in front of block)
  Trigger2LVL1 *ptr=(Trigger2LVL1*)AMSEvent::gethead()->getheadC("TriggerLVL1",0);
  if(ptr)return -52;
  else return 0;
}
integer Trigger2LVL1::getmaxblocks(){
  if(AMSJob::gethead()->isSimulation())return 1;//only 1 side (a) in MC-daq
  else return 2;//any sides possible
}
//----------------------------------------------------
void Trigger2LVL1::EventBitDump(integer leng, int16u *p, char * message){
// p points to length word !!!
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
  cout<<"======> Lev1DaqBlock::"<<message<<" for event:"<<AMSEvent::gethead()->getid()<<endl;
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
//------------------------------------------------------------
int16u Trigger2LVL1::lutbswap(int16u luti){
//solve RD L3<->L4 problem for LUTs
  int16u lutf(0);
//
  for(int bit=0;bit<16;bit++){
    if((luti&(1<<bit))>0){
      if(bit<=3 || bit>=12)lutf|=(1<<bit);
      else if(bit<=7)lutf|=(1<<(bit+4));
      else lutf|=(1<<(bit-4));
    }
  }
  return lutf;
}



