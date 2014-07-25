//  $Id$

//////////////////////////////////////////////////////////////////////////
///
///\file  TrCalDB.C
///\brief Source file of TrCalDB class
///
///\date  2008/01/17 PZ  First version
///\date  2008/01/20 SH  File name changed, some utils are added
///\date  2008/01/23 SH  Some comments are added
///$Date$
///
///$Revision$
///
//////////////////////////////////////////////////////////////////////////

#include "TrCalDB.h"
#include "TkDBc.h"
#include "TFile.h"

ClassImp(TrCalDB);

TrCalDB* TrCalDB::Head=0;

float* TrCalDB::linear=0;


void TrCalDB::init(){
  if(Head==0){
    Head=this;
    run=0;
    for(int oct=0;oct<8;oct++){
      int crate=TkDBc::Head-> _octant_crate[oct];
      for(int tdr=0;tdr<24;tdr++){
	int hwid=crate*100+tdr;
	trcal_hwidmap[hwid]=new TrLadCal(hwid);
      }
    }
  } else{ 
    printf("Only One instance of CalDB Allowed\n");
    printf("Use CalDB:Head->Load() to load a new calibration\n");
  }
}

TrCalDB::TrCalDB(char * filename){
    if(filename) Load(filename);
}



TrCalDB::~TrCalDB(){
  for ( trcalIT pp=trcal_hwidmap.begin();pp!=trcal_hwidmap.end();++pp)
    if(pp->second) delete pp->second;
  if(linear) delete [] linear;
}


void TrCalDB::Clear(const Option_t*aa){
  TObject::Clear(aa);
  for ( trcalIT pp=trcal_hwidmap.begin();pp!=trcal_hwidmap.end();++pp)
    if(pp->second)  pp->second->Clear();
   if(linear)memset(linear,0,(GetLinearSize()/4)*sizeof(linear[0]));
}




TrLadCal* TrCalDB::FindCal_TkAssemblyId( int tkassemblyid){
  TkLadder* ll=TkDBc::Head->FindTkAssemblyId(tkassemblyid);
  if(ll) return FindCal_HwId(ll->GetHwId());
  else return 0;
}


TrLadCal* TrCalDB::FindCal_HwId( int hwid){ 
  trcalIT aa=trcal_hwidmap.find(hwid);
  if(aa != trcal_hwidmap.end()) 
    return aa->second; 
  else 
    return 0;
}

TrLadCal* TrCalDB::FindCal_TkId( int tkid){
  TkLadder* ll=TkDBc::Head->FindTkId(tkid);
  if(ll) 
    return FindCal_HwId(ll->GetHwId());
  else 
    return 0;
}


TrLadCal* TrCalDB::FindCal_LadName( string& name){
  TkLadder* ll=TkDBc::Head->FindLadName(name);
  if(ll) 
    return FindCal_HwId(ll->GetHwId());
  else 
    return 0;
}


TrLadCal* TrCalDB::FindCal_JMDC( int JMDCid){
  int crate=(JMDCid-282)/24;
  int tdr=JMDCid-crate*24-282;
  int hwid=crate*100+tdr;
  printf(" crate %d tdr %d hwid %d \n",crate, tdr, hwid);
  return FindCal_HwId( hwid);
}

 
TrLadCal* TrCalDB::GetEntry(int ii){
  int count=0;
  for(trcalIT pp=trcal_hwidmap.begin();pp!=trcal_hwidmap.end();pp++){
    if(count==ii) return &(*pp->second);
    count++;
  }
  return 0;
}


void TrCalDB::Load(char * filename){
  TFile* f=TFile::Open(filename);
  Head=dynamic_cast<TrCalDB*>( f->Get("TrCalDB"));
  f->Close();
  return ;
}


