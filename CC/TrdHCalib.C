#include "TrdHCalib.h"
#include "root.h"
#include "timeid.h"
#include "amsdbc.h"
#include "TF1.h"

/*#ifndef _PGTRACK_
#include "commonsi.h"
AMSDATADIR_DEF amsdatadir_;
#endif*/

ClassImp(TrdHCalibR);
TrdHCalibR *TrdHCalibR::head=0;

void TrdHCalibR::update_medians(TrdHTrackR* track,int opt,float beta,int debug){
  int layer,ladder,tube;
  for(int seg=0;seg<2;seg++){
    if(!track->pTrdHSegment(seg)) continue;
    for(int i=0;i<(int)track->pTrdHSegment(seg)->fTrdRawHit.size();i++){
      TrdRawHitR* hit=track->pTrdHSegment(seg)->pTrdRawHit(i);
      if(!hit||hit->Amp<5)continue;

      int tubeid;
      GetTubeIdFromLLT(hit->Layer,hit->Ladder,hit->Tube,tubeid);
      float amp=hit->Amp;
      if(opt>0)amp*=GetPathCorr(track->TubePath(hit->Layer,hit->Ladder,hit->Tube));
      if(opt>1)amp*=GetBetaCorr(beta);// approx mip beta (betagamma 3.04)
      
#pragma omp critical (trdmed)
      {
	if(amp>tube_medians[tubeid])tube_medians[tubeid]+=0.04;
	if(amp<tube_medians[tubeid])tube_medians[tubeid]-=0.1;

	if(amp>mod_medians[hit->Layer][hit->Ladder])mod_medians[hit->Layer][hit->Ladder]+=0.04;
	if(amp<mod_medians[hit->Layer][hit->Ladder])mod_medians[hit->Layer][hit->Ladder]-=0.1;

	if(debug)
	  printf("LLT %02i%02i%02i id %i med %.2f\n",hit->Layer,hit->Ladder,hit->Tube,tubeid,tube_medians[tubeid]);
	tube_occupancy[tubeid]++;
      }
    }
  }
  
}

void TrdHCalibR::init_calibration(float start_value){
  calibrate=true;
  for(int i=0;i<5248;i++)
    if(tube_medians[i]==0)tube_medians[i]=start_value;
  for(int i=0;i<20;i++)for(int j=0;j<18;j++)mod_medians[i][j]=start_value;
}

float TrdHCalibR::MeanOccupancy(int opt){
  float sum=0.;
  for(int i=0;i<5248;i++)sum+=tube_occupancy[i];
  return sum/5248.;
}

float TrdHCalibR::MeanGaussMedian(int opt){
  TH1F h("h_gain","",100,0.,120.);
  for(int i=0;i<5248;i++)h.Fill(tube_medians[i]);
  TF1 f("f","gaus");
  h.Fit(&f,"Q0");
  return f.GetParameter(1);
}

float TrdHCalibR::MeanGaussGain(int opt){
  TH1F h("h_gain","",100,0.,3.);
  int layer,ladder,tube;

  for(int i=0;i<5248;i++){
    GetLLTFromTubeId(layer,ladder,tube,i);
    int ntdv=layer*18*16+ladder*16+tube;
    
    h.Fill(tube_gain[ntdv]);
  }
  
  TF1 f("f","gaus");
  h.Fit(&f,"Q0");

  // conversion factor to mean gain 1. stored here
  tube_gain[250]=f.GetParameter(1);
  
  // target ADC value of gain correction
  tube_gain[251]=norm_mop;

  return f.GetParameter(1);
}

