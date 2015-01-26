#include "TrdHCalib.h"
#include "root.h"
#include "timeid.h"
#include "amsdbc.h"
#include "TF1.h"
#include "TrPdf.h"

ClassImp(TrdHCalibR);
TrdHCalibR *TrdHCalibR::head=0;
float TrdHCalibR::tube_gain[6064];

void TrdHCalibR::update_medians(TrdHTrackR* track,int version,int opt,float beta,float corr,int debug){
	printf("TrdHCalibR::update_medians(TrdHTrackR* track,int version,int opt,float beta,float corr,int debug) - Begin\n");
#pragma omp critical (trdmed)
  {
    int tubeid;
    float amp;
    for(int seg=0;seg<2;seg++){
      if(!track->pTrdHSegment(seg)) continue;
      for(int i=0;i<(int)track->pTrdHSegment(seg)->fTrdRawHit.size();i++){
	TrdRawHitR* hit=track->pTrdHSegment(seg)->pTrdRawHit(i);
	if(!hit||hit->Amp<15)continue;
	GetTubeIdFromLLT(hit->Layer,hit->Ladder,hit->Tube,tubeid);
	amp=hit->Amp*corr;

	int mod=tubeid/16;
	int hv=mod/4;
	int gc=hv/2;
	int mf=GetManifold(gc);
	
	if(version==0){
	  if(opt>0)amp*=GetPathCorr(track->TubePath(hit->Layer,hit->Ladder,hit->Tube));
	  // should be changed to rigidity to correct for relativistic rise
	  if(opt>1)amp*=GetBetaCorr(beta);// approx mip beta (betagamma 3.04)
	}
	
	if(version==1){
	  // new path and betagamma corrections depend on energy deposition -> gain variation may hinder application here
	  
	  float current_median=det_median*mf_medians[mf]*hv_medians[hv]*mod_medians[mod]*tube_medians[tubeid];
	  float gaincorr=norm_mop/current_median;
	  amp*=gaincorr;
	  if(opt>0)amp*=GetPathCorr(track->TubePath(hit->Layer,hit->Ladder,hit->Tube)*10., amp, 3);
	  if(opt>1)amp*=GetBetaGammaCorr(beta,amp);//only protons
	}

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
	  cout<<"TrdHCalibR::update_medians-I-LLT "<<hit->Layer<<""<<hit->Ladder<<""<<hit->Tube<<" id "<<tubeid<<" amp "<<amp<<" -> median "<<current_median<<endl;
	  cout<<" DET "<<det_median<<" MF "<<mf<<" gain "<<mf_medians[mf]<<" HV "<<hv<<" gain "<< hv_medians[hv] <<" MOD "<<mod<<" gain "<<mod_medians[mod]<<" TUBE "<<tubeid<<" gain "<<tube_medians[tubeid]<<endl;
	}
	tube_occupancy[tubeid]++;
      }
    }

  }
  
}

void TrdHCalibR::init_calibration(float start_value){
	printf("TrdHCalibR::init_calibration(float start_value) - Begin\n");
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
	printf("TrdHCalibR::MeanOccupancy(int opt) - Begin\n");
  float sum=0.;
  for(int i=0;i<5248;i++)sum+=tube_occupancy[i];
  return sum/5248.;
}

float TrdHCalibR::MeanGaussMedian(int opt){
	printf("TrdHCalibR::MeanGaussMedian(int opt) - Begin\n");
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
	printf("TrdHCalibR::MeanGaussGain(int opt) - Begin\n");
  TH1F h("h_gain","",100,0.,3.);

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
	printf("TrdHCalibR::FillMedianFromTDV(int debug) - Begin\n");
  bool toReturn = true;
  
  norm_mop=tube_gain[5788];

  // detector median stored in tdv array
  if(tube_gain[5789]>10.)
    det_median=tube_gain[5789];
  else
    det_median=norm_mop;
  
  if(debug)cout<<"TrdHCalibR::FillMedianFromTDV-I-DET median "<<det_median<<" (in TDV "<<tube_gain[5789]<<")"<<endl;

  exit(-1);
  
  // manifold median stored in tdv array
  for(int i=0;i<10;i++){
    mf_medians[i]=tube_gain[5790+i];
    if(debug)cout<<" MF "<<i<<" "<<mf_medians[i]<<endl;
  }

  // high voltage median stored in tdv array
  for(int i=0;i<82;i++){
    hv_medians[i]=tube_gain[5800+i];
    if(debug)cout<<" HV "<<i<<" "<<hv_medians[i]<<endl;
  }  

  // module median NOT stored - needs to be calculated - first sum up tube amps of modules
  for(int i=0;i<328;i++)mod_medians[i]=0.;
  bool allone=true;
  for(int i=0;i<5248;i++){
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
    if(debug)
      cout<<" MOD "<<i<<" "<<mod_medians[i]<<" (mean "<<mean<<" HV "<<hv_amp<<")"<<endl;
  }
  
  // divide tube amplitude by module mean amplitude 
  for(int i=0;i<5248;i++){
    int ntdv=GetNTDV(i);
    
    int mod=i/16;
    int hv=mod/4;
    int mf=GetManifold(hv/2);

    tube_medians[i]=tube_gain[ntdv]*norm_mop/(mod_medians[mod]*det_median*mf_medians[mf]*hv_medians[hv]); // single tube amp / mod amp
    if( tube_medians[i] < 0. )
      toReturn = false;
    
    if(debug>1)cout<<"TrdHCalibR::FillMedianFromTDV-I-TUBE "<<i<<" median "<<tube_medians[i]<<endl;
    if (tube_gain[ntdv]!=1.)allone=false;
  }
  
  return toReturn+2*allone;
}

int TrdHCalibR::FillTDVFromMedian(int debug){
	printf("TrdHCalibR::FillTDVFromMedian(int debug) - Begin\n");
  tube_gain[5788]=norm_mop;
  tube_gain[5789]=det_median;
  for(int i=0;i<10;i++)tube_gain[5790+i]=mf_medians[i];
  for(int i=0;i<82;i++)tube_gain[5800+i]=hv_medians[i];

  float mean=0.;
  for(int i=0;i<5248;i++){
    int ntdv=GetNTDV(i);
    int mod=i/16;
    int hv=mod/4;
    int mf=GetManifold(hv/2);
    
    float current_median=det_median*mf_medians[mf]*hv_medians[hv]*mod_medians[mod]*tube_medians[i];

    if(debug)cout<<"TrdHCalibR::FillTDVFromMedian-I-DET "<<det_median<<" MF "<<mf<<" "<<mf_medians[mf]<<" HV "<<hv<<" "<<hv_medians[hv]<<" MOD "<<mod<<" "<<mod_medians[mod]<<" TUBE "<<i<<" "<<tube_medians[i]<<" -> A "<<current_median<<endl;
    
    tube_gain[ntdv]=current_median/norm_mop;
    mean+=current_median;
  }
  
  if(debug)cout<<"TrdHCalibR::FillTDVFromMedian-detector mean "<<mean/5248.<<endl;
  return 0;
}


bool TrdHCalibR::closeTDV(){
	printf("TrdHCalibR::closeTDV() - Begin\n");
  lastrun=0;
  for(map<string,AMSTimeID*>::iterator it=tdvmap.begin();it!=tdvmap.end();it++){
    delete it->second;
  }
  tdvmap.clear();
  return 1;
}

bool TrdHCalibR::readTDV(unsigned int t, int debug){
	printf("TrdHCalibR::readTDV(unsigned int t, int debug) - Begin\n");
  time_t thistime=(time_t)t;

  bool ok=true;
  for(map<string,AMSTimeID*>::iterator it=tdvmap.begin();it!=tdvmap.end();it++)
    if(!it->second->validate(thistime))ok=false;

  if(ok&&FillMedianFromTDV()) ok=false;

  if(debug)
    for(int i=0;i<5248;i++){
      if(i%500==0){
	int ntdv=GetNTDV(i);
	int mod=i/16;
	int hv=mod/4;
	int gc=hv/2;
	int mf=GetManifold(gc);
	cout<<"TrdHCalibR::readTDV-I-tube "<<i<<" reading gain "<<tube_gain[ntdv]<<" stat "<<tube_status[ntdv]<<" - total tube median "<<det_median*mf_medians[mf]*hv_medians[hv]*mod_medians[mod]*tube_medians[i]<<endl;
	cout<<" (DET "<<det_median<<" MF "<<mf<<" "<<mf_medians[mf]<<" HV "<<hv<<" "<<hv_medians[hv]<<" MOD "<<mod<<" "<<mod_medians[mod]<<" TUBE "<<i<<" "<<tube_medians[i]<<")"<<endl;

      }
    }
  
  return ok;
}

bool TrdHCalibR::readSpecificTDV(string which,unsigned int t, int debug){
	printf("TrdHCalibR::readSpecificTDV(string which,unsigned int t, int debug) - Begin\n");
  time_t thistime=(time_t)t;
  for(map<string,AMSTimeID*>::iterator it=tdvmap.begin();it!=tdvmap.end();it++)
    if(it->first==which){
      if(it->second->validate(thistime)){
	if(debug)cout<<"TrdHCalibR::readSpecificTDV-I-TDV "<<which<<" found and validated"<<endl;
	return true;
      }
      if(debug)cerr<<"TrdHCalibR::readSpecificTDV-W-TDV "<<which<<" found but NOT validated"<<endl;
    }
  
  return false;
}

bool TrdHCalibR::InitTDV(unsigned int bgtime, unsigned int edtime, int type,const char* tempdirname){
	printf("TrdHCalibR::InitTDV(unsigned int bgtime, unsigned int edtime, int type,const char* tempdirname) - Begin\n");
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
    cerr<<"TrdHCalibR::InitTDV-W-readTDV not successful"<<endl;
    return false;
  }

  return true;
}

bool TrdHCalibR::InitSpecificTDV(string which,int size, float *arr,unsigned int bgtime, unsigned int edtime, int type,const char * tempdirname){
	printf("TrdHCalibR::InitSpecificTDV(string which,int size, float *arr,unsigned int bgtime, unsigned int edtime, int type,const char * tempdirname) - Begin\n");
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
    cerr<<"TrdHCalibR::InitSpecificTDV-W-readSpecificTDV not successfulB"<<endl;
    return false;
  }

  return true;
}

bool TrdHCalibR::InitSpecificTDV(string which,int size, unsigned int *arr,unsigned int bgtime, unsigned int edtime, int type,const char * tempdirname){
	printf("TrdHCalibR::InitSpecificTDV(string which,int size, unsigned int *arr,unsigned int bgtime, unsigned int edtime, int type,const char * tempdirname) - Begin\n");
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
    cerr<<"TrdHCalibR::InitSpecificTDV-W-readSpecificTDV not successful"<<endl;
    return false;
  }

  return true;
}

// write calibration to TDV
int TrdHCalibR::writeTDV(unsigned int bgtime, unsigned int edtime, int debug, const char * tempdirname){
	printf("TrdHCalibR::writeTDV(unsigned int bgtime, unsigned int edtime, int debug, const char * tempdirname) - Begin\n");
  if( int nchan=FillTDVFromMedian() ){
    cerr<<"TrdHCalibR::writeTDV-W-Number of low occupancy channls "<<nchan<<endl;
  }
  
  time_t begin,end,insert;
  if(debug)
    for(int i=0;i<5248;i++){
      if(i%500==0){
	int ntdv=GetNTDV(i);
	cout<<"TrdHCalibR::writeTDV-I-tube "<<i<<" occ "<<tube_occupancy[i]<<" median "<<tube_medians[i]<<" - writing gain "<<tube_gain[ntdv]<<" status "<<tube_status[ntdv]<<endl;
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
	cout <<"TrdHCalibR::writeTDV-I-Write time:" << endl;
	cout <<"TrdHCalibR::writeTDV-I-Time Insert "<<ctime(&insert);
	cout <<"TrdHCalibR::writeTDV-I-Time Begin "<<ctime(&begin);
	cout <<"TrdHCalibR::writeTDV-I-Time End "<<ctime(&end);
      }
      
      char tdname[200] = "";
      sprintf(tdname, "%s/DataBase/", tempdirname);
      
      if(!it->second->write(tdname)) {
	cerr <<"TrdHCalibR::writeTDV-W-can not update tdv "<<it->first<<endl;
	return false;
      }

      if(!it->second->updatemap(tdname,true)){
	cerr <<"TrdHCalibR::writeTDV-W-can not update map file for "<<it->first<<endl;
	return false;
      }
    }
  }

  return true;
};

