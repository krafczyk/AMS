#include "TkSens.h"
#include "TrCharge.h"

/// set TrdSCalib options
int CalibLevel   = 4;
int TrdTrackType = 1; //0: trdhtrack, 1:trdtrack
int Debug        = 0;

/// construct TrdSCalib
TrdSCalibR* trdS = new TrdSCalibR();

TF1 *fTrdSigmaDx;
TF1 *fTrdSigmaDy;
TF1 *fTrd95Da;

//===================================================================================

double FunTrdSigma(double *x, double *par) {
  return 0.1*(par[0] + par[1]*exp(-par[2]*x[0]));
}


void Init_selection(){

  fTrdSigmaDx = new TF1("fTrdSigmaDx",FunTrdSigma,0.0,1000.0,3);
  fTrdSigmaDx->SetParameters(2.484,0.1183,0.3487);
  fTrdSigmaDy = new TF1("fTrdSigmaDy",FunTrdSigma,0.0,1000.0,3);
  fTrdSigmaDy->SetParameters(1.686,0.1505,0.2347);
  
  fTrd95Da = new TF1("fTrd95Da",FunTrdSigma,0.0,1000.0,3);
  fTrd95Da->SetParameters(0.7729,0.7324,0.2005);
  

  if( trdS->InitTrdSCalib(CalibLevel, TrdTrackType, Debug) ) {
    cerr << "Error in TrdSCalib Initialization" << endl;
  }
  
  TrExtAlignDB::ForceFromTDV=1;
  
}