void TrCalDB::CalDB2Lin(){
  if(! linear) {
    printf("TrCalDB::CalDB2Lin()-INFO the linear space is created NOW\n");
    linear= new float[GetLinearSize()/4];
  }

  // PZ WARNING to Account for the new format used to store the CAL_RUN_ID into the TDV 
  //             we use the most significant bit as a flag for the new format
  //             there are no problems for dates up to 25/12/2037
  
  unsigned int run_to_store=run;

  run_to_store|=0x80000000;

  linear[0]=*((float*) &run_to_store);
  
  
  for (trcalIT aa=trcal_hwidmap.begin(); aa!=trcal_hwidmap.end();aa++){
    int hwid=aa->second->GetHwId();
    int crate=hwid/100;
    int tdr  =hwid%100;
    int offset=1+(crate*24+tdr)*TrLadCal::GetSize();
    aa->second->Cal2Lin(&(linear[offset]));
  }
}



void TrCalDB::Lin2CalDB(){
  if(linear==0){
    printf("TrCalDB::Lin2CalDB()- Error! the linear space pointer is NULL!\n");
    printf(" Calibration is NOT updated!!!\n");
    return;
  }
  // PZ WARNING to Account for the new format used to store the CAL_RUN_ID into the TDV 
  //             we use the most significant bit as a flag for the new format
  //             there are no problems for dates up to 25/12/2037

  
  unsigned int run_to_decode=*((unsigned int*) &(linear[0]));
  if((run_to_decode & 0x80000000)==0){
    run=(unsigned int) linear[0];
    if(run<1278000000 ){
      TrLadCal::SetVersion(1);
      printf(" TrCalDB::Lin2CalDB-W- ACCESSING A SC-MAGNET CALIBRATION (V1) while version  is set to: %d\n",TrLadCal::GetVersion());
    }else{
      TrLadCal::SetVersion(2);
      printf(" TrCalDB::Lin2CalDB-W- ACCESSING A PRE-FLIGHT CALIBRATION (V2) while version  is set to: %d \n",TrLadCal::GetVersion());
    }
  }
  else{
    run=run_to_decode&0x7FFFFFFF;
    TrLadCal::SetVersion(3);
    printf(" TrCalDB::Lin2CalDB-W- ACCESSING A FLIGHT CALIBRATION (V3) while version  is set to: %d\n",TrLadCal::GetVersion());
  }
  for (trcalIT aa=trcal_hwidmap.begin(); aa!=trcal_hwidmap.end();aa++){
    int hwid=aa->second->GetHwId();
    //printf("Filling Calibration %03d\n",hwid);
    int crate=hwid/100;
    int tdr  =hwid%100;
    int offset=1+(crate*24+tdr)*TrLadCal::GetSize();
    aa->second->Lin2Cal(&(linear[offset]));
    aa->second->SetFilled();
  }
  return;
}


void SLin2CalDB(){
  if(TrCalDB::Head)
    TrCalDB::Head->Lin2CalDB();
  return;
}





#ifdef __ROOTSHAREDLIBRARY__
int  TrCalDB::SaveCal2DB(){return 0;}
integer TrCalDB::checkdaqidS(int16u id){return 0;}
int TrCalDB::DecodeOneCal( int hwid,int16u * rri,int pri){return 0;}
void TrCalDB::updtrcalib2009S(integer n, int16u* p){return;}

#else
#include "timeid.h"
#include "commonsi.h"
#include "daqevt.h"
#include "id.h"
#include "job.h"
#include "event.h"

using namespace std;

int  TrCalDB::SaveCal2DB(){

  if(!TrCalDB::Head){
    printf("TrCalDB::SaveCal2DB---TrCalDB::Head is zero Cals NOT SAVED TO DB \n");
    return -3;}
  TrCalDB::Head->CreateLinear(); //Creates a linear array to feed the DB
  TrCalDB::Head->CalDB2Lin();    //Copy the calibrations to the linear array
  time_t bb=TrCalDB::Head->GetRun();
  time_t endtime=bb+24*3600*31;
  tm begin;
  tm end;
  tm* mtim=localtime_r(&bb,&begin);

  tm* mtim2=localtime_r(&endtime,&end);


  AMSTimeID* tt= new AMSTimeID(
			       AMSID("TrackerCals",1),
			       begin,
			       end,
			       TrCalDB::GetLinearSize(),
			       TrCalDB::linear,
			       AMSTimeID::Standalone,
			       1);
  tt->UpdateMe();
  int pp=tt->write(AMSDATADIR.amsdatabase);

  return 0;

}