int TrdHCalibR::writeSpecificTDV(string which,unsigned int intime,unsigned int bgtime, unsigned int edtime, int debug, const char * tempdirname){
	printf("TrdHCalibR::writeSpecificTDV(string which,unsigned int intime,unsigned int bgtime, unsigned int edtime, int debug, const char * tempdirname) - Begin\n");
  if( int nchan=FillTDVFromMedian() ){
    cerr<<"TrdHCalibR::writeTDV-W-Number of low occupancy channls"<<nchan<<endl;
    //    return 2;
  }
  time_t begin,end,insert;
  for(map<string,AMSTimeID*>::const_iterator it=tdvmap.begin();it!=tdvmap.end();it++){
    if(it->first!=which)continue;
    
    it->second->UpdateMe()=1;
    it->second->UpdCRC();
    
    if(!intime)intime=time(NULL);
    
    insert=(time_t)intime;
    begin=(time_t)bgtime;
    end=(time_t)edtime;
    it->second->SetTime(insert,begin,end);
    if(debug){
      cout <<"TrdHCalibR::writeTDV-I-Write time:" << endl;
      cout <<"TrdHCalibR::writeTDV-I-Time Insert "<<ctime(&insert);
      cout <<"TrdHCalibR::writeTDV-I-Time Begin "<<ctime(&begin);
      cout <<"TrdHCalibR::writeTDV-I-Time End "<<ctime(&end);
    }
    
    char tdname[200] = "";
    sprintf(tdname, "%s/DataBase/", tempdirname);
    
    if(!it->second->write(tdname)) {
      cerr <<"TrdHCalibR::writeTDV-W-can not update tdvc"<<it->first<<endl;
      return false;
    }
    
    if(!it->second->updatemap(tdname,true)){
      cerr <<"TrdHCalibR::writeTDV-W-can not update map file for "<<it->first<<endl;
      return false;
    }
  }
  
  return true;
};


void TrdHCalibR::GetTubeIdFromLLT(int layer,int ladder,int tube,int &tubeid){
	printf("TrdHCalibR::GetTubeIdFromLLT(int layer,int ladder,int tube,int &tubeid) - Begin\n");
  if(layer>3)ladder+=14;
  if(layer>7)ladder+=16;
  if(layer>11)ladder+=16;
  if(layer>15)ladder+=18;
  
  tubeid=(ladder*4+layer%4)*16+tube;
}

void GetLLTfromTDV(int &layer, int &ladder, int &tube,int idsoft){
	printf("void GetLLTfromTDV(int &layer, int &ladder, int &tube,int idsoft) - Begin\n");
  tube=(idsoft-1)%16;
  ladder=((idsoft-1)/16)%18;
  layer=((idsoft-1)/16/18)%20;
}


void TrdHCalibR::GetLLTFromTubeId(int &layer,int &ladder,int &tube,int tubeid){
	printf("TrdHCalibR::GetLLTFromTubeId(int &layer,int &ladder,int &tube,int tubeid) - Begin\n");
  tube=tubeid%16;
  int hvid=tubeid/64;
  layer=tubeid%64/16;
  ladder=hvid;
  
  if(hvid>13){layer+=4;ladder-=14;}
  if(hvid>29){layer+=4;ladder-=16;}
  if(hvid>45){layer+=4;ladder-=16;}
  if(hvid>63){layer+=4;ladder-=18;}
}

const float par[4]={3.85478e+01,5.26885e-01,8.08710e-02,2.42610e+01};
const float path_pol3[4]={66.4444,-43.7317,10.9122,-0.575106};

float TrdHCalibR::PathParametrization(float path,int opt,int debug){
	printf("TrdHCalibR::PathParametrization(float path,int opt,int debug) - Begin\n");
  if(opt==1)return par[0]/(1+exp((par[1]-path)/par[2]))+par[3];
  else{
    float val=0.;
    for(int i=0;i<4;i++)val+=path_pol3[i]*pow(path*10.,i);
    return val;
  }
}

const double betapar[7]={1.94728e+01,-5.96794e+00,1.58489e-02,9.52168e-03,8.18354e+01,6.00944e+00,1.54202e+03};
const double betapar_pol4[5]={9161.75,-35936.6,53297.3,-35295.1,8828.5};
float TrdHCalibR::BetaParametrization(float beta,int opt,int debug){
	printf("TrdHCalibR::BetaParametrization(float beta,int opt,int debug) - Begin\n");
  if(opt==1){
    double x=beta/sqrt(1-beta*beta);//log10(betagamma)
    return betapar[0]*pow((1+1/x/x),betapar[1])*(pow(fabs(log(betapar[2]*x*x)),betapar[3])+betapar[4]*pow(1+1/x/x,betapar[5])) - betapar[6];
  }
  else{
    float val=0.;
    for(int i=0;i<5;i++)val+=betapar_pol4[i]*pow(beta,i);
    return val;
  }
}

float TrdHCalibR::GetBetaCorr(double beta, double tobeta, int opt,int debug){
	printf("TrdHCalibR::GetBetaCorr(double beta, double tobeta, int opt,int debug) - Begin\n");
  double toReturn=0;
  if(fabs(beta)>0.95)toReturn=1.;
  else if (fabs(beta)<0.5)toReturn =0.;
  else toReturn =BetaParametrization(tobeta,opt)/BetaParametrization(fabs(beta),opt);

  if(debug)cout<<"TrdHCalibR::GetBetaCorr-I-beta "<<beta<<" betagamma "<<beta/sqrt(1-beta*beta)<<" dEdx "<< BetaParametrization(beta) <<" correction "<< toReturn <<endl;
  
  return toReturn;
}

float TrdHCalibR::GetBetaGammaCorr(double betagamma,double dE, int opt,int debug){
	printf("TrdHCalibR::GetBetaGammaCorr(double betagamma,double dE, int opt,int debug) - Begin\n");
  double toReturn=pow(bgcorr(log10(betagamma),dE),0.9);//remove slight overcorrection
  if(debug)cout<<"TrdHCalibR::GetBetaGammaCorr-I-bg "<<betagamma<<" dE "<< dE <<" correction "<< toReturn <<endl;
  return toReturn;
}

float TrdHCalibR::GetPathCorr(float path, float val,int opt,int debug){
	printf("TrdHCalibR::GetPathCorr(float path, float val,int opt,int debug) - Begin\n");
  if(opt==3)
    return pathcorr(path,val);

  float toReturn=0.;
  if(path<0.1||path>0.7)toReturn=0.;
  else toReturn=PathParametrization(val,opt)/PathParametrization(path,opt);

  if(debug)cout<<"TrdHCalibR::GetPathCorr-I-path "<<path<<" correction "<<toReturn<<endl;
  if(debug>1)cout<<" topath: "<<val<<" val(path):"<<PathParametrization(path,opt)<<" val(topath):"<<PathParametrization(val,opt)<<endl;
  return toReturn;
}


unsigned int TrdHCalibR::GetStatus(TrdRawHitR* hit,int opt, int debug){
	printf("TrdHCalibR::GetStatus(TrdRawHitR* hit,int opt, int debug) - Begin\n");
  if(!hit)cerr<<"TrdHCalibR::GetStatus-W-hit not found"<<endl;
  return GetStatus(hit->Layer,hit->Ladder,hit->Tube,opt,debug);
}

unsigned int TrdHCalibR::GetStatus(int layer, int ladder, int tube,int opt,int debug){
	printf("TrdHCalibR::GetStatus(int layer, int ladder, int tube,int opt,int debug) - Begin\n");
  //  int ntdv=layer*18*16+ladder*16+tube;
  int ntdv=GetNTDV(layer,ladder,tube);
  if(debug)cout<<"TrdHCalibR::GetStatus-I-hit "<<layer<<""<<ladder<<""<<tube<<" - tdvid "<<ntdv<<endl;
  
  // check in ROOT file
  if(opt<2){
    if(debug)cout<<"TrdHCalibR::GetStatus-I-Read tdv from ROOT file"<<endl;
    AMSEventR::if_t value;
    value.u=0;
    int stat=AMSEventR::Head()->GetTDVEl("TRDStatus",ntdv,value);
    if(!stat){
      if(debug)cout<<" value read: " <<value.f<<endl;
      return (unsigned int)value.f;
    }
  }
  
  // check in tdv
  if(opt!=1){
    map<string,AMSTimeID*>::iterator it ;
    it=tdvmap.find("TRDStatus");
    if(it!=tdvmap.end()){
      if(debug)cout<<"TrdHCalibR::GetStatus-I-Read tdv from DATABASE"<<endl;
      
      // check if ptdv contains useful values
      bool ok=true;
      if(ok){
	if(debug)cout<<" value read: " <<tube_status[ntdv]<<endl;
	return (unsigned int)tube_status[ntdv];
      }
    }
  }
  return 0;
}

float TrdHCalibR::GetGainCorr(TrdRawHitR* hit,int opt, int debug){
	printf("TrdHCalibR::GetGainCorr(TrdRawHitR* hit,int opt, int debug) - Begin\n");
  if(!hit)cerr<<"TrdHCalibR::GetGainCorr-W-hit not found"<<endl;
  return GetGainCorr(hit->Layer,hit->Ladder,hit->Tube,opt,debug);
}

float TrdHCalibR::GetGainCorr(int layer, int ladder, int tube,int opt,int debug){
	printf("TrdHCalibR::GetGainCorr(int layer, int ladder, int tube,int opt,int debug) - Begin\n");
  int ntdv=GetNTDV(layer,ladder,tube);
  int tubeid;
  GetTubeIdFromLLT(layer,ladder,tube,tubeid);
  if(debug)cout<<"TrdHCalibR::GetGainCorr-I-hit "<<layer<<""<<ladder<<""<<tube<<" - tdvid "<<ntdv<<endl;

  float gaincorr=1.;
  
  // check in ROOT file
  if(opt<2){
    if(debug)cout<<"TrdHCalibR::GetGainCorr-I-Read tdv from ROOT file"<<endl;
    AMSEventR::if_t value;
    value.u=0;
    int stat=AMSEventR::Head()->GetTDVEl("TRDGains2",ntdv,value);
    if(!stat){
      if(debug)cout<<" value read: " <<1./value.f<<endl;
      gaincorr=1./value.f;
    }
    stat=AMSEventR::Head()->GetTDVEl("TRDGains",ntdv,value);
    if(!stat){
      if(debug)cout<<" value read: " <<1./value.f<<endl;
      gaincorr=1./value.f;
    } 
  }
  
  // check in tdv
  if(opt!=1){
    map<string,AMSTimeID*>::iterator it ;
    it=tdvmap.find("TRDGains2");
    if(it!=tdvmap.end()){
      if(debug)cout<<"TrdHCalibR::GetGaincorr-I-Read tdv from DATABASE"<<endl;

      int mod=tubeid/16;
      int hv=mod/4;
      int gc=hv/2;
      int mf=GetManifold(gc);
      
      float current_amp=det_median*mf_medians[mf]*hv_medians[hv]*mod_medians[mod]*tube_medians[tubeid];
      
      if(debug)cout<<" DET "<<det_median<<" MF "<<mf<<" "<<mf_medians[mf]<<" HV "<<hv<<" "<<hv_medians[hv]<<" MOD "<<mod<<" "<<mod_medians[mod]<<" TUBE "<<tubeid<<" "<<tube_medians[tubeid]<<" -> A "<<current_amp<<endl;

      gaincorr=norm_mop/current_amp;//1./tube
      gaincorr*=additional_modcorr(tubeid/16);
    }
    else{
      it=tdvmap.find("TRDGains");
      if(it!=tdvmap.end()){
	if(debug)cout<<"TrdHCalibR::GetGaincorr-I-Read tdv from DATABASE"<<endl;
	
	int tubeid;
	GetTubeIdFromLLT(layer,ladder,tube,tubeid);
	int mod=tubeid/16;
	int hv=mod/4;
	int gc=hv/2;
	int mf=GetManifold(gc);
	
	float current_amp=det_median*mf_medians[mf]*hv_medians[hv]*mod_medians[mod]*tube_medians[tubeid];
	
	if(debug)cout<<" DET "<<det_median<<" MF "<<mf<<" "<<mf_medians[mf]<<" HV "<<hv<<" "<<hv_medians[hv]<<" MOD "<<mod<<" "<<mod_medians[mod]<<" TUBE "<<tubeid<<" "<<tube_medians[tubeid]<<" -> A "<<current_amp<<endl;
	
	gaincorr=norm_mop/current_amp;
      }
    }
  }
  
  return gaincorr;
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
	printf("TrdHCalibR::GetManifold(int gascirc) - Begin\n");
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
  cerr << "TrdHCalibR::GetManifold-E-manifold for gascirc " << gascirc << "not found!" << endl;
  return -1;
}


