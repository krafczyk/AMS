#include "TrSim.h"
#include "tkdcards.h"
#include "random.h"

#ifndef __ROOTSHAREDLIBRARY__
#include "amsgobj.h"
#include "trrec.h"
#else
#include "TrRawCluster.h"
#include "TrMCCluster.h"
#include "TrCluster.h"
#include "TrRecHit.h"
#endif

extern "C" double rnormx();

AMSPoint TrSim::sitkrefp[trconst::maxlay];
AMSPoint TrSim::sitkangl[trconst::maxlay];


TrMap<TrMCClusterR> TrSim::MCClusterTkIdMap;

// TrSim* TrSim::GetHead() {
//   if (Head==0) new TrSim();
//   return Head;
// } 

// TrSim::TrSim() {}
//   printf("TrSim::TrSim() called: generating defaults for simulation type %d\n",TRMCFFKEY.SimulationType);
//   if (Head==0) {
//     Head = this;
//    //  if (TRMCFFKEY.SimulationType!=kTrSim2010) return;
// //     // only in case of TrSim2010 simulation 
// //     for (int type=0; type<=2; type++) _sensors.push_back(new TrSimSensor(type));
// //     _glo2loc = new TkSens(1);
//   }
//   else {
//     if (WARNING) printf("TrSim::TrSim-Warning the TrSim singleton has been already defined\n");
//   }

//   /* PARTICLE LIST
//   for(int ipart=0;ipart<1000;ipart++){
//     char chp[22]="";
//     integer itrtyp=0;
//     geant mass=0;
//     geant charge=0;
//     geant tlife=0;
//     geant ub[1];
//     integer nwb=0;
//     GFPART(ipart,chp,itrtyp,mass,charge,tlife,ub,nwb);
//     printf("%d %g %g\n",ipart,mass,charge);
//   }
//   */
// }





void TrSim::sitkhits(int idsoft, float vect[], float edep, float step, int itra) {
#ifndef __ROOTSHAREDLIBRARY__
  AMSgObj::BookTimer.start("SITKHITS");
#endif
  // XXPLSS | XX=sensor| P 0 neg 1 pos | L=layer | SS=slot |
  // int sensor = abs(idsoft)/10000;
  int tkid   = abs(idsoft)%1000;
  int ss     = abs(idsoft)%10000-tkid;
  if(!ss) tkid*=-1;
  TkLadder* lad=TkDBc::Head->FindTkId(tkid);
  // Skip not connected ladders
  if(!lad|| lad->GetHwId()==0) return;


  // fast simulation?
  if (TRMCFFKEY.SimulationType==kNoRawSim) {
    gencluster(idsoft, vect, edep, step, itra);
#ifndef __ROOTSHAREDLIBRARY__
    AMSgObj::BookTimer.stop("SITKHITS");
#endif
    return;
  }
  if (edep <= 0) edep = 1e-4;
  AMSPoint ppa (vect[0],vect[1],vect[2]);
  AMSPoint dirg(vect[3],vect[4],vect[5]);
  AMSPoint pmom(vect[3]*vect[6], vect[4]*vect[6], vect[5]*vect[6]);
  AMSPoint ppb = ppa-dirg*step;
  AMSPoint pgl = ppa-dirg*step/2;
  AMSPoint size(TkDBc::Head->_ssize_active[0]/2,TkDBc::Head->_ssize_active[1]/2,TkDBc::Head->_silicon_z/2);
  // Convert global coo into sensor local coo
  // The origin is the first strip of the sensor
  TkSens tksa(tkid, ppa,1);
  TkSens tksb(tkid, ppb,1);
  AMSPoint pa = tksa.GetSensCoo(); pa[2] += size[2];
  AMSPoint pb = tksb.GetSensCoo(); pb[2] += size[2];

  // Set reference points and angles
  double thx = TMath::Abs(TMath::ATan(dirg.x()/dirg.z())*TMath::RadToDeg());
  double thy = TMath::Abs(TMath::ATan(dirg.y()/dirg.z())*TMath::RadToDeg());
  int ily = abs(tkid)/100-1;
  sitkrefp[ily] = pgl;
  sitkangl[ily] = AMSPoint(thx, thy, 0);

  // Range check
  for (int i = 0; i < 3; i++) {
    if (pa[i] < 0) pa[i] = 0;
    if (pb[i] < 0) pb[i] = 0;
    if (pa[i] > 2*size[i]) pa[i] = 2*size[i];
    if (pb[i] > 2*size[i]) pb[i] = 2*size[i];
  }
  // Create a new object
  VCon* aa = GetVCon()->GetCont("AMSTrMCCluster");
  if (aa)
#ifndef __ROOTSHAREDLIBRARY__
    aa->addnext(new AMSTrMCCluster(idsoft, pa, pb, pgl,pmom,edep , itra));
#else
    aa->addnext(new TrMCClusterR(idsoft, pa, pb, pgl,pmom,edep , itra));
#endif
  if (aa) delete aa;
#ifndef __ROOTSHAREDLIBRARY__
  AMSgObj::BookTimer.stop("SITKHITS");
#endif
}