void TrCalDB::updtrcalib2009S(integer n, int16u* p){
  uinteger leng=(n&65535);
  uinteger in=(n>>16);
  uinteger ic=in/trconst::ntdr;
  uinteger tdr=in%trconst::ntdr;
  if(DAQEvent::CalibInit(0)){
    cout<<"AMSTrRawCluster::updtrcalib2009S-I-InitCalib "<<endl;
    Head->Clear();
      //TrLadCal::SetVersion(TKGEOMFFKEY.CalibVer);
    TrLadCal::SetVersion(2);
    Head->run=AMSEvent::gethead()->getrun();
  }

  cout <<"  Crate TDR "<<ic<<"  "<<tdr<<endl;

  if( ((*p)&0x4) == 0){
    cerr<<"TrCalDB::updtrrcalib2009S-E-SigmaNotPresent "<<endl;
    return;
  }
  if( ((*p)&0x2) == 0){
    cerr<<"TrCalDB::updtrrcalib2009S-E-StatusNotPresent "<<endl;
    return;
  }

  TrCalDB::DecodeOneCal(ic*100+tdr,p,1);

  // here we check if the calibrations are all read and may be written to DB
  bool update=DAQEvent::CalibDone(0);

  if(update){
    Head->CalDB2Lin();
//#define PZDEBUG 1   
#ifdef PZDEBUG    
#include <time.h>
    time_t bb=Head->GetRun();
    time_t endtime=bb+3600*24*31;
    tm begin;
    tm end;
    tm* mtim=localtime_r(&bb,&begin);

    tm* mtim2=localtime_r(&endtime,&end);


    AMSTimeID* tt= new AMSTimeID(
	AMSID("TrackerCals",1),
	begin,
	end,
	TrCalDB::GetLinearSize(),
	TrCalDB::linear,
	AMSTimeID::Standalone,
	1);
    tt->UpdateMe();	
    int pp=tt->write(AMSDATADIR.amsdatabase);
    if (tt) delete tt;
#else
    AMSTimeID * ptdv;
    AMSJob * gg= AMSJob::gethead();

    AMSID aa("TrackerCals",AMSJob::gethead()->isRealData());
    ptdv = gg->gettimestructure(aa);

    printf("tdv vale 0x%p\n",ptdv);
    printf("telav\n");
    cout<<*ptdv<<endl; 
    if(!ptdv) {
      cout<<"TrCalDB::updtrrcalib2009S-E-Cannot find TrackerCals TDV"<<endl;
      return;
    }
    
    ptdv->UpdateMe()=1;
    ptdv->UpdCRC();
    time_t begin,end,insert;
    time(&insert);
    if(CALIB.InsertTimeProc)insert=AMSEvent::gethead()->getrun();
    ptdv->SetTime(insert,AMSEvent::gethead()->getrun()-1,AMSEvent::gethead()->getrun()+8640000);
    cout <<" Tracker H/K  info has been read for "<<*ptdv;
    ptdv->gettime(insert,begin,end);
    cout <<" Time Insert "<<ctime(&insert);
    cout <<" Time Begin "<<ctime(&begin);
    cout <<" Time End "<<ctime(&end);
#endif    
  }
  return;
}