// numbering 0-40
static int gc_leftbotarr [13]={7,11,12,14,15,17,18,20,22,32,35,40,-1};
static int gc_rightbotarr[13]={8, 9,10,13,16,19,21,33,34,36,37,38,39};
static int gc_lefttoparr [13]={0, 4, 5, 6,24,25,26,28,30,-1,-1,-1,-1};
static int gc_righttoparr[13]={1, 2, 3,23,27,29,31,-1,-1,-1,-1,-1,-1};

int TrdHCalibR::GetInletGeom(int gascirc){
	printf("TrdHCalibR::GetInletGeom(int gascirc) - Begin\n");
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
    cerr << "TrdHCalibR::GetInletGeom-E-no inlet geometry for gascirc " << gascirc << endl; 
    return -1;
  }
  return inletgeom;
}

void TrdHCalibR::LLT2GCM(int layer, int ladder, int &gascirc_, int &gasmod){
	printf("TrdHCalibR::LLT2GCM(int layer, int ladder, int &gascirc_, int &gasmod) - Begin\n");
  int templadder=ladder;
  if(layer>3)templadder+=14;
  if(layer>7)templadder+=16;
  if(layer>11)templadder+=16;
  if(layer>15)templadder+=18;
  
  if(layer>19)
    cerr << "TrdHCalibR::LLT2GCM-F-layer 19+ found! layers defined as 0-19 here" << endl;
  
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
  else cerr << "TrdHCalibR::LLT2GCM-E-inlet for gascirc " << gascirc_ << "not found" << endl;

  if(gasmod==-1)cerr << "TrdHCalibR::LLT2GCM-E-gas module not found!" << endl;
}

int TrdHCalibR::DynamicCalibration(AMSEventR *pev, int opt){
	printf("TrdHCalibR::DynamicCalibration(AMSEventR *pev, int opt) - Begin\n");

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

      // reconstructed pos match to 1 cm
      if(dist>1.)continue;//return false;
      
      // reconstructed dir match to 5 degree
      if(dangle>5.)continue;//return false;
    }
    
    if(!TrdHReconR::gethead(0)->SelectTrack(pev->pParticle(p)->pTrdHTrack()))continue;

    if(!f_logmom){
      f_logmom=new TF1("logmom","pol4(0)*expo(4)");
      f_logmom->SetParameters(1.38450e+01,-5.96499e+00,2.10405e+01,-1.81421e+00,1.59392e+00,-9.25626e-01);
    }

    float momcorr=1.;
    if(pev->fHeader.Time[0]>1305800000) // space - protons
      momcorr=f_logmom->Eval(0.515)/f_logmom->Eval(log10(fabs(rig)));
    
    //    else // cosmics - muons
    //      momcorr=f_logmom->Eval(0.515*0.93827/0.10566)/f_logmom->Eval(log10(fabs(rig*0.93827/0.10566)));

    update_medians(pev->pParticle(p)->pTrdHTrack(),0,1,1,momcorr);
    n++;
  }
  
  return n;
}

int TrdHCalibR::GetNTDV(int tubeid){
	printf("TrdHCalibR::GetNTDV(int tubeid) - Begin\n");
  if(tubeid<0||tubeid>5247){
    cerr<<"TrdHCalibR::GetNTDV-E-undefined tube id "<<tubeid<<endl;
    return -1;
  }
  int layer,ladder,tube;
  GetLLTFromTubeId(layer,ladder,tube,tubeid);
  return GetNTDV(layer,ladder,tube);
}

int TrdHCalibR::GetNTDV(TrdRawHitR* hit){
	printf("TrdHCalibR::GetNTDV(TrdRawHitR* hit) - Begin\n");
  if(!hit){
    cerr<<"TrdHCalibR::GetNTDV-E-hit poiter not found "<<hit<<endl;
    return -1;
  }
  return GetNTDV(hit->Layer,hit->Ladder,hit->Tube);
}

int TrdHCalibR::GetNTDV(int layer, int ladder, int tube){
	printf("TrdHCalibR::GetNTDV(int layer, int ladder, int tube) - Begin\n");
  if(layer<0||layer>19)
    cerr<<"TrdHCalibR::GetNTDV-F-undefined layer "<<layer<<endl;
  
  if(ladder<0||ladder>17)
    cerr<<"TrdHCalibR::GetNTDV-F-undefined ladder "<<ladder<<endl;
  
  if(tube<0||tube>15)
    cerr<<"TrdHCalibR::GetNTDV-F-undefined tube "<<tube<<endl;
  
  return 16*18*layer+16*ladder+tube;
}

double TrdHCalibR::GraphXmax(TGraph *g){
	printf("TrdHCalibR::GraphXmax(TGraph *g) - Begin\n");
  double xmax=-1.e6;
  for(int i=0;i<g->GetN();i++)
    if(g->GetX()[i]>xmax)xmax=g->GetX()[i];
  return xmax;
}
double TrdHCalibR::bgval(float x,float dE){
	printf("TrdHCalibR::bgval(float x,float dE) - Begin\n");
  //  if(x>s_bg_p0->GetXmax())x=s_bg_p0->GetXmax();
  //  if(x>GraphXmax(s_bg_p0))x=GraphXmax(s_bg_p0);
  return s_bg_p0->Eval(x)+s_bg_p1->Eval(x)*dE+s_bg_p2->Eval(x)*dE*dE;
}
double TrdHCalibR::bgcorr(float x,float dE){
	printf("TrdHCalibR::bgcorr(float x,float dE) - Begin\n");
  return bgval(x,dE)/dE;
  //  return dE/bgval(x,dE);
}

TF1 *TrdHCalibR::bgfunc(float x){
	printf("TrdHCalibR::bgfunc(float x) - Begin\n");
  TF1 *f=new TF1("f","val([0],x)");
  f->SetParameter(1,x);
  return f;
}

int TrdHCalibR::bgmap(const char *fname){
	printf("TrdHCalibR::bgmap(const char *fname) - Begin\n");
  FILE *file;
  file=fopen(fname,"r");
  if(!file)return 1;
  float x,p0,p1,p2;

  double xarr[1000];
  double p0arr[1000];
  double p1arr[1000];
  double p2arr[1000];
  float chi2;
  int n=0,ndf;

  while( !feof (file) ){
    fscanf(file, "%f %f %f %f %f %i\n",&x,&p0,&p1,&p2,&chi2,&ndf);
    //    printf("%f %f %f %f\n",x,p0,p1,p2);

    xarr[n]=x;
    p0arr[n]=p0;
    p1arr[n]=p1;
    p2arr[n]=p2;
    n++;
  }
  fclose(file);
  /*  cout<<"FILE read"<<endl;
      for(int i=0;i<n;i++){
      printf("%.2e %.2e %.2e %.2e\n",xarr[i],p0arr[i],p1arr[i],p2arr[i]);
      }*/

  /*  s_bg_p0=new TSpline3("",xarr,p0arr,n);
      s_bg_p1=new TSpline3("",xarr,p1arr,n);
      s_bg_p2=new TSpline3("",xarr,p2arr,n);*/

  if(s_bg_p0) delete s_bg_p0;
  if(s_bg_p1) delete s_bg_p1;
  if(s_bg_p2) delete s_bg_p2;

  s_bg_p0=new TGraph(n,xarr,p0arr);
  s_bg_p1=new TGraph(n,xarr,p1arr);
  s_bg_p2=new TGraph(n,xarr,p2arr);

  return 0;
}

int TrdHCalibR::bgmap(int opt){
	printf("TrdHCalibR::bgmap(int opt) - Begin\n");
  double xarr[27];
  double p0arr[27];
  double p1arr[27];
  double p2arr[27];
  int n=27;

  xarr[0]=0.050; p0arr[0]=-7.65e-02; p1arr[0]=6.93e-01; p2arr[0]=1.92e-04; 
  xarr[1]=0.150; p0arr[1]=-8.81e-01; p1arr[1]=7.88e-01; p2arr[1]=-3.36e-04;
  xarr[2]=0.250; p0arr[2]=2.55e-01 ; p1arr[2]=9.03e-01; p2arr[2]=-8.40e-05;
  xarr[3]=0.350; p0arr[3]=-2.10e-01; p1arr[3]=9.69e-01; p2arr[3]=-4.02e-05;
  xarr[4]=0.450; p0arr[4]=1.11e-01 ; p1arr[4]=9.92e-01; p2arr[4]=-2.84e-05;
  xarr[5]=0.550; p0arr[5]=-2.24e-10; p1arr[5]=1.00e+00; p2arr[5]=-1.29e-16;
  xarr[6]=0.650; p0arr[6]=-1.42e-01; p1arr[6]=1.00e+00; p2arr[6]=-3.54e-06;
  xarr[7]=0.750; p0arr[7]=8.05e-02 ; p1arr[7]=9.71e-01; p2arr[7]=1.26e-04 ;
  xarr[8]=0.850; p0arr[8]=-2.60e-01; p1arr[8]=9.53e-01; p2arr[8]=1.53e-04 ;
  xarr[9]=0.950; p0arr[9]=-1.98e-01; p1arr[9]=9.16e-01; p2arr[9]=2.76e-04 ;
  xarr[10]=1.050; p0arr[10]=-3.85e-01; p1arr[10]=8.87e-01; p2arr[10]=3.38e-04; 
  xarr[11]=1.150; p0arr[11]=-6.62e-01; p1arr[11]=8.55e-01; p2arr[11]=4.44e-04 ;
  xarr[12]=1.250; p0arr[12]=-6.72e-01; p1arr[12]=8.25e-01; p2arr[12]=4.97e-04 ;
  xarr[13]=1.350; p0arr[13]=-9.29e-01; p1arr[13]=7.99e-01; p2arr[13]=5.48e-04 ;
  xarr[14]=1.450; p0arr[14]=-8.43e-01; p1arr[14]=7.71e-01; p2arr[14]=5.96e-04 ;
  xarr[15]=1.550; p0arr[15]=-6.80e-01; p1arr[15]=7.37e-01; p2arr[15]=6.64e-04 ;
  xarr[16]=1.650; p0arr[16]=-1.25e+00; p1arr[16]=7.28e-01; p2arr[16]=6.79e-04 ;
  xarr[17]=1.750; p0arr[17]=-6.09e-01; p1arr[17]=6.84e-01; p2arr[17]=7.81e-04 ;
  xarr[18]=1.850; p0arr[18]=-8.13e-01; p1arr[18]=6.71e-01; p2arr[18]=8.07e-04 ;
  xarr[19]=1.950; p0arr[19]=-5.28e-01; p1arr[19]=6.46e-01; p2arr[19]=8.20e-04 ;
  xarr[20]=2.050; p0arr[20]=-5.70e-01; p1arr[20]=6.40e-01; p2arr[20]=8.06e-04 ;
  xarr[21]=2.150; p0arr[21]=-6.13e-01; p1arr[21]=6.20e-01; p2arr[21]=8.13e-04 ;
  xarr[22]=2.250; p0arr[22]=-5.57e-01; p1arr[22]=6.16e-01; p2arr[22]=7.52e-04 ;
  xarr[23]=2.350; p0arr[23]=-9.64e-01; p1arr[23]=6.02e-01; p2arr[23]=6.88e-04 ;
  xarr[24]=2.450; p0arr[24]=-1.90e+00; p1arr[24]=6.38e-01; p2arr[24]=3.73e-04 ;
  xarr[25]=2.550; p0arr[25]=2.28e-02 ; p1arr[25]=5.81e-01; p2arr[25]=5.93e-04 ;
  xarr[26]=2.650; p0arr[26]=5.45e-01 ; p1arr[26]=5.83e-01; p2arr[26]=6.75e-04 ;


  if(s_bg_p0) delete s_bg_p0;
  if(s_bg_p1) delete s_bg_p1;
  if(s_bg_p2) delete s_bg_p2;

  s_bg_p0=new TGraph(n,xarr,p0arr);
  s_bg_p1=new TGraph(n,xarr,p1arr);
  s_bg_p2=new TGraph(n,xarr,p2arr);

  return 0;
}

double TrdHCalibR::pathval(float x,float dE){
	printf("TrdHCalibR::pathval(float x,float dE) - Begin\n");
  //  if(x>GraphXmax(s_path_p0))x=GraphXmax(s_path_p0);
  return s_path_p0->Eval(x)+s_path_p1->Eval(x)*dE+s_path_p2->Eval(x)*dE*dE+s_path_p3->Eval(x)*sqrt(dE);
}