void TrSim::gencluster(int idsoft, float vect[], float edep, float step, int itra) {
  if (edep <= 0) edep = 1e-4;
  AMSPoint ppa (vect[0],vect[1],vect[2]);
  AMSPoint dirg(vect[3],vect[4],vect[5]);
  AMSPoint pmom(vect[3]*vect[6], vect[4]*vect[6], vect[5]*vect[6]);
  AMSPoint ppb = ppa-dirg*step;
  AMSPoint pgl = ppa-dirg*step/2;
  AMSPoint size(TkDBc::Head->_ssize_active[0]/2,
                TkDBc::Head->_ssize_active[1]/2,
                TkDBc::Head->_silicon_z/2);
  //  XXPLSS  |XX=sensor|P 0 neg 1 pos |L=layer |SS=slot|
  int tkid   = abs(idsoft)%1000;
  int ss     = abs(idsoft)%10000-tkid;
  if(!ss) tkid*=-1;
  // Convert global coo into sensor local coo
  // The origin is the first strip of the sensor
  TkSens tksa(tkid, ppa,1);
  TkSens tksb(tkid, ppb,1);
  AMSPoint pa = tksa.GetSensCoo(); pa[2] += size[2];
  AMSPoint pb = tksb.GetSensCoo(); pb[2] += size[2];
  int ily = abs(tkid)/100-1;
  sitkrefp[ily] = pgl;
  TkSens tksc(tkid, pgl,1);
  AMSPoint pc = tksc.GetSensCoo();
  if (pc.x() < 0 || pc.x() > TkDBc::Head->_ssize_active[0]) return;
  VCon* cont=GetVCon()->GetCont("AMSTrCluster");
  double lx = pc.x();
  double ly = pc.y();
  double lz = pc.z();
  int stx = tksc.GetStripX();
  int sty = tksc.GetStripY();
  double ipx = tksc.GetImpactPointX();
  double ipy = tksc.GetImpactPointY();
  int seedx = (ipx > 0) ? 0 : 1;
  int seedy = (ipy > 0) ? 0 : 1;
  if (ipx < 0) stx--;
  if (ipy < 0) sty--;
  double sig0  = 30;
  double sigx  = 5;
  double sigy  = 4;
  int sig_mode = 1;
  double thx = TMath::Abs(TMath::ATan(dirg.x()/dirg.z())*TMath::RadToDeg());
  double thy = TMath::Abs(TMath::ATan(dirg.y()/dirg.z())*TMath::RadToDeg());
  if (sig_mode == 1) {
    sigx = (40-1.6*thx+0.05*thx*thx)/5;
    sigy = (10+0.5*thy)/2.5;
  }
  sitkangl[ily] = AMSPoint(thx, thy, 0);

  double a1x = (ipx > 0) ? sig0*(1-ipx) : -sig0*ipx;
  double a2x = (ipx < 0) ? sig0*(1+ipx) :  sig0*ipx;
  double a1y = (ipy > 0) ? sig0*(1-ipy) : -sig0*ipy;
  double a2y = (ipy < 0) ? sig0*(1+ipy) :  sig0*ipy;

  a1x += rnormx()*sigx; a2x += rnormx()*sigx;
  a1y += rnormx()*sigy; a2y += rnormx()*sigy;
  if (a1x < 0) a1x = 0;
  if (a2x < 0) a2x = 0;
  if (a1y < 0) a1y = 0;
  if (a2y < 0) a2y = 0;
  double asx = a1x+a2x;
  double asy = a1y+a2y;

  if (seedx == 0 && a1x < a2x) seedx = 1;
  if (seedx == 1 && a1x > a2x) seedx = 0;
  if (seedy == 0 && a1y < a2y) seedy = 1;
  if (seedy == 1 && a1y > a2y) seedy = 0;

  int lyr = TMath::Abs(tkid)/100;

  float adc[2] = { 0, 0 };

  float dummy = 0;
  float rndx  = RNDM(dummy);
  float rndy  = RNDM(dummy);
  int  layer = abs(tkid)/100;
  float effx = (layer == 8) ? 0.87 : 0.90;
  float effy = 1;

  // 20-23/Dec/2009 CR test (Offline S/N thd.=3.5)
  if (tkid == -202) effy = 0.25;
  if (tkid ==  213) effx = 0.70;
  if (tkid ==  214) effx = 0.50;
  if (tkid == -405) effx = 0.50;
  if (tkid ==  405) effx = 0;
  if (tkid == -509) effx = 0.75;
  if (tkid == -512) effx = 0.50;
  if (tkid ==  504) effx = 0.70;
  if (tkid ==  510) effx = 0.35;
  

  if (rndx < effx && asx+asy > 0 && 0 < stx && stx+seedx < 383) {
    adc[0] = a1x;
    adc[1] = a2x;
#ifndef __ROOTSHAREDLIBRARY__ 
    if(cont) cont->addnext(new AMSTrCluster
			   (tkid, 0, stx+640, 2, seedx, adc, 0));
#else
    if(cont) cont->addnext(new TrClusterR
			   (tkid, 0, stx+640, 2, seedx, adc, 0));
#endif
  }
  if (rndy < effy && asx+asy > 0 && 0 < sty && sty+seedy < 639) {
    adc[0] = a1y;
    adc[1] = a2y;
#ifndef __ROOTSHAREDLIBRARY__ 
    if (cont) cont->addnext(new AMSTrCluster(tkid, 0, sty, 2, seedy, adc, 0));
#else
    if (cont) cont->addnext(new   TrClusterR(tkid, 0, sty, 2, seedy, adc, 0));
#endif
  }

  delete cont;
  return; 
}


