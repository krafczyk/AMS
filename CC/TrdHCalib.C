#include "TrdHCalib.h"
#include "root.h"
#include "timeid.h"
#include "amsdbc.h"
#include "TF1.h"

ClassImp(TrdHCalibR);
TrdHCalibR *TrdHCalibR::head=0;

void TrdHCalibR::update_medians(TrdHTrackR* track,int opt,float beta,float corr,int debug){
#pragma omp critical (trdmed)
  {
    int layer,ladder,tube;
    for(int seg=0;seg<2;seg++){
      if(!track->pTrdHSegment(seg)) continue;
      for(int i=0;i<(int)track->pTrdHSegment(seg)->fTrdRawHit.size();i++){
	TrdRawHitR* hit=track->pTrdHSegment(seg)->pTrdRawHit(i);
	if(!hit||hit->Amp<10)continue;
	
	int tubeid;
	GetTubeIdFromLLT(hit->Layer,hit->Ladder,hit->Tube,tubeid);
	float amp=hit->Amp*corr;
	if(opt>0)amp*=GetPathCorr(track->TubePath(hit->Layer,hit->Ladder,hit->Tube));
	// should be changed to rigidity to correct for relativistic rise
	if(opt>1)amp*=GetBetaCorr(beta);// approx mip beta (betagamma 3.04)
	
	if(amp<5.)continue;

	int mod=tubeid/16;
	int hv=mod/4;
	int gc=hv/2;
	int mf=GetManifold(gc);
	
	if(amp>det_median)det_median+=0.001;
	if(amp<det_median)det_median-=0.0025;
	
	float mf_amp=amp/det_median;
	if(mf_amp>mf_medians[mf])mf_medians[mf]+=1.e-5;
	if(mf_amp<mf_medians[mf])mf_medians[mf]-=2.5e-5;;
	
	float hv_amp=amp/det_median/mf_medians[mf]; 
	if(hv_amp>hv_medians[hv])hv_medians[hv]+=1.e-5;
	if(hv_amp<hv_medians[hv])hv_medians[hv]-=2.5e-5;;
	
	float mod_amp=amp/det_median/mf_medians[mf]/hv_medians[hv];
	if(mod_amp>mod_medians[mod])mod_medians[mod]+=1.e-5;
	if(mod_amp<mod_medians[mod])mod_medians[mod]-=2.5e-5;;
	
	float tube_amp=amp/det_median/mf_medians[mf]/hv_medians[hv]/mod_medians[mod];
	if(tube_amp>tube_medians[tubeid])tube_medians[tubeid]+=1.e-5;
	if(tube_amp<tube_medians[tubeid])tube_medians[tubeid]-=2.5e-5;;

	if(debug){
	  float current_median=det_median*mf_medians[mf]*hv_medians[hv]*mod_medians[mod]*tube_medians[tubeid];
	  printf("LLT %02i%02i%02i id %i amp %.2f -> med %.2f\n",hit->Layer,hit->Ladder,hit->Tube,tubeid,amp,current_median);
	  printf(" (DET %.2f MF %i %.2e HV %i %.2e MOD %i %.2e TUBE %i %.2e)\n",det_median,mf,mf_medians[mf],hv,hv_medians[hv],mod,mod_medians[mod],tubeid,tube_medians[tubeid]);
	}
	tube_occupancy[tubeid]++;
      }
    }
  }
  
}

void TrdHCalibR::init_calibration(float start_value){
  calibrate=true;

  det_median=start_value;
  for(int i=0;i<5248;i++){
    if(i<10)mf_medians[i]=1.;
    if(i<82)hv_medians[i]=1.;
    if(i<328)mod_medians[i]=1.;
    tube_occupancy[i]=0;
    tube_medians[i]=1.;
  }
}

float TrdHCalibR::MeanOccupancy(int opt){
  float sum=0.;
  for(int i=0;i<5248;i++)sum+=tube_occupancy[i];
  return sum/5248.;
}

float TrdHCalibR::MeanGaussMedian(int opt){
  TH1F h("h_gain","",100,0.,120.);
  for(int i=0;i<5248;i++){
    int mod=i/16;
    int hv=mod/4;
    int gc=hv/2;
    int mf=GetManifold(gc);
    h.Fill(tube_medians[i]*mod_medians[mod]*hv_medians[hv]*mf_medians[mf]*det_median);
  }

  TF1 f("f","gaus");
  h.Fit(&f,"Q0");
  return f.GetParameter(1);
}