double TrdHCalibR::pathcorr(float x,float dE){
	printf("TrdHCalibR::pathcorr\n");
  return pathval(x,dE)/dE;
}

TF1 *TrdHCalibR::pathfunc(float x){
	printf("TrdHCalibR::pathfunc\n");
  TF1 *f=new TF1("f","[0]+[1]*x+[2]*x*x+[3]*TMath::Sqrt(x)");
  f->SetParameters(s_path_p0->Eval(x),s_path_p1->Eval(x),s_path_p2->Eval(x),s_path_p3->Eval(x));
  return f;
}

int TrdHCalibR::pathmap(const char *fname){
	printf("TrdHCalibR::pathmap(const char *fname) - Begin\n");
  FILE *file;
  file=fopen(fname,"r");
  if(!file)return 1;
  float x,p0,p1,p2,p3;

  double xarr[1000];
  double p0arr[1000];
  double p1arr[1000];
  double p2arr[1000];
  double p3arr[1000];
  float chi2;
  int n=0,ndf;

  while( !feof (file) ){
    fscanf(file, "%f %f %f %f %f %f %i\n",&x,&p0,&p1,&p2,&p3,&chi2,&ndf);
    //    printf("%f %f %f %f %f\n",x,p0,p1,p2,p3);

    xarr[n]=x;
    p0arr[n]=p0;
    p1arr[n]=p1;
    p2arr[n]=p2;
    p3arr[n]=p3;
    n++;
  }
  fclose(file);
  //  cout<<"FILE read"<<endl;
  //  for(int i=0;i<n;i++){
  //    printf("%.2e %.2e %.2e %.2e %.2e\n",xarr[i],p0arr[i],p1arr[i],p2arr[i],p3arr[i]);
  //  }
  
  /*  s_path_p0=new TSpline3("",xarr,p0arr,n);
      s_path_p1=new TSpline3("",xarr,p1arr,n);
      s_path_p2=new TSpline3("",xarr,p2arr,n);
      s_path_p3=new TSpline3("",xarr,p3arr,n);*/

  if(s_path_p0) delete s_path_p0;
  if(s_path_p1) delete s_path_p1;
  if(s_path_p2) delete s_path_p2;
  if(s_path_p3) delete s_path_p3;

  s_path_p0=new TGraph(n,xarr,p0arr);
  s_path_p1=new TGraph(n,xarr,p1arr);
  s_path_p2=new TGraph(n,xarr,p2arr);
  s_path_p3=new TGraph(n,xarr,p3arr);

  return 0;
}

int TrdHCalibR::pathmap(int opt){
	printf("TrdHCalibR::pathmap\n");

  double xarr[69];
  double p0arr[69];
  double p1arr[69];
  double p2arr[69];
  double p3arr[69];
  int n=69;
  xarr[0]=0.350; p0arr[0]=-3.03e+01; p1arr[0]=-1.91e-01; p2arr[0]= 6.80e-04; p3arr[0]= 1.71e+01;
  xarr[1]=0.450; p0arr[1]=-2.64e+01; p1arr[1]=-7.21e-02; p2arr[1]= 5.80e-04; p3arr[1]= 1.64e+01;
  xarr[2]=0.550; p0arr[2]=-3.71e+01; p1arr[2]=-2.93e-01; p2arr[2]= 7.12e-04; p3arr[2]= 1.95e+01;
  xarr[3]=0.650; p0arr[3]=-2.61e+01; p1arr[3]=-1.59e-01; p2arr[3]= 6.66e-04; p3arr[3]= 1.64e+01;
  xarr[4]=0.750; p0arr[4]=-2.62e+01; p1arr[4]=-8.51e-02; p2arr[4]= 5.89e-04; p3arr[4]= 1.65e+01;
  xarr[5]=0.850; p0arr[5]=-2.13e+01; p1arr[5]=2.60e-02 ; p2arr[5]= 5.25e-04; p3arr[5]= 1.49e+01;
  xarr[6]=0.950; p0arr[6]=-2.14e+01; p1arr[6]=9.56e-02 ; p2arr[6]= 4.48e-04; p3arr[6]= 1.51e+01;
  xarr[7]=1.050; p0arr[7]=-1.95e+01; p1arr[7]=1.08e-01 ; p2arr[7]= 4.60e-04; p3arr[7]= 1.43e+01;
  xarr[8]=1.150; p0arr[8]=-2.10e+01; p1arr[8]=7.61e-02 ; p2arr[8]= 4.86e-04; p3arr[8]= 1.45e+01;
  xarr[9]=1.250; p0arr[9]=-2.38e+01; p1arr[9]=1.37e-02 ; p2arr[9]= 5.20e-04; p3arr[9]= 1.55e+01;
  xarr[10]=1.350; p0arr[10]=-2.70e+01; p1arr[10]=-5.45e-02; p2arr[10]= 5.57e-04; p3arr[10]= 1.66e+01;
  xarr[11]=1.450; p0arr[11]=-2.20e+01; p1arr[11]=1.01e-01 ; p2arr[11]= 4.55e-04; p3arr[11]= 1.47e+01;
  xarr[12]=1.550; p0arr[12]=-2.30e+01; p1arr[12]=1.14e-01 ; p2arr[12]= 4.42e-04; p3arr[12]= 1.47e+01;
  xarr[13]=1.650; p0arr[13]=-2.16e+01; p1arr[13]=1.42e-01 ; p2arr[13]= 4.25e-04; p3arr[13]= 1.44e+01;
  xarr[14]=1.750; p0arr[14]=-2.37e+01; p1arr[14]=1.12e-01 ; p2arr[14]= 4.40e-04; p3arr[14]= 1.49e+01;
  xarr[15]=1.850; p0arr[15]=-2.22e+01; p1arr[15]=1.87e-01 ; p2arr[15]= 3.86e-04; p3arr[15]= 1.42e+01;
  xarr[16]=1.950; p0arr[16]=-2.05e+01; p1arr[16]=2.22e-01 ; p2arr[16]= 3.66e-04; p3arr[16]= 1.37e+01;
  xarr[17]=2.050; p0arr[17]=-2.13e+01; p1arr[17]=2.20e-01 ; p2arr[17]= 3.61e-04; p3arr[17]= 1.39e+01;
  xarr[18]=2.150; p0arr[18]=-1.81e+01; p1arr[18]=3.26e-01 ; p2arr[18]= 2.91e-04; p3arr[18]= 1.27e+01;
  xarr[19]=2.250; p0arr[19]=-1.89e+01; p1arr[19]=3.13e-01 ; p2arr[19]= 3.00e-04; p3arr[19]= 1.28e+01;
  xarr[20]=2.350; p0arr[20]=-2.09e+01; p1arr[20]=2.78e-01 ; p2arr[20]= 3.19e-04; p3arr[20]= 1.34e+01;
  xarr[21]=2.450; p0arr[21]=-1.81e+01; p1arr[21]=3.73e-01 ; p2arr[21]= 2.55e-04; p3arr[21]= 1.23e+01;
  xarr[22]=2.550; p0arr[22]=-1.80e+01; p1arr[22]=4.13e-01 ; p2arr[22]= 2.21e-04; p3arr[22]= 1.21e+01;
  xarr[23]=2.650; p0arr[23]=-1.74e+01; p1arr[23]=4.36e-01 ; p2arr[23]= 2.04e-04; p3arr[23]= 1.19e+01;
  xarr[24]=2.750; p0arr[24]=-1.83e+01; p1arr[24]=4.28e-01 ; p2arr[24]= 2.10e-04; p3arr[24]= 1.20e+01;
  xarr[25]=2.850; p0arr[25]=-1.70e+01; p1arr[25]=4.68e-01 ; p2arr[25]= 1.85e-04; p3arr[25]= 1.15e+01;
  xarr[26]=2.950; p0arr[26]=-1.68e+01; p1arr[26]=5.04e-01 ; p2arr[26]= 1.58e-04; p3arr[26]= 1.12e+01;
  xarr[27]=3.050; p0arr[27]=-1.66e+01; p1arr[27]=5.18e-01 ; p2arr[27]= 1.50e-04; p3arr[27]= 1.10e+01;
  xarr[28]=3.150; p0arr[28]=-1.55e+01; p1arr[28]=5.66e-01 ; p2arr[28]= 1.16e-04; p3arr[28]= 1.05e+01;
  xarr[29]=3.250; p0arr[29]=-1.41e+01; p1arr[29]=6.16e-01 ; p2arr[29]= 8.50e-05; p3arr[29]= 9.89e+00;
  xarr[30]=3.350; p0arr[30]=-1.48e+01; p1arr[30]=6.18e-01 ; p2arr[30]= 8.20e-05; p3arr[30]= 9.93e+00;
  xarr[31]=3.450; p0arr[31]=-1.22e+01; p1arr[31]=6.87e-01 ; p2arr[31]= 4.02e-05; p3arr[31]= 8.99e+00;
  xarr[32]=3.550; p0arr[32]=-1.11e+01; p1arr[32]=7.21e-01 ; p2arr[32]= 1.91e-05; p3arr[32]= 8.55e+00;
  xarr[33]=3.650; p0arr[33]=-1.18e+01; p1arr[33]=7.51e-01 ; p2arr[33]=-3.21e-06; p3arr[33]= 8.34e+00;
  xarr[34]=3.750; p0arr[34]=-1.14e+01; p1arr[34]=7.72e-01 ; p2arr[34]=-1.49e-05; p3arr[34]= 8.04e+00;
  xarr[35]=3.850; p0arr[35]=-9.25e+00; p1arr[35]=8.16e-01 ; p2arr[35]=-3.83e-05; p3arr[35]= 7.31e+00;
  xarr[36]=3.950; p0arr[36]=-8.14e+00; p1arr[36]=8.56e-01 ; p2arr[36]=-6.09e-05; p3arr[36]= 6.72e+00;
  xarr[37]=4.050; p0arr[37]=-7.98e+00; p1arr[37]=8.72e-01 ; p2arr[37]=-6.79e-05; p3arr[37]= 6.44e+00;
  xarr[38]=4.150; p0arr[38]=-7.56e+00; p1arr[38]=8.89e-01 ; p2arr[38]=-7.56e-05; p3arr[38]= 6.13e+00;
  xarr[39]=4.250; p0arr[39]=-7.11e+00; p1arr[39]=9.01e-01 ; p2arr[39]=-7.69e-05; p3arr[39]= 5.79e+00;
  xarr[40]=4.350; p0arr[40]=-6.49e+00; p1arr[40]=9.20e-01 ; p2arr[40]=-8.47e-05; p3arr[40]= 5.41e+00;
  xarr[41]=4.450; p0arr[41]=-5.70e+00; p1arr[41]=9.46e-01 ; p2arr[41]=-9.66e-05; p3arr[41]= 4.93e+00;
  xarr[42]=4.550; p0arr[42]=-4.19e+00; p1arr[42]=9.78e-01 ; p2arr[42]=-1.10e-04; p3arr[42]= 4.28e+00;
  xarr[43]=4.650; p0arr[43]=-3.96e+00; p1arr[43]=9.83e-01 ; p2arr[43]=-1.07e-04; p3arr[43]= 4.03e+00;
  xarr[44]=4.750; p0arr[44]=-3.42e+00; p1arr[44]=9.94e-01 ; p2arr[44]=-1.07e-04; p3arr[44]= 3.65e+00;
  xarr[45]=4.850; p0arr[45]=-3.18e+00; p1arr[45]=9.98e-01 ; p2arr[45]=-1.02e-04; p3arr[45]= 3.37e+00;
  xarr[46]=4.950; p0arr[46]=-1.44e+00; p1arr[46]=1.03e+00 ; p2arr[46]=-1.15e-04; p3arr[46]= 2.66e+00;
  xarr[47]=5.050; p0arr[47]=-1.94e+00; p1arr[47]=1.02e+00 ; p2arr[47]=-9.98e-05; p3arr[47]= 2.57e+00;
  xarr[48]=5.150; p0arr[48]=-1.27e+00; p1arr[48]=1.02e+00 ; p2arr[48]=-9.07e-05; p3arr[48]= 2.21e+00;
  xarr[49]=5.250; p0arr[49]=-2.15e+00; p1arr[49]=1.01e+00 ; p2arr[49]=-7.73e-05; p3arr[49]= 2.20e+00;
  xarr[50]=5.350; p0arr[50]=-2.63e+00; p1arr[50]=9.99e-01 ; p2arr[50]=-6.28e-05; p3arr[50]= 2.08e+00;
  xarr[51]=5.450; p0arr[51]=-2.47e+00; p1arr[51]=9.97e-01 ; p2arr[51]=-5.34e-05; p3arr[51]= 1.86e+00;
  xarr[52]=5.550; p0arr[52]=-2.48e+00; p1arr[52]=9.89e-01 ; p2arr[52]=-3.99e-05; p3arr[52]= 1.68e+00;
  xarr[53]=5.650; p0arr[53]=-2.88e+00; p1arr[53]=9.80e-01 ; p2arr[53]=-2.76e-05; p3arr[53]= 1.58e+00;
  xarr[54]=5.750; p0arr[54]=-3.31e+00; p1arr[54]=9.77e-01 ; p2arr[54]=-1.83e-05; p3arr[54]= 1.41e+00;
  xarr[55]=5.850; p0arr[55]=-3.70e+00; p1arr[55]=9.73e-01 ; p2arr[55]=-9.13e-06; p3arr[55]= 1.27e+00;
  //  xarr[56]=5.950; p0arr[56]=-1.65e+00; p1arr[56]=1.02e+00 ; p2arr[56]=-3.35e-05; p3arr[56]= 4.54e-01;
  //  xarr[57]=6.050; p0arr[57]= 1.92e+00; p1arr[57]=1.07e+00 ; p2arr[57]=-6.07e-05; p3arr[57]= -4.17e-0;
  /*  xarr[58]=6.150; p0arr[58]=-9.10e-02; p1arr[58]=1.01e+00 ; p2arr[58]=-1.53e-05; p3arr[58]= 6.73e-02;
  xarr[59]=6.350; p0arr[59]= 9.88e-02; p1arr[59]=9.90e-01 ; p2arr[59]=1.34e-05 ; p3arr[59]=-1.06e-01;
  xarr[60]=6.450; p0arr[60]= 7.93e-01; p1arr[60]=9.82e-01 ; p2arr[60]=2.65e-05 ; p3arr[60]=-2.82e-01;
  xarr[61]=6.550; p0arr[61]= 6.79e-01; p1arr[61]=9.71e-01 ; p2arr[61]=3.91e-05 ; p3arr[61]=-3.40e-01;
  xarr[62]=6.650; p0arr[62]=-2.48e-02; p1arr[62]=9.38e-01 ; p2arr[62]=6.59e-05 ; p3arr[62]=-1.06e-01;
  xarr[63]=6.750; p0arr[63]=-1.26e+00; p1arr[63]=9.13e-01 ; p2arr[63]=8.52e-05 ; p3arr[63]=1.05e-01 ;
  xarr[64]=6.850; p0arr[64]=-8.75e-01; p1arr[64]=9.03e-01 ; p2arr[64]=9.83e-05 ; p3arr[64]=-1.07e-02;
  xarr[65]=6.950; p0arr[65]= 4.11e-01; p1arr[65]=9.18e-01 ; p2arr[65]=9.53e-05 ; p3arr[65]=-4.20e-01;
  xarr[66]=7.050; p0arr[66]=-2.30e+00; p1arr[66]=8.59e-01 ; p2arr[66]=1.34e-04 ; p3arr[66]=2.81e-01 ;
  xarr[67]=7.150; p0arr[67]=-4.66e-01; p1arr[67]=8.74e-01 ; p2arr[67]=1.33e-04 ; p3arr[67]=-1.94e-01;
  xarr[68]=7.250; p0arr[68]= 3.98e+00; p1arr[68]=9.42e-01 ; p2arr[68]=9.94e-05 ; p3arr[68]=-1.41e+00;
  xarr[69]=7.350; p0arr[69]= 6.09e+00; p1arr[69]=9.60e-01 ; p2arr[69]=9.47e-05 ; p3arr[69]=-1.90e+00;
  xarr[70]=7.450; p0arr[70]=-1.41e+00; p1arr[70]=8.20e-01 ; p2arr[70]=1.90e-04 ; p3arr[70]=8.20e-02 ;*/

  xarr[56]=6.150; p0arr[56]=-9.10e-02; p1arr[56]=1.01e+00 ; p2arr[56]=-1.53e-05; p3arr[56]= 6.73e-02;
  xarr[57]=6.350; p0arr[57]= 9.88e-02; p1arr[57]=9.90e-01 ; p2arr[57]=1.34e-05 ; p3arr[57]=-1.06e-01;
  xarr[58]=6.450; p0arr[58]= 7.93e-01; p1arr[58]=9.82e-01 ; p2arr[58]=2.65e-05 ; p3arr[58]=-2.82e-01;
  xarr[59]=6.550; p0arr[59]= 6.79e-01; p1arr[59]=9.71e-01 ; p2arr[59]=3.91e-05 ; p3arr[59]=-3.40e-01;
  xarr[60]=6.650; p0arr[60]=-2.48e-02; p1arr[60]=9.38e-01 ; p2arr[60]=6.59e-05 ; p3arr[60]=-1.06e-01;
  xarr[61]=6.750; p0arr[61]=-1.26e+00; p1arr[61]=9.13e-01 ; p2arr[61]=8.52e-05 ; p3arr[61]=1.05e-01 ;
  xarr[62]=6.850; p0arr[62]=-8.75e-01; p1arr[62]=9.03e-01 ; p2arr[62]=9.83e-05 ; p3arr[62]=-1.07e-02;
  xarr[63]=6.950; p0arr[63]= 4.11e-01; p1arr[63]=9.18e-01 ; p2arr[63]=9.53e-05 ; p3arr[63]=-4.20e-01;
  xarr[64]=7.050; p0arr[64]=-2.30e+00; p1arr[64]=8.59e-01 ; p2arr[64]=1.34e-04 ; p3arr[64]=2.81e-01 ;
  xarr[65]=7.150; p0arr[65]=-4.66e-01; p1arr[65]=8.74e-01 ; p2arr[65]=1.33e-04 ; p3arr[65]=-1.94e-01;
  xarr[66]=7.250; p0arr[66]= 3.98e+00; p1arr[66]=9.42e-01 ; p2arr[66]=9.94e-05 ; p3arr[66]=-1.41e+00;
  xarr[67]=7.350; p0arr[67]= 6.09e+00; p1arr[67]=9.60e-01 ; p2arr[67]=9.47e-05 ; p3arr[67]=-1.90e+00;
  xarr[68]=7.450; p0arr[68]=-1.41e+00; p1arr[68]=8.20e-01 ; p2arr[68]=1.90e-04 ; p3arr[68]=8.20e-02 ;


  if(s_path_p0) delete s_path_p0;
  if(s_path_p1) delete s_path_p1;
  if(s_path_p2) delete s_path_p2;
  if(s_path_p3) delete s_path_p3;

  s_path_p0=new TGraph(n,xarr,p0arr);
  s_path_p1=new TGraph(n,xarr,p1arr);
  s_path_p2=new TGraph(n,xarr,p2arr);
  s_path_p3=new TGraph(n,xarr,p3arr);

  return 0;
}

