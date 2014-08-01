//  $Id: antidbc02.C,v 1.38 2012/05/05 19:44:01 choumilo Exp $
// Author E.Choumilov 2.06.97
//    18.03.03 changed to be compatible with AMS02 design.
//
#include "typedefs.h"
#include "commonsi.h"
#include "job.h"
#include "amsdbc.h"
#include <stdio.h>
#include <time.h>
#include "tofdbc02.h"
#include "antidbc02.h"
#include "antirec02.h"
#include "anticalib02.h"
//
ANTI2SPcal ANTI2SPcal::antispcal[ANTI2C::MAXANTI];//mem.resrv. for ANTI-ReadoutPaddles stab.calib.objects
ANTI2VPcal ANTI2VPcal::antivpcal[ANTI2C::MAXANTI];//mem.resrv. for ANTI-ReadoutPaddles variab.calib.objects
ANTIPeds ANTIPeds::anscped[ANTI2C::MAXANTI];//mem.reserv. for ANTI-ReadoutPaddles pedestals/sigmas
ANTIPedsMS ANTIPedsMS::anscped[ANTI2C::MAXANTI];//mem.reserv. for ANTI-ReadoutPaddles MC-seeds pedestals/sigmas
//
//======> just memory reservation for ANTI2DBc class variables:
// (real values are initialized at run-time from data cards in setgeom() or in...)
// 
  geant ANTI2DBc::_scradi=0.;
  geant ANTI2DBc::_scinth=0.;
  geant ANTI2DBc::_scleng=0.;
  geant ANTI2DBc::_wrapth=0.;
  geant ANTI2DBc::_groovr=0.;
  geant ANTI2DBc::_pdlgap=0.;
  geant ANTI2DBc::_stradi=0.;
  geant ANTI2DBc::_stleng=0.;
  geant ANTI2DBc::_stthic=0.;
//
  geant ANTI2DBc::_fadcbw=0.5; // Flash-ADC bin width(ns)
  geant ANTI2DBc::_htdcbw=0.0244;// bin width in Hist/FT-TDC (ns)
  integer ANTI2DBc::_daqscf=1; //daq scaling factor for charge(not used in new raw mode readout)
  geant ANTI2DBc::_ftcoinw=50.;//not used
  geant ANTI2DBc::_dopwmn=7.; // min.outPW of discr.(outPW=inpTovT-dipwmn when outPW>dipwmn)
  geant ANTI2DBc::_dipwmn=5.; // minimal inp.pulse width(TovT) to fire discr.(its "rise time")
  geant ANTI2DBc::_tgpwid=120.;// "Trig.patt" pulses width(Actel,ns) used for coinc.with FT(confirmed 30.04.2005)
  geant ANTI2DBc::_tgdtim=5.;  //dead time of "TrigPatt"-branch on ACTEL-output(going to SPT-inpts)
  geant ANTI2DBc::_didtim=11.;//input dead time of generic discr(min dist. of prev_down/next_up edges)[ns]
//                                     (i neglect by fall-time of generic discr, so DT=(11+5)ns for output signals !!!)
  geant ANTI2DBc::_tdcdtm=20.;//dead time of TDC-inputs, the same for LT-/FT-inputs[ns]
  geant ANTI2DBc::_daccur=0.02;//generic discr. internal accuracy[ns]
  geant ANTI2DBc::_pbgate=240.;//JLV1-output glFT-gate width to latch "pattern" pulses
  geant ANTI2DBc::_mev2pe=18.;//Average(over pads/sides) mev->pe conv.factor[pe/mev]
//    This is side_Npes/0.5mip(most prob, mev) at center(now 1.33/2 mev/side/pad -> 12pe)
//    One can tune this value to adjust Npe, but pe2adc values in calib.file should be coherently adjusted
//    to keep the same position of signal(in ADC-channels).
//
  geant ANTI2DBc::_perrms=0.5;//PMT relative RMS of 1pe signal
  geant ANTI2DBc::_pc2adc=1.;// ADC-chip pC->adc_ch convertion factor [adc/pC].
  geant ANTI2DBc::_t2qgainr=12.;//average empirical factor to keep amplitude(mV) in T-channel at ~160mv
//                                                                                          (norm.inc mip) 
//
//  ANTI2DBc class member functions :
//
  geant ANTI2DBc::scradi(){return _scradi;}
  geant ANTI2DBc::scinth(){return _scinth;}
  geant ANTI2DBc::scleng(){return _scleng;}
  geant ANTI2DBc::wrapth(){return _wrapth;}
  geant ANTI2DBc::groovr(){return _groovr;}
  geant ANTI2DBc::pdlgap(){return _pdlgap;}
  geant ANTI2DBc::stradi(){return _stradi;};
  geant ANTI2DBc::stleng(){return _stleng;};
  geant ANTI2DBc::stthic(){return _stthic;}
  geant ANTI2DBc::fadcbw(){return _fadcbw;}
  geant ANTI2DBc::htdcbw(){return _htdcbw;}
  integer ANTI2DBc::daqscf(){return _daqscf;}
  geant ANTI2DBc::ftcoinw(){return _ftcoinw;}
  geant ANTI2DBc::dopwmn(){return _dopwmn;}
  geant ANTI2DBc::dipwmn(){return _dipwmn;}
  geant ANTI2DBc::tgpwid(){return _tgpwid;}
  geant ANTI2DBc::tgdtim(){return _tgdtim;}
  geant ANTI2DBc::didtim(){return _didtim;}
  geant ANTI2DBc::tdcdtm(){return _tdcdtm;}
  geant ANTI2DBc::daccur(){return _daccur;}
  geant ANTI2DBc::pbgate(){return _pbgate;}
  geant ANTI2DBc::mev2pe(){return _mev2pe;}
  geant ANTI2DBc::perrms(){return _perrms;}
  geant ANTI2DBc::pc2adc(){return _pc2adc;}
  geant ANTI2DBc::t2qgainr(){return _t2qgainr;}
//----
  void ANTI2DBc::setgeom(){ //get parameters from data cards (for now)
    geant ZShift(0);
    _scradi=ATGEFFKEY.scradi;
    _scinth=ATGEFFKEY.scinth;
    _scleng=ATGEFFKEY.scleng;
    _wrapth=ATGEFFKEY.wrapth;
    _groovr=ATGEFFKEY.groovr;
    _pdlgap=ATGEFFKEY.pdlgap;
    _stradi=ATGEFFKEY.stradi;
    _stleng=ATGEFFKEY.stleng;
    _stthic=ATGEFFKEY.stthic;
//
    if(strstr(AMSJob::gethead()->getsetup(),"AMS02D")){
          cout <<"<----- ANTIGeom-I-AMS02D setup selected."<<endl;
	  ZShift=AMSDBc::amsdext;
	  cout<<"      ZShift="<<ZShift<<endl;
    }
    else if(strstr(AMSJob::gethead()->getsetup(),"AMS02")){
      if(strstr(AMSJob::gethead()->getsetup(),"AMS02Ass1")){
          cout <<"<----- ANTIGeom-I-AMS02Ass1 setup selected."<<endl;
      }
      else if(strstr(AMSJob::gethead()->getsetup(),"AMS02P")){
          cout <<"<----- ANTIGeom-I-AMS02P(perm.magnet) setup selected."<<endl;
      }
    }
    else
    {
          cout <<"<----- ANTI2DBc::setgeom: UNKNOWN setup !!!!"<<endl;
    }
    _scleng+=2*ZShift;
    _stleng+=2*ZShift;
  }