int TrdHCalibR::FillMedianFromTDV(){
  bool toReturn = true;
  int layer,ladder,tube;
  
  bool allone=true;
  for(int i=0;i<5248;i++){
    GetLLTFromTubeId(layer,ladder,tube,i);
    int ntdv=layer*18*16+ladder*16+tube;
    tube_medians[i]=tube_gain[ntdv]*norm_mop;
    //    if(i%500==0)printf("i %i norm %.2f gain %.2f med %.2f\n",i,norm_mop,tube_gain[ntdv],tube_medians[i]);

    if( tube_medians[i] < 0. )
      toReturn = false;
    
    if (tube_gain[ntdv]!=1.)allone=false;
  }

  return toReturn+2*allone;
}

int TrdHCalibR::FillTDVFromMedian(){
  int nlowocc=0;

  int layer,ladder,tube;
  for(int i=0;i<5248;i++){
    GetLLTFromTubeId(layer,ladder,tube,i);
    int ntdv=layer*18*16+ladder*16+tube;

    if(tube_occupancy[i]==0){
      tube_gain[ntdv]=tube_medians[i]/norm_mop;
     tube_status[ntdv]= tube_status[ntdv] = AMSDBc::BAD;
    }
    else if(tube_occupancy[i]>min_occupancy){
      tube_gain[ntdv]=tube_medians[i]/norm_mop;
      tube_status[ntdv]= tube_status[ntdv] | AMSDBc::BAD;
      tube_status[ntdv]= tube_status[ntdv] & AMSDBc::TRDLOWOCC;
      tube_status[ntdv]= tube_status[ntdv] & AMSDBc::TRDGROUPED;
    }
    else{
      tube_gain[ntdv]=mod_medians[layer][ladder]/norm_mop;
      tube_status[ntdv]= tube_status[ntdv] | AMSDBc::BAD;
      tube_status[ntdv]= tube_status[ntdv] | AMSDBc::TRDLOWOCC;
      tube_status[ntdv]= tube_status[ntdv] | AMSDBc::TRDGROUPED;
      nlowocc++;
    }
  }
  return nlowocc;
}


bool TrdHCalibR::closeTDV(){

  for(map<string,AMSTimeID*>::iterator it=tdvmap.begin();it!=tdvmap.end();it++){
    delete it->second;
  }
  tdvmap.clear();
  return 1;
}

bool TrdHCalibR::readTDV(unsigned int t, int debug){
  time_t thistime=(time_t)t;

  bool ok=true;
  for(map<string,AMSTimeID*>::iterator it=tdvmap.begin();it!=tdvmap.end();it++)
    if(!it->second->validate(thistime))ok=false;

  
  if(FillMedianFromTDV()) ok=false;

  if(debug)
    for(int i=0;i<5248;i++){
      if(i%500==0){
	int layer,ladder,tube;
	GetLLTFromTubeId(layer,ladder,tube,i);
	int ntdv=layer*18*16+ladder*16+tube;
	printf("TrdHCalibR::readTDV - tube %4i reading gain %.2f stat %u - setting median %.2f\n",i,tube_gain[ntdv],tube_status[ntdv],tube_medians[i]);
      }
    }
  
  return ok;
}

bool TrdHCalibR::readSpecificTDV(string which,unsigned int t, int debug){
  bool ok=true;
  time_t thistime=(time_t)t;
  for(map<string,AMSTimeID*>::iterator it=tdvmap.begin();it!=tdvmap.end();it++)
    if(it->first==which)
      if(!it->second->validate(thistime))ok=false;
  
  return ok;
}