int TrdHCalibR::Initialize(AMSEventR* pev,const char *databasedir){
  printf("TrdHCalibR::Initialize\n");
  if(lastrun!=pev->Run()){
    int bgntime=pev->Run();
    int endtime=pev->Run()+10;
    
    if(lastrun>0)closeTDV();
    else{
      pathmap(0);
      bgmap(0);
      

      if(!g_bgcorr_helium){
	g_bgcorr_helium=new TGraphErrors(26);
	g_bgcorr_helium->SetPoint(0,-0.05,380.363*1.15);
	g_bgcorr_helium->SetPointError(0,0,0.826046);
	g_bgcorr_helium->SetPoint(1,0.05,341.989*1.12);
	g_bgcorr_helium->SetPointError(1,0,0.524841);
	g_bgcorr_helium->SetPoint(2,0.15,317.76*1.08);
	g_bgcorr_helium->SetPointError(2,0,0.371851);
	g_bgcorr_helium->SetPoint(3,0.25,302.793*1.05);
	g_bgcorr_helium->SetPointError(3,0,0.315932);
	g_bgcorr_helium->SetPoint(4,0.35,295.399*1.03);
	g_bgcorr_helium->SetPointError(4,0,0.289967);
	g_bgcorr_helium->SetPoint(5,0.45,293.53*1.01);
	g_bgcorr_helium->SetPointError(5,0,0.284084);
	g_bgcorr_helium->SetPoint(6,0.55,295.874);
	g_bgcorr_helium->SetPointError(6,0,0.309261);
	g_bgcorr_helium->SetPoint(7,0.65,300.981);
	g_bgcorr_helium->SetPointError(7,0,0.323394);
	g_bgcorr_helium->SetPoint(8,0.75,307.611);
	g_bgcorr_helium->SetPointError(8,0,0.35119);
	g_bgcorr_helium->SetPoint(9,0.85,315.101);
	g_bgcorr_helium->SetPointError(9,0,0.401949);
	g_bgcorr_helium->SetPoint(10,0.95,323.863);
	g_bgcorr_helium->SetPointError(10,0,0.460864);
	g_bgcorr_helium->SetPoint(11,1.05,332.992);
	g_bgcorr_helium->SetPointError(11,0,0.547908);
	g_bgcorr_helium->SetPoint(12,1.15,341.718);
	g_bgcorr_helium->SetPointError(12,0,0.692394);
	g_bgcorr_helium->SetPoint(13,1.25,350.35);
	g_bgcorr_helium->SetPointError(13,0,0.871716);
	g_bgcorr_helium->SetPoint(14,1.35,358.972);
	g_bgcorr_helium->SetPointError(14,0,1.04386);
	g_bgcorr_helium->SetPoint(15,1.45,365.956);
	g_bgcorr_helium->SetPointError(15,0,1.34903);
	g_bgcorr_helium->SetPoint(16,1.55,373.813);
	g_bgcorr_helium->SetPointError(16,0,1.58405);
	g_bgcorr_helium->SetPoint(17,1.65,380.094);
	g_bgcorr_helium->SetPointError(17,0,2.22431);
	g_bgcorr_helium->SetPoint(18,1.75,387.097);
	g_bgcorr_helium->SetPointError(18,0,2.3447);
	g_bgcorr_helium->SetPoint(19,1.85,391.597);
	g_bgcorr_helium->SetPointError(19,0,2.95786);
	g_bgcorr_helium->SetPoint(20,1.95,394.019);
	g_bgcorr_helium->SetPointError(20,0,3.68512);
	g_bgcorr_helium->SetPoint(21,2.05,399.439);
	g_bgcorr_helium->SetPointError(21,0,4.56432);
	g_bgcorr_helium->SetPoint(22,2.15,395.735);
	g_bgcorr_helium->SetPointError(22,0,6.74708);
	g_bgcorr_helium->SetPoint(23,2.25,405.567);
	g_bgcorr_helium->SetPointError(23,0,7.97385);
	g_bgcorr_helium->SetPoint(24,2.35,388.705);
	g_bgcorr_helium->SetPointError(24,0,21.7004);
	g_bgcorr_helium->SetPoint(25,2.45,676.592);
	g_bgcorr_helium->SetPointError(25,0,2675.14);
      }  
    }
    
    for(int i=0;i<5248;i++)tube_occupancy[i]=0;
    InitSpecificTDV("TRDGains2",6064,tube_gain,bgntime,endtime,int(pev->nMCEventg()==0),databasedir);
    readSpecificTDV("TRDGains2",bgntime+1,1);
    FillMedianFromTDV();
    
    lastrun=pev->Run();
    return 1;
  }
  return 0;
}