float TrdHCalibR::MeanGaussGain(int opt){
  TH1F h("h_gain","",100,0.,3.);
  int layer,ladder,tube;

  for(int i=0;i<5248;i++){
    int mod=i/16;
    int hv=mod/4;
    int gc=hv/2;
    int mf=GetManifold(gc);
    h.Fill(tube_medians[i]*mod_medians[mod]*hv_medians[hv]*mf_medians[mf]*det_median/norm_mop);
  }
  
  TF1 f("f","gaus");
  h.Fit(&f,"Q0");
  
  // conversion factor to mean gain 1. stored here
  tube_gain[250]=f.GetParameter(1);
  
  // target ADC value of gain correction
  tube_gain[251]=norm_mop;

  return f.GetParameter(1);
}

int TrdHCalibR::FillMedianFromTDV(int debug){
  if(debug)cout<<"Enter TrdHCalibR::FillMedianFromTDV"<<endl;
  bool toReturn = true;
  int layer,ladder,tube;
  
  //  norm_mop=tube_gain[5788];

  // detector median stored in tdv array
  if(tube_gain[5789]>10.)
    det_median=tube_gain[5789];
  else
    det_median=norm_mop;
  
  if(debug)printf("DET %.2f (in TDV %.2f)\n",det_median,tube_gain[5789]);


  // manifold median stored in tdv array
  for(int i=0;i<10;i++){
    mf_medians[i]=tube_gain[5790+i];
    if(debug)printf("MF %i %.2f\n",i,mf_medians[i]);
  }

  // high voltage median stored in tdv array
  for(int i=0;i<82;i++){
    hv_medians[i]=tube_gain[5800+i];
    if(debug)printf("HV %i %.2f\n",i,hv_medians[i]);
  }  

  // module median NOT stored - needs to be calculated - first sum up tube amps of modules
  for(int i=0;i<328;i++)mod_medians[i]=0.;
  bool allone=true;
  for(int i=0;i<5248;i++){
    //    GetLLTFromTubeId(layer,ladder,tube,i);
    //    int ntdv=layer*18*16+ladder*16+tube;
    int ntdv=GetNTDV(i);
    int mod=i/16;
    mod_medians[mod]+=tube_gain[ntdv]*norm_mop;//sum up tube amplitudes
  }

  // divide mean module amplitude by HV 'median' to get module gain
  for(int i=0;i<328;i++){
    int hv=i/4;
    int mf=GetManifold(hv/2);
    float mean=mod_medians[i]/16.;
    float hv_amp=det_median*mf_medians[mf]*hv_medians[hv];
    mod_medians[i]=mean/hv_amp;// mean module tube amp / HV group amp
    if(debug)printf("MOD %i %.2f (mean %.2f HV %.2f)\n",i,mod_medians[i],mean,hv_amp);
  }
  
  // divide tube amplitude by module mean amplitude 
  for(int i=0;i<5248;i++){
    //    GetLLTFromTubeId(layer,ladder,tube,i);
    int ntdv=GetNTDV(i);//layer*18*16+ladder*16+tube;
    
    int mod=i/16;
    int hv=mod/4;
    int mf=GetManifold(hv/2);

    tube_medians[i]=tube_gain[ntdv]*norm_mop/(mod_medians[mod]*det_median*mf_medians[mf]*hv_medians[hv]); // single tube amp / mod amp
    if( tube_medians[i] < 0. )
      toReturn = false;
    
    if (tube_gain[ntdv]!=1.)allone=false;
  }
  
  if(debug)cout<<"Exit TrdHCalibR::FillMedianFromTDV"<<endl;
  return toReturn+2*allone;
}

