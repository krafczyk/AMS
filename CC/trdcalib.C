//  $Id: trdcalib.C,v 1.13 2011/01/29 03:48:37 mmilling Exp $
#include "trdcalib.h"
#include "event.h"
#include <math.h>
#include "timeid.h"
// PZ #include "trrawcluster.h"
#include <float.h>
#include "mccluster.h"
#include "trddbc.h"
#include "mceventg.h"

time_t AMSTRDIdCalib::_BeginTime=0;
time_t AMSTRDIdCalib::_CurTime=0;
uinteger AMSTRDIdCalib::_CurRun=0;


void AMSTRDIdCalib::_clear(){
int i;
for(i=0;i<getnchan();i++){
 _Count[i]=0;
 _ADC[i]=0;
 _ADC2[i]=0;
 _ADCRaw[i]=0;
 _ADC2Raw[i]=0;
 _BadCh[i]=0;
 _ADCMax[i]=-FLT_MAX;
}

}





void AMSTRDIdCalib::ntuple(integer s){
  if(IOPA.hlun){
   static TRDCalib_def TRDCALIB;
   CALIB.Ntuple++;
    cout <<  " CALIB " <<CALIB.Ntuple<<endl;
   HBNT(CALIB.Ntuple,"trd calibaration"," ");
   HBNAME(CALIB.Ntuple,"TrdCalib",(int*)(&TRDCALIB),"PSLayer:I,PSLadder:I,PSTube:I,Ped:R,Sigma:R,BadCh:R");
   int i,j,k;
     for(i=0;i<TRDDBc::nlay();i++){
       for(j=0;j<TRDDBc::LaddersNo(0,i);j++){
        for(k=0;k<trdconst::maxtube;k++){
         AMSTRDIdSoft id(i,j,k);
         if(id.dead())continue;
          TRDCALIB.Layer=i+1;
          TRDCALIB.Ladder=j+1;
          TRDCALIB.Tube=k;
          TRDCALIB.Ped=id.getped();
          TRDCALIB.Sigma=id.getsig();
          TRDCALIB.BadCh=id.checkstatus(AMSDBc::BAD);
          HFNT(CALIB.Ntuple);
         }
        }
    }
}
}
  

void AMSTRDIdCalib::_hist(){
  // write down the difference between system and calculated pedestals

}

void AMSTRDIdCalib::_add(){
if(!AMSEvent::gethead())return;
   _CurTime=AMSEvent::gethead()->gettime();
   _CurRun=AMSEvent::gethead()->getrun();
  integer static first=0;
  if(first++ == 0){
    _BeginTime=AMSEvent::gethead()->gettime();
  }
  
  for(int icll=0;icll<AMSTRDIdSoft::ncrates();icll++){
   int nn=0;
   int nnt=0;
   AMSTRDRawHit *p=(AMSTRDRawHit*)AMSEvent::gethead()->getheadC("AMSTRDRawHit",icll);
   while(p){
    AMSTRDIdCalib cid(p->getidsoft());
    if(!cid.dead()){
      cid.updADCRaw(p->Amp());
      cid.updADC(p->Amp());
      cid.updADC2(p->Amp());
      cid.updADC2Raw(p->Amp());
      cid.updADCMax(p->Amp());
      cid.updcounter();
      nn++;
    }
    nnt++;
    p=p->next();
   }
   cout <<" crate "<<icll<<" "<<nn<<" "<<nnt<<endl;
}

}

void AMSTRDIdCalib::printbadchanlist(){
  if(TRCALIB.PrintBadChList){
  ofstream fcluster;
  fcluster.open("../datacards/TRDBadChannelList.Clusters",ios::out);


 int i,j,k;
     for( i=0;i<TRDDBc::nlay();i++){
       for(j=0;j<TRDDBc::LaddersNo(0,i);j++){
        for(k=0;k<trdconst::maxtube;k++){
         AMSTRDIdSoft id(i,j,k);
         if(id.dead())continue;
         if(id.checkstatus(AMSDBc::BAD))fcluster << hex<< id.gethaddr()<<endl;
        }
       }
     }







  fcluster.close();

    
  }
}


void AMSTRDIdCalib::_calc(){

int badp=0;
 int i,j,k;
     for( i=0;i<TRDDBc::nlay();i++){
       for(j=0;j<TRDDBc::LaddersNo(0,i);j++){
        for(k=0;k<trdconst::maxtube;k++){
         AMSTRDIdSoft id(i,j,k);
        AMSTRDIdCalib cid(id);
        if(cid.dead())continue;
         if(cid.getcount()>1){
          int ch=cid.getchannel();
          _ADC[ch]=(_ADC[ch]-_ADCMax[ch])/(_Count[ch]-1);
          _ADC2[ch]=(_ADC2[ch]-_ADCMax[ch]*_ADCMax[ch])/(_Count[ch]-1);
          _ADC2[ch]=sqrt(fabs(_ADC2[ch]-_ADC[ch]*_ADC[ch]));
          _ADCRaw[ch]=_ADCRaw[ch]/_Count[ch];
          _ADC2Raw[ch]=_ADC2Raw[ch]/_Count[ch];
          _BadCh[ch]=_BadCh[ch]/_Count[ch];
          _ADC2Raw[ch]=sqrt(fabs(_ADC2Raw[ch]-_ADCRaw[ch]*_ADCRaw[ch]));
          if(cid.checkstatus(AMSDBc::BAD))badp++;
        }
       }
      }
}
}