void TrSim::fillreso(TrTrackR *track) {
  if (fabs(track->GetRigidity()) < 100) return;
  for (int i = 0; i < track->GetNhits(); i++) {
    TrRecHitR *hit = track->GetHit(i);
    int        ily = hit->GetLayer()-1;
    AMSPoint   dpc = hit->GetCoord()-sitkrefp[ily];
    hman.Fill("TrSimRx", sitkangl[ily].x(), dpc.x()*1e4);
    hman.Fill("TrSimRy", sitkangl[ily].y(), dpc.y()*1e4);
  }
}


void TrSim::sitkdigi() {
#ifndef __ROOTSHAREDLIBRARY__
  AMSgObj::BookTimer.start("SITKDIGI");
#endif
  switch (TRMCFFKEY.SimulationType) {
  case kRawSim:
    sitkdigiold();
    break;
  case kNoRawSim:
    return;
    break;
  case kTrSim2010:
    BuildTrRawClusters();
    break;
  }
#ifndef __ROOTSHAREDLIBRARY__
  AMSgObj::BookTimer.stop("SITKDIGI");
#endif
  return;
}


void TrSim::sitkdigiold() {
  //Get the pointer to the TrMCCluster container
  VCon* cont=GetVCon()->GetCont("AMSTrMCCluster");
  if(!cont){
    printf("TrSim::sitkdigi()  Cant Find AMSMCCluster Container Digitization is Impossible !!!\n");
    return ;
  }
  if(cont->getnelem()==0){
    // printf("TrSim::sitkdigi()  AMSTrMCCluster Container is Empty  Digitzation is Impossible !!!\n");
    return ;
  }
  //PZDEBUG else   printf("TrSim::sitkdigi()  AMSTrMCCluster Container has %d elements \n",cont->getnelem());
  //Create the map of TrMCClusters
  TrMap<TrMCClusterR> MCClMap;
  
#ifndef __ROOTSHAREDLIBRARY__
  AMSgObj::BookTimer.start("SITKDIGIa");
#endif
  for (int ii=0;ii<cont->getnelem();ii++){
    TrMCClusterR* clu=(TrMCClusterR*)cont->getelem(ii);
    clu->_shower();
    if (clu)MCClMap.Add(clu);
    //clu->_printEl(cerr); 
  }
#ifndef __ROOTSHAREDLIBRARY__
  AMSgObj::BookTimer.stop("SITKDIGIa");
#endif

#ifndef __ROOTSHAREDLIBRARY__
  AMSgObj::BookTimer.start("SITKDIGIb");
#endif
  //LOOP ON ALL THE LADDERS
  for (int lay=1;lay<=trconst::maxlay;lay++){
    for (int slot=1;slot<=trconst::maxlad;slot++){
      for(int side=-1;side<2;side=side+2){
	int tkid=(lay*100+slot)*side;
	if(!TkDBc::Head->FindTkId(tkid)) continue  ;
	
	TrLadCal *tcal = TrCalDB::Head->FindCal_TkId(tkid);
	if (!tcal) {
	  std::cerr << "ERROR(1) tcal not found: " << tkid << std::endl;
	  continue;
	}
	//	printf("Ladder %+03d Has been selected\n",tkid);
	geant* buf=new geant[1024];
	//Expand the noise
	for(int ii=0;ii<1024;ii++){
	  if(tcal->Status(ii)&TrLadCal::dead)
	    buf[ii]=-1000;
	  else
	    buf[ii]= tcal->Sigma(ii)*rnormx();
	  //printf("%4d: %6.2f ",ii, buf[ii]);
	}
	//expand the TrMCClusters
	//printf("\n");

	for( int icl=0;icl<MCClMap.GetNelem(tkid);icl++){
	  TrMCClusterR* cl=MCClMap.GetElem(tkid,icl);

	  int addK=cl->GetAdd(0);
	  //  printf("ClusterK: ");
	  for (int jj=0;jj<cl->GetSize(0);jj++){
	    buf[addK+jj] += cl->GetSignal(jj,0);
	    //printf(" %4d: %6.2f",addK+jj,cl->GetSignal(jj,0));
	  }
	  //printf("\n");
	  int addS=cl->GetAdd(1);
	  //printf("ClusterS: ");
	  for (int jj=0;jj<cl->GetSize(1);jj++){
	    buf[addS+jj] += cl->GetSignal(jj,1);
	    //printf(" %4d: %6.2f",addS+jj,cl->GetSignal(jj,1));
	  }
	  //printf("\n");
	  // printf("Ladder %+03d Has been selected\n",tkid);
	}
	
	
#ifndef __ROOTSHAREDLIBRARY__
	AMSgObj::BookTimer.start("SITKDIGIc");
#endif

	// Do clusterization the DSP way
	DSP_Clusterize(tkid,buf);


#ifndef __ROOTSHAREDLIBRARY__
  AMSgObj::BookTimer.stop("SITKDIGIc");
#endif
	delete [] buf;
      }

    }
  }
#ifndef __ROOTSHAREDLIBRARY__
  AMSgObj::BookTimer.stop("SITKDIGIb");
#endif

  if(cont) delete cont;
  return;
}


