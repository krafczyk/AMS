//  $Id: TrCalDB.C,v 1.5 2009/11/26 23:32:21 pzuccon Exp $

//////////////////////////////////////////////////////////////////////////
///
///\file  TrCalDB.C
///\brief Source file of TrCalDB class
///
///\date  2008/01/17 PZ  First version
///\date  2008/01/20 SH  File name changed, some utils are added
///\date  2008/01/23 SH  Some comments are added
///$Date: 2009/11/26 23:32:21 $
///
///$Revision: 1.5 $
///
//////////////////////////////////////////////////////////////////////////

#include "TrCalDB.h"
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
  if(linear) delete [] linear;
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
  Head=(TrCalDB*) f->Get("TrCalDB");
  f->Close();
  return ;
}

void TrCalDB::CalDB2Lin(){

  if(! linear) {
    printf("TrCalDB::CalDB2Lin()-INFO the linear space is created NOW\n");
    linear= new float[GetLinearSize()/4];
  }
  linear[0]=(float) run;
  for (trcalIT aa=trcal_hwidmap.begin(); aa!=trcal_hwidmap.end();aa++){
    int hwid=aa->second->GetHwId();
    int crate=hwid/100;
    int tdr  =hwid%100;
    int offset=1+(crate*24+tdr)*TrLadCal::GetSize();
    aa->second->Cal2Lin(&(linear[offset]));
  }

}



void TrCalDB::Lin2CalDB(){

  if(! linear){
    printf("TrCalDB::Lin2CalDB()- Error! the linear space pointer is NULL!\n");
    printf(" Calibration is NOT updated!!!\n");
    return;
  }
  run=(int) linear[0];
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




void TrCalDB::updtrcalibS(integer n, int16u* p){
  uinteger leng=(n&65535);
  leng-=10;
  int pri=0;
  uinteger in=(n>>16);
  uinteger ic=in/trconst::ntdr;
  uinteger tdr=in%trconst::ntdr;
  int leng4=leng/4;
  cout <<"  Crate TDR "<<ic<<"  "<<tdr<<endl;
  CaloutDSP cal;
  unsigned short size;
  int rr=0;
  int cpar=p[rr++];
  if(pri>0) printf("Command parameter is: %X \n",cpar); 

  if(cpar&0x1){ //pedestals
    if(pri>0) printf("Reading Pedestals\n");
    for (int ii=0;ii<1024;ii++){
      cal.ped[ii]=p[rr++]*1.;
    }
  }

  if(cpar&0x2){ //flags
    if(pri>0) printf("Reading flags\n");
    for (int ii=0;ii<1024;ii++){
      cal.status[ii]=p[rr++];
    }
  }

  if(cpar&0x4){ //sigma (low)
    if(pri>0) printf("Reading Sigmas\n");
    for (int ii=0;ii<1024;ii++){
      cal.sig[ii]=p[rr++]*1.;
    }
  }

  if(cpar&0x8){ //rawsigma (multiplied by parameter 7)
    if(pri>0) printf("Reading Raw Sigmas\n");
    for (int ii=0;ii<1024;ii++){
      cal.rsig[ii]=p[rr++]*1.;
    }
  }
  if(cpar&0x10){ // sigma high
    if(pri>0) printf("Reading Sigma High \n");
    for (int ii=0;ii<1024;ii++){
      // if(pri>0) printf("Sigma High  %d  %d\n",ii,(short int)p[rr]);
      p[rr++];
    }
  }

  if(cpar&0x20){ //Occupancytable double trigger
    if(pri>0) printf("Reading Occupancy table \n");
    for (int ii=0;ii<1024;ii++){
      // if(pri>0) printf("Occupancy  %d  %d\n",ii,p[rr]);
      cal.occupancy[ii]=(unsigned short int)p[rr++];
    }
  }else
    for (int ii=0;ii<1024;ii++){
      // if(pri>0) printf("CN mean  %d  %d\n",ii,(short int)p[rr]);
      cal.occupancy[ii]=0;
    }
  
  if(cpar&0x40){ //CN Sigma
    if(pri>0) printf("Reading CN rms \n");
    for (int ii=0;ii<16;ii++){ 
      //     if(pri>0) printf("CN rms  %d  %d\n",ii,(short int)p[rr]);
      cal.CNrms[ii]=((short int)p[rr++])/8.;
    }
  }
  
  if(cpar&0x80){ //CN mean
    if(pri>0) printf("Reading CN mean \n");
    for (int ii=0;ii<16;ii++){
      // if(pri>0) printf("CN mean  %d  %d\n",ii,(short int)p[rr]);
      cal.CNmean[ii]=((short int)p[rr++])/8.;
    }
  }

  
 
  cal.dspver=p[rr++];
  if(pri>0) printf("Detector DSP version %hX\n",cal.dspver);
  

  cal.S1_lowthres=p[rr++]/8.;
  if(pri>0) printf("Lowsigma factor S1 %f\n",cal.S1_lowthres);

  cal.S1_highthres=p[rr++]/8.;
  if(pri>0) printf("Highsigma factor S1 %f\n",cal.S1_highthres);
  
  cal.S2_lowthres=p[rr++]/8.;
  if(pri>0) printf("Lowsigma factor S2 %f\n",cal.S2_lowthres);
    
  cal.S2_highthres=p[rr++]/8.;
  if(pri>0) printf("Highsigma factor S2 %f\n",cal.S2_highthres);
  

  cal.K_lowthres=p[rr++]/8.;
  if(pri>0) printf("Lowsigma factor K %f\n",cal.K_lowthres);
  
  cal.K_highthres=p[rr++]/8.;
  if(pri>0) printf("Highsigma factor K %f\n",cal.K_highthres);
  
  cal.sigrawthres=p[rr++];
  if(pri>0) printf("sigmaraw factor %f\n",cal.sigrawthres);
  

  cal.Power_failureS=p[rr++];
  if(pri>0) printf("Power Failures on S %d\n",cal.Power_failureS);

  cal.Power_failureK=p[rr++];
  if(pri>0) printf("Power Failures on K %d\n",cal.Power_failureK);

  cal.calnum=p[rr++];
  if(pri>0) printf("Calibration Events %d\n",cal.calnum);

  cal.usedev=p[rr++];
  if(pri>0) printf("Used triggers %d\n",cal.usedev);
  
  cal.calstatus=p[rr++];
  if(pri>0) printf("Calibration status 0x%04X\n",cal.calstatus);

  int TDRStat=p[rr++];
  if(pri>0) printf("TDR status 0x%04X\n",TDRStat);
  
  if(pri>0) printf("---------> READ   %d  word for this cal\n",rr);
  //Fill the Calibration object
  //  int hwid=((bb->node-282)/24)*100+(bb->node-282)%24;
  int hwid=ic*100+tdr;
  if(!TrCalDB::Head) {
    printf("-------------------> WARNING I CANT FIND The calibration DBase!!! CALIBRATION NOT FILLED!!\n");
    return;
  }
  TrLadCal* ladcal=TrCalDB::Head->FindCal_HwId(hwid);
  if(ladcal){
    ladcal->Fill(&cal);
    if(pri>0)  ladcal->PrintInfo(pri-1);
  }else
    printf("-------------------> WARNING I CANT FIND The calibration object to be filled\n");

 
  
  return ;
}

#ifdef __ROOTSHAREDLIBRARY__
int  TrCalDB::SaveCal2DB(){return 0;}

#else
#include "timeid.h"
#include "commonsi.h"

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
#endif