float TrdHCalibR::additional_modcorr(int mod){
	printf("TrdHCalibR::additional_modcorr - Begin\n");
  if(mod<0||mod>327)return 0.;
  TGraphErrors *gre=g_additional_modcorr;
  if(!g_additional_modcorr){
    g_additional_modcorr= new TGraphErrors();
    gre=g_additional_modcorr;
    gre->SetName("g_mpv_hmod");
    gre->SetTitle("");
    gre->SetFillColor(1);
    gre->SetMarkerStyle(3);
    gre->SetPoint(0,0.5,60.8568);
    gre->SetPointError(0,0,11.2873);
    gre->SetPoint(1,1.5,49.3244);
    gre->SetPointError(1,0,3.81188);
    gre->SetPoint(2,2.5,50.0514);
    gre->SetPointError(2,0,3.44057);
    gre->SetPoint(3,3.5,48.0844);
    gre->SetPointError(3,0,1.99661);
    gre->SetPoint(4,4.5,49.5058);
    gre->SetPointError(4,0,1.75864);
    gre->SetPoint(5,5.5,49.9744);
    gre->SetPointError(5,0,1.48736);
    gre->SetPoint(6,6.5,49.8246);
    gre->SetPointError(6,0,1.28166);
    gre->SetPoint(7,7.5,50.5362);
    gre->SetPointError(7,0,1.17001);
    gre->SetPoint(8,8.5,49.8661);
    gre->SetPointError(8,0,1.05081);
    gre->SetPoint(9,9.5,50.9412);
    gre->SetPointError(9,0,0.945513);
    gre->SetPoint(10,10.5,49.8823);
    gre->SetPointError(10,0,1.06177);
    gre->SetPoint(11,11.5,50.7334);
    gre->SetPointError(11,0,1.03639);
    gre->SetPoint(12,12.5,50.3803);
    gre->SetPointError(12,0,0.904599);
    gre->SetPoint(13,13.5,50.4085);
    gre->SetPointError(13,0,0.871923);
    gre->SetPoint(14,14.5,50.2956);
    gre->SetPointError(14,0,0.9357);
    gre->SetPoint(15,15.5,51.3411);
    gre->SetPointError(15,0,0.951372);
    gre->SetPoint(16,16.5,50.6437);
    gre->SetPointError(16,0,0.901455);
    gre->SetPoint(17,17.5,50.0911);
    gre->SetPointError(17,0,0.916853);
    gre->SetPoint(18,18.5,49.6991);
    gre->SetPointError(18,0,0.861073);
    gre->SetPoint(19,19.5,50.4965);
    gre->SetPointError(19,0,0.926749);
    gre->SetPoint(20,20.5,50.499);
    gre->SetPointError(20,0,0.837972);
    gre->SetPoint(21,21.5,50.3064);
    gre->SetPointError(21,0,0.902362);
    gre->SetPoint(22,22.5,50.4479);
    gre->SetPointError(22,0,0.768648);
    gre->SetPoint(23,23.5,51.0831);
    gre->SetPointError(23,0,0.8514);
    gre->SetPoint(24,24.5,50.2853);
    gre->SetPointError(24,0,0.737703);
    gre->SetPoint(25,25.5,50.4139);
    gre->SetPointError(25,0,0.838468);
    gre->SetPoint(26,26.5,50.5);
    gre->SetPointError(26,0,0.882755);
    gre->SetPoint(27,27.5,50.8888);
    gre->SetPointError(27,0,0.849572);
    gre->SetPoint(28,28.5,49.8359);
    gre->SetPointError(28,0,0.832725);
    gre->SetPoint(29,29.5,50.3747);
    gre->SetPointError(29,0,0.959904);
    gre->SetPoint(30,30.5,50.3247);
    gre->SetPointError(30,0,0.792609);
    gre->SetPoint(31,31.5,50.6438);
    gre->SetPointError(31,0,0.931948);
    gre->SetPoint(32,32.5,49.8318);
    gre->SetPointError(32,0,0.867199);
    gre->SetPoint(33,33.5,50.8342);
    gre->SetPointError(33,0,0.926209);
    gre->SetPoint(34,34.5,50.162);
    gre->SetPointError(34,0,0.842226);
    gre->SetPoint(35,35.5,50.9911);
    gre->SetPointError(35,0,0.973929);
    gre->SetPoint(36,36.5,50.6135);
    gre->SetPointError(36,0,0.957172);
    gre->SetPoint(37,37.5,51.1067);
    gre->SetPointError(37,0,1.01224);
    gre->SetPoint(38,38.5,50.6015);
    gre->SetPointError(38,0,0.925449);
    gre->SetPoint(39,39.5,50.4845);
    gre->SetPointError(39,0,1.0812);
    gre->SetPoint(40,40.5,50.5183);
    gre->SetPointError(40,0,1.15675);
    gre->SetPoint(41,41.5,50.4193);
    gre->SetPointError(41,0,0.987317);
    gre->SetPoint(42,42.5,50.6586);
    gre->SetPointError(42,0,1.11198);
    gre->SetPoint(43,43.5,49.4852);
    gre->SetPointError(43,0,1.31378);
    gre->SetPoint(44,44.5,49.9061);
    gre->SetPointError(44,0,1.35299);
    gre->SetPoint(45,45.5,49.7954);
    gre->SetPointError(45,0,1.09665);
    gre->SetPoint(46,46.5,49.6234);
    gre->SetPointError(46,0,1.04475);
    gre->SetPoint(47,47.5,48.6738);
    gre->SetPointError(47,0,2.0792);
    gre->SetPoint(48,48.5,48.9246);
    gre->SetPointError(48,0,1.7453);
    gre->SetPoint(49,49.5,48.2105);
    gre->SetPointError(49,0,1.65409);
    gre->SetPoint(50,50.5,48.6346);
    gre->SetPointError(50,0,1.47069);
    gre->SetPoint(51,53.5,48.5182);
    gre->SetPointError(51,0,7.64013);
    gre->SetPoint(52,54.5,48.9268);
    gre->SetPointError(52,0,4.69475);
    gre->SetPoint(53,55.5,43.9589);
    gre->SetPointError(53,0,3.28919);
    gre->SetPoint(54,56.5,44.7615);
    gre->SetPointError(54,0,2.27991);
    gre->SetPoint(55,57.5,51.5998);
    gre->SetPointError(55,0,2.43322);
    gre->SetPoint(56,58.5,49.2219);
    gre->SetPointError(56,0,2.21948);
    gre->SetPoint(57,59.5,49.2111);
    gre->SetPointError(57,0,1.65544);
    gre->SetPoint(58,60.5,49.1901);
    gre->SetPointError(58,0,1.23852);
    gre->SetPoint(59,61.5,48.51);
    gre->SetPointError(59,0,1.21938);
    gre->SetPoint(60,62.5,48.2385);
    gre->SetPointError(60,0,1.20756);
    gre->SetPoint(61,63.5,50.3232);
    gre->SetPointError(61,0,1.35356);
    gre->SetPoint(62,64.5,49.3034);
    gre->SetPointError(62,0,1.14022);
    gre->SetPoint(63,65.5,49.6414);
    gre->SetPointError(63,0,1.04794);
    gre->SetPoint(64,66.5,48.8835);
    gre->SetPointError(64,0,1.10632);
    gre->SetPoint(65,67.5,50.3559);
    gre->SetPointError(65,0,1.03415);
    gre->SetPoint(66,68.5,49.463);
    gre->SetPointError(66,0,1.02372);
    gre->SetPoint(67,69.5,49.628);
    gre->SetPointError(67,0,1.0356);
    gre->SetPoint(68,70.5,49.0909);
    gre->SetPointError(68,0,0.951124);
    gre->SetPoint(69,71.5,50.8107);
    gre->SetPointError(69,0,1.03579);
    gre->SetPoint(70,72.5,49.5813);
    gre->SetPointError(70,0,1.00864);
    gre->SetPoint(71,73.5,49.3343);
    gre->SetPointError(71,0,0.897636);
    gre->SetPoint(72,74.5,49.0304);
    gre->SetPointError(72,0,0.909546);
    gre->SetPoint(73,75.5,50.0122);
    gre->SetPointError(73,0,1.14149);
    gre->SetPoint(74,76.5,49.5944);
    gre->SetPointError(74,0,0.93658);
    gre->SetPoint(75,77.5,49.1854);
    gre->SetPointError(75,0,0.94017);
    gre->SetPoint(76,78.5,49.0663);
    gre->SetPointError(76,0,0.862119);
    gre->SetPoint(77,79.5,50.8264);
    gre->SetPointError(77,0,1.01501);
    gre->SetPoint(78,80.5,49.2556);
    gre->SetPointError(78,0,0.827094);
    gre->SetPoint(79,81.5,49.2854);
    gre->SetPointError(79,0,0.87059);
    gre->SetPoint(80,82.5,48.8041);
    gre->SetPointError(80,0,0.875617);
    gre->SetPoint(81,83.5,50.2469);
    gre->SetPointError(81,0,1.03943);
    gre->SetPoint(82,84.5,49.3145);
    gre->SetPointError(82,0,0.965691);
    gre->SetPoint(83,85.5,49.1618);
    gre->SetPointError(83,0,0.820093);
    gre->SetPoint(84,86.5,48.2082);
    gre->SetPointError(84,0,0.844078);
    gre->SetPoint(85,87.5,50.3516);
    gre->SetPointError(85,0,1.0081);
    gre->SetPoint(86,88.5,49.2738);
    gre->SetPointError(86,0,0.95336);
    gre->SetPoint(87,89.5,49.1824);
    gre->SetPointError(87,0,0.84797);
    gre->SetPoint(88,90.5,48.4915);
    gre->SetPointError(88,0,0.834984);
    gre->SetPoint(89,91.5,50.5531);
    gre->SetPointError(89,0,0.973092);
    gre->SetPoint(90,92.5,49.2893);
    gre->SetPointError(90,0,0.916436);
    gre->SetPoint(91,93.5,48.9445);
    gre->SetPointError(91,0,0.869937);
    gre->SetPoint(92,94.5,48.847);
    gre->SetPointError(92,0,0.852846);
    gre->SetPoint(93,95.5,49.9553);
    gre->SetPointError(93,0,0.954257);
    gre->SetPoint(94,96.5,49.1735);
    gre->SetPointError(94,0,0.933277);
    gre->SetPoint(95,97.5,48.9946);
    gre->SetPointError(95,0,0.91292);
    gre->SetPoint(96,98.5,48.65);
    gre->SetPointError(96,0,0.813873);
    gre->SetPoint(97,99.5,50.7661);
    gre->SetPointError(97,0,1.01003);
    gre->SetPoint(98,100.5,49.7782);
    gre->SetPointError(98,0,1.05407);
    gre->SetPoint(99,101.5,49.8352);
    gre->SetPointError(99,0,0.886551);
    gre->SetPoint(100,102.5,48.7939);
    gre->SetPointError(100,0,0.842666);
    gre->SetPoint(101,103.5,50.7709);
    gre->SetPointError(101,0,1.08019);
    gre->SetPoint(102,104.5,49.5987);
    gre->SetPointError(102,0,1.04488);
    gre->SetPoint(103,105.5,48.9441);
    gre->SetPointError(103,0,1.07507);
    gre->SetPoint(104,106.5,48.8652);
    gre->SetPointError(104,0,0.873614);
    gre->SetPoint(105,107.5,49.8936);
    gre->SetPointError(105,0,1.08433);
    gre->SetPoint(106,108.5,49.7337);
    gre->SetPointError(106,0,0.907157);
    gre->SetPoint(107,109.5,49.2077);
    gre->SetPointError(107,0,1.05685);
    gre->SetPoint(108,110.5,48.4725);
    gre->SetPointError(108,0,1.0194);
    gre->SetPoint(109,111.5,50.4909);
    gre->SetPointError(109,0,1.40793);
    gre->SetPoint(110,112.5,48.6564);
    gre->SetPointError(110,0,1.21563);
    gre->SetPoint(111,113.5,49.2878);
    gre->SetPointError(111,0,1.23662);
    gre->SetPoint(112,114.5,48.8585);
    gre->SetPointError(112,0,1.19111);
    gre->SetPoint(113,115.5,49.0024);
    gre->SetPointError(113,0,4.03932);
    gre->SetPoint(114,116.5,47.9788);
    gre->SetPointError(114,0,2.06361);
    gre->SetPoint(115,117.5,48.9792);
    gre->SetPointError(115,0,1.85065);
    gre->SetPoint(116,118.5,48.4185);
    gre->SetPointError(116,0,1.70544);
    gre->SetPoint(117,119.5,47.0811);
    gre->SetPointError(117,0,1.67773);
    gre->SetPoint(118,120.5,48.2193);
    gre->SetPointError(118,0,1.38014);
    gre->SetPoint(119,121.5,47.429);
    gre->SetPointError(119,0,1.19795);
    gre->SetPoint(120,122.5,47.9857);
    gre->SetPointError(120,0,1.31458);
    gre->SetPoint(121,123.5,48.1784);
    gre->SetPointError(121,0,1.13372);
    gre->SetPoint(122,124.5,48.5373);
    gre->SetPointError(122,0,1.1462);
    gre->SetPoint(123,125.5,48.0804);
    gre->SetPointError(123,0,1.08286);
    gre->SetPoint(124,126.5,48.8403);
    gre->SetPointError(124,0,1.118);
    gre->SetPoint(125,127.5,49.0332);
    gre->SetPointError(125,0,1.09185);
    gre->SetPoint(126,128.5,48.4264);
    gre->SetPointError(126,0,1.00529);
    gre->SetPoint(127,129.5,47.7731);
    gre->SetPointError(127,0,0.955601);
    gre->SetPoint(128,130.5,47.875);
    gre->SetPointError(128,0,1.06046);
    gre->SetPoint(129,131.5,48.6961);
    gre->SetPointError(129,0,0.848715);
    gre->SetPoint(130,132.5,48.0624);
    gre->SetPointError(130,0,0.888586);
    gre->SetPoint(131,133.5,46.1796);
    gre->SetPointError(131,0,0.8379);
    gre->SetPoint(132,134.5,47.708);
    gre->SetPointError(132,0,0.932652);
    gre->SetPoint(133,135.5,49.1333);
    gre->SetPointError(133,0,0.821108);
    gre->SetPoint(134,136.5,48.1247);
    gre->SetPointError(134,0,0.843609);
    gre->SetPoint(135,137.5,48.3282);
    gre->SetPointError(135,0,0.824315);
    gre->SetPoint(136,138.5,48.7303);
    gre->SetPointError(136,0,0.886027);
    gre->SetPoint(137,139.5,48.164);
    gre->SetPointError(137,0,0.835315);
    gre->SetPoint(138,140.5,47.6951);
    gre->SetPointError(138,0,0.839216);
    gre->SetPoint(139,141.5,48.0538);
    gre->SetPointError(139,0,0.893671);
    gre->SetPoint(140,142.5,48.7111);
    gre->SetPointError(140,0,0.831573);
    gre->SetPoint(141,143.5,47.5859);
    gre->SetPointError(141,0,0.874164);
    gre->SetPoint(142,144.5,47.7595);
    gre->SetPointError(142,0,0.800781);
    gre->SetPoint(143,145.5,47.3889);
    gre->SetPointError(143,0,0.807107);
    gre->SetPoint(144,146.5,47.7994);
    gre->SetPointError(144,0,0.842243);
    gre->SetPoint(145,147.5,48.0446);
    gre->SetPointError(145,0,0.788595);
    gre->SetPoint(146,148.5,47.6536);
    gre->SetPointError(146,0,0.78313);
    gre->SetPoint(147,149.5,47.2458);
    gre->SetPointError(147,0,0.876516);
    gre->SetPoint(148,150.5,47.8436);
    gre->SetPointError(148,0,0.781727);
    gre->SetPoint(149,151.5,47.9621);
    gre->SetPointError(149,0,0.740992);
    gre->SetPoint(150,152.5,47.5173);
    gre->SetPointError(150,0,0.748253);
    gre->SetPoint(151,153.5,47.6523);
    gre->SetPointError(151,0,0.864584);
    gre->SetPoint(152,154.5,47.4699);
    gre->SetPointError(152,0,0.87352);
    gre->SetPoint(153,155.5,47.6473);
    gre->SetPointError(153,0,0.92203);
    gre->SetPoint(154,156.5,47.4401);
    gre->SetPointError(154,0,0.849856);
    gre->SetPoint(155,157.5,48.195);
    gre->SetPointError(155,0,0.812619);
    gre->SetPoint(156,158.5,47.7105);
    gre->SetPointError(156,0,0.868492);
    gre->SetPoint(157,159.5,47.9425);
    gre->SetPointError(157,0,0.755785);
    gre->SetPoint(158,160.5,47.355);
    gre->SetPointError(158,0,0.865663);
    gre->SetPoint(159,161.5,47.2465);
    gre->SetPointError(159,0,0.866397);
    gre->SetPoint(160,162.5,47.5331);
    gre->SetPointError(160,0,0.817595);
    gre->SetPoint(161,163.5,47.8887);
    gre->SetPointError(161,0,0.847047);
    gre->SetPoint(162,164.5,48.0366);
    gre->SetPointError(162,0,0.857191);
    gre->SetPoint(163,165.5,48.2199);
    gre->SetPointError(163,0,0.949289);
    gre->SetPoint(164,166.5,47.5214);
    gre->SetPointError(164,0,0.915672);
    gre->SetPoint(165,167.5,47.9422);
    gre->SetPointError(165,0,0.93005);
    gre->SetPoint(166,168.5,47.9472);
    gre->SetPointError(166,0,0.939351);
    gre->SetPoint(167,169.5,47.8711);
    gre->SetPointError(167,0,0.858344);
    gre->SetPoint(168,170.5,48.1241);
    gre->SetPointError(168,0,0.962103);
    gre->SetPoint(169,171.5,47.9161);
    gre->SetPointError(169,0,0.93892);
    gre->SetPoint(170,172.5,48.1227);
    gre->SetPointError(170,0,1.01265);
    gre->SetPoint(171,173.5,47.0992);
    gre->SetPointError(171,0,0.892547);
    gre->SetPoint(172,174.5,47.5865);
    gre->SetPointError(172,0,0.916901);
    gre->SetPoint(173,175.5,48.1109);
    gre->SetPointError(173,0,0.997756);
    gre->SetPoint(174,176.5,47.5984);
    gre->SetPointError(174,0,1.06681);
    gre->SetPoint(175,177.5,47.8965);
    gre->SetPointError(175,0,0.91732);
    gre->SetPoint(176,178.5,47.9553);
    gre->SetPointError(176,0,1.1084);
    gre->SetPoint(177,179.5,48.9007);
    gre->SetPointError(177,0,1.40904);
    gre->SetPoint(178,180.5,48.106);
    gre->SetPointError(178,0,1.47816);
    gre->SetPoint(179,181.5,47.5741);
    gre->SetPointError(179,0,1.22573);
    gre->SetPoint(180,182.5,47.3435);
    gre->SetPointError(180,0,1.18014);
    gre->SetPoint(181,183.5,48.5473);
    gre->SetPointError(181,0,1.8325);
    gre->SetPoint(182,184.5,48.7655);
    gre->SetPointError(182,0,1.89701);
    gre->SetPoint(183,185.5,48.9416);
    gre->SetPointError(183,0,1.84616);
    gre->SetPoint(184,186.5,49.3201);
    gre->SetPointError(184,0,1.63458);
    gre->SetPoint(185,187.5,48.4441);
    gre->SetPointError(185,0,1.58598);
    gre->SetPoint(186,188.5,49.0574);
    gre->SetPointError(186,0,1.40995);
    gre->SetPoint(187,189.5,49.461);
    gre->SetPointError(187,0,1.34259);
    gre->SetPoint(188,190.5,50.08);
    gre->SetPointError(188,0,1.71738);
    gre->SetPoint(189,191.5,47.7805);
    gre->SetPointError(189,0,1.16629);
    gre->SetPoint(190,192.5,49.9438);
    gre->SetPointError(190,0,1.15782);
    gre->SetPoint(191,193.5,50.2827);
    gre->SetPointError(191,0,1.42737);
    gre->SetPoint(192,194.5,50.0371);
    gre->SetPointError(192,0,1.44808);
    gre->SetPoint(193,195.5,48.6364);
    gre->SetPointError(193,0,1.05513);
    gre->SetPoint(194,196.5,49.4924);
    gre->SetPointError(194,0,1.1464);
    gre->SetPoint(195,197.5,50.2008);
    gre->SetPointError(195,0,1.069);
    gre->SetPoint(196,198.5,50.5102);
    gre->SetPointError(196,0,1.20757);
    gre->SetPoint(197,199.5,49.3054);
    gre->SetPointError(197,0,0.992103);
    gre->SetPoint(198,200.5,49.4451);
    gre->SetPointError(198,0,1.09141);
    gre->SetPoint(199,201.5,49.3923);
    gre->SetPointError(199,0,0.985771);
    gre->SetPoint(200,202.5,50.5343);
    gre->SetPointError(200,0,1.12546);
    gre->SetPoint(201,203.5,48.8774);
    gre->SetPointError(201,0,0.863312);
    gre->SetPoint(202,204.5,49.1963);
    gre->SetPointError(202,0,0.950132);
    gre->SetPoint(203,205.5,50.2418);
    gre->SetPointError(203,0,0.997508);
    gre->SetPoint(204,206.5,50.6893);
    gre->SetPointError(204,0,1.29251);
    gre->SetPoint(205,207.5,48.5683);
    gre->SetPointError(205,0,0.845572);
    gre->SetPoint(206,208.5,49.1239);
    gre->SetPointError(206,0,0.977408);
    gre->SetPoint(207,209.5,49.3987);
    gre->SetPointError(207,0,1.14016);
    gre->SetPoint(208,210.5,50.7896);
    gre->SetPointError(208,0,1.08832);
    gre->SetPoint(209,211.5,48.6387);
    gre->SetPointError(209,0,0.788928);
    gre->SetPoint(210,212.5,48.9744);
    gre->SetPointError(210,0,1.01541);
    gre->SetPoint(211,213.5,50.0218);
    gre->SetPointError(211,0,0.954812);
    gre->SetPoint(212,214.5,49.8532);
    gre->SetPointError(212,0,1.20053);
    gre->SetPoint(213,215.5,48.7636);
    gre->SetPointError(213,0,0.831835);
    gre->SetPoint(214,216.5,49.2866);
    gre->SetPointError(214,0,0.877379);
    gre->SetPoint(215,217.5,49.28);
    gre->SetPointError(215,0,1.03686);
    gre->SetPoint(216,218.5,49.7873);
    gre->SetPointError(216,0,1.09641);
    gre->SetPoint(217,219.5,48.6553);
    gre->SetPointError(217,0,0.825545);
    gre->SetPoint(218,220.5,48.4717);
    gre->SetPointError(218,0,0.899596);
    gre->SetPoint(219,221.5,49.6215);
    gre->SetPointError(219,0,0.932259);
    gre->SetPoint(220,222.5,50.0565);
    gre->SetPointError(220,0,1.18866);
    gre->SetPoint(221,223.5,48.8247);
    gre->SetPointError(221,0,0.845938);
    gre->SetPoint(222,224.5,49.6911);
    gre->SetPointError(222,0,1.04815);
    gre->SetPoint(223,225.5,48.8249);
    gre->SetPointError(223,0,0.927154);
    gre->SetPoint(224,226.5,50.0197);
    gre->SetPointError(224,0,1.01906);
    gre->SetPoint(225,227.5,48.7971);
    gre->SetPointError(225,0,0.820377);
    gre->SetPoint(226,228.5,49.3551);
    gre->SetPointError(226,0,0.893281);
    gre->SetPoint(227,229.5,50.0073);
    gre->SetPointError(227,0,1.00422);
    gre->SetPoint(228,230.5,50.6289);
    gre->SetPointError(228,0,1.07395);
    gre->SetPoint(229,231.5,48.7818);
    gre->SetPointError(229,0,0.896753);
    gre->SetPoint(230,232.5,49.5935);
    gre->SetPointError(230,0,1.00955);
    gre->SetPoint(231,233.5,49.26);
    gre->SetPointError(231,0,0.910688);
    gre->SetPoint(232,234.5,50.2228);
    gre->SetPointError(232,0,1.05326);
    gre->SetPoint(233,235.5,48.7979);
    gre->SetPointError(233,0,0.918282);
    gre->SetPoint(234,236.5,49.2787);
    gre->SetPointError(234,0,0.960356);
    gre->SetPoint(235,237.5,49.2179);
    gre->SetPointError(235,0,1.23667);
    gre->SetPoint(236,238.5,50.4737);
    gre->SetPointError(236,0,1.09661);
    gre->SetPoint(237,239.5,49.1226);
    gre->SetPointError(237,0,1.12529);
    gre->SetPoint(238,240.5,49.4431);
    gre->SetPointError(238,0,1.11615);
    gre->SetPoint(239,241.5,50.1285);
    gre->SetPointError(239,0,1.01168);
    gre->SetPoint(240,242.5,50.0121);
    gre->SetPointError(240,0,1.10582);
    gre->SetPoint(241,243.5,48.7944);
    gre->SetPointError(241,0,1.12888);
    gre->SetPoint(242,244.5,49.4456);
    gre->SetPointError(242,0,1.12051);
    gre->SetPoint(243,245.5,50.8511);
    gre->SetPointError(243,0,1.106);
    gre->SetPoint(244,246.5,49.7031);
    gre->SetPointError(244,0,1.16289);
    gre->SetPoint(245,247.5,48.3673);
    gre->SetPointError(245,0,1.28763);
    gre->SetPoint(246,248.5,48.6557);
    gre->SetPointError(246,0,1.13661);
    gre->SetPoint(247,249.5,48.3829);
    gre->SetPointError(247,0,1.29785);
    gre->SetPoint(248,250.5,48.7968);
    gre->SetPointError(248,0,1.6479);
    gre->SetPoint(249,251.5,48.5081);
    gre->SetPointError(249,0,1.84734);
    gre->SetPoint(250,252.5,48.9985);
    gre->SetPointError(250,0,1.72307);
    gre->SetPoint(251,253.5,49.5934);
    gre->SetPointError(251,0,1.65226);
    gre->SetPoint(252,254.5,48.6318);
    gre->SetPointError(252,0,1.54777);
    gre->SetPoint(253,255.5,50.4711);
    gre->SetPointError(253,0,2.59199);
    gre->SetPoint(254,256.5,49.4124);
    gre->SetPointError(254,0,2.22381);
    gre->SetPoint(255,257.5,50.2827);
    gre->SetPointError(255,0,2.37749);
    gre->SetPoint(256,258.5,49.3544);
    gre->SetPointError(256,0,1.932);
    gre->SetPoint(257,259.5,50.9345);
    gre->SetPointError(257,0,1.5013);
    gre->SetPoint(258,260.5,49.6367);
    gre->SetPointError(258,0,1.81287);
    gre->SetPoint(259,261.5,49.4105);
    gre->SetPointError(259,0,1.49667);
    gre->SetPoint(260,262.5,49.7129);
    gre->SetPointError(260,0,1.56951);
    gre->SetPoint(261,263.5,50.0948);
    gre->SetPointError(261,0,1.2891);
    gre->SetPoint(262,264.5,49.0077);
    gre->SetPointError(262,0,1.24638);
    gre->SetPoint(263,265.5,49.601);
    gre->SetPointError(263,0,1.27457);
    gre->SetPoint(264,266.5,49.8123);
    gre->SetPointError(264,0,1.36972);
    gre->SetPoint(265,267.5,50.8182);
    gre->SetPointError(265,0,1.12744);
    gre->SetPoint(266,268.5,50.1239);
    gre->SetPointError(266,0,1.17794);
    gre->SetPoint(267,269.5,50.8763);
    gre->SetPointError(267,0,1.16177);
    gre->SetPoint(268,270.5,51.727);
    gre->SetPointError(268,0,1.28939);
    gre->SetPoint(269,271.5,50.6272);
    gre->SetPointError(269,0,1.05542);
    gre->SetPoint(270,272.5,51.3573);
    gre->SetPointError(270,0,1.03082);
    gre->SetPoint(271,273.5,50.7498);
    gre->SetPointError(271,0,1.12412);
    gre->SetPoint(272,274.5,51.4096);
    gre->SetPointError(272,0,1.40228);
    gre->SetPoint(273,275.5,50.3837);
    gre->SetPointError(273,0,0.975028);
    gre->SetPoint(274,276.5,51.2898);
    gre->SetPointError(274,0,1.13245);
    gre->SetPoint(275,277.5,50.3993);
    gre->SetPointError(275,0,1.11825);
    gre->SetPoint(276,278.5,51.5095);
    gre->SetPointError(276,0,1.13987);
    gre->SetPoint(277,279.5,51.2602);
    gre->SetPointError(277,0,0.944348);
    gre->SetPoint(278,280.5,50.1756);
    gre->SetPointError(278,0,0.919636);
    gre->SetPoint(279,281.5,51.1252);
    gre->SetPointError(279,0,1.04464);
    gre->SetPoint(280,282.5,51.2404);
    gre->SetPointError(280,0,1.19978);
    gre->SetPoint(281,283.5,50.1893);
    gre->SetPointError(281,0,1.00766);
    gre->SetPoint(282,284.5,50.7923);
    gre->SetPointError(282,0,1.02714);
    gre->SetPoint(283,285.5,50.827);
    gre->SetPointError(283,0,0.956727);
    gre->SetPoint(284,286.5,51.8704);
    gre->SetPointError(284,0,1.12418);
    gre->SetPoint(285,287.5,50.6072);
    gre->SetPointError(285,0,0.868976);
    gre->SetPoint(286,288.5,51.0409);
    gre->SetPointError(286,0,0.983778);
    gre->SetPoint(287,289.5,50.9895);
    gre->SetPointError(287,0,0.989665);
    gre->SetPoint(288,290.5,51.3022);
    gre->SetPointError(288,0,1.07615);
    gre->SetPoint(289,291.5,50.8446);
    gre->SetPointError(289,0,0.954257);
    gre->SetPoint(290,292.5,50.3456);
    gre->SetPointError(290,0,1.08404);
    gre->SetPoint(291,293.5,50.6019);
    gre->SetPointError(291,0,0.905529);
    gre->SetPoint(292,294.5,51.7881);
    gre->SetPointError(292,0,1.00121);
    gre->SetPoint(293,295.5,50.7643);
    gre->SetPointError(293,0,0.953271);
    gre->SetPoint(294,296.5,50.9203);
    gre->SetPointError(294,0,0.963474);
    gre->SetPoint(295,297.5,48.7366);
    gre->SetPointError(295,0,1.16719);
    gre->SetPoint(296,298.5,50.5893);
    gre->SetPointError(296,0,0.961525);
    gre->SetPoint(297,299.5,50.6448);
    gre->SetPointError(297,0,1.01872);
    gre->SetPoint(298,300.5,50.1472);
    gre->SetPointError(298,0,1.15731);
    gre->SetPoint(299,301.5,50.5658);
    gre->SetPointError(299,0,1.07416);
    gre->SetPoint(300,302.5,50.8774);
    gre->SetPointError(300,0,1.00571);
    gre->SetPoint(301,303.5,51.5337);
    gre->SetPointError(301,0,1.17334);
    gre->SetPoint(302,304.5,50.9412);
    gre->SetPointError(302,0,1.24217);
    gre->SetPoint(303,305.5,50.1015);
    gre->SetPointError(303,0,1.28142);
    gre->SetPoint(304,306.5,50.7137);
    gre->SetPointError(304,0,1.2407);
    gre->SetPoint(305,307.5,50.4776);
    gre->SetPointError(305,0,1.58621);
    gre->SetPoint(306,308.5,50.873);
    gre->SetPointError(306,0,1.22031);
    gre->SetPoint(307,309.5,50.7685);
    gre->SetPointError(307,0,1.2078);
    gre->SetPoint(308,310.5,52.0635);
    gre->SetPointError(308,0,1.25884);
    gre->SetPoint(309,311.5,50.8565);
    gre->SetPointError(309,0,1.36863);
    gre->SetPoint(310,312.5,49.6296);
    gre->SetPointError(310,0,1.28152);
    gre->SetPoint(311,313.5,50.4984);
    gre->SetPointError(311,0,1.25195);
    gre->SetPoint(312,314.5,49.8019);
    gre->SetPointError(312,0,1.49823);
    gre->SetPoint(313,315.5,48.6923);
    gre->SetPointError(313,0,1.40011);
    gre->SetPoint(314,316.5,48.6248);
    gre->SetPointError(314,0,1.44555);
    gre->SetPoint(315,317.5,49.0099);
    gre->SetPointError(315,0,2.05889);
    gre->SetPoint(316,318.5,49.1606);
    gre->SetPointError(316,0,1.64153);
    gre->SetPoint(317,319.5,50.7995);
    gre->SetPointError(317,0,2.18407);
    gre->SetPoint(318,320.5,49.1989);
    gre->SetPointError(318,0,1.6306);
    gre->SetPoint(319,321.5,49.1493);
    gre->SetPointError(319,0,1.83757);
    gre->SetPoint(320,322.5,49.1493);
    gre->SetPointError(320,0,1.85912);
    gre->SetPoint(321,323.5,49.4622);
    gre->SetPointError(321,0,2.85813);
    gre->SetPoint(322,324.5,47.2657);
    gre->SetPointError(322,0,2.606);
    gre->SetPoint(323,325.5,49.6803);
    gre->SetPointError(323,0,2.87287);
    gre->SetPoint(324,326.5,51.0121);
    gre->SetPointError(324,0,2.47155);
  }

  if(additional_modcorr_mean==0){
    int nmean=0;
    for(int i=0;i<gre->GetN();i++){
      float valY=gre->GetY()[i];
      if(valY>0.&&valY<80){
	additional_modcorr_mean+=valY;
	nmean++;
      }
    }
    additional_modcorr_mean/=(float)nmean;
  }
  
  return additional_modcorr_mean/gre->Eval(mod);
}