void TrSim::DSP_Clusterize(int tkid,float *buf){
  float general[1024];

  //Get the pointer to the TrMCCluster container
  VCon* cont=GetVCon()->GetCont("AMSTrRawCluster");
  if(!cont){
    printf("TrRecon::DSP_Clusterize  Cant Find AMSTrRawCluster Container Digitization is Impossible !!!\n");
    return ;
  }

  TrLadCal *tcal = TrCalDB::Head->FindCal_TkId(tkid);
  if (!tcal) {
    std::cerr << "TrRecon::DSP_Clusterize ERROR(1) tcal not found: " << tkid << std::endl;
    std::cerr << "Clusterization aborted for ladder " << tkid << std::endl;
    return;
  }
  
  if(!buf){
    std::cerr << "TrRecon::DSP_Clusterize Buffer not available: " << tkid << std::endl;
    std::cerr << "Clusterization aborted for ladder " << tkid << std::endl;
    return;
  }
  int used[1024];
  memset(used,0,1024*sizeof(used[0]));

  for(int ii=0;ii<639;ii++){
//     printf("%4d: used: %d st: %4d S: %6.2f N: %6.2f  SN: %6.2f \n",
// 	   ii,used[ii],tcal->Status(ii),buf[ii],tcal->Sigma(ii),buf[ii]/tcal->Sigma(ii)
// 	   );
    if(used[ii]!=0) continue;
    if(tcal->Status(ii)!=0) continue;
    if(buf[ii]/(tcal->Sigma(ii))<=TRMCFFKEY.th1sim[0]) continue;
    if(buf[ii+1]/(tcal->Sigma(ii+1))>buf[ii]/(tcal->Sigma(ii)) ) continue;
    //SEED found
    //    printf("SEED %d \n",tkid);
    int seed=ii;
    used[ii]=1;
    int left=seed;
    for (int jj=seed-1;jj>=0;jj--)
      if(
	 ((buf[jj]/tcal->Sigma(jj))>TRMCFFKEY.th2sim[0]||tcal->Status(jj)!=0 )
	 && used[jj]==0){
	used[jj]=1;
	left=jj;
      }else break;
    
    int right=seed;
    for (int jj=seed+1;jj<640;jj++)
      if(
	 ((buf[jj]/tcal->Sigma(jj))>TRMCFFKEY.th2sim[0] ||tcal->Status(jj)!=0 )
	 && used[jj]==0){
	used[jj]=1;
	right=jj;
      }else break;

    if(left>0) left--;
    if(right<639) right++;
    //printf("LEFT:  %3d  RIGHT:  %3d \n",left,right);
    //    printf("Adding Raw Clus: %d\n",tkid);
    for(int ll=0;ll<(right-left+1);ll++)
      general[ll]=buf[left+ll];
#ifndef __ROOTSHAREDLIBRARY__
    cont->addnext(new AMSTrRawCluster(tkid, left, right-left, general));
#else
    cont->addnext(new TrRawClusterR(tkid, left, right-left, general));
#endif
  }

  for(int ii=640;ii<1024;ii++){

    if(used[ii]!=0) continue;
    if(tcal->Status(ii)!=0) continue;
    if(buf[ii]/(tcal->Sigma(ii))<=TRMCFFKEY.th1sim[1]) continue;
    if(ii+1<1024 && buf[ii+1]/(tcal->Sigma(ii+1))>buf[ii]/(tcal->Sigma(ii)) ) continue;
    //SEED found
    int seed=ii;
    used[ii]=1;
    int left=seed;
    for (int jj=seed-1;jj>=640;jj--)
      if(((buf[jj]/(tcal->Sigma(jj)))>TRMCFFKEY.th2sim[1])&& used[jj]==0){
	used[jj]=1;
	left=jj;
      }    else break;

    int right=seed;
    for (int jj=seed+1;jj<1024;jj++)
      if(((buf[jj]/(tcal->Sigma(jj)))>TRMCFFKEY.th2sim[1])&& used[jj]==0){
	used[jj]=1;
	right=jj;
      }else break;

    if(left>640) left--;
    if(right<1023) right++;
    //printf("LEFT:  %3d  RIGHT:  %3d \n",left,right);
    //    printf("Adding Raw Clus: %d\n",tkid);
    for(int ll=0;ll<(right-left+1);ll++)
      general[ll]=buf[left+ll];
#ifndef __ROOTSHAREDLIBRARY__
    cont->addnext(new AMSTrRawCluster(tkid, left, right-left, general));
#else
    cont->addnext(new TrRawClusterR(tkid, left, right-left, general));
#endif
  }
  if(cont) delete cont;

}