int TrCalDB::DecodeOneCal( int hwid,int16u * rr,int pri){

  CaloutDSP cal;
  ushort size;
  int16u* offset=rr;
  int cpar=*(rr++);
  if(pri>0) printf("Command parameter is: %X \n",cpar); 

  if(cpar&0x1){ // Pedestals (1/8 ADC units)
    if(pri>0) printf("Reading Pedestals\n");
    for (int ii=0;ii<1024;ii++){
      cal.ped[ii]=*(rr++)*1.;
    }
  }
  else {
    for (int ii=0;ii<1024;ii++){
      cal.ped[ii]=0.; // default
    }
  }

  if(cpar&0x2){ // Flags
    if(pri>0) printf("Reading Flags\n");
    for (int ii=0;ii<1024;ii++){
      cal.status[ii]=*(rr++);
    }
  }
  else {
    for (int ii=0;ii<1024;ii++){
      cal.status[ii]=0; // default
    }
  }

  if(cpar&0x4){ // Sigma low (1/8 ADC units, used in see LadCal::Fill_old method to construct sigma)
    if(pri>0) printf("Reading Low-Sigma\n");
    for (int ii=0;ii<1024;ii++){
      cal.sig[ii]=*(rr++)*1.;
    }
  }
  else {
    for (int ii=0;ii<1024;ii++){
      cal.sig[ii]=-1000.; // default
    }
  }

  if(cpar&0x8){ // Raw Sigma (multiplied by parameter 7, 1/8 ADC units)
    if(pri>0) printf("Reading Sigma-Raw\n");
    for (int ii=0;ii<1024;ii++){
      cal.rsig[ii]=*(rr++)*1.;
    }
  }
  else {
    for (int ii=0;ii<1024;ii++){
      cal.rsig[ii]=-1000.; // default
    }
  }

  if(cpar&0x10){ // Sigma High (1/8 ADC units), DISCARDED
    if(pri>0) printf("Reading Sigma-High\n");
    for (int ii=0;ii<1024;ii++){
      // if(pri>0) printf("Sigma High  %d  %d\n",ii,(short int)*rr));
      (void) *(rr++);
    }
  }

  if(cpar&0x20){ // Double-Trigger Occupancy Table
    // starting from version a810
    if(pri>0) printf("Reading Double-Trigger Occupancy Table\n");
    for (int ii=0;ii<1024;ii++){
      // if(pri>0) printf("Occupancy  %d  %d\n",ii,*rr));
      cal.occupancy[ii]=(unsigned short int)*(rr++);
    }
  }
  else {
    for (int ii=0;ii<1024;ii++){
      cal.occupancy[ii]=0; // default  
    }
  }
  
  if(cpar&0x40){ // CN Sigma
    // starting from version a810
    if(pri>0) printf("Reading CN RMS\n");
    for (int ii=0;ii<16;ii++){ 
      // if(pri>0) printf("CN rms  %d  %d\n",ii,(short int)*rr));
      cal.CNrms[ii]=((short int)*(rr++))/8.;
    }
  }
  else {
    for (int ii=0;ii<16;ii++) { 
      cal.CNrms[ii]=0; // default
    }
  }  
  
  if(cpar&0x80){ // CN mean
    // starting from version a810
    if(pri>0) printf("Reading CN Mean\n");
    for (int ii=0;ii<16;ii++){
      // if(pri>0) printf("CN mean  %d  %d\n",ii,(short int)*rr);
      cal.CNmean[ii]=((short int)*(rr++))/8.;
    }
  }
  else {
    for (int ii=0;ii<16;ii++) { 
      cal.CNmean[ii]=0; // default
    }
  }  

  if (cpar&0x100){ // Non-Gaussian Occupancy Table (from calibration)
    // starting from version a810
    // from a810: calculated from calibration
    // from aa02: calculated from data  
    // from aa1b: calculated from calibration
    // if (pri>0) printf("Reading Occupancy table (non-gaussian channels)\n");
    if(pri>0) printf("Reading Non-Gaussian Occupancy Table\n");
    for (int ii=0;ii<1024;ii++){
      // this table is temporary, as the memory space used by this table is also used by reduction process 
      cal.occupgaus[ii]=(unsigned short int)*(rr++);
    }
  }
  else {   
    for (int ii=0;ii<1024;ii++) { 
      cal.occupgaus[ii] = 0; // default for versions before than a810
    }
  }
     
  if(cpar&0x200){ // Sigma (1/8 ADC units, used in LadCal::Fill_new method)
    // starting from version a810
    if(pri>0) printf("Reading Sigma\n");
    for (int ii=0;ii<1024;ii++) {
      cal.sig[ii]=*(rr++)*1.;
    }
  }

  if(cpar&0x400){ // Non-Gaussian Occupancy Table (from data), DISCARDED
    // starting from version aa1b
    // to get the real occupancy value you will have to mask the value with 0x7FFF
    if(pri>0) printf("Reading Non-Gaussian Occupancy Table from data\n");
    for (int ii=0;ii<1024;ii++) {
      (void) *(rr++);
    }
  }


  cal.dspver=*(rr++);          if(pri>0) printf("Detector DSP version %hX\n",cal.dspver);
  cal.S1_lowthres=*(rr++)/8.;  if(pri>0) printf("Lowsigma factor S1 %f\n",cal.S1_lowthres);
  cal.S1_highthres=*(rr++)/8.; if(pri>0) printf("Highsigma factor S1 %f\n",cal.S1_highthres);
  cal.S2_lowthres=*(rr++)/8.;  if(pri>0) printf("Lowsigma factor S2 %f\n",cal.S2_lowthres);
  cal.S2_highthres=*(rr++)/8.; if(pri>0) printf("Highsigma factor S2 %f\n",cal.S2_highthres);
  cal.K_lowthres=*(rr++)/8.;   if(pri>0) printf("Lowsigma factor K %f\n",cal.K_lowthres);
  cal.K_highthres=*(rr++)/8.;  if(pri>0) printf("Highsigma factor K %f\n",cal.K_highthres);
  cal.sigrawthres=*(rr++);     if(pri>0) printf("SigmaRaw factor %f\n",cal.sigrawthres);
  cal.Power_failureS=*(rr++);  if(pri>0) printf("Power Failures on S %d\n",cal.Power_failureS);
  cal.Power_failureK=*(rr++);  if(pri>0) printf("Power Failures on K %d\n",cal.Power_failureK);
  cal.calnum=*(rr++);          if(pri>0) printf("Calibration Events %d\n",cal.calnum);
  cal.usedev=*(rr++);          if(pri>0) printf("Used triggers %d\n",cal.usedev);
  cal.calstatus=*(rr++);       if(pri>0) printf("Calibration status 0x%04X\n",cal.calstatus);
  int TDRStat=*(rr++);         if(pri>0) printf("TDR status 0x%04X\n",TDRStat);
  if(pri>0) printf("---------> READ   %ld  word for this cal\n",rr-offset);

  // Fill the Calibration object
  // int hwid=((bb->node-282)/24)*100+(bb->node-282)%24;
  TrLadCal* ladcal=Head->FindCal_HwId(hwid);
  if(ladcal){
    if ((cpar&0x4)&&((cpar&0x200)==0)) { 
      if(pri>0) printf("Calling TrLadPar::Fill_old\n");
      ladcal->Fill_old(&cal);
    }
    else if (cpar&0x200) { 
      if(pri>0) printf("Calling TrLadPar::Fill_new\n");
      ladcal->Fill_new(&cal);
    }
    if (pri>0) ladcal->PrintInfo(pri-1);
  }
  else {
    printf("TrCalDB-------------------> WARNING I CANT FIND The calibration object to be filled\n");
  }
  return 1;
}

int TrCalDB::checkdaqidS(unsigned short int id){
 
  for(int i=0;i<trconst::ncrt;i++){
    for(int j=0;j<trconst::ntdr;j++){
      char sstr[128];
      char ab[2];
      ab[1]='\0';
      ab[0]='A';
      if(j%2)ab[0]='B';
      sprintf(sstr,"TDR%X%X%s",i,j/2,ab);
      if(DAQEvent::ismynode(id,sstr)){
	return j+i*trconst::ntdr+1; 
      }
    }
  }
  return 0;
}


#endif