//----
// function below creates PMT pulse shape array arr[] with binning flash-adc :
//  peak value = 1.(e.g. 1mV@50ohm, or 1pe if everything is in pe's)
geant ANTI2DBc::inipulsh(integer & nbn, geant arr[])
{
//  integer tbins=20;
//  geant pmpsh[20]={0.,.08,.37,.74,.99, 1., .94,.84, .54, .34, .25,
//       .16,.14,.12,.08, .1, .12,.08, .04,  0.};// pulse heights at time points
//       .16,.14,.12,.09, .08, .06,.05, .03,  0.};// pulse heights at time points, changed by hends to remove afterpulse
//
//  geant pmpsb[20]={0.,2.5, 5., 7.5,10.,12.5,15.,17.5,22.5,27.5,32.5,
//        40.,45.,50.,55.,60.,67.5,77.5,85.,100.};//time points
// below i took SE pulse shape from TOF(because the same PMT); what is above is ANTI real(multi-pes) pulse shape
  integer tbins=12;
  geant pmpsh[12]={0.,0.132,0.85,1.,0.83,0.68,0.35,0.15,0.078,
       0.059,0.029,0.};// pulse heights at time points
  geant pmpsb[12]={0.,1.,2.25,3.5,4.75,6.,8.5,11.,13.5,16.,18.5,
                      21.}; // time points (min. step should be above fladctb)
//
  integer i,io,ib,lastiob(0);
  geant bl,bh,bol,boh,ao1,ao2,tgo,al,ah,tota;
  tota=0.;
  io=0;
  for(ib=0;ib<ANTI2C::ANFADC;ib++){
    bl=ib*ANTI2DBc::fadcbw();
    bh=bl+ANTI2DBc::fadcbw();
    bol=pmpsb[io];
    boh=pmpsb[io+1];
    arr[ib]=0.;
    if(bl>=bol && bh<=boh){
      ao1=pmpsh[io];
      ao2=pmpsh[io+1];
      tgo=(ao2-ao1)/(boh-bol);
      al=ao1+tgo*(bl-bol);
      ah=ao1+tgo*(bh-bol);
      arr[ib]=(al+ah)/2.;
      tota+=arr[ib];
    }
    else if(bl<boh && bh>boh){
      ao1=pmpsh[io];
      ao2=pmpsh[io+1];
      tgo=(ao2-ao1)/(boh-bol);
      al=ao1+tgo*(bl-bol);
      io+=1;//next io-bin
      if((io+1)<tbins){
        bol=pmpsb[io];
        boh=pmpsb[io+1];
        ao1=pmpsh[io];
        ao2=pmpsh[io+1];
        tgo=(ao2-ao1)/(boh-bol);
        ah=ao1+tgo*(bh-bol);
        arr[ib]=(al+ah)/2.;
        tota+=arr[ib];
      }
      else{                 // last io-bin
        lastiob=1;
        ah=0.;
        arr[ib]=(al+ah)/2.;
        tota+=arr[ib];
	nbn=ib+1;
      }
    }
    else{//bl>=boh
      io+=1;//next io-bin
      if((io+1)<tbins)ib-=1;//to continue loop with the same ib
      else{                // last io-bin
        lastiob=1;
	nbn=ib;
      }
    }
    if(lastiob){//<-- last iobin test
      tota=tota*ANTI2DBc::fadcbw()/50.;
//	tota*=51;//set peak value as on Aahen-picture(51mV)just to compare integral
      if(ATMCFFKEY.mcprtf >1){
        printf("ANTI-PM pulse shape:nbins=% 3d, Integral(pC) = %5.3e\n",nbn,tota);
        for(i=1;i<=nbn;i++){
          if(i%10 != 0)
                       printf("% 4.2e",arr[i-1]);
          else
                       printf("% 4.2e\n",arr[i-1]);
        }
        if(nbn%10 !=0)printf("\n");
      }
      return tota;
    }//--->endof last iobin test
  }//--->endof ib loop
//
  cerr<<"ANTI2DBc-inipulsh-Fatal: PM-pulse length exeeds Flash-ADC range !?"<<'\n';
  cerr<<"                 nbn="<<nbn<<endl;
  exit(1);
}
//----
// function below creates PhElectrons dispersion function array arr[] with binning flash-adc :
//  Integral of interpolated distribution(arr[] sum) is 1
void ANTI2DBc::inipedisp(integer & nbn, geant arr[])
{
  integer tbins=24;
  geant pmpsh[24]={0., 224., 691., 880., 856., 692., 516., 370., 262., 183.,
                 125.,  90.,  70.,  61.,  50.,  38.,  23.,  14.,  7.3,  6.1,
		  5.5,  2.5,   1.,   0.
                                       };//diff.prob at given time point(not normalized)
  geant pmpsb[24]={0., .8, 1.6, 2.4, 3.2, 4., 4.8, 5.6, 6.4, 7.2,
                   8., 8.8, 9.6, 10.8, 12., 13.2, 14.4, 16., 17.6, 20.,
		   22.4, 24.8, 27.2, 29.
                                        };//time points
//  (min. step should be >= fadcbw)
  integer i,io,ib,lastiob(0);
  geant bl,bh,bol,boh,ao1,ao2,tgo,al,ah,tota;
  tota=0.;
  io=0;
  for(ib=0;ib<ANTI2C::ANFADC;ib++){
    bl=ib*ANTI2DBc::fadcbw();
    bh=bl+ANTI2DBc::fadcbw();
    bol=pmpsb[io];
    boh=pmpsb[io+1];
    arr[ib]=0.;
    if(bl>=bol && bh<=boh){
      ao1=pmpsh[io];
      ao2=pmpsh[io+1];
      tgo=(ao2-ao1)/(boh-bol);
      al=ao1+tgo*(bl-bol);
      ah=ao1+tgo*(bh-bol);
      arr[ib]=(al+ah)/2.;
      tota+=arr[ib];
    }
    else if(bl<boh && bh>boh){
      ao1=pmpsh[io];
      ao2=pmpsh[io+1];
      tgo=(ao2-ao1)/(boh-bol);
      al=ao1+tgo*(bl-bol);
      io+=1;//next io-bin
      if((io+1)<tbins){
        bol=pmpsb[io];
        boh=pmpsb[io+1];
        ao1=pmpsh[io];
        ao2=pmpsh[io+1];
        tgo=(ao2-ao1)/(boh-bol);
        ah=ao1+tgo*(bh-bol);
        arr[ib]=(al+ah)/2.;
        tota+=arr[ib];
      }
      else{                 // last io-bin
        lastiob=1;
        ah=0.;
        arr[ib]=(al+ah)/2.;
        tota+=arr[ib];
	nbn=ib+1;
      }
    }
    else{//bl>=boh
      io+=1;//next io-bin
      if((io+1)<tbins)ib-=1;//to continue loop with the same ib
      else{                // last io-bin
        lastiob=1;
	nbn=ib;
      }
    }
    if(lastiob){//<-- last iobin test
      for(i=0;i<nbn;i++)arr[i]/=tota;//normalization
      tota/=tota;
      if(ATMCFFKEY.mcprtf > 1){
        printf("ANTI:PEs time dispersion function:nbins=% 3d, Integral = %4.2e\n",nbn,tota);
        for(i=1;i<=nbn;i++){
          if(i%10 != 0)
                       printf("% 4.2e",arr[i-1]);
          else
                       printf("% 4.2e\n",arr[i-1]);
        }
        if(nbn%10 !=0)printf("\n");
      }
      return;
    }//--->endof last iobin test
  }//--->endof ib loop
//
  cerr<<"ANTI2DBc-inipedisp-Fatal: PE-disp distr.length exeeds Flash-ADC range !?"<<'\n';
  cerr<<"                 nbn="<<nbn<<endl;
  exit(1);
}
//----
// function to display PMT pulse (flash-ADC array arr[]) :
//
void ANTI2DBc::displ_a(char comm[], int id, int mf, const geant arr[]){
  integer i;
  geant tm,a(0.);
  geant tb,tbi;
  char name[80], buf[10];
  sprintf(buf, "%4d\n",id);
  strcpy(name,comm);
  strcat(name,buf);
  tb=geant(mf)*ANTI2DBc::fadcbw();
  tbi=ANTI2DBc::fadcbw();
  HBOOK1(2638,name,100,0.,100*tb,0.);
  for(i=1;i<=ANTI2C::ANFADC;i++){
    if(i%mf==0){
      a+=arr[i-1];
      tm=i*tbi-0.5*tb;
      HF1(2638,tm,a/geant(mf));
      a=0.;
    }
    else{
      a+=arr[i-1];
    }
  }
  HPRINT(2638);
  HDELET(2638);
  return ;
}
//======================================================================
// ANTI2SPcal class member functions:
//
void ANTI2SPcal::build(){ // fill array of objects with data
  integer i,j,endflab;
  geant hthr,hdthr[ANTI2C::MAXANTI]; // hist-discr threshold(p.e.)
  geant dqthr,daqthr[ANTI2C::MAXANTI];//daq-readout thresh(adc-ch)
  geant ftdl[2],ftdel[ANTI2C::MAXANTI][2];
  geant t0[2],tzer[ANTI2C::MAXANTI][2],m2p[4],m2pe[ANTI2C::MAXANTI][4];
  char fname[1024];
  char name[256];
  int ctyp,ntypes;
//
  char datt[3];
  char ext[80];
  int date[2],year,mon,day,hour,min,sec;
  uinteger iutct;
  tm begin;
  time_t utct;
  uinteger verids[10],verid;
//
// ---> read cal.files-versions file :
//
  strcpy(name,"AccCflist");// basic name for vers.list-file  
  if(AMSJob::gethead()->isMCData()){
    strcpy(datt,"MC");
    sprintf(ext,"%d",ATMCFFKEY.calvern);//MC-versn
  }
  else{
    strcpy(datt,"RD");
    sprintf(ext,"%d",ATREFFKEY.calutc);//RD-utc
  }
  strcat(name,datt);
  strcat(name,".");
  strcat(name,ext);
//
  if(ATCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
  if(ATCAFFKEY.cafdir==1)strcpy(fname,"");
  strcat(fname,name);
  cout<<"====> ANTI2SPcal::build: Opening Calib_vers_list-file  "<<fname<<'\n';
  ifstream vlfile(fname,ios::in); // open needed verslist-file for reading
  if(!vlfile){
    cout <<"<---- Error: missing vers_list-file "<<fname<<endl;
    exit(1);
  }
  vlfile >> ntypes;// total number of calibr. file types in the list
  for(i=0;i<ntypes;i++){
    vlfile >> verids[i];// 
  }
  if(AMSJob::gethead()->isMCData()){
    vlfile >> date[0];//YYYYMMDD beg.validity of AccCflistMC.ext file
    vlfile >> date[1];//HHMMSS ......................................
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
    cout<<"      AccCflistMC-file begin_date: year:month:day = "<<year<<":"<<mon<<":"<<day<<endl;
    cout<<"                                     hour:min:sec = "<<hour<<":"<<min<<":"<<sec<<endl;
    cout<<"                                         UTC-time = "<<iutct<<endl;
  }
  else{
    utct=time_t(ATREFFKEY.calutc);
    printf("      AccCflistRD-file begin_date: %s",ctime(&utct)); 
  }
  vlfile.close();
//
//---------------------------------------------
//
//   --->  Read stable param(adc2pe/daqthr/hdthr/tzer/ftdel) calib. file:
//
 ctyp=1;
 verid=verids[ctyp-1];//MC-versn or RD-utc
 strcpy(name,"AccStpar");//generic name
 strcat(name,datt);
 strcat(name,".");
 sprintf(ext,"%d",verid);
 strcat(name,ext);
 if(ATCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
 if(ATCAFFKEY.cafdir==1)strcpy(fname,"");
 strcat(fname,name);
 cout<<"      Opening AccStableParams-file : "<<fname<<'\n';
 ifstream asfile(fname,ios::in); // open file for reading
 if(!asfile){
   cout <<"<---- Error: missing AccStableParams-file: "<<fname<<endl;
   exit(1);
 }
//
 for(i=0;i<ANTI2C::MAXANTI;i++){
   for(j=0;j<4;j++)asfile >> m2pe[i][j];// read mev->pe correction factors for ReadoutPaddle(C1S1,C2S1,C1S2,C2S2)
   asfile >> daqthr[i];// read DAQ-thresh for ReadoutPaddle(adc-ch)(side independ)
   asfile >> hdthr[i];// read history-discr thresh for ReadoutPaddle(p.e)(side independ)
   asfile >> tzer[i][0];// read PhysPaddle-1 time delay(ns, mainly due to clear fiber length)
   asfile >> tzer[i][1];// read PhysPaddle-2 time delay(ns, mainly due to clear fiber length)
   asfile >> ftdel[i][0];// FT-delay for ReadoutPaddle side-1 
   asfile >> ftdel[i][1];// FT-delay for ReadoutPaddle side-2
 }
//
 asfile >> endflab;//read endfile-label
//
 asfile.close();
//
 if(endflab==12345){
   cout<<"      AccStableParams file is successfully read !"<<endl;
 }
 else{cout<<"<---- ERROR: problems with AccStableParams file"<<endl;
   exit(1);
 }
//---------------------------------------------
// create ANTI2SPcal objects:
//
    for(i=0;i<ANTI2C::MAXANTI;i++){
      for(j=0;j<4;j++)m2p[j]=m2pe[i][j];//mev->pe conv.factor corrections
      dqthr=daqthr[i];//DAQ-readout-threshold (adc-ch)
      hthr=hdthr[i]; // History-discr threshold (pe)
      t0[0]=tzer[i][0];//ClfFiber-delay for PhysPaddle-1
      t0[1]=tzer[i][1];//ClfFiber-delay for PhysPaddle-2
      ftdl[0]=ftdel[i][0];//S1 true hist-hit delay wrt FT (not used now)
      ftdl[1]=ftdel[i][1];//S2
//
      antispcal[i]=ANTI2SPcal(i,m2p,dqthr,hthr,t0,ftdl);// create ANTI2SPcal object
    }
//
  cout<<"<---- ANTI2SPcal::build: succsessfully done !"<<endl<<endl;
}
//==================================================================
//
// ANTI2VPcal class member functions:
//
void ANTI2VPcal::build(){ // fill array of objects with data
  integer i,endflab;
  integer status[ANTI2C::MAXANTI][2][2],sta[2][2];
  geant p2a[2][2],pe2adc[ANTI2C::MAXANTI][2][2];//[physs][side]
  geant attlen[ANTI2C::MAXANTI][2][2],atl[2][2];
  geant powp[ANTI2C::MAXANTI][2][2],pow[2][2];
  geant snorp[ANTI2C::MAXANTI][2][2],snor[2][2];
  char fname[1024];
  char name[256];
  int ctyp,ntypes;
  int phs,sd;
//
  char datt[3];
  char ext[80];
  int date[2],year,mon,day,hour,min,sec;
  uinteger iutct;
  tm begin;
  time_t utct;
  uinteger verids[10],verid;
//
// ---> read cal.file-versions file :
//
  strcpy(name,"AccCflist");// basic name for vers.list-file  
  if(AMSJob::gethead()->isMCData()){
    strcpy(datt,"MC");
    sprintf(ext,"%d",ATMCFFKEY.calvern);//MC-versn
  }
  else{
    strcpy(datt,"RD");
    sprintf(ext,"%d",ATREFFKEY.calutc);//RD-utc
  }
  strcat(name,datt);
  strcat(name,".");
  strcat(name,ext);
//
  if(ATCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
  if(ATCAFFKEY.cafdir==1)strcpy(fname,"");
  strcat(fname,name);
  cout<<"====> ANTI2VPcal::build: Opening Calib_vers_list-file  "<<fname<<'\n';
  ifstream vlfile(fname,ios::in); // open needed verslist-file for reading
  if(!vlfile){
    cout <<"<---- Error: missing vers_list-file "<<fname<<endl;
    exit(1);
  }
  vlfile >> ntypes;// total number of calibr. file types in the list
  for(i=0;i<ntypes;i++){
    vlfile >> verids[i];// 
  }
  if(AMSJob::gethead()->isMCData()){
    vlfile >> date[0];//YYYYMMDD beg.validity of AccCflistMC.ext file
    vlfile >> date[1];//HHMMSS ......................................
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
    cout<<"      AccCflistMC-file begin_date: year:month:day = "<<year<<":"<<mon<<":"<<day<<endl;
    cout<<"                                     hour:min:sec = "<<hour<<":"<<min<<":"<<sec<<endl;
    cout<<"                                         UTC-time = "<<iutct<<endl;
  }
  else{
    utct=time_t(ATREFFKEY.calutc);
    printf("      AccCflistRD-file begin_date: %s",ctime(&utct)); 
  }
  vlfile.close();
//
//---------------------------------------------
//
//
 ctyp=2;
 verid=verids[ctyp-1];//MC-versn or RD-utc
 strcpy(name,"AccVrpar");//generic name
 strcat(name,datt);
 strcat(name,".");
 sprintf(ext,"%d",verid);
 strcat(name,ext);
 if(ATCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
 if(ATCAFFKEY.cafdir==1)strcpy(fname,"");
 strcat(fname,name);
 cout<<"      Opening AccVariableParams-file : "<<fname<<'\n';
 ifstream avfile(fname,ios::in); // open file for reading
 if(!avfile){
   cout <<"<---- Error: missing AccVariableParams-file: "<<fname<<endl;
   exit(1);
 }
//
 for(i=0;i<ANTI2C::MAXANTI;i++){
  for(sd=0;sd<2;sd++){
   for(phs=0;phs<2;phs++){
    avfile >> status[i][phs][sd]; 
    avfile >> pe2adc[i][phs][sd];
   }
  }
  for(sd=0;sd<2;sd++){
   for(phs=0;phs<2;phs++){
    avfile >> attlen[i][phs][sd];
   }
  } 
  for(sd=0;sd<2;sd++){
   for(phs=0;phs<2;phs++){
    avfile >> powp[i][phs][sd];
   }
  } 
  for(sd=0;sd<2;sd++){
   for(phs=0;phs<2;phs++){
    avfile >> snorp[i][phs][sd];
   }
  } 
 }
 avfile >> endflab;//read endfile-label
//
 avfile.close();
//
 if(endflab==12345){
   cout<<"      AccVariableParams file is successfully read !"<<endl;
 }
 else{cout<<"<---- ERROR: problems with AccVariableParams file"<<endl;
   exit(1);
 }
//
//----------------------------------------------------------------------
// create ANTI2VPcal objects:
//
    for(i=0;i<ANTI2C::MAXANTI;i++){
     for(phs=0;phs<2;phs++){
      for(sd=0;sd<2;sd++){
       sta[phs][sd]=status[i][phs][sd]; // alive status from CalibOutput-file
       p2a[phs][sd]=pe2adc[i][phs][sd];//pe->adc conv.factor(incl.clfib.atten.,PM-gain)
       atl[phs][sd]=attlen[i][phs][sd];//att.length
       pow[phs][sd]=powp[i][phs][sd];//power param
       snor[phs][sd]=snorp[i][phs][sd];//sidenorm param
      }
     }
//
      antivpcal[i]=ANTI2VPcal(i,sta,p2a,atl,pow,snor);// create ANTI2VPcal object
    }
//
  cout<<"<---- ANTI2VPcal::build: succsessfully done !"<<endl<<endl;
}
//
  geant ANTI2VPcal::getatlenc(int is){
    if(status[0][is]==0 && status[1][is]==0)return 0.5*(attlen[0][is]+attlen[1][is]);
    else if(status[0][is]==0)return attlen[0][is];
    else if(status[1][is]==0)return attlen[1][is];
    else return(1000);//def -> low attenuation
  }
//
  int ANTI2VPcal::NPhysSecOK(){//return 2 if BOTH phys.sectors OK, alive phys.sect#(0,1) if not
    if((status[0][0]==0 || status[0][1]==0) && (status[1][0]==0 || status[1][1]==0))return(2);
    else if(status[0][0]==0 || status[0][1]==0)return(0);
    else if(status[1][0]==0 || status[1][1]==0)return(1);
    else return(-1);
  }
//
  geant ANTI2VPcal::SignalAtt(integer phs, integer is, geant z){
    geant att;
    geant hlen=ATGEFFKEY.scleng/2;
    geant zlim=25;//lim on fabs(z) where responce is pure exponential(neglect by edge effect)
    att=exp(z/attlen[phs][is]);
    if(fabs(z)>zlim)att-=snorp[phs][is]*pow((fabs(z)-zlim)/(hlen-zlim),powp[phs][is]);//add edge corr
    return att;
  }
//
  geant ANTI2VPcal::SignalAttc(integer is, geant z){
    geant hlen=ATGEFFKEY.scleng/2;
    geant zlim=25;//lim on fabs(z) where responce is pure exponential(neglect by edge effect)
    geant att[2];
    for(int phs=0;phs<2;phs++){
      att[phs]=exp(z/attlen[phs][is]);
      if(fabs(z)>zlim)att[phs]-=snorp[phs][is]*pow((fabs(z)-zlim)/(hlen-zlim),powp[phs][is]);//add edge corr
    }
    if(status[0][is]==0 && status[1][is]==0)return 0.5*(att[0]+att[1]);
    else if(status[0][is]==0)return att[0];
    else if(status[1][is]==0)return att[1];
    else return 999;//means no att.(def)
  }
//
//
//==========================================================================
//
void ANTIPeds::mcbuild(){// create MC ANTI-Peds-objects for each ReadoutPaddle
//
  int i,is;
  integer sid;
  char fname[1024];
  char name[256];
  integer asta[ANTI2C::MAXANTI][2];// array of counter stat
  geant aped[ANTI2C::MAXANTI][2];// array of counter peds
  geant asig[ANTI2C::MAXANTI][2];// array of counter sigmas
  integer stata[2];
  geant peda[2],siga[2];
//
//
//   --->  Read  pedestals file :
//
  strcpy(name,"antp_df_mc");
  cout <<"====> ANTIPeds::mcbuild: Default Real_MC-peds file is used..."<<endl;
// ---> check setup:
//
  if (strstr(AMSJob::gethead()->getsetup(),"AMS02")){
    cout<<"      AMS02 setup selected."<<endl;
  }
  else
  {
        cout <<"      Unknown setup !!!"<<endl;
        exit(10);
  }
//
  strcat(name,".dat");
  if(ATCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
  if(ATCAFFKEY.cafdir==1)strcpy(fname,"");
  strcat(fname,name);
  cout<<"      Open file : "<<fname<<'\n';
  ifstream icfile(fname,ios::in); // open pedestals-file for reading
  if(!icfile){
    cerr <<"      missing Default Real_MC-peds file !!!"<<fname<<endl;
    exit(1);
  }
//
//---> Read anode stats/peds/sigs:
//
  for(is=0;is<ANTI2C::MAXANTI;is++){  // <---- loop over sectors
    for(i=0;i<2;i++)icfile >> asta[is][i];// sequence: side1,side2
    for(i=0;i<2;i++)icfile >> aped[is][i];
    for(i=0;i<2;i++)icfile >> asig[is][i];
  } // ---> endof sectors loop
//
//
  icfile.close();
//---------------------------------------------
//   ===> fill ANTIPeds bank :
//
  for(is=0;is<ANTI2C::MAXANTI;is++){  // <--- loop over sectors
    sid=(is+1);
    for(i=0;i<2;i++){
      stata[i]=asta[is][i];
      peda[i]=aped[is][i];
      siga[i]=asig[is][i];
    }
//
    anscped[is]=ANTIPeds(sid,stata,peda,siga);
//
  } // ---> endof sector loop
  cout <<"<---- ANTIPeds::mcbuild: succsessfully done !"<<endl<<endl;
}
//==========================================================================
//
void ANTIPeds::build(){//tempor solution for RealData peds. 
//
  int i,is;
  integer sid;
  char fname[1024];
  char name[256];
  integer asta[ANTI2C::MAXANTI][2];// array of counter stat
  geant aped[ANTI2C::MAXANTI][2];// array of counter peds
  geant asig[ANTI2C::MAXANTI][2];// array of counter sigmas
  integer stata[2];
  geant peda[2],siga[2];
//
//
//   --->  Read  pedestals file :
//
  strcpy(name,"antp_df_rl");
  cout <<"====> ANTIPeds::build: Default Real_Data-Peds file is used..."<<endl;
// ---> check setup:
//
  if (strstr(AMSJob::gethead()->getsetup(),"AMS02")){
    cout<<"      AMS02 setup selected."<<endl;
  }
  else
  {
        cout <<"      Unknown setup !!!"<<endl;
        exit(10);
  }
//
  strcat(name,".dat");
  strcpy(fname,"");
#ifndef __TFADBW__
  if(ATCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);//use official-dir only when not in pedDBWR-mode
#endif
  strcat(fname,name);
  cout<<"      Open file : "<<fname<<'\n';
  ifstream icfile(fname,ios::in); // open pedestals-file for reading
  if(!icfile){
    cerr <<"      missing Default Real_Data-Peds file !!! "<<fname<<endl;
    exit(1);
  }
//
//---> Read anode stats/peds/sigs:
//
  for(is=0;is<ANTI2C::MAXANTI;is++){  // <---- loop over sectors
    for(i=0;i<2;i++)icfile >> asta[is][i];// sequence: side1,side2
    for(i=0;i<2;i++)icfile >> aped[is][i];
    for(i=0;i<2;i++)icfile >> asig[is][i];
  } // ---> endof sectors loop
//
//
  icfile.close();
//---------------------------------------------
//   ===> fill ANTIPeds bank :
//
  for(is=0;is<ANTI2C::MAXANTI;is++){  // <--- loop over sectors
    sid=(is+1);
    for(i=0;i<2;i++){
      stata[i]=asta[is][i];
      peda[i]=aped[is][i];
      siga[i]=asig[is][i];
    }
//
    anscped[is]=ANTIPeds(sid,stata,peda,siga);
//
  } // ---> endof sector loop
  cout <<"<---- ANTIPeds::build: succsessfully done !"<<endl<<endl;
}
//==========================================================================
//
void ANTIPedsMS::build(){//tempor solution for MC-Seed peds. 
//
  int i,is;
  integer sid;
  char fname[1024];
  char name[256];
  integer asta[ANTI2C::MAXANTI][2];// array of counter stat
  geant aped[ANTI2C::MAXANTI][2];// array of counter peds
  geant asig[ANTI2C::MAXANTI][2];// array of counter sigmas
  integer stata[2];
  geant peda[2],siga[2];
//
//
//   --->  Read  pedestals file :
//
  strcpy(name,"antp_df_sd");
  cout <<"====> ANTIPedsMS::build: Default Seed_MC-Peds file is used..."<<endl;
// ---> check setup:
//
  if (strstr(AMSJob::gethead()->getsetup(),"AMS02")){
    cout<<"      AMS02 setup selected."<<endl;
  }
  else
  {
        cout <<"      Unknown setup !!!"<<endl;
        exit(10);
  }
//
  strcat(name,".dat");
  if(ATCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
  if(ATCAFFKEY.cafdir==1)strcpy(fname,"");
  strcat(fname,name);
  cout<<"      Open file : "<<fname<<'\n';
  ifstream icfile(fname,ios::in); // open pedestals-file for reading
  if(!icfile){
    cerr <<"      missing Default Seed_MC-Peds file !!! "<<fname<<endl;
    exit(1);
  }
//
//---> Read anode stats/peds/sigs:
//
  for(is=0;is<ANTI2C::MAXANTI;is++){  // <---- loop over sectors
    for(i=0;i<2;i++)icfile >> asta[is][i];// sequence: side1,side2
    for(i=0;i<2;i++)icfile >> aped[is][i];
    for(i=0;i<2;i++)icfile >> asig[is][i];
  } // ---> endof sectors loop
//
//
  icfile.close();
//---------------------------------------------
//   ===> fill ANTIPeds bank :
//
  for(is=0;is<ANTI2C::MAXANTI;is++){  // <--- loop over sectors
    sid=(is+1);
    for(i=0;i<2;i++){
      stata[i]=asta[is][i];
      peda[i]=aped[is][i];
      siga[i]=asig[is][i];
    }
//
    anscped[is]=ANTIPedsMS(sid,stata,peda,siga);
//
  } // ---> endof sector loop
  cout <<"<---- ANTIPedsMS::build: succsessfully done !"<<endl<<endl;
}
//=====================================================================  
//
//   ANTIJobStat static variables definition (memory reservation):
//
integer ANTI2JobStat::mccount[ANTI2C::ANJSTA];
integer ANTI2JobStat::recount[ANTI2C::ANJSTA];
integer ANTI2JobStat::chcount[ANTI2C::ANCHMX][ANTI2C::ANCSTA];
integer ANTI2JobStat::brcount[ANTI2C::MAXANTI][ANTI2C::ANCSTA];
//---
  void ANTI2JobStat::clear(){
    int i,j;
    for(i=0;i<ANTI2C::ANJSTA;i++)mccount[i]=0;
    for(i=0;i<ANTI2C::ANJSTA;i++)recount[i]=0;
    for (i=0;i<ANTI2C::ANCHMX;i++)
                  for(j=0;j<ANTI2C::ANCSTA;j++)
                                       chcount[i][j]=0;
    for (i=0;i<ANTI2C::MAXANTI;i++)
                  for(j=0;j<ANTI2C::ANCSTA;j++)
                                       brcount[i][j]=0;
  }
//
  void ANTI2JobStat::addmc(int i){
    assert(i>=0 && i< ANTI2C::ANJSTA);
#pragma omp critical (ac_addmc) 
    mccount[i]+=1;
  }
//
  void ANTI2JobStat::addre(int i){
    assert(i>=0 && i< ANTI2C::ANJSTA);
#pragma omp critical (ac_addre) 
    recount[i]+=1;
  }
//
  void ANTI2JobStat::addch(int chnum, int i){
    assert(chnum>=0 && i>=0);
    assert(chnum < ANTI2C::ANCHMX && i < ANTI2C::ANCSTA);
#pragma omp critical (ac_addch) 
    chcount[chnum][i]+=1;
  }
//
  void ANTI2JobStat::addbr(int brnum, int i){
    assert(brnum>=0 && i>=0);
    assert(brnum < ANTI2C::MAXANTI && i < ANTI2C::ANCSTA);
#pragma omp critical (ac_addbr) 
    brcount[brnum][i]+=1;
  }
//---
//
// function to print Job-statistics at the end of JOB(RUN):
//
void ANTI2JobStat::printstat(){
  int ib,ic;
  geant rc;
  printf("\n");
//
  if(MISCFFKEY.dbwrbeg>0 || ATREFFKEY.reprtf[0]==0)return;//dbwriter job, don't need statistics
//
  printf("    ============ JOB ANTI2-statistics =============\n");
  printf("\n");
  printf(" MC: entries                           : % 6d\n",mccount[0]);
  printf(" MC: Have FastTrig to digitize ACC     : % 6d\n",mccount[7]);
  printf(" MC: Have MC-hits in ACC               : % 6d\n",mccount[9]);
  printf(" MC: RawEvent created(err.report below): % 6d\n",mccount[6]);
  printf(" MC:     Flash-ADC time-range ovfl     : % 6d\n",mccount[1]);
  printf(" MC:     TimeHist-TDC stack ovfl       : % 6d\n",mccount[2]);
  printf(" MC:     TrigTime-TDC stack ovfl       : % 6d\n",mccount[8]);
  printf(" MC:     ADC range ovfl                : % 6d\n",mccount[3]);
  printf(" MC:     Hist-hits out of search window: % 6d\n",mccount[4]);
  printf(" MC: FT/ACC coincidence                : % 6d\n",mccount[5]);
  printf("--------\n");
  printf(" RECO-entries                   : % 6d\n",recount[0]);
  printf(" GlobFT(FTC|FTZ|FTE|Ext) OK     : % 6d\n",recount[1]);
  printf(" TOF_FT(FTC|FTZ) found in LVL1  : % 6d\n",recount[5]);
  printf(" NoTOF_FT but ECAL_FT only      : % 6d\n",recount[6]);
  printf(" RawEvent-validation OK     : % 6d\n",recount[2]);
  printf(" RawEvent->Cluster OK       : % 6d\n",recount[3]);
  printf("--------\n");
  printf(" AmplCalib entries       : % 6d\n",recount[11]);
  printf(" Nsectors OK             : % 6d\n",recount[12]);
  printf(" Particle found          : % 6d\n",recount[19]);
  printf(" Any track particle      : % 6d\n",recount[20]);
  printf(" TrueTrack(trd|trk) part.: % 6d\n",recount[21]);
  printf(" TrkTrack particle       : % 6d\n",recount[22]);
  printf(" Good TrkTrack particle  : % 6d\n",recount[23]);
  printf(" TrdTrack particle       : % 6d\n",recount[24]);
  printf(" Good TrdTrack particle  : % 6d\n",recount[25]);
  printf(" GoogTrackPart selected  : % 6d\n",recount[26]);
  printf(" Finally TRD-track used  : % 6d\n",recount[27]);
  printf(" Finally TRK-track used  : % 6d\n",recount[28]);
  printf(" One Track-Sector cross  : % 6d\n",recount[14]);
  printf(" Track Z=1               : % 6d\n",recount[15]);
  printf(" Good matching/impact    : % 6d\n",recount[16]);
  printf(" P/m in MIP range        : % 6d\n",recount[17]);
  printf(" Cross.Length OK         : % 6d\n",recount[18]);
  printf("--------\n");
  printf(" PedCalib entries      : % 6d\n",recount[30]);
  printf(" PedCalib-events found : % 6d\n",recount[31]);
  printf("\n\n");
//
  printf("==========> Bars reconstruction report :\n\n");
//
  printf("Have signal on any side :\n\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      printf(" % 6d",brcount[ib][0]);
    }
    printf("\n\n");
//
  printf("Have tot.signal > threshold :\n\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      rc=geant(brcount[ib][0]);
      if(rc>0.)rc=geant(brcount[ib][1])/rc;
      printf("% 5.3f",rc);
    }
    printf("\n\n");
  printf("Good for calibration :\n\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      rc=geant(brcount[ib][0]);
      if(rc>0.)rc=geant(brcount[ib][2])/rc;
      printf("% 5.3f",rc);
    }
    printf("\n\n");
//
//
  printf("===========> Channels validation report :\n\n");
//
  printf("Validation entries :\n\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      ic=ib*2;
      printf(" % 6d",chcount[ic][5]);
    }
    printf("\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      ic=ib*2+1;
      printf(" % 6d",chcount[ic][5]);
    }
    printf("\n\n");
  printf("Missing FTtime info (percentage) :\n");
  printf("\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      ic=ib*2;
      rc=geant(chcount[ic][5]);
      if(rc>0.)rc=100.*geant(chcount[ic][6])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      ic=ib*2+1;
      rc=geant(chcount[ic][5]);
      if(rc>0.)rc=100.*geant(chcount[ic][6])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
//
//
  printf("Multiple FTtime info (percentage) :\n");
  printf("\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      ic=ib*2;
      rc=geant(chcount[ic][5]);
      if(rc>0.)rc=100.*geant(chcount[ic][7])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      ic=ib*2+1;
      rc=geant(chcount[ic][5]);
      if(rc>0.)rc=100.*geant(chcount[ic][7])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
//
  printf("Missing Time info (percentage) :\n");
  printf("\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      ic=ib*2;
      rc=geant(chcount[ic][5]);
      if(rc>0.)rc=100.*geant(chcount[ic][8])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      ic=ib*2+1;
      rc=geant(chcount[ic][5]);
      if(rc>0.)rc=100.*geant(chcount[ic][8])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
//
  printf("Missing Ampl info (percentage) :\n");
  printf("\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      ic=ib*2;
      rc=geant(chcount[ic][5]);
      if(rc>0.)rc=100.*geant(chcount[ic][9])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      ic=ib*2+1;
      rc=geant(chcount[ic][5]);
      if(rc>0.)rc=100.*geant(chcount[ic][9])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
//
  printf("============> Channels reconstruction report :\n\n");
//
  printf("DB-statusOK hit found :\n\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      ic=ib*2;
      printf(" % 6d",chcount[ic][0]);
    }
    printf("\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      ic=ib*2+1;
      printf(" % 6d",chcount[ic][0]);
    }
    printf("\n\n");
//
  printf("ADC-ch 'ON' (wrt total):\n");
  printf("\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      ic=ib*2;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][1])/rc;
      printf("% 5.3f",rc);
    }
    printf("\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      ic=ib*2+1;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][1])/rc;
      printf("% 5.3f",rc);
    }
    printf("\n\n");
//
  printf("ADC-ch 'AbovePed' (wrt total):\n");
  printf("\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      ic=ib*2;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][2])/rc;
      printf("% 5.3f",rc);
    }
    printf("\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      ic=ib*2+1;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][2])/rc;
      printf("% 5.3f",rc);
    }
    printf("\n\n");
//
  printf("TDC-ch 'ON' (wrt AbovePed):\n");
  printf("\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      ic=ib*2;
      rc=geant(chcount[ic][2]);
      if(rc>0.)rc=geant(chcount[ic][3])/rc;
      printf("% 5.3f",rc);
    }
    printf("\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      ic=ib*2+1;
      rc=geant(chcount[ic][2]);
      if(rc>0.)rc=geant(chcount[ic][3])/rc;
      printf("% 5.3f",rc);
    }
    printf("\n\n");
//
//
  printf("TDC-ch '1 hit' (wrt AbovePed):\n");
  printf("\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      ic=ib*2;
      rc=geant(chcount[ic][2]);
      if(rc>0.)rc=geant(chcount[ic][4])/rc;
      printf("% 5.3f",rc);
    }
    printf("\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      ic=ib*2+1;
      rc=geant(chcount[ic][2]);
      if(rc>0.)rc=geant(chcount[ic][4])/rc;
      printf("% 5.3f",rc);
    }
    printf("\n\n");
//
}
//======================================================================
//
void ANTI2JobStat::bookmch(){
//
  HBOOK1(2630,"ANTI-MC: Counters Etot (geant,Mev)",60,0.,30.,0.); 
  HBOOK1(2631,"ANTI-MC(lvl1): GlobFTtime-SideTrigPattSignalTime(ns, at JLV1)",80,-160.,160.,0.); 
  HBOOK1(2632,"ANTI-MC: DownSideEdep (pe,FillRawEvent)",80,0.,160.,0.); 
  HBOOK1(2633,"ANTI-MC: UpSideEdep (pe,FillRawEvent)",80,0.,160.,0.); 
  HBOOK1(2634,"ANTI-MC: DownSideEdep (ADCch-ped,FillRawEvent)",80,0.,80.,0.); 
  HBOOK1(2635,"ANTI-MC: UpSideEdep (ADCch-ped,FillRawEvent)",80,0.,80.,0.); 
  HBOOK1(2636,"ANTI-MC: TimeHistory hits per side ",16,0.,16.,0.); 
  HBOOK1(2637,"ANTI-MC: NumbOfGeant-hits per event ",80,0.,80.,0.);
  HBOOK1(2639,"ANTI-MC: LogSector GeantEloss(mev)",100,0.,20.,0.);
  HBOOK1(2640,"ANTI-MC: FTchannel PulseWidth",90,0.,450.,0.);
  HBOOK1(2641,"ANTI-MC: T-channel pulse heights spectrum[mV]",80,0.,1600.,0.);
  HBOOK1(2642,"ANTI-MC: T-channel pulse charge (pC)",80,0.,320.,0.);
  HBOOK1(2643,"ANTI-MC: Final FTtime-TrigPattSideSignalTime",80,-160.,160.,0.);
  
//  HBOOK1(2638,... reserved for PM-pulse  
}
void ANTI2JobStat::bookh(){
//
  if(ATREFFKEY.reprtf[0]>1){
    HBOOK1(2515,"ANTI-VAL:HitsNumber(adc/tdc(+10)/ft(+20), allchann)",30,0.,30.,0.);
    HBOOK1(2516,"ANTI-VAL:FTtime-HistTime(all chann)",80,0.,240.,0.);
//
    HBOOK1(2500,"ANTI-REC: EtotSectors(Mev,Esect>thr)",80,0.,20.,0.);
    HBOOK1(2501,"ANTI-REC: NumbOfSectors(Esect>thr)",16,0.,16.,0.);
    HBOOK1(2512,"ANTI-REC: NumbOfSectors(Esect>thr, + FT-coinc)",16,0.,16.,0.);
    HBOOK1(2502,"ANTI-REC: Total Time-hits per sector",16,0.,16.,0.);
    HBOOK1(2503,"ANTI-REC: MadeOfPair-TimeHits per sector",16,0.,16.,0.);
    HBOOK1(2504,"ANTI-REC: Sector Z-coo(cm,1Pair)",60,-60.,60.,0.);
    HBOOK1(2505,"ANTI-REC: Sector Z-coo(cm, 2sided but pairs!=1)",60,-60.,60.,0.);
    HBOOK1(2506,"ANTI-REC: Sector appearance frequency(Esect>Thr)",16,1.,17.,0.);
    HBOOK1(2507,"ANTI-REC: NumbOfPairedSectors(Esect>thr)",16,0.,16.,0.);
    HBOOK1(2508,"ANTI-REC: Edep per sector(mev, Eside*2 if one-sided)",80,0.,16.,0.);
    HBOOK1(2509,"ANTI-REC: FTtime-SectorSideTime(+T0compens)",80,0.,240.,0.);
    HBOOK1(2510,"ANTI-REC: Total FTCoincSectors(fromTrigPatt)",10,0.,10.,0.);
    HBOOK1(2511,"ANTI-REC: FTCoincSector frequency(0 when TrPatt=0)",10,0.,10.,0.);
    
    HBOOK1(2520,"ANTI-REC: SideTimesDiff(1-2), Sector-1",50,-50.,50.,0.);
    HBOOK1(2521,"ANTI-REC: SideTimesDiff(1-2), Sector-2",50,-50.,50.,0.);
    HBOOK1(2522,"ANTI-REC: SideTimesDiff(1-2), Sector-3",50,-50.,50.,0.);
    HBOOK1(2523,"ANTI-REC: SideTimesDiff(1-2), Sector-4",50,-50.,50.,0.);
    HBOOK1(2524,"ANTI-REC: SideTimesDiff(1-2), Sector-5",50,-50.,50.,0.);
    HBOOK1(2525,"ANTI-REC: SideTimesDiff(1-2), Sector-6",50,-50.,50.,0.);
    HBOOK1(2526,"ANTI-REC: SideTimesDiff(1-2), Sector-7",50,-50.,50.,0.);
    HBOOK1(2527,"ANTI-REC: SideTimesDiff(1-2), Sector-8",50,-50.,50.,0.);
  }
  if(ATREFFKEY.reprtf[0]>2){//low prior.hist
    HBOOK1(2570,"ANTI-VAL:ADC Sect-1, Side-1",80,0.,4000.,0.);
    HBOOK1(2571,"ANTI-VAL:ADC Sect-2, Side-1",80,0.,4000.,0.);
    HBOOK1(2572,"ANTI-VAL:ADC Sect-3, Side-1",80,0.,4000.,0.);
    HBOOK1(2573,"ANTI-VAL:ADC Sect-4, Side-1",80,0.,4000.,0.);
    HBOOK1(2574,"ANTI-VAL:ADC Sect-5, Side-1",80,0.,4000.,0.);
    HBOOK1(2575,"ANTI-VAL:ADC Sect-6, Side-1",80,0.,4000.,0.);
    HBOOK1(2576,"ANTI-VAL:ADC Sect-7, Side-1",80,0.,4000.,0.);
    HBOOK1(2577,"ANTI-VAL:ADC Sect-8, Side-1",80,0.,4000.,0.);
    HBOOK1(2578,"ANTI-VAL:ADC Sect-1, Side-2",80,0.,4000.,0.);
    HBOOK1(2579,"ANTI-VAL:ADC Sect-2, Side-2",80,0.,4000.,0.);
    HBOOK1(2580,"ANTI-VAL:ADC Sect-3, Side-2",80,0.,4000.,0.);
    HBOOK1(2581,"ANTI-VAL:ADC Sect-4, Side-2",80,0.,4000.,0.);
    HBOOK1(2582,"ANTI-VAL:ADC Sect-5, Side-2",80,0.,4000.,0.);
    HBOOK1(2583,"ANTI-VAL:ADC Sect-6, Side-2",80,0.,4000.,0.);
    HBOOK1(2584,"ANTI-VAL:ADC Sect-7, Side-2",80,0.,4000.,0.);
    HBOOK1(2585,"ANTI-VAL:ADC Sect-8, Side-2",80,0.,4000.,0.);
    
    HBOOK1(2586,"ANTI-VAL:Found(ADC+Time) for Sect+(Side-1)*8",16,1.,17.,0.);
    HBOOK1(2587,"ANTI-VAL:Found(Time) for Sect+(Side-1)*8",16,1.,17.,0.);
  }
  if(ATREFFKEY.relogic==1  && ATREFFKEY.reprtf[0]>1){//book calib.hist
    HBOOK1(2530,"AntiCalib:Nfired/Nmatched sectors",20,0.,20.,0.);//spare
    HBOOK1(2531,"AntiCalib:Cyl-track Zcross(noCuts,both dirs)",75,-75.,75.,0.);
    HBOOK1(2532,"AntiCalib:PHIsect-PHIcros(Zcross OK)",91,-182.,182.,0.);
    HBOOK1(2533,"AntiCalib:PHIcros-PHIimp(Zcross OK)",91,-182.,182.,0.);
    HBOOK1(2534,"AntiCalib:PartMomentum(gev)",100,0.,50.,0.);
    HBOOK1(2535,"AntiCalib:PartBeta",50,-1.,1.,0.);
    HBOOK1(2536,"AntiCalib:PartMass(gev)",80,0.,8.,0.);
    HBOOK1(2537,"AntiCalib:Part mom/mass",80,0.,20.,0.);
    HBOOK1(2538,"AntiCalib:PartPassLength in scint",80,0.,20.,0.);
//    HBOOK2(2625,"AntiCalib:AmplS1([adc],trlen/beta-norm) vs bet",60,0.6,1.2,50,10.,1000.,0.);
    HBOOK1(2539,"AntiCalib:Matched Physical sector number",16,1.,17.,0.);
    HBOOK1(2540,"AntiCalib:TRK-track CrossPointPhi(Zcr OK)",91,0.,364.,0.);
    HBOOK1(2541,"AntiCalib:TRK-track ImpactPhi(Zcr OK)",91,0.,364.,0.);
    
    HBOOK1(2542,"AntiCalib:PhysSectorAttLength(side1)",100,0.,1000.,0.);
    HBOOK1(2543,"AntiCalib:PhysSectorAttLength(side2)",100,0.,1000.,0.);
    HBOOK1(2544,"AntiCalib:PhysSectPE2ADC(s1/2,[adc/pe])",80,0.,160.,0.);
    HBOOK1(2593,"AntiCalib:Fit Chi2/dof",80,0.,40.,0.);
    HBOOK1(2594,"AntiCalib:RawAmpl,LinCorrected(S1 sect4)",100,0.,5000.,0.);
    HBOOK1(2595,"AntiCalib:RawAmpl,LinCorrected(S2 sect4)",100,0.,5000.,0.);
    HBOOK1(2596,"AntiCalib:RawAmpl(S1 sect4)",100,0.,5000.,0.);
    HBOOK1(2597,"AntiCalib:RawAmpl(S2 sect4)",100,0.,5000.,0.);
    
    HBOOK1(2545,"AntiCalib:Cyl-track Zcross(sector-1)",84,-42.,42.,0.);
    HBOOK1(2546,"AntiCalib:Cyl-track Zcross(sector-2)",84,-42.,42.,0.);
    HBOOK1(2547,"AntiCalib:Cyl-track Zcross(sector-3)",84,-42.,42.,0.);
    HBOOK1(2548,"AntiCalib:Cyl-track Zcross(sector-4)",84,-42.,42.,0.);
    HBOOK1(2549,"AntiCalib:Cyl-track Zcross(sector-5)",84,-42.,42.,0.);
    HBOOK1(2550,"AntiCalib:Cyl-track Zcross(sector-6)",84,-42.,42.,0.);
    HBOOK1(2551,"AntiCalib:Cyl-track Zcross(sector-7)",84,-42.,42.,0.);
    HBOOK1(2552,"AntiCalib:Cyl-track Zcross(sector-8)",84,-42.,42.,0.);
    HBOOK1(2553,"AntiCalib:Cyl-track Zcross(sector-9)",84,-42.,42.,0.);
    HBOOK1(2554,"AntiCalib:Cyl-track Zcross(sector-10)",84,-42.,42.,0.);
    HBOOK1(2555,"AntiCalib:Cyl-track Zcross(sector-11)",84,-42.,42.,0.);
    HBOOK1(2556,"AntiCalib:Cyl-track Zcross(sector-12)",84,-42.,42.,0.);
    HBOOK1(2557,"AntiCalib:Cyl-track Zcross(sector-13)",84,-42.,42.,0.);
    HBOOK1(2558,"AntiCalib:Cyl-track Zcross(sector-14)",84,-42.,42.,0.);
    HBOOK1(2559,"AntiCalib:Cyl-track Zcross(sector-15)",84,-42.,42.,0.);
    HBOOK1(2560,"AntiCalib:Cyl-track Zcross(sector-16)",84,-42.,42.,0.);
    HBOOK1(2626,"FiredLogSectNumb(zcrOK,dir=-1)",8,0.,8.,0.);
    HBOOK1(2627,"FiredLogSectNumb(zcrOK,dir=1)",8,0.,8.,0.);
    HBOOK1(2628,"FiredLogSectNumb(zcrOK,phi-mat,dir=-1)",8,0.,8.,0.);
    HBOOK1(2629,"FiredLogSectNumb(zcrOK,phi-mat,dir=1)",8,0.,8.,0.);
//hist 2561-2592,2624, 2596-2623 free
  }
//   Hist.2570-2599 reserved for PedCalib needs in ANTPedCalib::init()
}
void ANTI2JobStat::outpmc(){
//
  HPRINT(2639);
  HPRINT(2641);
  HPRINT(2642);
  HPRINT(2630);
  HPRINT(2631);
  HPRINT(2643);
  HPRINT(2632);
  HPRINT(2633);
  HPRINT(2634);
  HPRINT(2635);
  HPRINT(2636);
  HPRINT(2637);
  HPRINT(2640);
}
//
void ANTI2JobStat::outp(){
//
  if(ATREFFKEY.reprtf[0]>1){
    HPRINT(2515);
    HPRINT(2516);
    HPRINT(2500);
    HPRINT(2501);
    HPRINT(2512);
    HPRINT(2507);
    HPRINT(2502);
    HPRINT(2503);
    HPRINT(2504);
    HPRINT(2505);
    HPRINT(2506);
    HPRINT(2508);
    HPRINT(2509);
    HPRINT(2510);
    HPRINT(2511);
    
    for(int i=0;i<8;i++)HPRINT(2520+i);
  }
  if(ATREFFKEY.reprtf[0]>2){//low prior.hist
    HPRINT(2570);
    HPRINT(2571);
    HPRINT(2572);
    HPRINT(2573);
    HPRINT(2574);
    HPRINT(2575);
    HPRINT(2576);
    HPRINT(2577);
    HPRINT(2578);
    HPRINT(2579);
    HPRINT(2580);
    HPRINT(2581);
    HPRINT(2582);
    HPRINT(2583);
    HPRINT(2584);
    HPRINT(2585);
    HPRINT(2586);
    HPRINT(2587);
  }
  if(ATREFFKEY.relogic==1 && ATREFFKEY.reprtf[0]>1){
    HPRINT(2626);
    HPRINT(2627);
    HPRINT(2628);
    HPRINT(2629);
    HPRINT(2530);
    HPRINT(2531);
    HPRINT(2540);
    HPRINT(2541);
    HPRINT(2532);
    HPRINT(2533);
    HPRINT(2534);
    HPRINT(2535);
    HPRINT(2536);
    HPRINT(2537);
    HPRINT(2538);
    HPRINT(2625);
    HPRINT(2539);
    HPRINT(2545);
    HPRINT(2546);
    HPRINT(2547);
    HPRINT(2548);
    HPRINT(2549);
    HPRINT(2550);
    HPRINT(2551);
    HPRINT(2552);
    HPRINT(2553);
    HPRINT(2554);
    HPRINT(2555);
    HPRINT(2556);
    HPRINT(2557);
    HPRINT(2558);
    HPRINT(2559);
    HPRINT(2560);
  }
//
  if(ATREFFKEY.relogic==1)AntiCalib::fit();
  if(ATREFFKEY.relogic==1 && ATREFFKEY.reprtf[0]>1){
    HPRINT(2542);
    HPRINT(2543);
    HPRINT(2544);
    HPRINT(2593);
    HPRINT(2596);
    HPRINT(2597);
    HPRINT(2594);
    HPRINT(2595);
  }
}
//======================================================================