int TrdHCalibR::FillTDVFromMedian(int debug){
  if(debug)cout<<"Enter TrdHCalibR::FillTDVFromMedian"<<endl;

  tube_gain[5788]=norm_mop;
  tube_gain[5789]=det_median;
  for(int i=0;i<10;i++)tube_gain[5790+i]=mf_medians[i];
  for(int i=0;i<82;i++)tube_gain[5800+i]=hv_medians[i];

  int layer,ladder,tube;
  float mean=0.;
  for(int i=0;i<5248;i++){
    //    GetLLTFromTubeId(layer,ladder,tube,i);
    //    int ntdv=layer*18*16+ladder*16+tube;
    int ntdv=GetNTDV(i);
    int mod=i/16;
    int hv=mod/4;
    int mf=GetManifold(hv/2);
    
    float current_median=det_median*mf_medians[mf]*hv_medians[hv]*mod_medians[mod]*tube_medians[i];

    if(debug)printf("DET %3.2f MF %i %.2e HV %i %.2e % MOD %i %.2e TUBE %i %.2e-> A %.2f\n",det_median,mf,mf_medians[mf],hv,hv_medians[hv],mod,mod_medians[mod],i,tube_medians[i],current_median);

    tube_gain[ntdv]=current_median/norm_mop;
    mean+=current_median;
  }
  
  if(debug)printf("detector mean %.2f",mean/5248.);
  if(debug)cout<<"Exit TrdHCalibR::FillTDVFromMedian"<<endl;
  return 0;
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

  if(ok&&FillMedianFromTDV()) ok=false;

  if(debug)
    for(int i=0;i<5248;i++){
      if(i%500==0){
	//	int layer,ladder,tube;
	//	GetLLTFromTubeId(layer,ladder,tube,i);
	//	int ntdv=layer*18*16+ladder*16+tube;
	int ntdv=GetNTDV(i);
	int mod=i/16;
	int hv=mod/4;
	int gc=hv/2;
	int mf=GetManifold(gc);
	printf("TrdHCalibR::readTDV - tube %4i reading gain %.2f stat %u - total tube median %.2f\n",i,tube_gain[ntdv],tube_status[ntdv],
	       det_median*mf_medians[mf]*hv_medians[hv]*mod_medians[mod]*tube_medians[i]);
	printf(" (DET %.2f MF %i %.2e HV %i %.2e MOD %i %.2e TUBE %i %.2e)\n",det_median,mf,mf_medians[mf],hv,hv_medians[hv],mod,mod_medians[mod],i,tube_medians[i]);

      }
    }
  
  return ok;
}

bool TrdHCalibR::readSpecificTDV(string which,unsigned int t, int debug){
  time_t thistime=(time_t)t;
  for(map<string,AMSTimeID*>::iterator it=tdvmap.begin();it!=tdvmap.end();it++)
    if(it->first==which){
      if(it->second->validate(thistime)){
	if(debug)cout<<"TDV "<<which<<" found and validated"<<endl;
	return true;
      }
      if(debug)cout<<"TDV "<<which<<" found but NOT validated"<<endl;
    }
  
  return false;
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


  /*  ptdv=new AMSTimeID(AMSID("TRDMCGains",type),
		     begin,end,sizeof(tube_mcgain[0])*size,
		     (void*)tube_mcgain,server);
  
		     tdvmap.insert(pair<string,AMSTimeID*>("TRDMCGains",ptdv));*/


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

  char tdname[200];
  sprintf(tdname, "%s/DataBase/", tempdirname);
  if(!AMSDBc::amsdatabase)
    AMSDBc::amsdatabase=new char[200];
  strcpy(AMSDBc::amsdatabase,tdname);
  
  AMSTimeID *ptdv=new AMSTimeID(AMSID(which.c_str(),type),
				begin,end,sizeof(arr[0])*size,
				(void*)arr,server);

  tdvmap.insert(pair<string,AMSTimeID*>(which,ptdv));
  
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
  }

  time_t begin,end,insert;
  if(debug)
    for(int i=0;i<5248;i++){
      if(i%500==0){
	//	int layer,ladder,tube;
	//	GetLLTFromTubeId(layer,ladder,tube,i);
	//	int ntdv=layer*18*16+ladder*16+tube;
	int ntdv=GetNTDV(i);
	printf("TrdHCalibR::writeTDV - tube %4i occ %4i median %.2f - writing gain %.2f stat %u\n",i,tube_occupancy[i],tube_medians[i],tube_gain[ntdv],tube_status[ntdv]);
      }
    }
  
  for(map<string,AMSTimeID*>::const_iterator it=tdvmap.begin();it!=tdvmap.end();it++){
    it->second->UpdateMe()=1;
    unsigned int crcold=it->second->getCRC();
    it->second->UpdCRC();
    
    if(crcold!=it->second->getCRC()){
      // valid for 10 days
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
    //    unsigned int crcold=it->second->getCRC();
    it->second->UpdCRC();
    
    //    if/(crcold!=it->second->getCRC()){
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
    //  }
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
    Double_t x=beta/sqrt(1-beta*beta);//log10(betagamma)
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
  if(path<0.3||path>0.7)toReturn=0.;
  else toReturn=PathParametrization(topath*10.,opt)/PathParametrization(path*10.,opt);
  //  else toReturn=0.6/path;

  if(debug)cout<<"TrdHCalibR::GetPathCorr - path "<<path<<" correction "<<toReturn<<endl;
  if(debug>1)cout<<"topath: "<<topath<<" val(path):"<<PathParametrization(path,opt)<<" val(topath):"<<PathParametrization(topath,opt)<<endl;
  return toReturn;
}


unsigned int TrdHCalibR::GetStatus(TrdRawHitR* hit,int opt, int debug){
  if(!hit)cerr<<"TrdHCalibR::GetStatus - hit not found"<<endl;
  return GetStatus(hit->Layer,hit->Ladder,hit->Tube,opt,debug);
}

unsigned int TrdHCalibR::GetStatus(int layer, int ladder, int tube,int opt,int debug){
  if(debug)cout<<"Entering TrdHCalibR::GetStatus"<<endl;
  
  //  int ntdv=layer*18*16+ladder*16+tube;
  int ntdv=GetNTDV(layer,ladder,tube);
  if(debug)printf("hit %02i%02i%02i - tdvid %i\n",layer,ladder,tube,ntdv);
  
  // check in ROOT file
  if(opt<2){
    if(debug)cout<<"TDV from ROOT file"<<endl;
    AMSEventR::if_t value;
    value.u=0;
    int stat=AMSEventR::Head()->GetTDVEl("TRDStatus",ntdv,value);
    if(!stat){
      if(debug)cout<<"from tdvr" <<value.f<<endl;
      return (unsigned int)value.f;
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
	return (unsigned int)tube_status[ntdv];
      }
    }
  }
  return 0;
}