bool TrdHCalibR::InitTDV(unsigned int bgtime, unsigned int edtime, int type,char * tempdirname){
  time_t begintime = (time_t) bgtime;
  time_t endtime   = (time_t) edtime;
  
  //maxtube*trdconst::maxlad*trdconst::maxlay+maxtube*trdconst::maxlad+maxtube;
  int size=16*18*20+16*18+16;

  for(int i=0;i<size;i++){
    tube_gain[i]=1.;
    tube_status[i]=0;
  }
  for(int i=0;i<5248;i++)
    tube_occupancy[i]=0;
  
  AMSTimeID::CType server=AMSTimeID::Standalone;
  tm begin = *gmtime(&begintime);
  tm end   = *gmtime(&endtime);
  
  char tdname[200];
  sprintf(tdname, "%s/DataBase/", tempdirname);
  if(!AMSDBc::amsdatabase)
    AMSDBc::amsdatabase=new char[200];
  
  strcpy(AMSDBc::amsdatabase,tdname);
  
  AMSTimeID *ptdv=new AMSTimeID(AMSID("TRDPedestals",type),
				begin,end,sizeof(tube_pedestals[0])*size,
				(void*)tube_pedestals,server);
  
  tdvmap.insert(pair<string,AMSTimeID*>("TRDPedestals",ptdv));


  ptdv=new AMSTimeID(AMSID("TRDSigmas",type),
				begin,end,sizeof(tube_sigmas[0])*size,
				(void*)tube_sigmas,server);
  
  tdvmap.insert(pair<string,AMSTimeID*>("TRDSigmas",ptdv));


  ptdv=new AMSTimeID(AMSID("TRDGains",type),
				begin,end,sizeof(tube_gain[0])*size,
				(void*)tube_gain,server);
  
  tdvmap.insert(pair<string,AMSTimeID*>("TRDGains",ptdv));


  ptdv=new AMSTimeID(AMSID("TRDMCGains",type),
		     begin,end,sizeof(tube_gain[0])*size,
		     (void*)tube_gain,server);
  
  tdvmap.insert(pair<string,AMSTimeID*>("TRDMCGains",ptdv));


  //  AMSTimeID *ptdv;
  tdvmap.insert(pair<string,AMSTimeID*>("TRDStatus",ptdv=new AMSTimeID(AMSID("TRDStatus",type),
								       begin,end,sizeof(tube_status[0])*size,
								       (void*)tube_status,server)));
  
  if( ! readTDV(bgtime) ){
    cerr<<"Warning: readTDV inside InitTDV"<<endl;
    return false;
  }

  return true;
}

bool TrdHCalibR::InitSpecificTDV(string which,int size, float *arr,unsigned int bgtime, unsigned int edtime, int type,char * tempdirname){
  time_t begintime = (time_t) bgtime;
  time_t endtime   = (time_t) edtime;
  
  for(int i=0;i<size;i++)arr[i]=0;
  
  AMSTimeID::CType server=AMSTimeID::Standalone;
  tm begin = *gmtime(&begintime);
  tm end   = *gmtime(&endtime);

  if(strlen(tempdirname)){
    char tdname[200];
    sprintf(tdname, "%s/DataBase/", tempdirname);
    if(!AMSDBc::amsdatabase)
      AMSDBc::amsdatabase=new char[200];
    
    strcpy(AMSDBc::amsdatabase,tdname);
  }

  AMSTimeID *ptdv=new AMSTimeID(AMSID(which.c_str(),type),
				begin,end,sizeof(arr[0])*size,
				(void*)arr,server);
  
  tdvmap.insert(pair<string,AMSTimeID*>(which,ptdv));
  //  ptdv->validate(bg);
  
  if( ! readSpecificTDV(which,bgtime) ){
    cerr<<"Warning: readSpecificTDV inside InitTDV"<<endl;
    return false;
  }

  return true;
}

bool TrdHCalibR::InitSpecificTDV(string which,int size, unsigned int *arr,unsigned int bgtime, unsigned int edtime, int type,char * tempdirname){
  time_t begintime = (time_t) bgtime;
  time_t endtime   = (time_t) edtime;
  
  for(int i=0;i<size;i++)arr[i]=0;
  
  AMSTimeID::CType server=AMSTimeID::Standalone;
  tm begin = *gmtime(&begintime);
  tm end   = *gmtime(&endtime);

  //  if(strlen(tempdirname)){
    char tdname[200];
    sprintf(tdname, "%s/DataBase/", tempdirname);
    if(!AMSDBc::amsdatabase)
      AMSDBc::amsdatabase=new char[200];
    
    strcpy(AMSDBc::amsdatabase,tdname);
    //  }
  
  AMSTimeID *ptdv=new AMSTimeID(AMSID(which.c_str(),type),
				begin,end,sizeof(arr[0])*size,
				(void*)arr,server);

  tdvmap.insert(pair<string,AMSTimeID*>(which,ptdv));
  //  ptdv->validate(bg);
  
  if( ! readSpecificTDV(which,bgtime) ){
    cerr<<"Warning: readSpecificTDV inside InitTDV"<<endl;
    return false;
  }

  return true;
}

