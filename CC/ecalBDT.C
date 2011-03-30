#include "root.h"
#ifdef _ECALBDT_
 #include "ecalBDT.h"
#endif

float EcalShowerR::GetEcalBDT(){

#ifdef _ECALBDT_
  const int nECAL_LAYERs = 18;
  const int nECALBDTVARs = 12;
  
  float LayerEneDep[nECAL_LAYERs];     // Energy in one layer
  
  EcalHitR *hit = NULL;


  ReadBDT* BDTResponse = new ReadBDT( );
  std::vector<double> InputVec(nECALBDTVARs);

  //ECAL variables
  float LayerCOG;            // Center Of Gravity (layer number)
  float LayerSigma;
  float LayerSkewness;
  float LayerKurtosis;
  float ShowEne; 
  float Enedep;
  float L2LFrac;
  float F2LFrac;
  float F2LEnedep;


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
  float S1S3Corr;
  float DifoSumCorr;

  float ECALBDT;  // from the classifier
  
  ShowEne=EnergyC;
  
  
  // Build MVA variables
  Enedep=0.;
  for (int ilayer=0;ilayer<nECAL_LAYERs;ilayer++){
    LayerEneDep[ilayer]=0;
  }
  int layer;
  float ene1;
  int n2DCLUSTERs= NEcal2DCluster();
  for (int i2dcluster=0; i2dcluster<n2DCLUSTERs; i2dcluster++){
    Ecal2DClusterR *cluster2d = pEcal2DCluster(i2dcluster);
    int nCLUSTERs= cluster2d->NEcalCluster();
    for (int icluster=0; icluster<nCLUSTERs; icluster++){
      EcalClusterR *cluster = cluster2d->pEcalCluster(icluster);
      int nclHITs = cluster->NEcalHit();
      for(int ihit=0; ihit<nclHITs;ihit++){
	hit = cluster->pEcalHit(ihit);            
	if(hit->ADC[0]>4){
	  layer= hit->Plane;
	  if (layer>nECAL_LAYERs-1 || layer<0 ) std::cout << "WRONG LAYER NUMBER: " << layer << endl; 
	  ene1= hit->Edep/1000.;
	  Enedep+=ene1;
	  LayerEneDep[layer]+=ene1;
	}    
      }
    }
  }
  
  F2LEnedep=LayerEneDep[0]+LayerEneDep[1];
  F2LFrac=F2LEnedep/Enedep;
  L2LFrac=(LayerEneDep[16]+LayerEneDep[17])/Enedep;
  
  

  LayerCOG=0.;
  LayerSigma=0.;
  LayerSkewness=0.;
  LayerKurtosis=0.;
  for (int ilayer=0;ilayer<nECAL_LAYERs;ilayer++){
    LayerCOG+=LayerEneDep[ilayer]*ilayer;
  }
  if (Enedep>0){
    LayerCOG/=Enedep;
    for (int ilayer=0;ilayer<nECAL_LAYERs;ilayer++){
      LayerSigma+=pow(ilayer-LayerCOG,2)*LayerEneDep[ilayer];
      LayerSkewness+=pow(ilayer-LayerCOG,3)*LayerEneDep[ilayer];
      LayerKurtosis+=pow(ilayer-LayerCOG,4)*LayerEneDep[ilayer];
    }
    LayerSigma/=Enedep;
    LayerSigma=sqrt(LayerSigma);
    LayerSkewness/=Enedep;
    LayerKurtosis/=Enedep;
  }

  // Shower energy overflow correction
  if(TMath::Log10(ShowEne)>10) 
    ShowEne = Enedep *(0.993248 + 0.0188282 * TMath::Log(Enedep)); 

  // Define energy-independent variables
  L2LFracCorr = ( (L2LFrac) - (1.452674 + (-1.472075) * TMath::Power(TMath::Max((float)ShowEne,(float)10.0) , -0.011317)) ) / (0.029917 + (-0.030689) * TMath::Power(TMath::Max((float)ShowEne,(float)10.0) , -0.149615));

  F2LFracCorr = ( (F2LFrac) - (-0.002184 + (0.138853) * TMath::Power(TMath::Max((float)ShowEne,(float)10.0) , -0.553048)) ) / (-0.002029 + (0.055741) * TMath::Power(TMath::Max((float)ShowEne,(float)10.0) , -0.474460));

  F2LEnedepCorr = ( (F2LEnedep) - ((0.169726) * TMath::Power(TMath::Max((float)ShowEne,(float)10.0) , 0.331049)) ) / ((0.077370) * TMath::Power(TMath::Max((float)ShowEne,(float)10.0) , 0.346888));

  Chi2ProfileCorr = ( (Chi2Profile) - (0.525637 + (13.415025) * TMath::Power(TMath::Max((float)ShowEne,(float)10.0) , -0.976891)) ) / (0.137990 + (4.981889) * TMath::Power(TMath::Max((float)ShowEne,(float)10.0) , -0.872135));

  LayerCOGCorr = ( (LayerCOG) - ((5.353758) * TMath::Power(TMath::Max((float)ShowEne,(float)10.0) , 0.112971)) ) / ((0.786314) * TMath::Power(TMath::Max((float)ShowEne,(float)10.0) , -0.048909));

  LayerSigmaCorr = ( (LayerSigma) - (3.735562) ) / ((0.456239) * TMath::Power(TMath::Max((float)ShowEne,(float)10.0) , -0.271576));

  LayerSkewnessCorr = ( (LayerSkewness) - (-153.354798 + (203.498587) * TMath::Power(TMath::Max((float)ShowEne,(float)10.0) , -0.049281)) ) / ((11.192672) * TMath::Power(TMath::Max((float)ShowEne,(float)10.0) , -0.094710));

  LayerKurtosisCorr = ( (LayerKurtosis) - (-1390.691276 + (1982.383303) * TMath::Power(TMath::Max((float)ShowEne,(float)10.0) , -0.013917)) ) / ((141.144221) * TMath::Power(TMath::Max((float)ShowEne,(float)10.0) , -0.215015));

  S1S3Corr = ( (S13R) - (0.699606 + (-0.094959) * TMath::Power(TMath::Max((float)ShowEne,(float)10.0) , -0.234160)) ) / ((0.116839) * TMath::Power(TMath::Max((float)ShowEne,(float)10.0) , -0.286103));

  DifoSumCorr = ( (DifoSum) - (-0.203622 + (0.218995) * TMath::Power(TMath::Max((float)ShowEne,(float)10.0) , -0.035885)) ) / ((0.089697) * TMath::Power(TMath::Max((float)ShowEne,(float)10.0) , -0.263149));

  ShowerRadiusEnergy3cmCorr = ( (Energy3C[0]) - (0.958877 + (0.017709) * TMath::Power(TMath::Max((float)ShowEne,(float)10.0) , -0.212167)) ) / (0.003383 + (0.072859) * TMath::Power(TMath::Max((float)ShowEne,(float)10.0) , -0.973797));

  ShowerRadiusEnergy5cmCorr = ( (Energy3C[1]) - (0.975270 + (0.026045) * TMath::Power(TMath::Max((float)ShowEne,(float)10.0) , -0.126507)) ) / (0.001202 + (0.011107) * TMath::Power(TMath::Max((float)ShowEne,(float)10.0) , -0.655295));

  
  InputVec[0] =(double) L2LFracCorr;
  InputVec[1] =(double) F2LEnedepCorr;
  InputVec[2] =(double) F2LFracCorr;
  InputVec[3] =(double) LayerCOGCorr;
  InputVec[4] =(double) LayerSigmaCorr;
  InputVec[5] =(double) LayerSkewnessCorr;
  InputVec[6] =(double) LayerKurtosisCorr;
  InputVec[7] =(double) Chi2ProfileCorr;
  InputVec[8] =(double) ShowerRadiusEnergy3cmCorr;
  InputVec[9] =(double) ShowerRadiusEnergy5cmCorr;
  InputVec[10]=(double) S1S3Corr;
  InputVec[11]=(double) DifoSumCorr;
     


  //Get ECALBDT
  ECALBDT = (float) BDTResponse->GetMvaValue( InputVec );
  
  delete BDTResponse;

  return ECALBDT;
#else
  cout << "*************** WARNING *****************" << endl;
  cout << "FUNCTION GetEcalBDT is dummy in your current executable or  shared library"<< endl;
  cout << "if you want the method to work remove ecalBDT.o and link again your shared library with the variable ECALBDT set to 1" << endl;
  cout << "CAVEAT: the linking of this method only requires about 12 minutes with g++ and an undefinite time (>1 hour) with icc" << endl; 
  return -1.;
#endif  
}