////////////////////
/// TrSim2010
////////////////////


int TrSim::BuildTrRawClusters() {
  int nclusters = 0;

#ifndef __ROOTSHAREDLIBRARY__
  AMSgObj::BookTimer.start("SITKDIGIa");
#endif
  
  // Create the map and produce the simulated cluster
  CreateMCClusterTkIdMap();
  
#ifndef __ROOTSHAREDLIBRARY__
  AMSgObj::BookTimer.stop("SITKDIGIa");
#endif
  

#ifndef __ROOTSHAREDLIBRARY__
    AMSgObj::BookTimer.start("SITKDIGIb");
#endif  
  // Loop on all the Ladders 
  for (int ientry=0; ientry<TkDBc::Head->GetEntries(); ientry++) { 
    TkLadder* ladder = TkDBc::Head->GetEntry(ientry);
    int _tkid = ladder->GetTkId();
    
    // Calibration
    TrLadCal* _ladcal = TrCalDB::Head->FindCal_TkId(_tkid);
    if (_ladcal==0) {
      if (WARNING) printf("TrSim::AddNoiseOnBuffer-Warning no ladder calibration found, no noise for ladder tkid=%+04d\n",_tkid);
      continue;
    }


    //Create the ladder buffer and intialize it 
    double    _ladbuf[1024];
    memset(_ladbuf,0,1024*sizeof(_ladbuf[0]));
    if(MCClusterTkIdMap.GetNelem(_tkid)>0)
      AddSimulatedClustersOnBuffer(_tkid,_ladbuf,_ladcal);
    if(  TRMCFFKEY.TrSim2010_NoiseType  != 1) continue;
    
    AddNoiseOnBuffer(_ladbuf,_ladcal);


    
    // Add Noise? 
    // Add Fake Cluster? 
#ifndef __ROOTSHAREDLIBRARY__
    AMSgObj::BookTimer.start("SITKDIGIc");
#endif
    int nclusters = 0;
    // N side
    nclusters += BuildTrRawClustersWithDSP(0, _tkid,_ladcal, _ladbuf);         
    // P side
    nclusters += BuildTrRawClustersWithDSP(1, _tkid,_ladcal, _ladbuf);         
#ifndef __ROOTSHAREDLIBRARY__
    AMSgObj::BookTimer.stop("SITKDIGIc");
#endif
    
  }
#ifndef __ROOTSHAREDLIBRARY__
    AMSgObj::BookTimer.stop("SITKDIGIb");
#endif
  return nclusters;
}