// write calibration to TDV
int TrdHCalibR::writeTDV(unsigned int bgtime, unsigned int edtime, int debug, char * tempdirname){
  if( int nchan=FillTDVFromMedian() ){
    printf("TrdHCalibR::writeTDV - Warning: contains %i low occupancy channls\n",nchan);
    //    return 2;
  }

  time_t begin,end,insert;
  if(debug)
    for(int i=0;i<5248;i++){
      if(i%500==0){
	int layer,ladder,tube;
	GetLLTFromTubeId(layer,ladder,tube,i);
	int ntdv=layer*18*16+ladder*16+tube;
	printf("TrdHCalibR::writeTDV - tube %4i occ %4i median %.2f - writing gain %.2f stat %u\n",i,tube_occupancy[i],tube_medians[i],tube_gain[ntdv],tube_status[ntdv]);
      }
    }
  
  for(map<string,AMSTimeID*>::const_iterator it=tdvmap.begin();it!=tdvmap.end();it++){
    it->second->UpdateMe()=1;
    unsigned int crcold=it->second->getCRC();
    it->second->UpdCRC();
    
    if(crcold!=it->second->getCRC()){
      // valid for 10 days
      //    it->gettime(insert,begin,end);
      time(&insert);
      begin=(time_t)bgtime;
      end=(time_t)edtime;
      it->second->SetTime(insert,begin,end);
      if(debug){
	cout <<" Write time:" << endl;
	cout <<" Time Insert "<<ctime(&insert);
	cout <<" Time Begin "<<ctime(&begin);
	cout <<" Time End "<<ctime(&end);
      }
      
      char tdname[200] = "";
      sprintf(tdname, "%s/DataBase/", tempdirname);
      
      if(!it->second->write(tdname)) {
	cerr <<"TrdHCalibR::writeTDV - problem to update tdv "<<it->first<<endl;
	return false;
      }

      if(!it->second->updatemap(tdname,true)){
	cerr <<"TrdHCalibR::writeTDV - problem to update map file for "<<it->first<<endl;
	return false;
      }
    }
  }

  return true;
};

int TrdHCalibR::writeSpecificTDV(string which,unsigned int intime,unsigned int bgtime, unsigned int edtime, int debug, char * tempdirname){
  if( int nchan=FillTDVFromMedian() ){
    printf("TrdHCalibR::writeTDV - Warning: contains %i low occupancy channls\n",nchan);
    //    return 2;
  }
  time_t begin,end,insert;
  for(map<string,AMSTimeID*>::const_iterator it=tdvmap.begin();it!=tdvmap.end();it++){
    if(it->first!=which)continue;
    printf("TrdHCalibR::writeSpecificTDV %s\n",which.c_str());
    
    it->second->UpdateMe()=1;
    unsigned int crcold=it->second->getCRC();
    it->second->UpdCRC();
    
    if(crcold!=it->second->getCRC()){
      // valid for 10 days
      //    it->gettime(insert,begin,end);
      if(!intime)intime=time(NULL);
      
      insert=(time_t)intime;
      begin=(time_t)bgtime;
      end=(time_t)edtime;
      it->second->SetTime(insert,begin,end);
      if(debug){
	cout <<" Write time:" << endl;
	cout <<" Time Insert "<<ctime(&insert);
	cout <<" Time Begin "<<ctime(&begin);
	cout <<" Time End "<<ctime(&end);
      }
      
      char tdname[200] = "";
      sprintf(tdname, "%s/DataBase/", tempdirname);
      
      if(!it->second->write(tdname)) {
	cerr <<"TrdHCalibR::writeTDV - problem to update tdvc"<<it->first<<endl;
	return false;
      }

      if(!it->second->updatemap(tdname,true)){
	cerr <<"TrdHCalibR::writeTDV - problem to update map file for "<<it->first<<endl;
	return false;
      }
    }
  }
  
  return true;
};