void AMSTRDIdCalib::_update(){
 int i,j,k;
     int total=0;
     for( i=0;i<TRDDBc::nlay();i++){
       for(j=0;j<TRDDBc::LaddersNo(0,i);j++){
        for(k=0;k<trdconst::maxtube;k++){
         AMSTRDIdSoft id(i,j,k);
         AMSTRDIdCalib cid(id);
         if(cid.dead())continue;
         if(cid.getcount()){
          total++;
          int ch=cid.getchannel();
          cout <<cid.getped()<<" "<<_ADCRaw[ch]<<endl;
          cid.setped()=cid.getped()+_ADCRaw[ch];
	  //          cid.setgain()=1;
          cout <<"sig "<<cid.getsig()<<" "<<_ADC2[ch]<<endl;
          //cid.setsig()=_ADC2[ch];
         }
        }
       }
     }
     
    if (AMSFFKEY.Update){
    AMSTimeID *ptdv;
     time_t begin,end,insert;
    if(total){
      const int ntrd=3;
      const char* TDV2Update[ntrd]={"TRDPedestals","TRDSigmas","TRDGains"};
      for (int i=0;i<ntrd;i++){
      ptdv = AMSJob::gethead()->gettimestructure(AMSID(TDV2Update[i],AMSJob::gethead()->isRealData()));
      ptdv->UpdateMe()=1;
      ptdv->UpdCRC();
      time(&insert);
      ptdv->SetTime(insert,getrun()-TRDCALIB.Validity[0],_CurTime+TRDCALIB.Validity[1]+100000000);
      cout <<" TRD H/K  info has been updated for "<<*ptdv;
      ptdv->gettime(insert,begin,end);
      cout <<" Time Insert "<<ctime(&insert);
      cout <<" Time Begin "<<ctime(&begin);
      cout <<" Time End "<<ctime(&end);

 }

}
    cout << "AMSTRDIdCalib::_update-I-total of "<<total<<" channels updated"<<endl;
}




    
   




        // Here update dbase

     AMSTimeID * offspring = 
     (AMSTimeID*)((AMSJob::gethead()->gettimestructure())->down());
     while(offspring){
       if(offspring->UpdateMe())cout << " Starting to update "<<*offspring; 
      if(offspring->UpdateMe() && !offspring->write(AMSDATADIR.amsdatabase))
      cerr <<"AMSJob::_timeinitjob-S-ProblemtoUpdate "<<*offspring;
      offspring=(AMSTimeID*)offspring->next();
     }
    



    _BeginTime=_CurTime;

   }


integer * AMSTRDIdCalib::_Count=0;
geant * AMSTRDIdCalib::_BadCh=0;
number  *  AMSTRDIdCalib::_ADC=0;
number *  AMSTRDIdCalib::_ADC2=0;
number  *  AMSTRDIdCalib::_ADCRaw=0;
number *  AMSTRDIdCalib::_ADC2Raw=0;
number *  AMSTRDIdCalib::_ADCMax=0;


void AMSTRDIdCalib::initcalib(){
_Count= new integer[getnchan()];
_BadCh= new geant[getnchan()];
_ADC= new number[getnchan()];
_ADC2= new number[getnchan()];
_ADCRaw= new number[getnchan()];
_ADC2Raw= new number[getnchan()];
_ADCMax= new number[getnchan()];

assert (_Count && _ADC && _ADC2 && _ADCMax && _ADC2Raw && _ADCRaw && _BadCh);
_clear();




}


void AMSTRDIdCalib::check(integer forcedw){
_add();
static integer counter=0;
static integer hist=0;
int cnt=0;
double acc=0;
if(++counter%TRDCALIB.EventsPerCheck == 0 || forcedw){
 int i,j,k;
     for( i=0;i<TRDDBc::nlay();i++){
       for(j=0;j<TRDDBc::LaddersNo(0,i);j++){
        for(k=0;k<trdconst::maxtube;k++){
         AMSTRDIdSoft id(i,j,k);
        AMSTRDIdCalib cid(id);
        if(cid.dead())continue;
         if(cid.getcount()){
          int ch=cid.getchannel();
          acc+=
          sqrt(fabs(_ADC2[ch]-_ADC[ch]*_ADC[ch]/_Count[ch]))/_Count[ch];
          cnt++;
       }
     }
    }
   }

   if(cnt>0 ){
     if(!forcedw && acc/cnt < TRDCALIB.PedAccRequired){
       cout << "AMSTRDIdCalib::check-I-peds & sigmas succesfully calculated with accuracy "<<
         acc/cnt<<" ( "<<counter<<" ) events."<<endl;
       cout << "AMSTRDIdCalib::check-I-peds & sigmas succesfully calculated for  "<< cnt<< " Channels"<<endl;
       _calc();
       _hist();
       _update();
       _clear();
       counter=0;
       hist=1;
     }
     else if(forcedw && hist==0){
       cout << "AMSTRDIdCalib::check-I-peds & sigmas succesfully calculated with accuracies "<<
         acc/cnt<<" ( "<<counter<<" ) events."<<endl;
       cout << "AMSTRDIdCalib::check-I-peds & sigmas succesfully calculated for  "<< cnt<< " Channels"<<endl;
       _calc();
       _hist();
       _update();
       _clear();
       counter=0;
     }
   }
}
}