void TrSim::AddNoiseOnBuffer(double* _ladbuf,TrLadCal * _ladcal) {
// #ifndef __ROOTSHAREDLIBRARY__
//   AMSgObj::BookTimer.start("SITKNOISE");
// #endif

  if (TRMCFFKEY.TrSim2010_NoiseType==0) return;
  // Noise baseline and calibration load
  for (int ii=0; ii<1024; ii++) {
    if (_ladcal->GetStatus(ii)&TrLadCal::dead) _ladbuf[ii] = DEADSTRIPADC;                // dead strip    
    else {
      // NoiseType=2: Add noise on only the strips near the signal
      if (TRMCFFKEY.TrSim2010_NoiseType==2) {
        if (_ladbuf[ii] > 0 ||
            (ii+1 < 1024 && _ladbuf[ii+1] > 0) ||
            (ii-1 >=   0 && _ladbuf[ii-1] > 0))
          _ladbuf[ii] += _ladcal->Sigma(ii)*rnormx();
      }
      // NoiseType=1: Add noise on all the strips
      else
        _ladbuf[ii] += _ladcal->Sigma(ii)*rnormx(); // normal/hot strip
    }
  }

// #ifndef __ROOTSHAREDLIBRARY__
//   AMSgObj::BookTimer.stop("SITKNOISE");
// #endif
}