bool Clean_Event(AMSEventR *pev){

  //sciense runtag
  HeaderR* header = &(pev->fHeader);
  if((header->RunType>>12)!=0xf) return false; 
  // printf("%04x\n", header->RunType);
  

  //Live Time >0.5
  Level1R * trig=pev-> pLevel1(0);
  if( trig->LiveTime <=0.65) return false;


  // 1! Tracker track
  if(pev->nTrTrack()!=1) return false;
  TrTrackR *track=pev->pTrTrack(0);
  int id=track->iTrTrackPar(1,3,1);
  if(id<0)return false;


  // Rigidity > 3 GeV 
  if(fabs(track->GetRigidity(id))<=3.0) return false;

  //downgoing particle with good beta
   
  BetaR *beta;
  int track_beta=0;   
  float Beta=0;
  float betasum=0;
  if(pev->nBeta()>0){
    for( int i=0; i<pev->nBeta(); i++){
      beta=pev->pBeta(i);      
      if(beta->iTrTrack()!=-1){
	betasum+=1;
	track_beta=beta->iTrTrack();
      }
    }
    if(betasum!=1) return false;
    else{
	beta=pev->pBeta(track_beta);
	Beta=beta->Beta;
    }
  }
  
  if(Beta<=0.6 || Beta >=1.2) return false;
  
  //input Marco on d Track/Hits in ToF && 4/4 ToF

  bool minimum_bias;
  
  TofClusterR* cluster;
  
  bool goodlayer[4]={false,false,false,false};
  
  minimum_bias=false;
  if(pev->nTofCluster()<=0) return false;
  for (int icl=0; icl<pev->nTofCluster(); icl++) { 
    int layer;
    cluster = pev->pTofCluster(icl);
    bool good_c=false;
    if (cluster>0) { // cluster exists
      good_c=true;
      layer=cluster->Layer-1;
      for (int i=7; i<13; i++) if ((cluster->Status>>i)&1==1) good_c=false;
      if ((cluster->Status>>2)&1==1) good_c=false;
      if ((cluster->Status>>4)&1==1) good_c=false;
    }
    if(good_c) goodlayer[layer]=true;
  }
  minimum_bias=(goodlayer[0] && goodlayer[1] && goodlayer[2] && goodlayer[3]);
  //if(!minimum_bias) return false;

  float LONGCUT[4][10]={
    9.,8.,8.,8.,8.,8.,8.,9.,0.,0.,
    12.,8.,8.,8.,8.,8.,8.,12.,0.,0.,
    12.,8.,8.,8.,8.,8.,8.,8.,8.,12.,
    10.,8.,8.,8.,8.,8.,8.,10.,0.,0.}; // cm (cut on tof-track transversal coord.)
  float TRANCUT[4][10]={
    13.,6.,6.,6.,6.,6.,6.,13.,0.,0.,
    14.,6.,6.,6.,6.,6.,6.,14.,0.,0.,
    10.,6.,6.,6.,6.,6.,6.,6.,6.,10.,
    14.,6.,6.,6.,6.,6.,6.,14.,0.,0.}; // cm (cut on tof-track longitudinal coord.)
  
  bool good_match;
  double tlen;
  AMSPoint tofpnt;
  AMSDir tofdir;
  int longit[4]={0,1,1,0};
  int tranit[4]={1,0,0,1};
  double dlong,dtran;
  bool goodlay[4]={false,false,false,false};
  
  // search for track-cluster matching
  good_match=false;
  // match with cluster
  for (int icl=0; icl<pev->nTofCluster(); icl++) { 
    cluster = pev->pTofCluster(icl);
    if (cluster>0) { // cluster exists
      int layer=cluster->Layer-1;
      int bar=cluster->Bar-1;
      tlen=track->Interpolate(cluster->Coo[2],tofpnt,tofdir, id);
      dlong=cluster->Coo[longit[layer]]-tofpnt[longit[layer]];
      dtran=cluster->Coo[tranit[layer]]-tofpnt[tranit[layer]];
      if(fabs(dlong)<LONGCUT[layer][bar] && fabs(dtran)<TRANCUT[layer][bar]) goodlay[layer]=true;
    }
  }
  good_match=(goodlay[0] && goodlay[1] && goodlay[2] && goodlay[3]);
  //if(!good_match) return false;
  int nlay=0;
  for(int i=0; i<4; i++) if(goodlayer[i] && goodlay[i]) nlay++;
  if(nlay<3) return false;

  int tr_lay[9][2];
  for(int i=0;i<9;i++){tr_lay[i][0]=0;tr_lay[i][1]=0;}

  TrRecHitR *hit;
  for(int i=2; i<9; i++){
    hit=track->GetHitLJ(i);
    if(hit){
      if(!hit->OnlyY()) tr_lay[i-1][0]++;
      if(!hit->OnlyX()) tr_lay[i-1][1]++;
    }
  }

  int n_layers[2]={0,0};
  
  for(int i=1; i<8; i++){
    if(tr_lay[i][0]>0) n_layers[0]++;
    if(tr_lay[i][1]>0) n_layers[1]++;
  }
  
  
  // One hit on every inner tracker plane in X,Y
  if(!((tr_lay[2][0]||tr_lay[3][0]) && (tr_lay[4][0]||tr_lay[5][0]) && (tr_lay[6][0]||tr_lay[7][0]))) return false;
  if(!(tr_lay[1][1]&&(tr_lay[2][1]||tr_lay[3][1]) && (tr_lay[4][1]||tr_lay[5][1]) && (tr_lay[6][1]||tr_lay[7][1]))) return false;
  
  
  // if(track->GetChisqX(id)/ n_layers[0] >5 ) return false;
  if(track->GetNormChisqY(id) >=15. ) return false;

  AMSPoint pnt[2];

 
  pnt[0]=track->InterpolateLayerJ(1, id);
  pnt[1]=track->InterpolateLayerJ(9, id);

  TkSens ss(0);
  float err=0.1;
  bool sen[4][2];
  AMSPoint a[4], b[4];
  
  a[0].setp(pnt[0].x()+err, pnt[0].y(),pnt[0].z());
  a[1].setp(pnt[0].x(), pnt[0].y()+err,pnt[0].z());
  a[2].setp(pnt[0].x()-err, pnt[0].y(),pnt[0].z());
  a[3].setp(pnt[0].x(), pnt[0].y()-err,pnt[0].z());
  
  for(int i=0; i<4; i++){
    sen[i][0]=false;
    ss.SetGlobalCoo(a[i]);    
    if(ss.LadFound ()) sen[i][0]=true;
  }
  if(!(sen[0][0] || sen[1][0] || sen[2][0] || sen[3][0])) return false;
 
  
  b[0].setp(pnt[1].x()+err, pnt[1].y(),pnt[1].z());
  b[1].setp(pnt[1].x(), pnt[1].y()+err,pnt[1].z());
  b[2].setp(pnt[1].x()-err, pnt[1].y(),pnt[1].z());
  b[3].setp(pnt[1].x(), pnt[1].y()-err,pnt[1].z());
  
  for(int i=0; i<4; i++){
    sen[i][1]=false;
    ss.SetGlobalCoo(b[i]);    
    if(ss.LadFound ()) sen[i][1]=true;
  }
  if(!((sen[0][1] || sen[1][1] || sen[2][1] || sen[3][1]) /*&& fabs(pnt[1].x())<30.*/)) return false;
  

  AMSPoint ecaltop;
  AMSDir ecal;
  float z=-142.8; 
  
  track->Interpolate(z, ecaltop, ecal, id);
  
  if(fabs(ecaltop.x())>=32.4 || fabs(ecaltop.y())>=32.4 ) return false;
  

  return true;

}