float TrdHCalibR::bgcorr_helium(float bg){
	printf("TrdHCalibR::bgcorr_helium - Begin\n");
  if(!g_bgcorr_helium){
    g_bgcorr_helium=new TGraphErrors(26);
    g_bgcorr_helium->SetPoint(0,-0.05,380.363*1.15);
    g_bgcorr_helium->SetPointError(0,0,0.826046);
    g_bgcorr_helium->SetPoint(1,0.05,341.989*1.12);
    g_bgcorr_helium->SetPointError(1,0,0.524841);
    g_bgcorr_helium->SetPoint(2,0.15,317.76*1.08);
    g_bgcorr_helium->SetPointError(2,0,0.371851);
    g_bgcorr_helium->SetPoint(3,0.25,302.793*1.05);
    g_bgcorr_helium->SetPointError(3,0,0.315932);
    g_bgcorr_helium->SetPoint(4,0.35,295.399*1.03);
    g_bgcorr_helium->SetPointError(4,0,0.289967);
    g_bgcorr_helium->SetPoint(5,0.45,293.53*1.01);
    g_bgcorr_helium->SetPointError(5,0,0.284084);
    g_bgcorr_helium->SetPoint(6,0.55,295.874);
    g_bgcorr_helium->SetPointError(6,0,0.309261);
    g_bgcorr_helium->SetPoint(7,0.65,300.981);
    g_bgcorr_helium->SetPointError(7,0,0.323394);
    g_bgcorr_helium->SetPoint(8,0.75,307.611);
    g_bgcorr_helium->SetPointError(8,0,0.35119);
    g_bgcorr_helium->SetPoint(9,0.85,315.101);
    g_bgcorr_helium->SetPointError(9,0,0.401949);
    g_bgcorr_helium->SetPoint(10,0.95,323.863);
    g_bgcorr_helium->SetPointError(10,0,0.460864);
    g_bgcorr_helium->SetPoint(11,1.05,332.992);
    g_bgcorr_helium->SetPointError(11,0,0.547908);
    g_bgcorr_helium->SetPoint(12,1.15,341.718);
    g_bgcorr_helium->SetPointError(12,0,0.692394);
    g_bgcorr_helium->SetPoint(13,1.25,350.35);
    g_bgcorr_helium->SetPointError(13,0,0.871716);
    g_bgcorr_helium->SetPoint(14,1.35,358.972);
    g_bgcorr_helium->SetPointError(14,0,1.04386);
    g_bgcorr_helium->SetPoint(15,1.45,365.956);
    g_bgcorr_helium->SetPointError(15,0,1.34903);
    g_bgcorr_helium->SetPoint(16,1.55,373.813);
    g_bgcorr_helium->SetPointError(16,0,1.58405);
    g_bgcorr_helium->SetPoint(17,1.65,380.094);
    g_bgcorr_helium->SetPointError(17,0,2.22431);
    g_bgcorr_helium->SetPoint(18,1.75,387.097);
    g_bgcorr_helium->SetPointError(18,0,2.3447);
    g_bgcorr_helium->SetPoint(19,1.85,391.597);
    g_bgcorr_helium->SetPointError(19,0,2.95786);
    g_bgcorr_helium->SetPoint(20,1.95,394.019);
    g_bgcorr_helium->SetPointError(20,0,3.68512);
    g_bgcorr_helium->SetPoint(21,2.05,399.439);
    g_bgcorr_helium->SetPointError(21,0,4.56432);
    g_bgcorr_helium->SetPoint(22,2.15,395.735);
    g_bgcorr_helium->SetPointError(22,0,6.74708);
    g_bgcorr_helium->SetPoint(23,2.25,405.567);
    g_bgcorr_helium->SetPointError(23,0,7.97385);
    g_bgcorr_helium->SetPoint(24,2.35,388.705);
    g_bgcorr_helium->SetPointError(24,0,21.7004);
    g_bgcorr_helium->SetPoint(25,2.45,676.592);
    g_bgcorr_helium->SetPointError(25,0,2675.14);
  }  

printf("TrdHCalibR::bgcorr_helium - End\n");
  return g_bgcorr_helium->Eval(0.445)/g_bgcorr_helium->Eval(bg);
}