void TrSim::CreateMCClusterTkIdMap() {
  MCClusterTkIdMap.Clear();
  // Get the pointer to the TrMCCluster container
  VCon* container = GetVCon()->GetCont("AMSTrMCCluster");
  if (!container) {
    if (WARNING) printf("TrSim::CreateMCClusterTkIdMap-Warning no TrMCCluster container\n");
    return;
  }
  if (container->getnelem()==0) {
    if (WARNING) printf("TrSim::CreateMCClusterTkIdMap-Warning TrMCCluster container is empty\n");
    return;
  }
  for (int ii=0; ii<container->getnelem(); ii++) {
    TrMCClusterR* cluster = (TrMCClusterR*) container->getelem(ii);
    if (cluster){cluster->GenSimClusters(); MCClusterTkIdMap.Add(cluster);}
  }
} 


int TrSim::AddSimulatedClustersOnBuffer(int _tkid,double* _ladbuf,TrLadCal * _ladcal) {
#ifndef __ROOTSHAREDLIBRARY__
  AMSgObj::BookTimer.start("SITKDIGIa");
#endif

  int nclusters = 0;
  // Loop on MC Cluster on the selected ladder 
  for(int icl=0; icl<MCClusterTkIdMap.GetNelem(_tkid); icl++) {
    TrMCClusterR* cluster = (TrMCClusterR*) MCClusterTkIdMap.GetElem(_tkid,icl);
    if (cluster==0) continue;  

    if (VERBOSE) {
      printf("TrSim::MCCluster\n");
      cluster->Print();
    }
  }
#ifndef __ROOTSHAREDLIBRARY__
  AMSgObj::BookTimer.stop("SITKDIGIa");
#endif
  return nclusters;
}




