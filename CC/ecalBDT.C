#include "root.h"
#ifdef _ECALBDT_
 #include "ecalBDT.h"
#endif

float EcalShowerR::GetEcalBDT(){

#ifdef _ECALBDT_
  const int nECAL_VIEWs = 2;
  const int nECAL_LAYERs = 18;
  const int nECAL_CELLs = 72;
  const int nECALBDTVARs = 12;
  
  EcalHitR *hit = NULL;


  ReadBDT* BDTResponse = new ReadBDT( );
  std::vector<double> InputVec(nECALBDTVARs);

  //ECAL variables
  float LayerCOG=0.;            // Center Of Gravity (layer number)
  float LayerSigma=0.;
  float LayerSkewness=0.;
  float LayerKurtosis=0.;
  float ShowEne; 
  float Enedep=0.;
  float L2LFrac=0.;
  float F2LFrac=0.;
  float F2LEnedep=0.;
  float S1S3x;
  float S1S3y;


  float L2LFracCorr;
  float F2LEnedepCorr;
  float F2LFracCorr;
  float LayerCOGCorr;
  float LayerSigmaCorr;
  float LayerSkewnessCorr;
  float LayerKurtosisCorr;
  float Chi2ProfileCorr;
  float ShowerRadiusEnergy3cmCorr;
  float ShowerRadiusEnergy5cmCorr;
  float S1S3xCorr;
  float S1S3yCorr;
  float DifoSumCorr;

  float ECALBDT;  // from the classifier
  
  float LayerEneDep[nECAL_LAYERs]={0};     // Energy in one layer
  float LayerCentroid[nECAL_LAYERs]={0}; 
  float LayerLateralSigma[nECAL_LAYERs]={0}; 
  float LayerTruncLateralSigma[nECAL_LAYERs]={0}; 
  float LayerTruncEneDep[nECAL_LAYERs]={0}; 
  float HitEneDep[nECAL_LAYERs][nECAL_CELLs]={0};


  ShowEne=EnergyC;
  

  // Build MVA variables
  int icellmax[nECAL_LAYERs]; //cell with max deposit
  float s1[nECAL_LAYERs], s3[nECAL_LAYERs];
  for(int ilayer=0; ilayer<nECAL_LAYERs;ilayer++) {icellmax[ilayer]=0; s1[ilayer]=0; s3[ilayer]=0;}

  //look for max energy deposit per layer
  int n2DCLUSTERs= NEcal2DCluster();
  for (int i2dcluster=0; i2dcluster<n2DCLUSTERs; i2dcluster++){
    Ecal2DClusterR *cluster2d = pEcal2DCluster(i2dcluster);
    int nCLUSTERs= cluster2d->NEcalCluster();
    for (int icluster=0; icluster<nCLUSTERs; icluster++){
      EcalClusterR *cluster = cluster2d->pEcalCluster(icluster);		  
      int nclHITs = cluster->NEcalHit();
      for(int ihit=0; ihit<nclHITs;ihit++){
	hit = cluster->pEcalHit(ihit);
	if(hit->Edep > s1[hit->Plane]){
	  icellmax[hit->Plane]=hit->Cell;
	  s1[hit->Plane]=hit->Edep;
	  s3[hit->Plane]=hit->Edep;
	}
      }
    }
  }
  
  // compute S3
  for (int i2dcluster=0; i2dcluster<n2DCLUSTERs; i2dcluster++){
    Ecal2DClusterR *cluster2d = pEcal2DCluster(i2dcluster);
    int nCLUSTERs= cluster2d->NEcalCluster();
    for (int icluster=0; icluster<nCLUSTERs; icluster++){
      EcalClusterR *cluster = cluster2d->pEcalCluster(icluster);		  
      int nclHITs = cluster->NEcalHit();
      for(int ihit=0; ihit<nclHITs;ihit++){
	hit = cluster->pEcalHit(ihit);
	if(TMath::Abs( (hit->Cell) - (icellmax[hit->Plane])) == 1) s3[hit->Plane]+=hit->Edep;
      }
    }
  }

  //S1S3 per view
  float ss1[nECAL_VIEWs]={0};
  float ss3[nECAL_VIEWs]={0};
  for(int ilayer=0; ilayer<nECAL_LAYERs; ilayer++){
    int sl=(int) ilayer/2;
    int view=(int) sl%2;    //0->Y, 1->X
    ss1[view]+=s1[ilayer];
    ss3[view]+=s3[ilayer];
  }            
  ss3[1]!=0 ? S1S3x=ss1[1]/ss3[1] : S1S3x=-9999.;
  ss3[0]!=0 ? S1S3y=ss1[0]/ss3[0] : S1S3y=-9999.;
  //

  int HitPerLayer[nECAL_LAYERs]={0};//auxiliary variable for sigma calc

  for (int i2dcluster=0; i2dcluster<n2DCLUSTERs; i2dcluster++){
    Ecal2DClusterR *cluster2d = pEcal2DCluster(i2dcluster);
    int nCLUSTERs= cluster2d->NEcalCluster();
    for (int icluster=0; icluster<nCLUSTERs; icluster++){
      EcalClusterR *cluster = cluster2d->pEcalCluster(icluster);		  
      int nclHITs = cluster->NEcalHit();
      for(int ihit=0; ihit<nclHITs;ihit++){
	hit = cluster->pEcalHit(ihit);	
	if(hit->ADC[0]>4){ //Cell is hit
	  //if(HitEneDep[hit->Plane][hit->Cell]!=0) printf("Warning: Run:%d Event%d Rewriting HitEneDep[%d][%d]\n",Run(),Event(),hit->Plane,hit->Cell); 
	  HitPerLayer[hit->Plane]++;
	  HitEneDep[hit->Plane][hit->Cell]=+hit->Edep;
	  Enedep+=hit->Edep;
	  if(hit->Plane==16 || hit->Plane==17) L2LFrac+=hit->Edep;
	  if(hit->Plane==0 || hit->Plane==1) F2LFrac+=hit->Edep;
	}
      }
    }
  }

  for(int ilayer=0; ilayer<nECAL_LAYERs; ilayer++){
    for(int icell=0; icell<72; icell++){
      LayerEneDep[ilayer]+=HitEneDep[ilayer][icell];
      LayerCentroid[ilayer]+=icell*HitEneDep[ilayer][icell];
    }
    LayerCentroid[ilayer]/=LayerEneDep[ilayer];	  
  }
      
  F2LEnedep=F2LFrac/1000.;   //MeV to GeV

  if(Enedep!=0){    
    L2LFrac=L2LFrac/Enedep;
    F2LFrac=F2LFrac/Enedep;
    Enedep=Enedep/1000.;   //MeV to GeV
    for(int ilayer=0; ilayer<nECAL_LAYERs; ilayer++){
      LayerEneDep[ilayer]/=1000.;   //MeV to GeV
      LayerCOG+=LayerEneDep[ilayer]*ilayer;
    }
    LayerCOG/=Enedep;
	  
    for(int ilayer=0; ilayer<nECAL_LAYERs; ilayer++){
      LayerSigma+=pow(ilayer-LayerCOG,2)*LayerEneDep[ilayer];
      LayerSkewness+=pow(ilayer-LayerCOG,3)*LayerEneDep[ilayer];
      LayerKurtosis+=pow(ilayer-LayerCOG,4)*LayerEneDep[ilayer];
    }
    LayerSigma/=Enedep;
    LayerSigma=sqrt(LayerSigma);
    LayerSkewness/=Enedep;
    LayerKurtosis/=Enedep;
    
    //LayerLateralSigma
    for(int ilayer=0; ilayer<nECAL_LAYERs; ilayer++){
      if(HitPerLayer[ilayer]>1){
	for(int icell=0;icell<72;icell++){
	  LayerLateralSigma[ilayer]+=pow(icell-LayerCentroid[ilayer],2)*(HitEneDep[ilayer][icell]/1000.);
	  //		  if(fabs(diff)<0.01) cout<<ilayer<<" "<<icell<<" "<<" "<<HitEneDep[ilayer][icell]<<" "<<LayerCentroid[ilayer]<<" "<<LayerLateralSigma[ilayer]<<endl;
	}
	if(LayerEneDep[ilayer]>0){
	  LayerLateralSigma[ilayer]/=LayerEneDep[ilayer];
	  //cout<<ilayer<<" "<<LayerLateralSigma[ilayer]<<endl;
	  LayerLateralSigma[ilayer]=sqrt(LayerLateralSigma[ilayer]);
	  //cout<<ilayer<<" "<<LayerLateralSigma[ilayer]<<endl;
	}
	else{
	  LayerLateralSigma[ilayer]=-1.;
	}
      }
    }
    
    //LayerLateralSigma Truncated
    float LayerTruncEneDep[nECAL_LAYERs];	  
    for(int ilayer=0; ilayer<nECAL_LAYERs; ilayer++){
      if(LayerLateralSigma[ilayer]==-1) LayerTruncLateralSigma[ilayer]=-1;
      else{
	if(LayerLateralSigma[ilayer]==0) LayerTruncLateralSigma[ilayer]=0;
	else {
	  int icellmin = (int)(LayerCentroid[ilayer] -  3*LayerLateralSigma[ilayer]);
	  int icellmax = (int)(LayerCentroid[ilayer] +  3*LayerLateralSigma[ilayer]);
	  //if(icellmin==icellmax-1) for(int icell=0;icell<72;icell++) cout<<HitEneDep[ilayer][icell]<<" ";
	  //   if(icellmin==icellmax-1)  cout<<ilayer<<" "<<LayerCentroid[ilayer]<<" "<<LayerLateralSigma[ilayer]<<" "<<LayerEneDep[ilayer]<<" "<<icellmin<<" "<<HitEneDep[ilayer][icellmin]<<" "<<HitEneDep[ilayer][icellmax]<<" "<<icellmax<<endl;
	  LayerTruncEneDep[ilayer]=0;	  
	  for(int icell=icellmin;icell<=icellmax;icell++){
	    LayerTruncLateralSigma[ilayer]+=pow(icell-LayerCentroid[ilayer],2)*(HitEneDep[ilayer][icell]/1000.);
	    LayerTruncEneDep[ilayer]+=HitEneDep[ilayer][icell]/1000.;
	  }
	  if(LayerTruncEneDep[ilayer]>0){
	    LayerTruncLateralSigma[ilayer]/=LayerTruncEneDep[ilayer];
	    LayerTruncLateralSigma[ilayer]=sqrt(LayerTruncLateralSigma[ilayer]);
	  }
	}
	
      }
      //if(LayerLateralSigma[ilayer]<0.001 && LayerLateralSigma[ilayer]>0) cout<<LayerLateralSigma[ilayer]<<" "<<LayerTruncLateralSigma[ilayer]<<endl;
    }
  }
  

  // Shower energy overflow correction
  if(TMath::Log10(ShowEne)>10) 
    ShowEne = Enedep *(0.993248 + 0.0188282 * TMath::Log(Enedep)); 

  // Define energy-independent variables
  float mean;
  float sigma;  

  mean = 0.617265 + (-0.643634) * TMath::Power(TMath::Max((float)ShowEne,(float)17.5) , -0.030115);
  sigma = 0.017119 + (-0.047646) * TMath::Power(TMath::Max((float)ShowEne,(float)17.5) , -0.648504);
  L2LFracCorr = (L2LFrac - mean) / sigma;

  mean = 0.001111 + (0.175785) * TMath::Power(TMath::Max((float)ShowEne,(float)17.5) , -0.706562);
  sigma = 0.000517 + (0.088094) * TMath::Power(TMath::Max((float)ShowEne,(float)17.5) , -0.724408);
  F2LFracCorr = (F2LFrac - mean) / sigma;

  mean = 14.597099 + (-14.844641) * TMath::Power(TMath::Max((float)ShowEne,(float)17.5) , -0.015038);
  sigma = 7.727252 + (-7.831882) * TMath::Power(TMath::Max((float)ShowEne,(float)17.5) , -0.013117);
  F2LEnedepCorr = (F2LEnedep - mean) / sigma;
  
  mean = 0.534625 + (12.974988) * TMath::Power(TMath::Max((float)ShowEne,(float)17.5) , -0.967365);
  sigma = 0.139916 + (6.070468) * TMath::Power(TMath::Max((float)ShowEne,(float)17.5) , -0.933346);
  Chi2ProfileCorr = (Chi2Profile - mean) / sigma;

  mean = -28.826965 + (34.054758) * TMath::Power(TMath::Max((float)ShowEne,(float)17.5) , 0.023294);
  sigma = -5.054101 + (5.906145) * TMath::Power(TMath::Max((float)ShowEne,(float)17.5) , -0.008423);
  LayerCOGCorr = (LayerCOG - mean) / sigma;

  mean = 0.097237 + (3.610085) * TMath::Power(TMath::Max((float)ShowEne,(float)17.5) , 0.001628);
  sigma = 0.110153 + (1.169860) * TMath::Power(TMath::Max((float)ShowEne,(float)17.5) , -0.935832);
  LayerSigmaCorr = (LayerSigma - mean) / sigma;
  
  mean = -177.523116 + (227.487222) * TMath::Power(TMath::Max((float)ShowEne,(float)17.5) , -0.043674);
  sigma = -100.412388 + (109.622530) * TMath::Power(TMath::Max((float)ShowEne,(float)17.5) , -0.003906);
  LayerSkewnessCorr = (LayerSkewness - mean) / sigma;

  mean = 849.242604 + (-285.617632) * TMath::Power(TMath::Max((float)ShowEne,(float)17.5) , 0.064228);
  sigma = 828.084113 + (-730.358404) * TMath::Power(TMath::Max((float)ShowEne,(float)17.5) , 0.013514);
  LayerKurtosisCorr = (LayerKurtosis - mean) / sigma;

  mean = 0.962830 + (0.038170) * TMath::Power(TMath::Max((float)ShowEne,(float)17.5) , -0.559655);
  sigma = 0.002703 + (0.035826) * TMath::Power(TMath::Max((float)ShowEne,(float)17.5) , -0.724716);
  ShowerRadiusEnergy3cmCorr = (Energy3C[0] - mean) / sigma;

  mean = 0.986180 + (0.036482) * TMath::Power(TMath::Max((float)ShowEne,(float)17.5) , -0.502468);
  sigma = 0.001080 + (0.015345) * TMath::Power(TMath::Max((float)ShowEne,(float)17.5) , -0.725059);
  ShowerRadiusEnergy5cmCorr = (Energy3C[1] - mean) / sigma;
  
  mean = 0.694679 + (-0.017440) * TMath::Power(TMath::Max((float)ShowEne,(float)17.5) , -0.167898);
  sigma = 0.049645 + (56.399974) * TMath::Power(TMath::Max((float)ShowEne,(float)17.5) , -2.890351);
  S1S3xCorr = (S1S3x - mean) / sigma;

  mean = 0.798384 + (-0.192606) * TMath::Power(TMath::Max((float)ShowEne,(float)17.5) , -0.089215);
  sigma = 0.050891 + (6.223029) * TMath::Power(TMath::Max((float)ShowEne,(float)17.5) , -1.702355);
  S1S3yCorr = (S1S3y - mean) / sigma;

  mean = -0.026374 + (0.154984) * TMath::Power(TMath::Max((float)ShowEne,(float)17.5) , -0.708300);
  sigma = 0.024670 + (4.395653) * TMath::Power(TMath::Max((float)ShowEne,(float)17.5) , -1.942318);
  DifoSumCorr = (DifoSum - mean) / sigma;
  
  InputVec[0] =(double) L2LFracCorr;
  InputVec[1] =(double) F2LEnedepCorr;
  InputVec[2] =(double) F2LFracCorr;
  InputVec[3] =(double) LayerCOGCorr;
  InputVec[4] =(double) LayerSigmaCorr;
  InputVec[5] =(double) LayerSkewnessCorr;
  InputVec[6] =(double) LayerKurtosisCorr;
  InputVec[7] =(double) ShowerRadiusEnergy3cmCorr;
  InputVec[8] =(double) ShowerRadiusEnergy5cmCorr;
  InputVec[9] =(double) S1S3xCorr;
  InputVec[10]=(double) S1S3yCorr;
  InputVec[11]=(double) DifoSumCorr;
     

  //Get ECALBDT
  ECALBDT = (float) BDTResponse->GetMvaValue( InputVec );
  
  delete BDTResponse;

  return ECALBDT;
#else
  cout << "*************** WARNING *****************" << endl;
  cout << "FUNCTION GetEcalBDT is dummy in your current executable or  shared library"<< endl;
  cout << "if you want the method to work remove ecalBDT.o and link again your shared library with the variable ECALBDT set to 1" << endl;
  cout << "CAVEAT: the linking of this method only requires about 6 minutes on ams.cern.ch with icc64" << endl; 
  return -1.;
#endif  
}