void TrdHCalibR::GetTubeIdFromLLT(int layer,int ladder,int tube,int &tubeid){
  if(layer>3)ladder+=14;
  if(layer>7)ladder+=16;
  if(layer>11)ladder+=16;
  if(layer>15)ladder+=18;
  
  tubeid=(ladder*4+layer%4)*16+tube;
}

void GetLLTfromTDV(int &layer, int &ladder, int &tube,int idsoft){
  tube=(idsoft-1)%16;
  ladder=((idsoft-1)/16)%18;
  layer=((idsoft-1)/16/18)%20;
}


void TrdHCalibR::GetLLTFromTubeId(int &layer,int &ladder,int &tube,int tubeid){
  tube=tubeid%16;
  int hvid=tubeid/64;
  layer=tubeid%64/16;
  ladder=hvid;
  
  if(hvid>13){layer+=4;ladder-=14;}
  if(hvid>29){layer+=4;ladder-=16;}
  if(hvid>45){layer+=4;ladder-=16;}
  if(hvid>63){layer+=4;ladder-=18;}
}

//float par[4]={6.37838e+01,5.26182e-01,8.96826e-02,5.50886e+01};;
float par[4]={3.85478e+01,5.26885e-01,8.08710e-02,2.42610e+01};
float path_pol3[4]={66.4444,-43.7317,10.9122,-0.575106};
float TrdHCalibR::PathParametrization(float path,int opt,int debug){
  if(opt==1)return par[0]/(1+exp((par[1]-path)/par[2]))+par[3];
  else{
    float val=0.;
    for(int i=0;i<4;i++)val+=path_pol3[i]*pow(path,i);
    return val;
  }
}

Double_t betapar[7]={1.94728e+01,-5.96794e+00,1.58489e-02,9.52168e-03,8.18354e+01,6.00944e+00,1.54202e+03};
Double_t betapar_pol4[5]={9161.75,-35936.6,53297.3,-35295.1,8828.5};
float TrdHCalibR::BetaParametrization(float beta,int opt,int debug){
  if(opt==1){
    Double_t x=beta/sqrt(1-beta*beta);//log10(betagamma
    return betapar[0]*pow((1+1/x/x),betapar[1])*(pow(fabs(log(betapar[2]*x*x)),betapar[3])+betapar[4]*pow(1+1/x/x,betapar[5])) - betapar[6];
  }
  else{
    float val=0.;
    for(int i=0;i<5;i++)val+=betapar_pol4[i]*pow(beta,i);
    return val;
  }
}

float TrdHCalibR::GetBetaCorr(double beta, double tobeta, int opt,int debug){
  double toReturn=0;
  if(fabs(beta)>0.95)toReturn=1.;
  else if (fabs(beta)<0.5)toReturn =0.;
  else toReturn =BetaParametrization(tobeta,opt)/BetaParametrization(fabs(beta),opt);

  if(debug)cout<<"TrdHCalibR::GetBetaCorr - beta "<<beta<<" betagamma "<<beta/sqrt(1-beta*beta)<<" dEdx "<< BetaParametrization(beta) <<" correction "<< toReturn <<endl;
  
  return toReturn;
}