int TrSim::BuildTrRawClustersWithDSP(const int iside, const int _tkid,  TrLadCal* _ladcal,double * _ladbuf) {
  // Does the AMSTrRawCluster container exist?
  VCon* cont = GetVCon()->GetCont("AMSTrRawCluster");
  if(!cont){
    if (WARNING) printf("TrSim::BuildTrRawClustersOnSide-Warning cannot find AMSTrRawCluster, no cluster produced\n");
    return 0;
  } 

  // Does the calibration exist?
  if (!_ladcal) {
    if (WARNING) printf("TrSim::BuildTrRawClustersOnSide-Warning no ladder calibration found, no noise for ladder tkid=%+04d\n",_tkid);
    return 0;
  }

  // Init
  int nclusters = 0;
  int addmin[2] = {  0, 640};
  int addmax[2] = {640,1024};
  int used[1024]; memset(used,0,1024*sizeof(used[0]));

  // Loop on address (0 - 640, or 640 - 1024)
  for (int ii=addmin[iside]; ii<addmax[iside]; ii++) {

    if (used[ii]!=0)            continue; // a strip used in a cluster 
    if (_ladcal->Status(ii)!=0) continue; // a strip with status different from 0
    if ( (_ladbuf[ii]/_ladcal->Sigma(ii))<=TRMCFFKEY.TrSim2010_DSPSeedThr[iside] ) continue; // a strip under seed threshold

    // "Seed" Found (not max)
    int seed = ii;
    used[ii] = 1;
    int maxstrip = ii;

    // Left Loop
    int left = seed;
    for (int jj=seed-1; jj>=addmin[iside]; jj--) {
      if ( (((_ladbuf[jj]/_ladcal->Sigma(jj))>TRMCFFKEY.TrSim2010_DSPNeigThr[iside])||(_ladcal->Status(jj)!=0)) && (used[jj]==0) ) {
        used[jj] = 1;
        left = jj;
      } 
      else 
        break;
    }
 
    // Right Loop
    int right = seed;
    for (int jj=seed+1; jj<addmax[iside]; jj++) {
      if ( (((_ladbuf[jj]/_ladcal->Sigma(jj))>TRMCFFKEY.TrSim2010_DSPNeigThr[iside]) || (_ladcal->Status(jj)!=0)) && used[jj]==0) {
        used[jj] = 1;
        right = jj;
      } 
      else 
        break;
    }

    // at least 3 strips!
    if(left >addmin[iside]) left--;
    if(right<addmax[iside]) right++;

    // Preparing the cluster
    short int signal[128];
    int nstrips = right - left + 1;
    for(int ist=0; ist<nstrips; ist++) { 
      signal[ist%128] = int(8*_ladbuf[left + ist]); 
      if ( (((ist+1)%128)==0) || ((ist+1)==nstrips) ) { 
        int cluslenraw = (((ist+1)%128)==0) ? 127 : (nstrips%128)-1; // 0->1, 1->2, ..., 127->128
        int seedvalue  = int(4*_ladbuf[maxstrip]);
        // cluslenraw |= (seedvalue<<7);
        int n128 = int((ist+1)/128);
        int clusaddraw = left + 128*n128;
        // clusaddraw |= (0<<10); // CN bits status (not simulated)
        // clusaddraw |= (0<<14); // Power Bits status (not simulated)
#ifndef __ROOTSHAREDLIBRARY__
        if (cont) cont->addnext(new AMSTrRawCluster(_tkid,clusaddraw,cluslenraw,signal));
#else
        if (cont) cont->addnext(new TrRawClusterR(  _tkid,clusaddraw,cluslenraw,signal));
#endif
	nclusters++;
        if (VERBOSE) {
          printf("TrSim::TrRawCluster\n");
          TrRawClusterR* cluster = (TrRawClusterR*) cont->getelem((int)cont->getnelem()-1);
          cluster->Print(10);
          printf("TrSim::TrRawCluster-More Local Seed Coordinate = %f\n",TkCoo::GetLocalCoo(_tkid,cluster->GetSeedAddress(),0));
        }
      }
    }
  }
  if (cont) delete cont;
  return nclusters;
}


void TrSim::PrintBuffer(double *_ladbuf) {
  for (int ii=0; ii<64; ii++) {
    for (int jj=0; jj<16; jj++) {
      printf("%5.2f ",_ladbuf[ii*16+jj]);
    }
    printf("\n");
  }
}




int TrSim::AddSimClusterNew(TrMCClusterR* cluster, double* _ladbuf){
  int nclusters=0;
  // Loop on two Sides of the ladder
  for (int iside=0; iside<2; iside++) {
    TrSimCluster* simcluster = cluster->GetSimCluster(iside);
    if (simcluster==0) continue; // it happens! 
    // Putting cluster on the ladder buffer 
    for (int ist=0; ist<simcluster->GetWidth(); ist++) {
      int address = ist + simcluster->GetAddress() + 640*(1-iside); // address on buffer (P and N side together)
      // sometimes address is out of range and cause crash
      if (0 <= address && address < 1024)
	_ladbuf[address] += simcluster->GetSignal(ist); 
    }
    nclusters++;
  }
  return nclusters;  
}