float TrdHCalibR::GetGainCorr(TrdRawHitR* hit,int opt, int debug){
  if(!hit)cerr<<"TrdHCalibR::GetGainCorr - hit not found"<<endl;
  return GetGainCorr(hit->Layer,hit->Ladder,hit->Tube,opt,debug);
}

float TrdHCalibR::GetGainCorr(int layer, int ladder, int tube,int opt,int debug){
  if(debug)cout<<"Entering TrdHCalibR::GetGainCorr"<<endl;
  
  //  int ntdv=layer*18*16+ladder*16+tube;
  int ntdv=GetNTDV(layer,ladder,tube);
  if(debug)printf("hit %02i%02i%02i - tdvid %i\n",layer,ladder,tube,ntdv);

  // check in ROOT file
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

      int tubeid;
      GetTubeIdFromLLT(layer,ladder,tube,tubeid);
      int mod=tubeid/16;
      int hv=mod/4;
      int gc=hv/2;
      int mf=GetManifold(gc);
      
      float current_amp=det_median*mf_medians[mf]*hv_medians[hv]*mod_medians[mod]*tube_medians[tubeid];

      if(debug)printf("GetGainCorr LLT %02i%02i%02i DET %3.2f MF %i %.2e HV %i %.2e % MOD %i %.2e TUBE %i %.2e-> A %.2f\n",layer,ladder,tube,det_median,mf,mf_medians[mf],hv,hv_medians[hv],mod,mod_medians[mod],tubeid,tube_medians[tubeid],current_amp);

      return norm_mop/current_amp;//1./tube
      /*      if(tube_again[ntdv]>=0.2&&tube_gain[ntdv]<5){
	if(debug)cout<<"from DataBase" <<1./tube_gain[ntdv]<<endl;
	return 1./tube_gain[ntdv];
	}*/
    }
  }
  return 1.;
}


//inlets
static int mnf3_1[4]={13,15,28,38};
static int mnf3_2[4]={5 ,11,17,40};
static int mnf3_3[4]={3 ,9 ,19,30};
static int mnf3_4[4]={6 ,7 ,26,37};
static int mnf3_5[4]={4 ,21,24,39};

//outlets
static int mnf7_1[4]={2 ,10,25,29};
static int mnf7_2[4]={12,16,23,35};
static int mnf7_3[4]={8 ,22,27,32};
static int mnf7_4[4]={1 ,14,20,36};
static int mnf7_5[4]={0 ,18,31,34};