float TrdHCalibR::GetPathCorr(float path, float topath,int opt,int debug){
  float toReturn=0.;
  if(path<0.3)toReturn=1.;
  else if(path<0.7)toReturn=PathParametrization(topath)/PathParametrization(path);
  else toReturn=0.;

  if(debug)cout<<"TrdHCalibR::GetPathCorr - path "<<path<<" correction "<<toReturn<<endl;
  return toReturn;
}


unsigned int TrdHCalibR::GetStatus(TrdRawHitR* hit,int opt, int debug){
  if(!hit)cerr<<"TrdHCalibR::GetStatus - hit not found"<<endl;
  return GetStatus(hit->Layer,hit->Ladder,hit->Tube,opt,debug);
}

unsigned int TrdHCalibR::GetStatus(int layer, int ladder, int tube,int opt,int debug){
  if(debug)cout<<"Entering TrdHCalibR::GetStatus"<<endl;
  
  int ntdv=layer*18*16+ladder*16+tube;
  if(debug)printf("hit %02i%02i%02i - tdvid %i\n",layer,ladder,tube,ntdv);
  
  // check in ROOT file
  if(opt<2){
    if(debug)cout<<"TDV from ROOT file"<<endl;
    AMSEventR::if_t value;
    value.u=0;
    int stat=AMSEventR::Head()->GetTDVEl("TRDStatus",ntdv,value);
    if(!stat){
      if(debug)cout<<"from tdvr" <<value.f<<endl;
      return value.f;
    }
  }
  
  // check in tdv
  if(opt!=1){
    map<string,AMSTimeID*>::iterator it ;
    it=tdvmap.find("TRDStatus");
    
    if(it!=tdvmap.end()){
      if(debug)cout<<"TDV from Database"<<endl;

      // check if ptdv contains useful values
      bool ok=true;

      if(ok){
	if(debug)cout<<"from DataBase " <<tube_status[ntdv]<<endl;
	return tube_status[ntdv];
      }
    }
  }
  return 1.;
}

float TrdHCalibR::GetGainCorr(TrdRawHitR* hit,int opt, int debug){
  if(!hit)cerr<<"TrdHCalibR::GetGainCorr - hit not found"<<endl;
  return GetGainCorr(hit->Layer,hit->Ladder,hit->Tube,opt,debug);
}

float TrdHCalibR::GetGainCorr(int layer, int ladder, int tube,int opt,int debug){
  if(debug)cout<<"Entering TrdHCalibR::GetGainCorr"<<endl;
  
  int ntdv=layer*18*16+ladder*16+tube;
  if(debug)printf("hit %02i%02i%02i - tdvid %i\n",layer,ladder,tube,ntdv);
  
  // check in ROOT file
  //  vector<float> values;
  if(opt<2){
    if(debug)cout<<"TDV from ROOT file"<<endl;
    AMSEventR::if_t value;
    value.u=0;
    int stat=AMSEventR::Head()->GetTDVEl("TRDGains",ntdv,value);
    if(!stat){
      if(debug)cout<<"from tdvr" <<1./value.f<<endl;
      return 1./value.f;
    }
  }
  
  
  // check in tdv
  if(opt!=1){
    map<string,AMSTimeID*>::iterator it ;
    it=tdvmap.find("TRDGains");


    if(it!=tdvmap.end()){
      if(debug)cout<<"TDV from Database"<<endl;
      // check if ptdv contains useful values
      bool ok=false;


      if(tube_gain[ntdv]>=0.2&&tube_gain[ntdv]<5)ok=true; 

      if(ok){
	if(debug)cout<<"from DataBase" <<1./tube_gain[ntdv]<<endl;
	return 1./tube_gain[ntdv];
      }
      /*      else{
	      float modsum=0.;
	      for(int i=0;i<16;i++){
	      GetTubeIdFromLLT(layer,ladder,i,ntdv);
	      modsum+=tube_gain[ntdv];
	      }
	      modsum/=16;
	      
	      if(debug)cout<<"return mean gain corection of module"<<endl;
	      return 1./modsum;
	      }*/
    }
  }
  return 1.;
}