bool good_trd_event(AMSEventR *pev){
  
  if( pev->nTrdTrack()!=1) return false;
  TrdTrackR *trd_track=pev->pTrdTrack(0);

  bool bad=false;
  if(pev->nTrdSegment()>4){
    int arr[20][18][16]={((20*18)*16)*0};
    
    for(int i=0; i<pev->nTrdSegment(); i++){
      TrdSegmentR *seg=pev->pTrdSegment(i);
      for(int j=0; j<seg->NTrdCluster(); j++){
	TrdClusterR *clu=seg->pTrdCluster(j);
	TrdRawHitR *hit =clu->pTrdRawHit();
	if(hit->Amp>15){
	  arr[hit->Layer][hit->Ladder][hit->Tube]+=1;
	}
      }
    }
    for( int i=0; i<20; i++){
      for(int j=0; j<18; j++){
	for(int k=0; k<16; k++){
	  if(arr[i][j][k]>1) bad=true;
	}
      }
    }
  }
  if( (pev->nTrdSegment() >4 && bad) || pev->pTrdTrack(0)->NTrdSegment()!=4) return false;

  float TrdChi2=trd_track->Chi2;
 
  if (TrdChi2<0.0 || TrdChi2>=3.0) return false;
  
  int hitsontrack=0;
  int pat[3]={0,0,0};
  int lay[20];
  for(int i=0; i<20;i++) lay[i]=0;

  for(int is=0; is<trd_track->NTrdSegment(); is++){
    TrdSegmentR *seg=trd_track->pTrdSegment(is);
    for(int ih=0; ih<seg->NTrdCluster(); ih++){
      TrdClusterR *clu=seg->pTrdCluster(ih);
      TrdRawHitR *hit=clu->pTrdRawHit();
      if(hit->Amp>10){
	hitsontrack++;
	lay[hit->Layer]++;
      }
    }
  }
    
  for(int i=0; i<4; i++){
    if(lay[i]>0) pat[0]++;
  }
  for(int i=4; i<16; i++){
    if(lay[i]>0) pat[1]++;
  }
  for(int i=16; i<20; i++){
    if(lay[i]>0) pat[2]++;
  }
     
  if(!(pat[0]>2 && pat[1]>8 && pat[2]>2)) return false;
  
  TrTrackR *tr_track=pev->pTrTrack(0);
  int id=tr_track->iTrTrackPar(1,3,1);
  float P=tr_track->GetRigidity(id);

  // TRD point and direction at z=UToF;
  float zpl= 65.2 ;  // UToF;
  AMSPoint trd_pnt0(trd_track->Coo[0], trd_track->Coo[1], trd_track->Coo[2]);
  AMSDir   trd_dir(sin(trd_track->Theta)*cos(trd_track->Phi),sin(trd_track->Theta)*sin(trd_track->Phi), cos(trd_track->Theta) );

  double X_TRD= (zpl-trd_pnt0[2])*trd_dir[0]/trd_dir[2]+trd_pnt0[0];
  double Y_TRD= (zpl-trd_pnt0[2])*trd_dir[1]/trd_dir[2]+trd_pnt0[1];
  AMSPoint trd_pnt(X_TRD,Y_TRD,zpl);

  // Tracker point and direction at z=UToF;
  AMSPoint tk_pnt;
  AMSDir tk_dir;

  tr_track->Interpolate(zpl, tk_pnt, tk_dir, id);

  //float TrdTrkDr=sqrt(pow(trd_pnt[0]-tk_pnt[0],2)+pow(trd_pnt[1]-tk_pnt[1],2));
  float TrdTrkDa=fabs(TMath::ACos(tk_dir.prod(trd_dir))*TMath::RadToDeg()-180);
  
  double const Pi=4*atan(1);
  float TrdTrkDx=trd_pnt[0]-tk_pnt[0];
  float TrdTrkDy=trd_pnt[1]-tk_pnt[1];

  double TrdCutDa =  10.0*fTrd95Da->Eval(fabs(P));
  double TrdCutDx =  10.0*fTrdSigmaDx->Eval(fabs(P));
  double TrdCutDy =  10.0*fTrdSigmaDy->Eval(fabs(P));
     
      
  if (fabs(TrdTrkDx)>TrdCutDx || fabs(TrdTrkDy)>TrdCutDy) return false;
  if( TrdTrkDa>TrdCutDa ) return false;

  return true;
}


bool good_ecal_event(AMSEventR *pev){

  if(pev->nEcalShower()<1) return false;

  EcalShowerR *shr;
  int highE=0;
  float Ehigh=0;

  for(int i=0; i<pev->nEcalShower(); i++){
    shr=pev->pEcalShower(i);
    
    float Edep=shr->EnergyE;
    if(Edep>Ehigh) {
      highE=i;
      Ehigh=Edep;
    }
  }
  
  shr=pev->pEcalShower(highE);

  if(fabs(shr->Entry[0])>=30.6 || fabs(shr->Entry[1])>=30.6) return false;
  if(fabs(shr->Exit[0])>=30.6 || fabs(shr->Exit[1])>=30.6) return false;

  TrTrackR *tr_track=pev->pTrTrack(0);
  int id=tr_track->iTrTrackPar(1,3,1);
  
  AMSPoint tk_pnt;
  AMSDir tk_dir;

  tr_track->Interpolate(shr->CofG[2], tk_pnt, tk_dir, id);

  float d= sqrt(pow(tk_pnt[0]-shr->CofG[0], 2)+ pow(tk_pnt[1]-shr->CofG[1], 2));

  if(d>=3.6) return false;

  return true;

}