int TrdHCalibR::GetManifold(int gascirc){
  if(gascirc==33)
    return 7;
  else{
    for(int i=0;i<4;i++){
      if(gascirc==mnf3_1[i])return 0;
      if(gascirc==mnf3_2[i])return 1;
      if(gascirc==mnf3_3[i])return 2;
      if(gascirc==mnf3_4[i])return 3;
      if(gascirc==mnf3_5[i])return 4;
      if(gascirc==mnf7_1[i])return 5;
      if(gascirc==mnf7_2[i])return 6;
      if(gascirc==mnf7_3[i])return 7;
      if(gascirc==mnf7_4[i])return 8;
      if(gascirc==mnf7_5[i])return 9;
    }
  }
  cerr << "manifold for gascirc " << gascirc << "not found!" << endl;
  return -1;
}


// numbering 0-40
static int gc_leftbotarr [13]={7,11,12,14,15,17,18,20,22,32,35,40,-1};
static int gc_rightbotarr[13]={8, 9,10,13,16,19,21,33,34,36,37,38,39};
static int gc_lefttoparr [13]={0, 4, 5, 6,24,25,26,28,30,-1,-1,-1,-1};
static int gc_righttoparr[13]={1, 2, 3,23,27,29,31,-1,-1,-1,-1,-1,-1};

int TrdHCalibR::GetInletGeom(int gascirc){
  int inletgeom=-1;
  for(int i=0;i!=13;i++){
    if(gc_leftbotarr[i]==gascirc){
      inletgeom=0;
      break;
    }
    else if (gc_rightbotarr[i]==gascirc){
      inletgeom=1;
      break;
    }
    else if (gc_lefttoparr[i]==gascirc){
      inletgeom=2;
      break;
    }
    else if (gc_righttoparr[i]==gascirc){
      inletgeom=3;
      break;
    }
  }
  if(inletgeom==-1){
    cerr << "no inlet geometry for gascirc " << gascirc << endl; 
    return -1;
  }
  return inletgeom;
}

void TrdHCalibR::LLT2GCM(int layer, int ladder, int &gascirc_, int &gasmod){
  int templadder=ladder;
  if(layer>3)templadder+=14;
  if(layer>7)templadder+=16;
  if(layer>11)templadder+=16;
  if(layer>15)templadder+=18;
  
  if(layer>19)
    cerr << "layer 19+ found! layers should be 0-19 here" << endl;
  
  if(templadder%2==1) gascirc_=(templadder-1)/2;
  else gascirc_=templadder/2;
  
  int side=-1; // 0 left - 1 right
                                                 
  if(templadder%2==0)side=0;
  else side=1;

  if(side==0) gasmod=layer%4;
  else gasmod = 4+layer%4;

  int inletgeom = GetInletGeom(gascirc_);

  // modules numbering depending on gas inlet
  if(inletgeom==0){    // left bottom
    if(side)gasmod=7-layer%4;
    if(!side)gasmod=layer%4;
  }
  else if(inletgeom==1){    // right bottom
    if(side)gasmod=layer%4;
    if(!side)gasmod=7-layer%4;
  }
  else if(inletgeom==2){  // left top
    if(side)gasmod=4+layer%4;
    if(!side)gasmod=3-layer%4;
  }
  else if(inletgeom==3){  // right top
    if(side)gasmod=3-layer%4;
    if(!side)gasmod=4+layer%4;
  }
  else cerr << "ERROR ! Inlet for gascirc " << gascirc_ << "not found" << endl;

  if(gasmod==-1)cerr << "gasmodule not found!" << endl;
}