float GetTrackerCharge(ParticleR* particle) {  
  BetaR* tof = particle->pBeta();
  float tof_beta = tof->Beta;
  TrTrackR *track=particle->pTrTrack();
  float xx = sqrt(TrCharge::GetMean(TrCharge::kTruncMean|TrCharge::kInner,track,TrCharge::kX,tof_beta).Mean);
  float trk_q_x  = -0.129643 + 0.202016*xx - 0.00208005*xx*xx + 2.60621e-05*xx*xx*xx;
  return trk_q_x;
}

float GetTofCharge(ParticleR* particle) {
  float q_tof = -1;
  if (particle->pCharge()==0) return q_tof;
  ChargeR* charge = particle->pCharge();
  ChargeSubDR* charge_tof = (ChargeSubDR*) charge->getSubD("AMSChargeTOF");
  if (charge_tof==0) return q_tof;
  q_tof = charge_tof->Q;
  return q_tof;
}

int trd_particle(AMSEventR *pev){

  if(pev->nParticle()==1){
    ParticleR *par=pev->pParticle(0);
    
    float tof_charge=GetTofCharge(par);
    float tr_charge=GetTrackerCharge(par);
    
    TrTrackR* track=pev->pTrTrack(0);
    int id=track->iTrTrackPar(1,3,1);
    float rig=track->GetRigidity(id);
  
    double like_ep=0, like_eh=0, like_ph=0;
    if(rig>99.) pev->pParticle(0)->Momentum=99.;
    if(rig<-99.) pev->pParticle(0)->Momentum=-99.;
    // calculate likelihoods 
    //  if(fabs(rig)<100.){
      if(! trdS->ProcessTrdEvt(pev, Debug) ) {
	vector<double>::iterator lr = trdS->TrdLRs.begin();
	//	lr++;
	like_ep= *lr;
	lr++;
	like_ph= *lr;
	lr++;
	like_eh= *lr;
      } 
      // }
      // printf("mom: %f tof charge: %f like_ep: %f like_eh: %f like_ph: %f \n", pev->pParticle(0)->Momentum, tof_charge, like_ep, like_eh, like_ph);
    if(tr_charge<1.5 && tof_charge>0 && tof_charge<1.5 && like_ep <0.54 && like_eh <0.6) return 0; 
    else if(tr_charge<1.5 && tof_charge>0 && tof_charge<1.5 && like_ep >0.85 && like_ph >1.9) return 1; 
    else if(tr_charge>1.5 && tof_charge>1.5 && like_ph < 0.24 && like_eh > 0.8) return 2; 
    else return -1;
  }
  return -1; 
}

int ecal_particle(AMSEventR *pev){
  
  if(pev->nParticle()==1){
    ParticleR *par=pev->pParticle(0);
    
    TrTrackR *track=pev->pTrTrack(0);
    int id=track->iTrTrackPar(1,3,1);
    float rig=track->GetRigidity(id);    
   
    float tof_charge=GetTofCharge(par);
    float tr_charge=GetTrackerCharge(par);  
   
    EcalShowerR *shr;
    int highE=0;
    float Ehigh=0;
    
    for(int i=0; i<pev->nEcalShower(); i++){
      shr=pev->pEcalShower(i);
      
      float Edep=shr->EnergyE;
      float ecal_E=0;
      
      if(Edep>Ehigh){
	highE=i;
	Ehigh=Edep;
      }
    }
    
    shr=pev->pEcalShower(highE);
    
    float ecalbdt=shr->GetEcalBDT();
    float ecal_Ec=shr->EnergyE;
    
    if(tr_charge<1.5 && tof_charge>0 && tof_charge<1.5 &&  fabs(ecal_Ec/rig)>0.85 && fabs(ecal_Ec/rig)<1.45 && ecalbdt>0.5 ) return 0;
    else if(tr_charge<1.5 && tof_charge>0 && tof_charge<1.5 &&  fabs(ecal_Ec/rig)<0.1  && ecalbdt<-0.8) return 1; 
    else if(tr_charge>1.5 && tof_charge>1.5 &&  fabs(ecal_Ec/rig)<0.1  && ecalbdt<-0.8) return 2;
    else return -1;
  }
  return -1;
}
  