int TrdHCalibR::DynamicCalibration(AMSEventR *pev){
  if(pev->nLevel1()!=1)return 0;
  int n=0;
  for(int p=0;p<pev->nParticle();p++){
    if(!pev->pParticle(p)->pTrTrack())continue;
    
    int ntrlayer=0;
 #ifdef _PGTRACK_
    for(int i=1;i<8;i++)if(pev->pParticle(p)->pTrTrack()->GetHitLJ(i))ntrlayer++;	
#else
    for(int i=1;i<8;i++)if(pev->pParticle(p)->pTrTrack()->ToBitPattern(i))ntrlayer++;	
#endif
    if(ntrlayer<5)continue;

    if(!pev->pParticle(p)->pCharge())continue;
    if(pev->pParticle(p)->pCharge()->Charge()!=1)continue;
    
    if(pev->pParticle(p)->Beta<0.9)continue;
    
#ifdef _PGTRACK_
    double rig=pev->pParticle(p)->pTrTrack()->GetRigidity();
    double erig=pev->pParticle(p)->pTrTrack()->GetErrRinv();
#else
    double rig=pev->pParticle(p)->pTrTrack()->Rigidity;
    double erig=pev->pParticle(p)->pTrTrack()->ErrRigidity;
#endif
    
    //space
    if(pev->Run()>1305800000){
      if( rig < 2.0 || rig > 100.0 ||
	  fabs(erig * rig) > 0.1 )
	continue;
    }
    else{
      if(fabs(rig) < 2.0 ||
	 fabs(rig) > 100.0 ||
	 fabs(erig * rig) > 0.1 ) continue;
    }
    
    if(!pev->pParticle(p)->pTrdHTrack())continue;
    
    //    if(!MatchTrdToTracker(pev->pParticle(p)->pTrdHTrack(),pev->pParticle(p)->pTrTrack()))continue;
    {

#ifdef _PGTRACK_
      AMSPoint trpos;
      AMSDir trdir;
      pev->pParticle(p)->pTrTrack()->Interpolate(pev->pParticle(p)->pTrdHTrack()->Coo[2],trpos,trdir,1);
#else
      AMSPoint trpos(pev->pParticle(p)->pTrTrack()->HP0[0]);
      AMSDir trdir(pev->pParticle(p)->pTrTrack()->HTheta[0],pev->pParticle(p)->pTrTrack()->HPhi[0]);
#endif
      
      float dist=sqrt(pow(trpos.x()-pev->pParticle(p)->pTrdHTrack()->Coo[0],2)+
		      pow(trpos.y()-pev->pParticle(p)->pTrdHTrack()->Coo[1],2)+
		      pow(trpos.z()-pev->pParticle(p)->pTrdHTrack()->Coo[2],2));
      
      AMSDir trddir(pev->pParticle(p)->pTrdHTrack()->Dir[0],pev->pParticle(p)->pTrdHTrack()->Dir[1],pev->pParticle(p)->pTrdHTrack()->Dir[2]);

      float dangle=fabs(acos(trdir.prod(trddir))*180./3.1415926);
      if(dangle>90.)dangle=180.-dangle;

      //      printf("dist %.2f angle %.2f\n",dist,dangle);
      // reconstructed pos match to 1 cm
      if(dist>1.)continue;//return false;
      
      // reconstructed dir match to 5 degree
      if(dangle>5.)continue;//return false;
    }
    
    if(!TrdHReconR::gethead(0)->SelectTrack(pev->pParticle(p)->pTrdHTrack()))continue;

    if(!f_logmom){
      f_logmom=new TF1("logmom","pol4(0)*expo(4)");
      //      f_logmom->SetParameters(3.79375e+00,-1.65421e+00,8.94492e+00,-2.07662e+00,2.83535e+00,-1.45187e+00);
      f_logmom->SetParameters(1.38450e+01,-5.96499e+00,2.10405e+01,-1.81421e+00,1.59392e+00,-9.25626e-01);
    }

    double momcorr=1.;
    
    if(pev->fHeader.Time[0]>1305800000) // space - protons
      momcorr=f_logmom->Eval(0.515)/f_logmom->Eval(log10(fabs(rig)));
    
    //    else // cosmics - muons
    //      momcorr=f_logmom->Eval(0.515*0.93827/0.10566)/f_logmom->Eval(log10(fabs(rig*0.93827/0.10566)));

    update_medians(pev->pParticle(p)->pTrdHTrack(),1,1.,momcorr);
    n++;
  }
  
  return n;
}

int TrdHCalibR::GetNTDV(int tubeid){
  if(tubeid<0||tubeid>5247){
    cerr<<"TrdHCalibR::GetNTDV - undefined tube id "<<tubeid<<endl;
    return -1;
  }
  int layer,ladder,tube;
  GetLLTFromTubeId(layer,ladder,tube,tubeid);
  return GetNTDV(layer,ladder,tube);
}

int TrdHCalibR::GetNTDV(TrdRawHitR* hit){
  if(!hit){
    cerr<<"TrdHCalibR::GetNTDV - hit poiter not found "<<hit<<endl;
    return -1;
  }
  return GetNTDV(hit->Layer,hit->Ladder,hit->Tube);
}

int TrdHCalibR::GetNTDV(int layer, int ladder, int tube){
  if(layer<0||layer>19)
    cerr<<"TrdHCalibR::GetNTDV - undefined layer "<<layer<<endl;
  
  if(ladder<0||ladder>17)
    cerr<<"TrdHCalibR::GetNTDV - undefined ladder "<<ladder<<endl;
  
  if(tube<0||tube>15)
    cerr<<"TrdHCalibR::GetNTDV - undefined tube "<<tube<<endl;
  
  return 16*18*layer+16*ladder+tube;
}
