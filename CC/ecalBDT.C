#include "root.h"
#include "TMVA/Tools.h"
#include "TMVA/Version.h"
#include "TH1F.h"

TMVA::Reader *ecalpisareader_v3 = NULL;
TMVA::Reader *ecalpisareader_v4 = NULL;
TMVA::Reader *ecalpisareader_v5_ODD = NULL;      //uses EnergyD for classification
TMVA::Reader *ecalpisareader_v5_EVEN = NULL;     //uses EnergyD for classification
TMVA::Reader *ecalpisareaderS_v5_ODD = NULL;     //uses EnergyD for classification
TMVA::Reader *ecalpisareaderS_v5_EVEN = NULL;    //uses EnergyD for classification
TMVA::Reader *ecalpisareader_E_v5_ODD = NULL;    //uses EnergyE for classification
TMVA::Reader *ecalpisareader_E_v5_EVEN = NULL;   //uses EnergyE for classification
TMVA::Reader *ecalpisareaderS_E_v5_ODD = NULL;   //uses EnergyE for classification
TMVA::Reader *ecalpisareaderS_E_v5_EVEN = NULL;  //uses EnergyE for classification
TMVA::Reader *ecalpisareader_v6_ODD = NULL;      //uses EnergyD for classification
TMVA::Reader *ecalpisareader_v6_EVEN = NULL;     //uses EnergyD for classification
TMVA::Reader *ecalpisareaderS_v6_ODD = NULL;     //uses EnergyD for classification
TMVA::Reader *ecalpisareaderS_v6_EVEN = NULL;    //uses EnergyD for classification
TMVA::Reader *ecalpisareader_E_v6_ODD = NULL;    //uses GetCorrectedEnergy(2,2) for classification
TMVA::Reader *ecalpisareader_E_v6_EVEN = NULL;   //uses GetCorrectedEnergy(2,2) for classification
TMVA::Reader *ecalpisareaderS_E_v6_ODD = NULL;   //uses GetCorrectedEnergy(2,2) for classification
TMVA::Reader *ecalpisareaderS_E_v6_EVEN = NULL;  //uses GetCorrectedEnergy(2,2) for classification
TMVA::Reader *ecalpisareader_v7_ODD = NULL;      //uses EnergyD for classification
TMVA::Reader *ecalpisareader_v7_EVEN = NULL;     //uses EnergyD for classification
TMVA::Reader *ecalpisareaderS_v7_ODD = NULL;     //uses EnergyD for classification
TMVA::Reader *ecalpisareaderS_v7_EVEN = NULL;    //uses EnergyD for classification
TMVA::Reader *ecalpisareader_E_v7_ODD = NULL;    //uses GetCorrectedEnergy(2,2) for classification
TMVA::Reader *ecalpisareader_E_v7_EVEN = NULL;   //uses GetCorrectedEnergy(2,2) for classification
TMVA::Reader *ecalpisareaderS_E_v7_ODD = NULL;   //uses GetCorrectedEnergy(2,2) for classification
TMVA::Reader *ecalpisareaderS_E_v7_EVEN = NULL;  //uses GetCorrectedEnergy(2,2) for classification

const unsigned int nPISABDTVARs = 61;
float pisanormvar[nPISABDTVARs + 1];
float pisainputvar[nPISABDTVARs + 1];
char *pisavarname[nPISABDTVARs + 1] = {
   "ShowerMean", "ShowerSigma",
   "L2LFrac", "R3cmFrac", "S3totx", "S3toty",
   "NEcalHits",
   "ShowerFootprintX", "ShowerFootprintY",
   "LayerEneFrac0",  "LayerEneFrac1",  "LayerEneFrac2",  "LayerEneFrac3",  "LayerEneFrac4",  "LayerEneFrac5",
   "LayerEneFrac6",  "LayerEneFrac7",  "LayerEneFrac8",  "LayerEneFrac9",  "LayerEneFrac10", "LayerEneFrac11",
   "LayerEneFrac12", "LayerEneFrac13", "LayerEneFrac14", "LayerEneFrac15", "LayerEneFrac16", "LayerEneFrac17",
   "LayerSigma1",    "LayerSigma2",    "LayerSigma3",    "LayerSigma4",    "LayerSigma5",    "LayerSigma6",
   "LayerSigma7",    "LayerSigma8",    "LayerSigma9",    "LayerSigma10",   "LayerSigma11",   "LayerSigma12",
   "LayerSigma13",   "LayerSigma14",   "LayerSigma15",   "LayerSigma16",   "LayerSigma17",
   "LayerS3Frac1",   "LayerS3Frac2",   "LayerS3Frac3",   "LayerS3Frac4",   "LayerS3Frac5",   "LayerS3Frac6",
   "LayerS3Frac7",   "LayerS3Frac8",   "LayerS3Frac9",   "LayerS3Frac10",  "LayerS3Frac11",  "LayerS3Frac12",
   "LayerS3Frac13",  "LayerS3Frac14",  "LayerS3Frac15",  "LayerS3Frac16",  "LayerS3Frac17",
   "EnergyD"
};

bool MDT_DEBUG = false;
bool BDT_DEBUG = false;
bool BDT_HISTOS = false;
bool BDT_HISTOS_DECLARE = true;
int iVersionNumberBDT=0;
int ECALBDT_TMVAVER = 0;
TH1F *hECALBDT[nPISABDTVARs];

float EcalShowerR::GetEcalBDT()
{
  AMSEventR *pev = AMSEventR::Head();
  unsigned int iBDTVERSION = 7;
  int TMVAClassifier=0;
  return GetEcalBDT(pev, iBDTVERSION, TMVAClassifier);
}

float EcalShowerR::GetEcalBDT(unsigned int iBDTVERSION)
{
  AMSEventR *pev = AMSEventR::Head();
  int TMVAClassifier=0;
  return GetEcalBDT(pev, iBDTVERSION, TMVAClassifier);
}

float EcalShowerR::GetEcalBDT(unsigned int iBDTVERSION, int TMVAClassifier)
{
  AMSEventR *pev = AMSEventR::Head();
  return GetEcalBDT(pev, iBDTVERSION, TMVAClassifier);
}

float EcalShowerR::GetEcalBDT(unsigned int iBDTVERSION, int TMVAClassifier, int EnergyFlag)
{
  AMSEventR *pev = AMSEventR::Head();
  return GetEcalBDT(pev, iBDTVERSION, TMVAClassifier, EnergyFlag);
}

float EcalShowerR::GetEcalBDT(AMSEventR *pev, unsigned int iBDTVERSION)
{
  int TMVAClassifier=0;
  return GetEcalBDT(pev, iBDTVERSION, TMVAClassifier);
}

float EcalShowerR::GetEcalBDT(AMSEventR *pev, unsigned int iBDTVERSION, int TMVAClassifier)
{
  int EnergyFlag=0;
  return GetEcalBDT(pev, iBDTVERSION, TMVAClassifier, EnergyFlag);
}

float EcalShowerR::GetEcalBDT(AMSEventR *pev, unsigned int iBDTVERSION, int TMVAClassifier, int EnergyFlag)
{
  
   if (BDT_DEBUG)
     {
       std::cout << " ??? EcalShowerR::GetEcalBDT() \n";
       std::cout << " ??? ******************************\n";
       std::cout << " ??? *****Compute the variables****\n";
       std::cout << " ??? ******************************\n" << flush;
     }

   if (BDT_HISTOS)
     {
       if (BDT_HISTOS_DECLARE)
	 {
	   for (int i=0; i<nPISABDTVARs; i++)
	     {
	       std::cout<<" GetEcalBDT :: Defining histogram hECALBDT["<<i<<"]\n"<<flush;
	       hECALBDT[i] = new TH1F(Form("hECALBDT_%02d",i),Form("hECALBDT_%02d",i),1000,-10.,10.);
	     }
	   BDT_HISTOS_DECLARE = false;
	 }
     }

   if (ECALBDT_TMVAVER==0)
     {
       ECALBDT_TMVAVER = 412;
       if ( TMVA_RELEASE == "4.1.4" ) ECALBDT_TMVAVER = 414;
       //For BDT versions >6 only 4.1.4 used for training
       if ( iBDTVERSION>6 )  ECALBDT_TMVAVER = 414;
     }
   //Check if the version is > 100. In this case, it means that we want to use version X-100 and do not apply the cuts on F2SL and s1s3
   //Example: iBDTVERSION==103 means v3 but no cuts applied 
   bool apply_precuts = true;
   if( iBDTVERSION>=100 ) { iBDTVERSION-=100; apply_precuts = false; }

   const unsigned int nLAYERs = 18;
   const unsigned int nCELLs  = 72;
   const Float_t EneDepThreshold = 2.;//threshold on single cell in MeV (1MeV~2ADC)

   float MapEneDep[nLAYERs][nCELLs]; // Energy deposit in every cell of ECAL [GeV]
   float ClusterEnergy[nLAYERs];     // Lateral leak corrected energy deposit [GeV]
   float LayerEneDep[nLAYERs];       // Energy deposit [GeV] in each layer (sum of every cell of each layer)

   float LayerEnergy = 0.; // sum_i ClusterEnergy[i]

   float EneDep = 0.;
   float EneDepXY[2]; // 0 = x, 1 = y

   float ClusterEnergyXY[2]; // 0 = x, 1 = y

   float LayerMean[nLAYERs]; // Mean [cell] for each layer: (sum_j j*MapEneDep[i][j])/(sum_ij MapEneDep[i][j])
   float LayerSigma[nLAYERs];
   float LayerEneFrac[nLAYERs];
   float LayerS1S3[nLAYERs];
   float LayerS3Frac[nLAYERs];

   float ShowerMean  = 0.;
   float ShowerSigma = 0.;
   float L2LFrac = 0.;
   float S3totx = 0.;
   float S3toty = 0.;
   float R3cmFrac = Energy3C[0];
   float ShowerFootprintX = 0.;
   float ShowerFootprintY = 0.;
   float NEcalHits = 0.;

   float ShowerMeanXY[2]; // 0 = x, 1 = y

   float sigmaXY[2];  // 0 = x, 1 = y
   float sigmaXYZ[2]; // 0 = x, 1 = y
   float sigmaZ[2];   // 0 = x, 1 = y

   float F2SLEneDep = 0.;

   // Initialize array variables
   for (unsigned int iproj = 0; iproj < 2; ++iproj)
   {
      EneDepXY[iproj]        = 0.;
      ClusterEnergyXY[iproj] = 0.;
      ShowerMeanXY[iproj]    = 0.;
      sigmaXY[iproj]         = 0.;
      sigmaXYZ[iproj]        = 0.;
      sigmaZ[iproj]          = 0.;
   }

   for (unsigned int ilayer = 0; ilayer < nLAYERs; ++ilayer)
   {
      ClusterEnergy[ilayer] = 0.;
      LayerEneDep[ilayer]   = 0.;
      LayerEneFrac[ilayer]  = 0.;

      LayerMean[ilayer]  = 0.;
      LayerSigma[ilayer] = 0.;

      LayerS1S3[ilayer]   = 1.;
      LayerS3Frac[ilayer] = 1.;

      for (unsigned int icell = 0; icell < nCELLs; ++icell)
         MapEneDep[ilayer][icell] = 0.;
   }

   int n2DCLUSTERs = NEcal2DCluster();
   for (int i2dcluster = 0; i2dcluster < n2DCLUSTERs; ++i2dcluster)
   {
      Ecal2DClusterR *cluster2d = pEcal2DCluster(i2dcluster);

      if (!cluster2d || !(cluster2d->Status & 32)) continue;

      int nCLUSTERs = cluster2d->NEcalCluster();
      for (int icluster = 0; icluster < nCLUSTERs; ++icluster)
      {
         EcalClusterR *cluster = cluster2d->pEcalCluster(icluster);
	 if (!cluster) return -2;
         if (!(cluster->Status & 32)) continue;

         ClusterEnergy[cluster->Plane] += cluster->Edep;

         int nclHITs = cluster->NEcalHit();
         for (int ihit = 0; ihit < nclHITs; ++ihit)
         {
            EcalHitR *hit = cluster->pEcalHit(ihit);
	    if (!hit) return -2;
            if ((hit->Status & 32) && hit->ADC[0] > 4)
               MapEneDep[hit->Plane][hit->Cell] = hit->Edep;
         }
      }
   }

   // Compute mean, s1, s3 and energies
   for (unsigned int ilayer = 0; ilayer < nLAYERs; ++ilayer)
   {
      if (ClusterEnergy[ilayer] == 0.) continue;

      UChar_t proj = !((ilayer/2) % 2);

      ClusterEnergyXY[proj] += ClusterEnergy[ilayer];
      LayerEnergy           += ClusterEnergy[ilayer];

      ShowerMean += (ilayer + 1)*ClusterEnergy[ilayer];

      int imaxcell = 0;
      float maxcellene      = 0.;

      for (unsigned int icell = 0; icell < nCELLs; ++icell)
      {
	if (MapEneDep[ilayer][icell] <= EneDepThreshold ) continue;
	
	LayerEneDep[ilayer] += MapEneDep[ilayer][icell];
	
	++NEcalHits;

        if (MapEneDep[ilayer][icell] >= maxcellene)
         {
            maxcellene = MapEneDep[ilayer][icell];
            imaxcell   = icell;
         }

         LayerMean[ilayer]  += (icell + 1)*MapEneDep[ilayer][icell];
         ShowerMeanXY[proj] += (icell + 1)*MapEneDep[ilayer][icell];
      }

      //
      // Now apply corrections for lateral leakage -- ADDED 27may2013 MI
      //
      float LatLeak[10]={0.};
      //case 1: cell at left border -> simmetrize shower
      if (imaxcell==0)
	{
	  int iLatLeak=1;
	  while(MapEneDep[ilayer][imaxcell+iLatLeak]>0.&&iLatLeak<10)
	    {
	      LatLeak[iLatLeak-1] = MapEneDep[ilayer][imaxcell+iLatLeak];
	      iLatLeak++;
	    }
	}
      //case 2: cell close to left border within 10 cells 
      //-> simmetrize shower using ratio of adjacent cells
      else if (imaxcell<9)
	{
	  //find ratio
	  float LatRatio = MapEneDep[ilayer][imaxcell-1]/MapEneDep[ilayer][imaxcell+1] ;
	  int iLatLeak=1;
	  while(MapEneDep[ilayer][imaxcell+iLatLeak]>0.&&iLatLeak<10)
	    {
	      if ((int)imaxcell-iLatLeak < 0) LatLeak[iLatLeak-imaxcell-1] = LatRatio*MapEneDep[ilayer][imaxcell+iLatLeak];
	      iLatLeak++;
	    }
	}
      //case 3: cell at right border -> simmetrize shower
      if (imaxcell==71)
	{
	  int iLatLeak=1;
	  while(MapEneDep[ilayer][imaxcell-iLatLeak]>0.&&iLatLeak<10)
	    {
	      LatLeak[iLatLeak-1] = MapEneDep[ilayer][imaxcell-iLatLeak];
	      iLatLeak++;
	    }
	}
      //case 4: cell close to left border within 10 cells 
      //-> simmetrize shower using ratio of adjacent cells
      else if (imaxcell>62)
	{
	  //find ratio
	  float LatRatio = MapEneDep[ilayer][imaxcell+1]/MapEneDep[ilayer][imaxcell-1] ;
	  int iLatLeak=1;
	  while(MapEneDep[ilayer][imaxcell-iLatLeak]>0.&&iLatLeak<10)
	    {
	      if ((int)imaxcell+iLatLeak > 71) LatLeak[iLatLeak+imaxcell-72] = LatRatio*MapEneDep[ilayer][imaxcell-iLatLeak];
	      iLatLeak++;
	    }
	}
      //
      for(int i=0;i<10;i++) LayerEneDep[ilayer] += LatLeak[i];
      //End of correction for lateral leakage
      //

      EneDep         += LayerEneDep[ilayer];
      EneDepXY[proj] += LayerEneDep[ilayer];

      float S1 = MapEneDep[ilayer][imaxcell];
      float S3 = S1;
      if (imaxcell==0 || imaxcell==71) S3 += LatLeak[0];
      if (imaxcell > 0) S3 += MapEneDep[ilayer][imaxcell - 1];
      if (imaxcell < nCELLs - 1) S3 += MapEneDep[ilayer][imaxcell + 1];

      if (!proj) S3totx += S3;
      else S3toty += S3;

      if (S1*S3 > 0.) LayerS1S3[ilayer] = S1/S3;

      if (LayerEneDep[ilayer] > 0.)
      {
         LayerS3Frac[ilayer] = S3/LayerEneDep[ilayer];
         LayerMean[ilayer]   = LayerMean[ilayer]/LayerEneDep[ilayer];
      }
      else
      {
         LayerMean[ilayer] = -1.;
      }
   } //end of loop on layers

   if (EneDep <= 0. || LayerEnergy <= 0. || EneDepXY[0] <= 0. || EneDepXY[1] <= 0.) return -0.999;

   ShowerMeanXY[0] /= EneDepXY[0];
   ShowerMeanXY[1] /= EneDepXY[1];
   ShowerMean      /= LayerEnergy;

   F2SLEneDep = (LayerEneDep[0] + LayerEneDep[1] + LayerEneDep[2] + LayerEneDep[3])/1000.;
   L2LFrac    = (LayerEneDep[16] + LayerEneDep[17])/EneDep;

   S3totx /= EneDepXY[0];
   S3toty /= EneDepXY[1];

   for (unsigned int ilayer = 0; ilayer < nLAYERs; ++ilayer)
   {
      if (ClusterEnergy[ilayer] == 0.) continue;

      UChar_t proj = !((ilayer/2) % 2);

      LayerEneFrac[ilayer] = LayerEneDep[ilayer]/EneDep;

      ShowerSigma += TMath::Power(ilayer + 1 - ShowerMean, 2)*ClusterEnergy[ilayer];

      for (unsigned int icell = 0; icell < nCELLs; ++icell)
      {
         if (MapEneDep[ilayer][icell]  <= EneDepThreshold) continue;

         LayerSigma[ilayer] += TMath::Power(icell + 1 - LayerMean[ilayer], 2)*MapEneDep[ilayer][icell];

         sigmaXY[proj]  += TMath::Power(icell + 1 - ShowerMeanXY[proj], 2)*MapEneDep[ilayer][icell];
         sigmaXYZ[proj] += (icell + 1 - ShowerMeanXY[proj])*(ilayer + 1 - ShowerMean)*MapEneDep[ilayer][icell];
         sigmaZ[proj]   += TMath::Power(ilayer + 1 - ShowerMean, 2)*MapEneDep[ilayer][icell];
      }

      if (LayerEneDep[ilayer] > 0.)
         LayerSigma[ilayer] = TMath::Sqrt(LayerSigma[ilayer]/LayerEneDep[ilayer]);
      else LayerSigma[ilayer] = -1.;
   }

   ShowerSigma = TMath::Sqrt(ShowerSigma/LayerEnergy); // ShowerSigma from 0 to 17
   ShowerMean  = ShowerMean - 1.;

   for (unsigned int iproj = 0; iproj < 2; ++iproj)
   {
      sigmaXY[iproj]  /= EneDepXY[iproj];
      sigmaXYZ[iproj] /= EneDepXY[iproj];
      sigmaZ[iproj]   /= EneDepXY[iproj];
   }

   ShowerFootprintX = TMath::Sqrt(TMath::Abs(sigmaXY[0]*sigmaZ[0] - TMath::Power(sigmaXYZ[0], 2)));
   ShowerFootprintY = TMath::Sqrt(TMath::Abs(sigmaXY[1]*sigmaZ[1] - TMath::Power(sigmaXYZ[1], 2)));

   //S3totx = this->S3tot[1];
   //S3toty = this->S3tot[2];
   //NEcalHits = this->Nhits;

   float energyd = EnergyD/1000.;
   float classification_energy = energyd;
   if( iBDTVERSION==5 ) { if( EnergyFlag!=0) classification_energy=EnergyE; }
   if( iBDTVERSION>5 ) { if( EnergyFlag!=0) classification_energy=GetCorrectedEnergy(2,2); }

   if (BDT_DEBUG) std::cout << " ??? BDT variables computed\n" << flush;

   unsigned int ivar = 0;

   pisainputvar[ivar++] = ShowerMean;
   pisainputvar[ivar++] = ShowerSigma;
   pisainputvar[ivar++] = L2LFrac;
   pisainputvar[ivar++] = R3cmFrac;
   pisainputvar[ivar++] = S3totx;
   pisainputvar[ivar++] = S3toty;
   pisainputvar[ivar++] = NEcalHits;
   pisainputvar[ivar++] = ShowerFootprint[1];
   pisainputvar[ivar++] = ShowerFootprint[2];
   pisainputvar[ivar++] = LayerEneFrac[0];
   pisainputvar[ivar++] = LayerEneFrac[1];
   pisainputvar[ivar++] = LayerEneFrac[2];
   pisainputvar[ivar++] = LayerEneFrac[3];
   pisainputvar[ivar++] = LayerEneFrac[4];
   pisainputvar[ivar++] = LayerEneFrac[5];
   pisainputvar[ivar++] = LayerEneFrac[6];
   pisainputvar[ivar++] = LayerEneFrac[7];
   pisainputvar[ivar++] = LayerEneFrac[8];
   pisainputvar[ivar++] = LayerEneFrac[9];
   pisainputvar[ivar++] = LayerEneFrac[10];
   pisainputvar[ivar++] = LayerEneFrac[11];
   pisainputvar[ivar++] = LayerEneFrac[12];
   pisainputvar[ivar++] = LayerEneFrac[13];
   pisainputvar[ivar++] = LayerEneFrac[14];
   pisainputvar[ivar++] = LayerEneFrac[15];
   pisainputvar[ivar++] = LayerEneFrac[16];
   pisainputvar[ivar++] = LayerEneFrac[17];
   pisainputvar[ivar++] = LayerSigma[1];
   pisainputvar[ivar++] = LayerSigma[2];
   pisainputvar[ivar++] = LayerSigma[3];
   pisainputvar[ivar++] = LayerSigma[4];
   pisainputvar[ivar++] = LayerSigma[5];
   pisainputvar[ivar++] = LayerSigma[6];
   pisainputvar[ivar++] = LayerSigma[7];
   pisainputvar[ivar++] = LayerSigma[8];
   pisainputvar[ivar++] = LayerSigma[9];
   pisainputvar[ivar++] = LayerSigma[10];
   pisainputvar[ivar++] = LayerSigma[11];
   pisainputvar[ivar++] = LayerSigma[12];
   pisainputvar[ivar++] = LayerSigma[13];
   pisainputvar[ivar++] = LayerSigma[14];
   pisainputvar[ivar++] = LayerSigma[15];
   pisainputvar[ivar++] = LayerSigma[16];
   pisainputvar[ivar++] = LayerSigma[17];
   pisainputvar[ivar++] = LayerS3Frac[1];
   pisainputvar[ivar++] = LayerS3Frac[2];
   pisainputvar[ivar++] = LayerS3Frac[3];
   pisainputvar[ivar++] = LayerS3Frac[4];
   pisainputvar[ivar++] = LayerS3Frac[5];
   pisainputvar[ivar++] = LayerS3Frac[6];
   pisainputvar[ivar++] = LayerS3Frac[7];
   pisainputvar[ivar++] = LayerS3Frac[8];
   pisainputvar[ivar++] = LayerS3Frac[9];
   pisainputvar[ivar++] = LayerS3Frac[10];
   pisainputvar[ivar++] = LayerS3Frac[11];
   pisainputvar[ivar++] = LayerS3Frac[12];
   pisainputvar[ivar++] = LayerS3Frac[13];
   pisainputvar[ivar++] = LayerS3Frac[14];
   pisainputvar[ivar++] = LayerS3Frac[15];
   pisainputvar[ivar++] = LayerS3Frac[16];
   pisainputvar[ivar++] = LayerS3Frac[17];
   pisainputvar[ivar++] = classification_energy;


   //********************************
   //*****Init the Reader************
   //********************************

   TMVA::Reader *ecalpisareader = NULL;
   TMVA::Reader *ecalpisareader_ODD = NULL;
   TMVA::Reader *ecalpisareader_EVEN = NULL;
   TMVA::Reader *ecalpisareaderS_ODD = NULL;
   TMVA::Reader *ecalpisareaderS_EVEN = NULL;
   TMVA::Reader *ecalpisareader_E_ODD = NULL;
   TMVA::Reader *ecalpisareader_E_EVEN = NULL;
   TMVA::Reader *ecalpisareaderS_E_ODD = NULL;
   TMVA::Reader *ecalpisareaderS_E_EVEN = NULL;


   if ( iBDTVERSION == 3 )
   {
     ecalpisareader = ecalpisareader_v3;
   }
   else if ( iBDTVERSION == 4 )
   {
     ecalpisareader = ecalpisareader_v4;
   }
   else if ( iBDTVERSION == 5 )
   {
     if (TMVAClassifier==0)
       {
	 ecalpisareader_ODD = ecalpisareader_v5_ODD;
	 ecalpisareader_EVEN = ecalpisareader_v5_EVEN;
	 ecalpisareader_E_ODD = ecalpisareader_E_v5_ODD;
	 ecalpisareader_E_EVEN = ecalpisareader_E_v5_EVEN;
       }
     else if (TMVAClassifier==1)
       {
	 ecalpisareaderS_ODD = ecalpisareaderS_v5_ODD;
	 ecalpisareaderS_EVEN = ecalpisareaderS_v5_EVEN;
	 ecalpisareaderS_E_ODD = ecalpisareaderS_E_v5_ODD;
	 ecalpisareaderS_E_EVEN = ecalpisareaderS_E_v5_EVEN;
       }
   }
   else if ( iBDTVERSION == 6 )
   {
     if (TMVAClassifier==0)
       {
	 ecalpisareader_ODD = ecalpisareader_v6_ODD;
	 ecalpisareader_EVEN = ecalpisareader_v6_EVEN;
	 ecalpisareader_E_ODD = ecalpisareader_E_v6_ODD;
	 ecalpisareader_E_EVEN = ecalpisareader_E_v6_EVEN;
       }
     else if (TMVAClassifier==1)
       {
	 ecalpisareaderS_ODD = ecalpisareaderS_v6_ODD;
	 ecalpisareaderS_EVEN = ecalpisareaderS_v6_EVEN;
	 ecalpisareaderS_E_ODD = ecalpisareaderS_E_v6_ODD;
	 ecalpisareaderS_E_EVEN = ecalpisareaderS_E_v6_EVEN;
       }
   }
   else if ( iBDTVERSION == 7 )
   {
     if (TMVAClassifier==0)
       {
	 ecalpisareader_ODD = ecalpisareader_v7_ODD;
	 ecalpisareader_EVEN = ecalpisareader_v7_EVEN;
	 ecalpisareader_E_ODD = ecalpisareader_E_v7_ODD;
	 ecalpisareader_E_EVEN = ecalpisareader_E_v7_EVEN;
       }
     else if (TMVAClassifier==1)
       {
	 ecalpisareaderS_ODD = ecalpisareaderS_v7_ODD;
	 ecalpisareaderS_EVEN = ecalpisareaderS_v7_EVEN;
	 ecalpisareaderS_E_ODD = ecalpisareaderS_E_v7_ODD;
	 ecalpisareaderS_E_EVEN = ecalpisareaderS_E_v7_EVEN;
       }
   }
   else
   {
     if ( iVersionNumberBDT==0 )
       {
	 cout<<" "<<endl;
	 cout<<" =================================================================="<<endl;
	 cout<<" [ecalBDT] ATTENTION    only versions 3, 4, 5, 6 and 7 of BDT supported"<<endl;
	 cout<<" [ecalBDT] ATTENTION    you have called it with version "<<iBDTVERSION<<endl;
	 cout<<" [ecalBDT] ATTENTION    BDT will be set to -999 for all entries!!!"<<endl;
	 cout<<" =================================================================="<<endl;
	 cout<<" "<<endl;
	 iVersionNumberBDT = 1;
       }
     return -999;
   }

   //check if reader already initialized
   bool InitReader = false;
   if (iBDTVERSION<5 && ecalpisareader==NULL) InitReader=true;
   else if (iBDTVERSION>=5)
     {
       if(TMVAClassifier==0)   
	 {
	   if( EnergyFlag==0 ) { if( ecalpisareader_ODD==NULL   ) InitReader=true; }
	   else                { if( ecalpisareader_E_ODD==NULL ) InitReader=true; }
	 }
       else //TMVAClass==1
	 {
	   if( EnergyFlag==0 ) { if( ecalpisareaderS_ODD==NULL   ) InitReader=true; }
	   else                { if( ecalpisareaderS_E_ODD==NULL ) InitReader=true; }
	 }
     }

   if (InitReader)     //if not already Init.....
   {
      std::cout << "##############################################################"                          << std::endl;
      std::cout << "                Create GetEcalBDT v"<<iBDTVERSION<<" Reader   "                          << std::endl;
      if ( iBDTVERSION==5 ){
      std::cout << "                TMVAClassifier type: "<<TMVAClassifier                                   << std::endl;
      std::cout << "                Classification Energy: "<<Form("%s",EnergyFlag==0?"EnergyD":"EnergyE")         << std::endl;
      }
      else if ( iBDTVERSION>5 ){
      std::cout << "                TMVAClassifier type: "<<TMVAClassifier                                   << std::endl;
      std::cout << "                Classification Energy: "<<Form("%s",EnergyFlag==0?"EnergyD":"GetCorrectedEnergy(2,2)")         << std::endl;
      }
      std::cout << "##############################################################"                          << std::endl;

      //~ TMVA::Tools::Instance();
      //
      //
      char WeightsDir[100];
      char WeightsDir_v5[100];
      sprintf(WeightsDir,"%s/v5.00", getenv("AMSDataDir"));
      sprintf(WeightsDir_v5,"%s/v5.00", getenv("AMSDataDir"));
      //sprintf(WeightsDir_v5,"/afs/cern.ch/user/i/incaglim/public/bdt-marco/WEIGHTS");
      
      if ( iBDTVERSION == 3 )
	{
	  ecalpisareader = new TMVA::Reader("Color:Silent");
	  ecalpisareader->AddSpectator("EnergyD", &pisanormvar[nPISABDTVARs]);
	  ivar = 0;
	  ecalpisareader->AddVariable("ShowerMeanNorm",       &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("ShowerSigmaNorm",      &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("L2LFracNorm",          &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("R3cmFracNorm",         &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("S3totxNorm",           &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("S3totyNorm",           &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("NEcalHitsNorm",        &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("ShowerFootprintXNorm", &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("ShowerFootprintYNorm", &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerEneFracNorm0",    &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerEneFracNorm1",    &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerEneFracNorm2",    &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerEneFracNorm3",    &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerEneFracNorm4",    &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerEneFracNorm5",    &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerEneFracNorm6",    &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerEneFracNorm7",    &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerEneFracNorm8",    &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerEneFracNorm9",    &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerEneFracNorm10",   &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerEneFracNorm11",   &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerEneFracNorm12",   &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerEneFracNorm13",   &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerEneFracNorm14",   &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerEneFracNorm15",   &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerEneFracNorm16",   &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerEneFracNorm17",   &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerSigmaNorm1",      &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerSigmaNorm2",      &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerSigmaNorm3",      &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerSigmaNorm4",      &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerSigmaNorm5",      &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerSigmaNorm6",      &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerSigmaNorm7",      &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerSigmaNorm8",      &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerSigmaNorm9",      &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerSigmaNorm10",     &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerSigmaNorm11",     &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerSigmaNorm12",     &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerSigmaNorm13",     &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerSigmaNorm14",     &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerSigmaNorm15",     &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerSigmaNorm16",     &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerSigmaNorm17",     &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerS3FracNorm1",     &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerS3FracNorm2",     &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerS3FracNorm3",     &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerS3FracNorm4",     &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerS3FracNorm5",     &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerS3FracNorm6",     &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerS3FracNorm7",     &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerS3FracNorm8",     &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerS3FracNorm9",     &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerS3FracNorm10",    &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerS3FracNorm11",    &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerS3FracNorm12",    &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerS3FracNorm13",    &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerS3FracNorm14",    &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerS3FracNorm15",    &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerS3FracNorm16",    &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerS3FracNorm17",    &pisanormvar[ivar++]);
	  ecalpisareader->BookMVA("BDTG_LAYERS", Form("%s/ECAL_PISA_BDT_412_v3.weights.xml", WeightsDir));
	  ecalpisareader_v3 = ecalpisareader;
	}
      if ( iBDTVERSION == 4 )
	{
	  ecalpisareader = new TMVA::Reader("Color:Silent");
	  ecalpisareader->AddSpectator("EnergyD", &pisanormvar[nPISABDTVARs]);
	  ivar = 0;
	  ecalpisareader->AddVariable("ShowerMeanNorm",       &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("ShowerSigmaNorm",      &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("L2LFracNorm",          &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("R3cmFracNorm",         &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("S3totxNorm",           &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("S3totyNorm",           &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("NEcalHitsNorm",        &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("ShowerFootprintXNorm", &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("ShowerFootprintYNorm", &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerEneFracNorm0",    &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerEneFracNorm1",    &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerEneFracNorm2",    &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerEneFracNorm3",    &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerEneFracNorm4",    &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerEneFracNorm5",    &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerEneFracNorm6",    &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerEneFracNorm7",    &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerEneFracNorm8",    &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerEneFracNorm9",    &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerEneFracNorm10",   &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerEneFracNorm11",   &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerEneFracNorm12",   &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerEneFracNorm13",   &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerEneFracNorm14",   &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerEneFracNorm15",   &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerEneFracNorm16",   &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerEneFracNorm17",   &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerSigmaNorm1",      &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerSigmaNorm2",      &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerSigmaNorm3",      &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerSigmaNorm4",      &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerSigmaNorm5",      &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerSigmaNorm6",      &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerSigmaNorm7",      &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerSigmaNorm8",      &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerSigmaNorm9",      &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerSigmaNorm10",     &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerSigmaNorm11",     &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerSigmaNorm12",     &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerSigmaNorm13",     &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerSigmaNorm14",     &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerSigmaNorm15",     &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerSigmaNorm16",     &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerSigmaNorm17",     &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerS3FracNorm1",     &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerS3FracNorm2",     &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerS3FracNorm3",     &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerS3FracNorm4",     &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerS3FracNorm5",     &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerS3FracNorm6",     &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerS3FracNorm7",     &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerS3FracNorm8",     &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerS3FracNorm9",     &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerS3FracNorm10",    &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerS3FracNorm11",    &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerS3FracNorm12",    &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerS3FracNorm13",    &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerS3FracNorm14",    &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerS3FracNorm15",    &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerS3FracNorm16",    &pisanormvar[ivar++]);
	  ecalpisareader->AddVariable("LayerS3FracNorm17",    &pisanormvar[ivar++]);
	  ecalpisareader->BookMVA("BDTG_LAYERS", Form("%s/ECAL_PISA_BDT_412_v4.weights.xml", WeightsDir));
	  ecalpisareader_v4 = ecalpisareader;
	}
      if ( iBDTVERSION >= 5 && TMVAClassifier == 0 && EnergyFlag==0 )
	{
	  ecalpisareader_ODD = new TMVA::Reader("Color:Silent");
	  ecalpisareader_ODD->AddSpectator("EnergyD", &pisanormvar[nPISABDTVARs]);
	  ivar = 0;
	  ecalpisareader_ODD->AddVariable("ShowerMeanNorm",       &pisanormvar[ivar++]);
	  ecalpisareader_ODD->AddVariable("F2SLEneDep",           &pisanormvar[ivar++]);
	  ecalpisareader_ODD->AddVariable("L2LFracNorm",          &pisanormvar[ivar++]);
	  ecalpisareader_ODD->AddVariable("R3cmFracNorm",         &pisanormvar[ivar++]);
	  ecalpisareader_ODD->AddVariable("S3totxNorm",           &pisanormvar[ivar++]);
	  ecalpisareader_ODD->AddVariable("S3totyNorm",           &pisanormvar[ivar++]);
	  ecalpisareader_ODD->AddVariable("NEcalHitsNorm",        &pisanormvar[ivar++]);
	  ecalpisareader_ODD->AddVariable("ShowerFootprintXNorm", &pisanormvar[ivar++]);
	  ecalpisareader_ODD->AddVariable("ShowerFootprintYNorm", &pisanormvar[ivar++]);
	  ecalpisareader_ODD->AddVariable("LayerEneFracNorm0",    &pisanormvar[ivar++]);
	  ecalpisareader_ODD->AddVariable("LayerEneFracNorm1",    &pisanormvar[ivar++]);
	  ecalpisareader_ODD->AddVariable("LayerEneFracNorm2",    &pisanormvar[ivar++]);
	  ecalpisareader_ODD->AddVariable("LayerEneFracNorm3",    &pisanormvar[ivar++]);
	  ecalpisareader_ODD->AddVariable("LayerEneFracNorm4",    &pisanormvar[ivar++]);
	  ecalpisareader_ODD->AddVariable("LayerEneFracNorm5",    &pisanormvar[ivar++]);
	  ecalpisareader_ODD->AddVariable("LayerEneFracNorm6",    &pisanormvar[ivar++]);
	  ecalpisareader_ODD->AddVariable("LayerEneFracNorm7",    &pisanormvar[ivar++]);
	  ecalpisareader_ODD->AddVariable("LayerEneFracNorm8",    &pisanormvar[ivar++]);
	  ecalpisareader_ODD->AddVariable("LayerEneFracNorm9",    &pisanormvar[ivar++]);
	  ecalpisareader_ODD->AddVariable("LayerEneFracNorm10",   &pisanormvar[ivar++]);
	  ecalpisareader_ODD->AddVariable("LayerEneFracNorm11",   &pisanormvar[ivar++]);
	  ecalpisareader_ODD->AddVariable("LayerEneFracNorm12",   &pisanormvar[ivar++]);
	  ecalpisareader_ODD->AddVariable("LayerEneFracNorm13",   &pisanormvar[ivar++]);
	  ecalpisareader_ODD->AddVariable("LayerEneFracNorm14",   &pisanormvar[ivar++]);
	  ecalpisareader_ODD->AddVariable("LayerEneFracNorm15",   &pisanormvar[ivar++]);
	  ecalpisareader_ODD->AddVariable("LayerEneFracNorm16",   &pisanormvar[ivar++]);
	  ecalpisareader_ODD->AddVariable("LayerEneFracNorm17",   &pisanormvar[ivar++]);
	  ecalpisareader_ODD->AddVariable("LayerS1S31",      &pisanormvar[ivar++]);
	  ecalpisareader_ODD->AddVariable("LayerS1S32",      &pisanormvar[ivar++]);
	  ecalpisareader_ODD->AddVariable("LayerS1S33",      &pisanormvar[ivar++]);
	  ecalpisareader_ODD->AddVariable("LayerS1S34",      &pisanormvar[ivar++]);
	  ecalpisareader_ODD->AddVariable("LayerS1S35",      &pisanormvar[ivar++]);
	  ecalpisareader_ODD->AddVariable("LayerS1S36",      &pisanormvar[ivar++]);
	  ecalpisareader_ODD->AddVariable("LayerS1S37",      &pisanormvar[ivar++]);
	  ecalpisareader_ODD->AddVariable("LayerSigmaNorm8",      &pisanormvar[ivar++]);
	  ecalpisareader_ODD->AddVariable("LayerSigmaNorm9",      &pisanormvar[ivar++]);
	  ecalpisareader_ODD->AddVariable("LayerSigmaNorm10",     &pisanormvar[ivar++]);
	  ecalpisareader_ODD->AddVariable("LayerSigmaNorm11",     &pisanormvar[ivar++]);
	  ecalpisareader_ODD->AddVariable("LayerSigmaNorm12",     &pisanormvar[ivar++]);
	  ecalpisareader_ODD->AddVariable("LayerSigmaNorm13",     &pisanormvar[ivar++]);
	  ecalpisareader_ODD->AddVariable("LayerSigmaNorm14",     &pisanormvar[ivar++]);
	  ecalpisareader_ODD->AddVariable("LayerSigmaNorm15",     &pisanormvar[ivar++]);
	  ecalpisareader_ODD->AddVariable("LayerSigmaNorm16",     &pisanormvar[ivar++]);
	  ecalpisareader_ODD->AddVariable("LayerSigmaNorm17",     &pisanormvar[ivar++]);
	  ecalpisareader_ODD->AddVariable("LayerS3FracNorm1",     &pisanormvar[ivar++]);
	  ecalpisareader_ODD->AddVariable("LayerS3FracNorm2",     &pisanormvar[ivar++]);
	  ecalpisareader_ODD->AddVariable("LayerS3FracNorm3",     &pisanormvar[ivar++]);
	  ecalpisareader_ODD->AddVariable("LayerS3FracNorm4",     &pisanormvar[ivar++]);
	  ecalpisareader_ODD->AddVariable("LayerS3FracNorm5",     &pisanormvar[ivar++]);
	  ecalpisareader_ODD->AddVariable("LayerS3FracNorm6",     &pisanormvar[ivar++]);
	  ecalpisareader_ODD->AddVariable("LayerS3FracNorm7",     &pisanormvar[ivar++]);
	  ecalpisareader_ODD->AddVariable("LayerS3FracNorm8",     &pisanormvar[ivar++]);
	  ecalpisareader_ODD->AddVariable("LayerS3FracNorm9",     &pisanormvar[ivar++]);
	  ecalpisareader_ODD->AddVariable("LayerS3FracNorm10",    &pisanormvar[ivar++]);
	  ecalpisareader_ODD->AddVariable("LayerS3FracNorm11",    &pisanormvar[ivar++]);
	  ecalpisareader_ODD->AddVariable("LayerS3FracNorm12",    &pisanormvar[ivar++]);
	  ecalpisareader_ODD->AddVariable("LayerS3FracNorm13",    &pisanormvar[ivar++]);
	  ecalpisareader_ODD->AddVariable("LayerS3FracNorm14",    &pisanormvar[ivar++]);
	  ecalpisareader_ODD->AddVariable("LayerS3FracNorm15",    &pisanormvar[ivar++]);
	  ecalpisareader_ODD->AddVariable("LayerS3FracNorm16",    &pisanormvar[ivar++]);
	  ecalpisareader_ODD->AddVariable("LayerS3FracNorm17",    &pisanormvar[ivar++]);
	  //
	  ecalpisareader_EVEN = new TMVA::Reader("Color:Silent");
	  ecalpisareader_EVEN->AddSpectator("EnergyD", &pisanormvar[nPISABDTVARs]);
	  ivar = 0;
	  ecalpisareader_EVEN->AddVariable("ShowerMeanNorm",       &pisanormvar[ivar++]);
	  ecalpisareader_EVEN->AddVariable("F2SLEneDep",           &pisanormvar[ivar++]);
	  ecalpisareader_EVEN->AddVariable("L2LFracNorm",          &pisanormvar[ivar++]);
	  ecalpisareader_EVEN->AddVariable("R3cmFracNorm",         &pisanormvar[ivar++]);
	  ecalpisareader_EVEN->AddVariable("S3totxNorm",           &pisanormvar[ivar++]);
	  ecalpisareader_EVEN->AddVariable("S3totyNorm",           &pisanormvar[ivar++]);
	  ecalpisareader_EVEN->AddVariable("NEcalHitsNorm",        &pisanormvar[ivar++]);
	  ecalpisareader_EVEN->AddVariable("ShowerFootprintXNorm", &pisanormvar[ivar++]);
	  ecalpisareader_EVEN->AddVariable("ShowerFootprintYNorm", &pisanormvar[ivar++]);
	  ecalpisareader_EVEN->AddVariable("LayerEneFracNorm0",    &pisanormvar[ivar++]);
	  ecalpisareader_EVEN->AddVariable("LayerEneFracNorm1",    &pisanormvar[ivar++]);
	  ecalpisareader_EVEN->AddVariable("LayerEneFracNorm2",    &pisanormvar[ivar++]);
	  ecalpisareader_EVEN->AddVariable("LayerEneFracNorm3",    &pisanormvar[ivar++]);
	  ecalpisareader_EVEN->AddVariable("LayerEneFracNorm4",    &pisanormvar[ivar++]);
	  ecalpisareader_EVEN->AddVariable("LayerEneFracNorm5",    &pisanormvar[ivar++]);
	  ecalpisareader_EVEN->AddVariable("LayerEneFracNorm6",    &pisanormvar[ivar++]);
	  ecalpisareader_EVEN->AddVariable("LayerEneFracNorm7",    &pisanormvar[ivar++]);
	  ecalpisareader_EVEN->AddVariable("LayerEneFracNorm8",    &pisanormvar[ivar++]);
	  ecalpisareader_EVEN->AddVariable("LayerEneFracNorm9",    &pisanormvar[ivar++]);
	  ecalpisareader_EVEN->AddVariable("LayerEneFracNorm10",   &pisanormvar[ivar++]);
	  ecalpisareader_EVEN->AddVariable("LayerEneFracNorm11",   &pisanormvar[ivar++]);
	  ecalpisareader_EVEN->AddVariable("LayerEneFracNorm12",   &pisanormvar[ivar++]);
	  ecalpisareader_EVEN->AddVariable("LayerEneFracNorm13",   &pisanormvar[ivar++]);
	  ecalpisareader_EVEN->AddVariable("LayerEneFracNorm14",   &pisanormvar[ivar++]);
	  ecalpisareader_EVEN->AddVariable("LayerEneFracNorm15",   &pisanormvar[ivar++]);
	  ecalpisareader_EVEN->AddVariable("LayerEneFracNorm16",   &pisanormvar[ivar++]);
	  ecalpisareader_EVEN->AddVariable("LayerEneFracNorm17",   &pisanormvar[ivar++]);
	  ecalpisareader_EVEN->AddVariable("LayerS1S31",      &pisanormvar[ivar++]);
	  ecalpisareader_EVEN->AddVariable("LayerS1S32",      &pisanormvar[ivar++]);
	  ecalpisareader_EVEN->AddVariable("LayerS1S33",      &pisanormvar[ivar++]);
	  ecalpisareader_EVEN->AddVariable("LayerS1S34",      &pisanormvar[ivar++]);
	  ecalpisareader_EVEN->AddVariable("LayerS1S35",      &pisanormvar[ivar++]);
	  ecalpisareader_EVEN->AddVariable("LayerS1S36",      &pisanormvar[ivar++]);
	  ecalpisareader_EVEN->AddVariable("LayerS1S37",      &pisanormvar[ivar++]);
	  ecalpisareader_EVEN->AddVariable("LayerSigmaNorm8",      &pisanormvar[ivar++]);
	  ecalpisareader_EVEN->AddVariable("LayerSigmaNorm9",      &pisanormvar[ivar++]);
	  ecalpisareader_EVEN->AddVariable("LayerSigmaNorm10",     &pisanormvar[ivar++]);
	  ecalpisareader_EVEN->AddVariable("LayerSigmaNorm11",     &pisanormvar[ivar++]);
	  ecalpisareader_EVEN->AddVariable("LayerSigmaNorm12",     &pisanormvar[ivar++]);
	  ecalpisareader_EVEN->AddVariable("LayerSigmaNorm13",     &pisanormvar[ivar++]);
	  ecalpisareader_EVEN->AddVariable("LayerSigmaNorm14",     &pisanormvar[ivar++]);
	  ecalpisareader_EVEN->AddVariable("LayerSigmaNorm15",     &pisanormvar[ivar++]);
	  ecalpisareader_EVEN->AddVariable("LayerSigmaNorm16",     &pisanormvar[ivar++]);
	  ecalpisareader_EVEN->AddVariable("LayerSigmaNorm17",     &pisanormvar[ivar++]);
	  ecalpisareader_EVEN->AddVariable("LayerS3FracNorm1",     &pisanormvar[ivar++]);
	  ecalpisareader_EVEN->AddVariable("LayerS3FracNorm2",     &pisanormvar[ivar++]);
	  ecalpisareader_EVEN->AddVariable("LayerS3FracNorm3",     &pisanormvar[ivar++]);
	  ecalpisareader_EVEN->AddVariable("LayerS3FracNorm4",     &pisanormvar[ivar++]);
	  ecalpisareader_EVEN->AddVariable("LayerS3FracNorm5",     &pisanormvar[ivar++]);
	  ecalpisareader_EVEN->AddVariable("LayerS3FracNorm6",     &pisanormvar[ivar++]);
	  ecalpisareader_EVEN->AddVariable("LayerS3FracNorm7",     &pisanormvar[ivar++]);
	  ecalpisareader_EVEN->AddVariable("LayerS3FracNorm8",     &pisanormvar[ivar++]);
	  ecalpisareader_EVEN->AddVariable("LayerS3FracNorm9",     &pisanormvar[ivar++]);
	  ecalpisareader_EVEN->AddVariable("LayerS3FracNorm10",    &pisanormvar[ivar++]);
	  ecalpisareader_EVEN->AddVariable("LayerS3FracNorm11",    &pisanormvar[ivar++]);
	  ecalpisareader_EVEN->AddVariable("LayerS3FracNorm12",    &pisanormvar[ivar++]);
	  ecalpisareader_EVEN->AddVariable("LayerS3FracNorm13",    &pisanormvar[ivar++]);
	  ecalpisareader_EVEN->AddVariable("LayerS3FracNorm14",    &pisanormvar[ivar++]);
	  ecalpisareader_EVEN->AddVariable("LayerS3FracNorm15",    &pisanormvar[ivar++]);
	  ecalpisareader_EVEN->AddVariable("LayerS3FracNorm16",    &pisanormvar[ivar++]);
	  ecalpisareader_EVEN->AddVariable("LayerS3FracNorm17",    &pisanormvar[ivar++]);
	  //
	  ecalpisareader_ODD->BookMVA("BDTG_LAYERS_ODD", Form("%s/ECAL_PISA_BDT_%d_v%dfinal_ODD.weights.xml", WeightsDir_v5,ECALBDT_TMVAVER,iBDTVERSION));
	  ecalpisareader_EVEN->BookMVA("BDTG_LAYERS_EVEN", Form("%s/ECAL_PISA_BDT_%d_v%dfinal_EVEN.weights.xml", WeightsDir_v5,ECALBDT_TMVAVER,iBDTVERSION));
	  if ( iBDTVERSION == 5 ) 
	    {
	      ecalpisareader_v5_ODD = ecalpisareader_ODD;
	      ecalpisareader_v5_EVEN = ecalpisareader_EVEN;
	    }
	  else if ( iBDTVERSION == 6 )
	    {
	      ecalpisareader_v6_ODD = ecalpisareader_ODD;
	      ecalpisareader_v6_EVEN = ecalpisareader_EVEN;
	    }
	  else if ( iBDTVERSION == 7 )
	    {
	      ecalpisareader_v7_ODD = ecalpisareader_ODD;
	      ecalpisareader_v7_EVEN = ecalpisareader_EVEN;
	    }
	  else
	    {
	      cout<<" BDT reader NOT initialized; error in version number. iBDTVERSION = "<<iBDTVERSION<<endl;
	    }
	  //
	}
      if ( iBDTVERSION >= 5 && TMVAClassifier == 1 && EnergyFlag==0 )
	{
	  ecalpisareaderS_ODD = new TMVA::Reader("Color:Silent");
	  ecalpisareaderS_ODD->AddSpectator("EnergyD", &pisanormvar[nPISABDTVARs]);
	  ivar = 0;
	  ecalpisareaderS_ODD->AddVariable("ShowerMeanNorm",       &pisanormvar[ivar++]);
	  ecalpisareaderS_ODD->AddVariable("F2SLEneDep",           &pisanormvar[ivar++]);
	  ecalpisareaderS_ODD->AddVariable("L2LFracNorm",          &pisanormvar[ivar++]);
	  ecalpisareaderS_ODD->AddVariable("R3cmFracNorm",         &pisanormvar[ivar++]);
	  ecalpisareaderS_ODD->AddVariable("S3totxNorm",           &pisanormvar[ivar++]);
	  ecalpisareaderS_ODD->AddVariable("S3totyNorm",           &pisanormvar[ivar++]);
	  ecalpisareaderS_ODD->AddVariable("NEcalHitsNorm",        &pisanormvar[ivar++]);
	  ecalpisareaderS_ODD->AddVariable("ShowerFootprintXNorm", &pisanormvar[ivar++]);
	  ecalpisareaderS_ODD->AddVariable("ShowerFootprintYNorm", &pisanormvar[ivar++]);
	  ecalpisareaderS_ODD->AddVariable("LayerEneFracNorm0",    &pisanormvar[ivar++]);
	  ecalpisareaderS_ODD->AddVariable("LayerEneFracNorm1",    &pisanormvar[ivar++]);
	  ecalpisareaderS_ODD->AddVariable("LayerEneFracNorm2",    &pisanormvar[ivar++]);
	  ecalpisareaderS_ODD->AddVariable("LayerEneFracNorm3",    &pisanormvar[ivar++]);
	  ecalpisareaderS_ODD->AddVariable("LayerEneFracNorm4",    &pisanormvar[ivar++]);
	  ecalpisareaderS_ODD->AddVariable("LayerEneFracNorm5",    &pisanormvar[ivar++]);
	  ecalpisareaderS_ODD->AddVariable("LayerEneFracNorm6",    &pisanormvar[ivar++]);
	  ecalpisareaderS_ODD->AddVariable("LayerEneFracNorm7",    &pisanormvar[ivar++]);
	  ecalpisareaderS_ODD->AddVariable("LayerEneFracNorm8",    &pisanormvar[ivar++]);
	  ecalpisareaderS_ODD->AddVariable("LayerEneFracNorm9",    &pisanormvar[ivar++]);
	  ecalpisareaderS_ODD->AddVariable("LayerEneFracNorm10",   &pisanormvar[ivar++]);
	  ecalpisareaderS_ODD->AddVariable("LayerEneFracNorm11",   &pisanormvar[ivar++]);
	  ecalpisareaderS_ODD->AddVariable("LayerEneFracNorm12",   &pisanormvar[ivar++]);
	  ecalpisareaderS_ODD->AddVariable("LayerEneFracNorm13",   &pisanormvar[ivar++]);
	  ecalpisareaderS_ODD->AddVariable("LayerEneFracNorm14",   &pisanormvar[ivar++]);
	  ecalpisareaderS_ODD->AddVariable("LayerEneFracNorm15",   &pisanormvar[ivar++]);
	  ecalpisareaderS_ODD->AddVariable("LayerEneFracNorm16",   &pisanormvar[ivar++]);
	  ecalpisareaderS_ODD->AddVariable("LayerEneFracNorm17",   &pisanormvar[ivar++]);
	  ecalpisareaderS_ODD->AddVariable("LayerS1S31",      &pisanormvar[ivar++]);
	  ecalpisareaderS_ODD->AddVariable("LayerS1S32",      &pisanormvar[ivar++]);
	  ecalpisareaderS_ODD->AddVariable("LayerS1S33",      &pisanormvar[ivar++]);
	  ecalpisareaderS_ODD->AddVariable("LayerS1S34",      &pisanormvar[ivar++]);
	  ecalpisareaderS_ODD->AddVariable("LayerS1S35",      &pisanormvar[ivar++]);
	  ecalpisareaderS_ODD->AddVariable("LayerS1S36",      &pisanormvar[ivar++]);
	  ecalpisareaderS_ODD->AddVariable("LayerS1S37",      &pisanormvar[ivar++]);
	  ecalpisareaderS_ODD->AddVariable("LayerSigmaNorm8",      &pisanormvar[ivar++]);
	  ecalpisareaderS_ODD->AddVariable("LayerSigmaNorm9",      &pisanormvar[ivar++]);
	  ecalpisareaderS_ODD->AddVariable("LayerSigmaNorm10",     &pisanormvar[ivar++]);
	  ecalpisareaderS_ODD->AddVariable("LayerSigmaNorm11",     &pisanormvar[ivar++]);
	  ecalpisareaderS_ODD->AddVariable("LayerSigmaNorm12",     &pisanormvar[ivar++]);
	  ecalpisareaderS_ODD->AddVariable("LayerSigmaNorm13",     &pisanormvar[ivar++]);
	  ecalpisareaderS_ODD->AddVariable("LayerSigmaNorm14",     &pisanormvar[ivar++]);
	  ecalpisareaderS_ODD->AddVariable("LayerSigmaNorm15",     &pisanormvar[ivar++]);
	  ecalpisareaderS_ODD->AddVariable("LayerSigmaNorm16",     &pisanormvar[ivar++]);
	  ecalpisareaderS_ODD->AddVariable("LayerSigmaNorm17",     &pisanormvar[ivar++]);
	  ecalpisareaderS_ODD->AddVariable("LayerS3FracNorm1",     &pisanormvar[ivar++]);
	  ecalpisareaderS_ODD->AddVariable("LayerS3FracNorm2",     &pisanormvar[ivar++]);
	  ecalpisareaderS_ODD->AddVariable("LayerS3FracNorm3",     &pisanormvar[ivar++]);
	  ecalpisareaderS_ODD->AddVariable("LayerS3FracNorm4",     &pisanormvar[ivar++]);
	  ecalpisareaderS_ODD->AddVariable("LayerS3FracNorm5",     &pisanormvar[ivar++]);
	  ecalpisareaderS_ODD->AddVariable("LayerS3FracNorm6",     &pisanormvar[ivar++]);
	  ecalpisareaderS_ODD->AddVariable("LayerS3FracNorm7",     &pisanormvar[ivar++]);
	  ecalpisareaderS_ODD->AddVariable("LayerS3FracNorm8",     &pisanormvar[ivar++]);
	  ecalpisareaderS_ODD->AddVariable("LayerS3FracNorm9",     &pisanormvar[ivar++]);
	  ecalpisareaderS_ODD->AddVariable("LayerS3FracNorm10",    &pisanormvar[ivar++]);
	  ecalpisareaderS_ODD->AddVariable("LayerS3FracNorm11",    &pisanormvar[ivar++]);
	  ecalpisareaderS_ODD->AddVariable("LayerS3FracNorm12",    &pisanormvar[ivar++]);
	  ecalpisareaderS_ODD->AddVariable("LayerS3FracNorm13",    &pisanormvar[ivar++]);
	  ecalpisareaderS_ODD->AddVariable("LayerS3FracNorm14",    &pisanormvar[ivar++]);
	  ecalpisareaderS_ODD->AddVariable("LayerS3FracNorm15",    &pisanormvar[ivar++]);
	  ecalpisareaderS_ODD->AddVariable("LayerS3FracNorm16",    &pisanormvar[ivar++]);
	  ecalpisareaderS_ODD->AddVariable("LayerS3FracNorm17",    &pisanormvar[ivar++]);
	  //
	  ecalpisareaderS_EVEN = new TMVA::Reader("Color:Silent");
	  ecalpisareaderS_EVEN->AddSpectator("EnergyD", &pisanormvar[nPISABDTVARs]);
	  ivar = 0;
	  ecalpisareaderS_EVEN->AddVariable("ShowerMeanNorm",       &pisanormvar[ivar++]);
	  ecalpisareaderS_EVEN->AddVariable("F2SLEneDep",           &pisanormvar[ivar++]);
	  ecalpisareaderS_EVEN->AddVariable("L2LFracNorm",          &pisanormvar[ivar++]);
	  ecalpisareaderS_EVEN->AddVariable("R3cmFracNorm",         &pisanormvar[ivar++]);
	  ecalpisareaderS_EVEN->AddVariable("S3totxNorm",           &pisanormvar[ivar++]);
	  ecalpisareaderS_EVEN->AddVariable("S3totyNorm",           &pisanormvar[ivar++]);
	  ecalpisareaderS_EVEN->AddVariable("NEcalHitsNorm",        &pisanormvar[ivar++]);
	  ecalpisareaderS_EVEN->AddVariable("ShowerFootprintXNorm", &pisanormvar[ivar++]);
	  ecalpisareaderS_EVEN->AddVariable("ShowerFootprintYNorm", &pisanormvar[ivar++]);
	  ecalpisareaderS_EVEN->AddVariable("LayerEneFracNorm0",    &pisanormvar[ivar++]);
	  ecalpisareaderS_EVEN->AddVariable("LayerEneFracNorm1",    &pisanormvar[ivar++]);
	  ecalpisareaderS_EVEN->AddVariable("LayerEneFracNorm2",    &pisanormvar[ivar++]);
	  ecalpisareaderS_EVEN->AddVariable("LayerEneFracNorm3",    &pisanormvar[ivar++]);
	  ecalpisareaderS_EVEN->AddVariable("LayerEneFracNorm4",    &pisanormvar[ivar++]);
	  ecalpisareaderS_EVEN->AddVariable("LayerEneFracNorm5",    &pisanormvar[ivar++]);
	  ecalpisareaderS_EVEN->AddVariable("LayerEneFracNorm6",    &pisanormvar[ivar++]);
	  ecalpisareaderS_EVEN->AddVariable("LayerEneFracNorm7",    &pisanormvar[ivar++]);
	  ecalpisareaderS_EVEN->AddVariable("LayerEneFracNorm8",    &pisanormvar[ivar++]);
	  ecalpisareaderS_EVEN->AddVariable("LayerEneFracNorm9",    &pisanormvar[ivar++]);
	  ecalpisareaderS_EVEN->AddVariable("LayerEneFracNorm10",   &pisanormvar[ivar++]);
	  ecalpisareaderS_EVEN->AddVariable("LayerEneFracNorm11",   &pisanormvar[ivar++]);
	  ecalpisareaderS_EVEN->AddVariable("LayerEneFracNorm12",   &pisanormvar[ivar++]);
	  ecalpisareaderS_EVEN->AddVariable("LayerEneFracNorm13",   &pisanormvar[ivar++]);
	  ecalpisareaderS_EVEN->AddVariable("LayerEneFracNorm14",   &pisanormvar[ivar++]);
	  ecalpisareaderS_EVEN->AddVariable("LayerEneFracNorm15",   &pisanormvar[ivar++]);
	  ecalpisareaderS_EVEN->AddVariable("LayerEneFracNorm16",   &pisanormvar[ivar++]);
	  ecalpisareaderS_EVEN->AddVariable("LayerEneFracNorm17",   &pisanormvar[ivar++]);
	  ecalpisareaderS_EVEN->AddVariable("LayerS1S31",      &pisanormvar[ivar++]);
	  ecalpisareaderS_EVEN->AddVariable("LayerS1S32",      &pisanormvar[ivar++]);
	  ecalpisareaderS_EVEN->AddVariable("LayerS1S33",      &pisanormvar[ivar++]);
	  ecalpisareaderS_EVEN->AddVariable("LayerS1S34",      &pisanormvar[ivar++]);
	  ecalpisareaderS_EVEN->AddVariable("LayerS1S35",      &pisanormvar[ivar++]);
	  ecalpisareaderS_EVEN->AddVariable("LayerS1S36",      &pisanormvar[ivar++]);
	  ecalpisareaderS_EVEN->AddVariable("LayerS1S37",      &pisanormvar[ivar++]);
	  ecalpisareaderS_EVEN->AddVariable("LayerSigmaNorm8",      &pisanormvar[ivar++]);
	  ecalpisareaderS_EVEN->AddVariable("LayerSigmaNorm9",      &pisanormvar[ivar++]);
	  ecalpisareaderS_EVEN->AddVariable("LayerSigmaNorm10",     &pisanormvar[ivar++]);
	  ecalpisareaderS_EVEN->AddVariable("LayerSigmaNorm11",     &pisanormvar[ivar++]);
	  ecalpisareaderS_EVEN->AddVariable("LayerSigmaNorm12",     &pisanormvar[ivar++]);
	  ecalpisareaderS_EVEN->AddVariable("LayerSigmaNorm13",     &pisanormvar[ivar++]);
	  ecalpisareaderS_EVEN->AddVariable("LayerSigmaNorm14",     &pisanormvar[ivar++]);
	  ecalpisareaderS_EVEN->AddVariable("LayerSigmaNorm15",     &pisanormvar[ivar++]);
	  ecalpisareaderS_EVEN->AddVariable("LayerSigmaNorm16",     &pisanormvar[ivar++]);
	  ecalpisareaderS_EVEN->AddVariable("LayerSigmaNorm17",     &pisanormvar[ivar++]);
	  ecalpisareaderS_EVEN->AddVariable("LayerS3FracNorm1",     &pisanormvar[ivar++]);
	  ecalpisareaderS_EVEN->AddVariable("LayerS3FracNorm2",     &pisanormvar[ivar++]);
	  ecalpisareaderS_EVEN->AddVariable("LayerS3FracNorm3",     &pisanormvar[ivar++]);
	  ecalpisareaderS_EVEN->AddVariable("LayerS3FracNorm4",     &pisanormvar[ivar++]);
	  ecalpisareaderS_EVEN->AddVariable("LayerS3FracNorm5",     &pisanormvar[ivar++]);
	  ecalpisareaderS_EVEN->AddVariable("LayerS3FracNorm6",     &pisanormvar[ivar++]);
	  ecalpisareaderS_EVEN->AddVariable("LayerS3FracNorm7",     &pisanormvar[ivar++]);
	  ecalpisareaderS_EVEN->AddVariable("LayerS3FracNorm8",     &pisanormvar[ivar++]);
	  ecalpisareaderS_EVEN->AddVariable("LayerS3FracNorm9",     &pisanormvar[ivar++]);
	  ecalpisareaderS_EVEN->AddVariable("LayerS3FracNorm10",    &pisanormvar[ivar++]);
	  ecalpisareaderS_EVEN->AddVariable("LayerS3FracNorm11",    &pisanormvar[ivar++]);
	  ecalpisareaderS_EVEN->AddVariable("LayerS3FracNorm12",    &pisanormvar[ivar++]);
	  ecalpisareaderS_EVEN->AddVariable("LayerS3FracNorm13",    &pisanormvar[ivar++]);
	  ecalpisareaderS_EVEN->AddVariable("LayerS3FracNorm14",    &pisanormvar[ivar++]);
	  ecalpisareaderS_EVEN->AddVariable("LayerS3FracNorm15",    &pisanormvar[ivar++]);
	  ecalpisareaderS_EVEN->AddVariable("LayerS3FracNorm16",    &pisanormvar[ivar++]);
	  ecalpisareaderS_EVEN->AddVariable("LayerS3FracNorm17",    &pisanormvar[ivar++]);
	  //	  
	  ecalpisareaderS_ODD->BookMVA("BDTS_LAYERS_ODD", Form("%s/ECAL_PISA_BDTS_%d_v%dfinal_ODD.weights.xml", WeightsDir_v5,ECALBDT_TMVAVER,iBDTVERSION));
	  ecalpisareaderS_EVEN->BookMVA("BDTS_LAYERS_EVEN", Form("%s/ECAL_PISA_BDTS_%d_v%dfinal_EVEN.weights.xml", WeightsDir_v5,ECALBDT_TMVAVER,iBDTVERSION));
	  if ( iBDTVERSION == 5 ) 
	    {
	      ecalpisareaderS_v5_ODD = ecalpisareaderS_ODD;
	      ecalpisareaderS_v5_EVEN = ecalpisareaderS_EVEN;
	    }
	  else if ( iBDTVERSION == 6 )
	    {
	      ecalpisareaderS_v6_ODD = ecalpisareaderS_ODD;
	      ecalpisareaderS_v6_EVEN = ecalpisareaderS_EVEN;
	    }
	  else if ( iBDTVERSION == 7 )
	    {
	      ecalpisareaderS_v7_ODD = ecalpisareaderS_ODD;
	      ecalpisareaderS_v7_EVEN = ecalpisareaderS_EVEN;
	    }
	  else
	    {
	      cout<<" BDT reader NOT initialized; error in version number. iBDTVERSION = "<<iBDTVERSION<<endl;
	    }
	  //
	}
     if ( iBDTVERSION >= 5 && TMVAClassifier == 0 && EnergyFlag!=0 )
	{
	  ecalpisareader_E_ODD = new TMVA::Reader("Color:Silent");
	  ecalpisareader_E_ODD->AddSpectator("EnergyD", &pisanormvar[nPISABDTVARs]);
	  ivar = 0;
	  ecalpisareader_E_ODD->AddVariable("ShowerMeanNorm",       &pisanormvar[ivar++]);
	  ecalpisareader_E_ODD->AddVariable("F2SLEneDep",           &pisanormvar[ivar++]);
	  ecalpisareader_E_ODD->AddVariable("L2LFracNorm",          &pisanormvar[ivar++]);
	  ecalpisareader_E_ODD->AddVariable("R3cmFracNorm",         &pisanormvar[ivar++]);
	  ecalpisareader_E_ODD->AddVariable("S3totxNorm",           &pisanormvar[ivar++]);
	  ecalpisareader_E_ODD->AddVariable("S3totyNorm",           &pisanormvar[ivar++]);
	  ecalpisareader_E_ODD->AddVariable("NEcalHitsNorm",        &pisanormvar[ivar++]);
	  ecalpisareader_E_ODD->AddVariable("ShowerFootprintXNorm", &pisanormvar[ivar++]);
	  ecalpisareader_E_ODD->AddVariable("ShowerFootprintYNorm", &pisanormvar[ivar++]);
	  ecalpisareader_E_ODD->AddVariable("LayerEneFracNorm0",    &pisanormvar[ivar++]);
	  ecalpisareader_E_ODD->AddVariable("LayerEneFracNorm1",    &pisanormvar[ivar++]);
	  ecalpisareader_E_ODD->AddVariable("LayerEneFracNorm2",    &pisanormvar[ivar++]);
	  ecalpisareader_E_ODD->AddVariable("LayerEneFracNorm3",    &pisanormvar[ivar++]);
	  ecalpisareader_E_ODD->AddVariable("LayerEneFracNorm4",    &pisanormvar[ivar++]);
	  ecalpisareader_E_ODD->AddVariable("LayerEneFracNorm5",    &pisanormvar[ivar++]);
	  ecalpisareader_E_ODD->AddVariable("LayerEneFracNorm6",    &pisanormvar[ivar++]);
	  ecalpisareader_E_ODD->AddVariable("LayerEneFracNorm7",    &pisanormvar[ivar++]);
	  ecalpisareader_E_ODD->AddVariable("LayerEneFracNorm8",    &pisanormvar[ivar++]);
	  ecalpisareader_E_ODD->AddVariable("LayerEneFracNorm9",    &pisanormvar[ivar++]);
	  ecalpisareader_E_ODD->AddVariable("LayerEneFracNorm10",   &pisanormvar[ivar++]);
	  ecalpisareader_E_ODD->AddVariable("LayerEneFracNorm11",   &pisanormvar[ivar++]);
	  ecalpisareader_E_ODD->AddVariable("LayerEneFracNorm12",   &pisanormvar[ivar++]);
	  ecalpisareader_E_ODD->AddVariable("LayerEneFracNorm13",   &pisanormvar[ivar++]);
	  ecalpisareader_E_ODD->AddVariable("LayerEneFracNorm14",   &pisanormvar[ivar++]);
	  ecalpisareader_E_ODD->AddVariable("LayerEneFracNorm15",   &pisanormvar[ivar++]);
	  ecalpisareader_E_ODD->AddVariable("LayerEneFracNorm16",   &pisanormvar[ivar++]);
	  ecalpisareader_E_ODD->AddVariable("LayerEneFracNorm17",   &pisanormvar[ivar++]);
	  ecalpisareader_E_ODD->AddVariable("LayerS1S31",      &pisanormvar[ivar++]);
	  ecalpisareader_E_ODD->AddVariable("LayerS1S32",      &pisanormvar[ivar++]);
	  ecalpisareader_E_ODD->AddVariable("LayerS1S33",      &pisanormvar[ivar++]);
	  ecalpisareader_E_ODD->AddVariable("LayerS1S34",      &pisanormvar[ivar++]);
	  ecalpisareader_E_ODD->AddVariable("LayerS1S35",      &pisanormvar[ivar++]);
	  ecalpisareader_E_ODD->AddVariable("LayerS1S36",      &pisanormvar[ivar++]);
	  ecalpisareader_E_ODD->AddVariable("LayerS1S37",      &pisanormvar[ivar++]);
	  ecalpisareader_E_ODD->AddVariable("LayerSigmaNorm8",      &pisanormvar[ivar++]);
	  ecalpisareader_E_ODD->AddVariable("LayerSigmaNorm9",      &pisanormvar[ivar++]);
	  ecalpisareader_E_ODD->AddVariable("LayerSigmaNorm10",     &pisanormvar[ivar++]);
	  ecalpisareader_E_ODD->AddVariable("LayerSigmaNorm11",     &pisanormvar[ivar++]);
	  ecalpisareader_E_ODD->AddVariable("LayerSigmaNorm12",     &pisanormvar[ivar++]);
	  ecalpisareader_E_ODD->AddVariable("LayerSigmaNorm13",     &pisanormvar[ivar++]);
	  ecalpisareader_E_ODD->AddVariable("LayerSigmaNorm14",     &pisanormvar[ivar++]);
	  ecalpisareader_E_ODD->AddVariable("LayerSigmaNorm15",     &pisanormvar[ivar++]);
	  ecalpisareader_E_ODD->AddVariable("LayerSigmaNorm16",     &pisanormvar[ivar++]);
	  ecalpisareader_E_ODD->AddVariable("LayerSigmaNorm17",     &pisanormvar[ivar++]);
	  ecalpisareader_E_ODD->AddVariable("LayerS3FracNorm1",     &pisanormvar[ivar++]);
	  ecalpisareader_E_ODD->AddVariable("LayerS3FracNorm2",     &pisanormvar[ivar++]);
	  ecalpisareader_E_ODD->AddVariable("LayerS3FracNorm3",     &pisanormvar[ivar++]);
	  ecalpisareader_E_ODD->AddVariable("LayerS3FracNorm4",     &pisanormvar[ivar++]);
	  ecalpisareader_E_ODD->AddVariable("LayerS3FracNorm5",     &pisanormvar[ivar++]);
	  ecalpisareader_E_ODD->AddVariable("LayerS3FracNorm6",     &pisanormvar[ivar++]);
	  ecalpisareader_E_ODD->AddVariable("LayerS3FracNorm7",     &pisanormvar[ivar++]);
	  ecalpisareader_E_ODD->AddVariable("LayerS3FracNorm8",     &pisanormvar[ivar++]);
	  ecalpisareader_E_ODD->AddVariable("LayerS3FracNorm9",     &pisanormvar[ivar++]);
	  ecalpisareader_E_ODD->AddVariable("LayerS3FracNorm10",    &pisanormvar[ivar++]);
	  ecalpisareader_E_ODD->AddVariable("LayerS3FracNorm11",    &pisanormvar[ivar++]);
	  ecalpisareader_E_ODD->AddVariable("LayerS3FracNorm12",    &pisanormvar[ivar++]);
	  ecalpisareader_E_ODD->AddVariable("LayerS3FracNorm13",    &pisanormvar[ivar++]);
	  ecalpisareader_E_ODD->AddVariable("LayerS3FracNorm14",    &pisanormvar[ivar++]);
	  ecalpisareader_E_ODD->AddVariable("LayerS3FracNorm15",    &pisanormvar[ivar++]);
	  ecalpisareader_E_ODD->AddVariable("LayerS3FracNorm16",    &pisanormvar[ivar++]);
	  ecalpisareader_E_ODD->AddVariable("LayerS3FracNorm17",    &pisanormvar[ivar++]);
	  //
	  ecalpisareader_E_EVEN = new TMVA::Reader("Color:Silent");
	  ecalpisareader_E_EVEN->AddSpectator("EnergyD", &pisanormvar[nPISABDTVARs]);
	  ivar = 0;
	  ecalpisareader_E_EVEN->AddVariable("ShowerMeanNorm",       &pisanormvar[ivar++]);
	  ecalpisareader_E_EVEN->AddVariable("F2SLEneDep",           &pisanormvar[ivar++]);
	  ecalpisareader_E_EVEN->AddVariable("L2LFracNorm",          &pisanormvar[ivar++]);
	  ecalpisareader_E_EVEN->AddVariable("R3cmFracNorm",         &pisanormvar[ivar++]);
	  ecalpisareader_E_EVEN->AddVariable("S3totxNorm",           &pisanormvar[ivar++]);
	  ecalpisareader_E_EVEN->AddVariable("S3totyNorm",           &pisanormvar[ivar++]);
	  ecalpisareader_E_EVEN->AddVariable("NEcalHitsNorm",        &pisanormvar[ivar++]);
	  ecalpisareader_E_EVEN->AddVariable("ShowerFootprintXNorm", &pisanormvar[ivar++]);
	  ecalpisareader_E_EVEN->AddVariable("ShowerFootprintYNorm", &pisanormvar[ivar++]);
	  ecalpisareader_E_EVEN->AddVariable("LayerEneFracNorm0",    &pisanormvar[ivar++]);
	  ecalpisareader_E_EVEN->AddVariable("LayerEneFracNorm1",    &pisanormvar[ivar++]);
	  ecalpisareader_E_EVEN->AddVariable("LayerEneFracNorm2",    &pisanormvar[ivar++]);
	  ecalpisareader_E_EVEN->AddVariable("LayerEneFracNorm3",    &pisanormvar[ivar++]);
	  ecalpisareader_E_EVEN->AddVariable("LayerEneFracNorm4",    &pisanormvar[ivar++]);
	  ecalpisareader_E_EVEN->AddVariable("LayerEneFracNorm5",    &pisanormvar[ivar++]);
	  ecalpisareader_E_EVEN->AddVariable("LayerEneFracNorm6",    &pisanormvar[ivar++]);
	  ecalpisareader_E_EVEN->AddVariable("LayerEneFracNorm7",    &pisanormvar[ivar++]);
	  ecalpisareader_E_EVEN->AddVariable("LayerEneFracNorm8",    &pisanormvar[ivar++]);
	  ecalpisareader_E_EVEN->AddVariable("LayerEneFracNorm9",    &pisanormvar[ivar++]);
	  ecalpisareader_E_EVEN->AddVariable("LayerEneFracNorm10",   &pisanormvar[ivar++]);
	  ecalpisareader_E_EVEN->AddVariable("LayerEneFracNorm11",   &pisanormvar[ivar++]);
	  ecalpisareader_E_EVEN->AddVariable("LayerEneFracNorm12",   &pisanormvar[ivar++]);
	  ecalpisareader_E_EVEN->AddVariable("LayerEneFracNorm13",   &pisanormvar[ivar++]);
	  ecalpisareader_E_EVEN->AddVariable("LayerEneFracNorm14",   &pisanormvar[ivar++]);
	  ecalpisareader_E_EVEN->AddVariable("LayerEneFracNorm15",   &pisanormvar[ivar++]);
	  ecalpisareader_E_EVEN->AddVariable("LayerEneFracNorm16",   &pisanormvar[ivar++]);
	  ecalpisareader_E_EVEN->AddVariable("LayerEneFracNorm17",   &pisanormvar[ivar++]);
	  ecalpisareader_E_EVEN->AddVariable("LayerS1S31",      &pisanormvar[ivar++]);
	  ecalpisareader_E_EVEN->AddVariable("LayerS1S32",      &pisanormvar[ivar++]);
	  ecalpisareader_E_EVEN->AddVariable("LayerS1S33",      &pisanormvar[ivar++]);
	  ecalpisareader_E_EVEN->AddVariable("LayerS1S34",      &pisanormvar[ivar++]);
	  ecalpisareader_E_EVEN->AddVariable("LayerS1S35",      &pisanormvar[ivar++]);
	  ecalpisareader_E_EVEN->AddVariable("LayerS1S36",      &pisanormvar[ivar++]);
	  ecalpisareader_E_EVEN->AddVariable("LayerS1S37",      &pisanormvar[ivar++]);
	  ecalpisareader_E_EVEN->AddVariable("LayerSigmaNorm8",      &pisanormvar[ivar++]);
	  ecalpisareader_E_EVEN->AddVariable("LayerSigmaNorm9",      &pisanormvar[ivar++]);
	  ecalpisareader_E_EVEN->AddVariable("LayerSigmaNorm10",     &pisanormvar[ivar++]);
	  ecalpisareader_E_EVEN->AddVariable("LayerSigmaNorm11",     &pisanormvar[ivar++]);
	  ecalpisareader_E_EVEN->AddVariable("LayerSigmaNorm12",     &pisanormvar[ivar++]);
	  ecalpisareader_E_EVEN->AddVariable("LayerSigmaNorm13",     &pisanormvar[ivar++]);
	  ecalpisareader_E_EVEN->AddVariable("LayerSigmaNorm14",     &pisanormvar[ivar++]);
	  ecalpisareader_E_EVEN->AddVariable("LayerSigmaNorm15",     &pisanormvar[ivar++]);
	  ecalpisareader_E_EVEN->AddVariable("LayerSigmaNorm16",     &pisanormvar[ivar++]);
	  ecalpisareader_E_EVEN->AddVariable("LayerSigmaNorm17",     &pisanormvar[ivar++]);
	  ecalpisareader_E_EVEN->AddVariable("LayerS3FracNorm1",     &pisanormvar[ivar++]);
	  ecalpisareader_E_EVEN->AddVariable("LayerS3FracNorm2",     &pisanormvar[ivar++]);
	  ecalpisareader_E_EVEN->AddVariable("LayerS3FracNorm3",     &pisanormvar[ivar++]);
	  ecalpisareader_E_EVEN->AddVariable("LayerS3FracNorm4",     &pisanormvar[ivar++]);
	  ecalpisareader_E_EVEN->AddVariable("LayerS3FracNorm5",     &pisanormvar[ivar++]);
	  ecalpisareader_E_EVEN->AddVariable("LayerS3FracNorm6",     &pisanormvar[ivar++]);
	  ecalpisareader_E_EVEN->AddVariable("LayerS3FracNorm7",     &pisanormvar[ivar++]);
	  ecalpisareader_E_EVEN->AddVariable("LayerS3FracNorm8",     &pisanormvar[ivar++]);
	  ecalpisareader_E_EVEN->AddVariable("LayerS3FracNorm9",     &pisanormvar[ivar++]);
	  ecalpisareader_E_EVEN->AddVariable("LayerS3FracNorm10",    &pisanormvar[ivar++]);
	  ecalpisareader_E_EVEN->AddVariable("LayerS3FracNorm11",    &pisanormvar[ivar++]);
	  ecalpisareader_E_EVEN->AddVariable("LayerS3FracNorm12",    &pisanormvar[ivar++]);
	  ecalpisareader_E_EVEN->AddVariable("LayerS3FracNorm13",    &pisanormvar[ivar++]);
	  ecalpisareader_E_EVEN->AddVariable("LayerS3FracNorm14",    &pisanormvar[ivar++]);
	  ecalpisareader_E_EVEN->AddVariable("LayerS3FracNorm15",    &pisanormvar[ivar++]);
	  ecalpisareader_E_EVEN->AddVariable("LayerS3FracNorm16",    &pisanormvar[ivar++]);
	  ecalpisareader_E_EVEN->AddVariable("LayerS3FracNorm17",    &pisanormvar[ivar++]);
	  //
	  ecalpisareader_E_ODD->BookMVA("BDTG_LAYERS_ODD", Form("%s/ECAL_PISA_BDT_%d_v%dfinal_E_ODD.weights.xml", WeightsDir_v5,ECALBDT_TMVAVER,iBDTVERSION));
	  ecalpisareader_E_EVEN->BookMVA("BDTG_LAYERS_EVEN", Form("%s/ECAL_PISA_BDT_%d_v%dfinal_E_EVEN.weights.xml", WeightsDir_v5,ECALBDT_TMVAVER,iBDTVERSION));
	  if ( iBDTVERSION == 5 ) 
	    {
	      ecalpisareader_E_v5_ODD = ecalpisareader_E_ODD;
	      ecalpisareader_E_v5_EVEN = ecalpisareader_E_EVEN;
	    }
	  else if ( iBDTVERSION == 6 )
	    {
	      ecalpisareader_E_v6_ODD = ecalpisareader_E_ODD;
	      ecalpisareader_E_v6_EVEN = ecalpisareader_E_EVEN;
	    }
	  else if ( iBDTVERSION == 7 )
	    {
	      ecalpisareader_E_v7_ODD = ecalpisareader_E_ODD;
	      ecalpisareader_E_v7_EVEN = ecalpisareader_E_EVEN;
	    }
	  else
	    {
	      cout<<" BDT reader NOT initialized; error in version number. iBDTVERSION = "<<iBDTVERSION<<endl;
	    }
	  //
	}
      if ( iBDTVERSION >= 5 && TMVAClassifier == 1 && EnergyFlag!=0 )
	{
	  ecalpisareaderS_E_ODD = new TMVA::Reader("Color:Silent");
	  ecalpisareaderS_E_ODD->AddSpectator("EnergyD", &pisanormvar[nPISABDTVARs]);
	  ivar = 0;
	  ecalpisareaderS_E_ODD->AddVariable("ShowerMeanNorm",       &pisanormvar[ivar++]);
	  ecalpisareaderS_E_ODD->AddVariable("F2SLEneDep",           &pisanormvar[ivar++]);
	  ecalpisareaderS_E_ODD->AddVariable("L2LFracNorm",          &pisanormvar[ivar++]);
	  ecalpisareaderS_E_ODD->AddVariable("R3cmFracNorm",         &pisanormvar[ivar++]);
	  ecalpisareaderS_E_ODD->AddVariable("S3totxNorm",           &pisanormvar[ivar++]);
	  ecalpisareaderS_E_ODD->AddVariable("S3totyNorm",           &pisanormvar[ivar++]);
	  ecalpisareaderS_E_ODD->AddVariable("NEcalHitsNorm",        &pisanormvar[ivar++]);
	  ecalpisareaderS_E_ODD->AddVariable("ShowerFootprintXNorm", &pisanormvar[ivar++]);
	  ecalpisareaderS_E_ODD->AddVariable("ShowerFootprintYNorm", &pisanormvar[ivar++]);
	  ecalpisareaderS_E_ODD->AddVariable("LayerEneFracNorm0",    &pisanormvar[ivar++]);
	  ecalpisareaderS_E_ODD->AddVariable("LayerEneFracNorm1",    &pisanormvar[ivar++]);
	  ecalpisareaderS_E_ODD->AddVariable("LayerEneFracNorm2",    &pisanormvar[ivar++]);
	  ecalpisareaderS_E_ODD->AddVariable("LayerEneFracNorm3",    &pisanormvar[ivar++]);
	  ecalpisareaderS_E_ODD->AddVariable("LayerEneFracNorm4",    &pisanormvar[ivar++]);
	  ecalpisareaderS_E_ODD->AddVariable("LayerEneFracNorm5",    &pisanormvar[ivar++]);
	  ecalpisareaderS_E_ODD->AddVariable("LayerEneFracNorm6",    &pisanormvar[ivar++]);
	  ecalpisareaderS_E_ODD->AddVariable("LayerEneFracNorm7",    &pisanormvar[ivar++]);
	  ecalpisareaderS_E_ODD->AddVariable("LayerEneFracNorm8",    &pisanormvar[ivar++]);
	  ecalpisareaderS_E_ODD->AddVariable("LayerEneFracNorm9",    &pisanormvar[ivar++]);
	  ecalpisareaderS_E_ODD->AddVariable("LayerEneFracNorm10",   &pisanormvar[ivar++]);
	  ecalpisareaderS_E_ODD->AddVariable("LayerEneFracNorm11",   &pisanormvar[ivar++]);
	  ecalpisareaderS_E_ODD->AddVariable("LayerEneFracNorm12",   &pisanormvar[ivar++]);
	  ecalpisareaderS_E_ODD->AddVariable("LayerEneFracNorm13",   &pisanormvar[ivar++]);
	  ecalpisareaderS_E_ODD->AddVariable("LayerEneFracNorm14",   &pisanormvar[ivar++]);
	  ecalpisareaderS_E_ODD->AddVariable("LayerEneFracNorm15",   &pisanormvar[ivar++]);
	  ecalpisareaderS_E_ODD->AddVariable("LayerEneFracNorm16",   &pisanormvar[ivar++]);
	  ecalpisareaderS_E_ODD->AddVariable("LayerEneFracNorm17",   &pisanormvar[ivar++]);
	  ecalpisareaderS_E_ODD->AddVariable("LayerS1S31",      &pisanormvar[ivar++]);
	  ecalpisareaderS_E_ODD->AddVariable("LayerS1S32",      &pisanormvar[ivar++]);
	  ecalpisareaderS_E_ODD->AddVariable("LayerS1S33",      &pisanormvar[ivar++]);
	  ecalpisareaderS_E_ODD->AddVariable("LayerS1S34",      &pisanormvar[ivar++]);
	  ecalpisareaderS_E_ODD->AddVariable("LayerS1S35",      &pisanormvar[ivar++]);
	  ecalpisareaderS_E_ODD->AddVariable("LayerS1S36",      &pisanormvar[ivar++]);
	  ecalpisareaderS_E_ODD->AddVariable("LayerS1S37",      &pisanormvar[ivar++]);
	  ecalpisareaderS_E_ODD->AddVariable("LayerSigmaNorm8",      &pisanormvar[ivar++]);
	  ecalpisareaderS_E_ODD->AddVariable("LayerSigmaNorm9",      &pisanormvar[ivar++]);
	  ecalpisareaderS_E_ODD->AddVariable("LayerSigmaNorm10",     &pisanormvar[ivar++]);
	  ecalpisareaderS_E_ODD->AddVariable("LayerSigmaNorm11",     &pisanormvar[ivar++]);
	  ecalpisareaderS_E_ODD->AddVariable("LayerSigmaNorm12",     &pisanormvar[ivar++]);
	  ecalpisareaderS_E_ODD->AddVariable("LayerSigmaNorm13",     &pisanormvar[ivar++]);
	  ecalpisareaderS_E_ODD->AddVariable("LayerSigmaNorm14",     &pisanormvar[ivar++]);
	  ecalpisareaderS_E_ODD->AddVariable("LayerSigmaNorm15",     &pisanormvar[ivar++]);
	  ecalpisareaderS_E_ODD->AddVariable("LayerSigmaNorm16",     &pisanormvar[ivar++]);
	  ecalpisareaderS_E_ODD->AddVariable("LayerSigmaNorm17",     &pisanormvar[ivar++]);
	  ecalpisareaderS_E_ODD->AddVariable("LayerS3FracNorm1",     &pisanormvar[ivar++]);
	  ecalpisareaderS_E_ODD->AddVariable("LayerS3FracNorm2",     &pisanormvar[ivar++]);
	  ecalpisareaderS_E_ODD->AddVariable("LayerS3FracNorm3",     &pisanormvar[ivar++]);
	  ecalpisareaderS_E_ODD->AddVariable("LayerS3FracNorm4",     &pisanormvar[ivar++]);
	  ecalpisareaderS_E_ODD->AddVariable("LayerS3FracNorm5",     &pisanormvar[ivar++]);
	  ecalpisareaderS_E_ODD->AddVariable("LayerS3FracNorm6",     &pisanormvar[ivar++]);
	  ecalpisareaderS_E_ODD->AddVariable("LayerS3FracNorm7",     &pisanormvar[ivar++]);
	  ecalpisareaderS_E_ODD->AddVariable("LayerS3FracNorm8",     &pisanormvar[ivar++]);
	  ecalpisareaderS_E_ODD->AddVariable("LayerS3FracNorm9",     &pisanormvar[ivar++]);
	  ecalpisareaderS_E_ODD->AddVariable("LayerS3FracNorm10",    &pisanormvar[ivar++]);
	  ecalpisareaderS_E_ODD->AddVariable("LayerS3FracNorm11",    &pisanormvar[ivar++]);
	  ecalpisareaderS_E_ODD->AddVariable("LayerS3FracNorm12",    &pisanormvar[ivar++]);
	  ecalpisareaderS_E_ODD->AddVariable("LayerS3FracNorm13",    &pisanormvar[ivar++]);
	  ecalpisareaderS_E_ODD->AddVariable("LayerS3FracNorm14",    &pisanormvar[ivar++]);
	  ecalpisareaderS_E_ODD->AddVariable("LayerS3FracNorm15",    &pisanormvar[ivar++]);
	  ecalpisareaderS_E_ODD->AddVariable("LayerS3FracNorm16",    &pisanormvar[ivar++]);
	  ecalpisareaderS_E_ODD->AddVariable("LayerS3FracNorm17",    &pisanormvar[ivar++]);
	  //
	  ecalpisareaderS_E_EVEN = new TMVA::Reader("Color:Silent");
	  ecalpisareaderS_E_EVEN->AddSpectator("EnergyD", &pisanormvar[nPISABDTVARs]);
	  ivar = 0;
	  ecalpisareaderS_E_EVEN->AddVariable("ShowerMeanNorm",       &pisanormvar[ivar++]);
	  ecalpisareaderS_E_EVEN->AddVariable("F2SLEneDep",           &pisanormvar[ivar++]);
	  ecalpisareaderS_E_EVEN->AddVariable("L2LFracNorm",          &pisanormvar[ivar++]);
	  ecalpisareaderS_E_EVEN->AddVariable("R3cmFracNorm",         &pisanormvar[ivar++]);
	  ecalpisareaderS_E_EVEN->AddVariable("S3totxNorm",           &pisanormvar[ivar++]);
	  ecalpisareaderS_E_EVEN->AddVariable("S3totyNorm",           &pisanormvar[ivar++]);
	  ecalpisareaderS_E_EVEN->AddVariable("NEcalHitsNorm",        &pisanormvar[ivar++]);
	  ecalpisareaderS_E_EVEN->AddVariable("ShowerFootprintXNorm", &pisanormvar[ivar++]);
	  ecalpisareaderS_E_EVEN->AddVariable("ShowerFootprintYNorm", &pisanormvar[ivar++]);
	  ecalpisareaderS_E_EVEN->AddVariable("LayerEneFracNorm0",    &pisanormvar[ivar++]);
	  ecalpisareaderS_E_EVEN->AddVariable("LayerEneFracNorm1",    &pisanormvar[ivar++]);
	  ecalpisareaderS_E_EVEN->AddVariable("LayerEneFracNorm2",    &pisanormvar[ivar++]);
	  ecalpisareaderS_E_EVEN->AddVariable("LayerEneFracNorm3",    &pisanormvar[ivar++]);
	  ecalpisareaderS_E_EVEN->AddVariable("LayerEneFracNorm4",    &pisanormvar[ivar++]);
	  ecalpisareaderS_E_EVEN->AddVariable("LayerEneFracNorm5",    &pisanormvar[ivar++]);
	  ecalpisareaderS_E_EVEN->AddVariable("LayerEneFracNorm6",    &pisanormvar[ivar++]);
	  ecalpisareaderS_E_EVEN->AddVariable("LayerEneFracNorm7",    &pisanormvar[ivar++]);
	  ecalpisareaderS_E_EVEN->AddVariable("LayerEneFracNorm8",    &pisanormvar[ivar++]);
	  ecalpisareaderS_E_EVEN->AddVariable("LayerEneFracNorm9",    &pisanormvar[ivar++]);
	  ecalpisareaderS_E_EVEN->AddVariable("LayerEneFracNorm10",   &pisanormvar[ivar++]);
	  ecalpisareaderS_E_EVEN->AddVariable("LayerEneFracNorm11",   &pisanormvar[ivar++]);
	  ecalpisareaderS_E_EVEN->AddVariable("LayerEneFracNorm12",   &pisanormvar[ivar++]);
	  ecalpisareaderS_E_EVEN->AddVariable("LayerEneFracNorm13",   &pisanormvar[ivar++]);
	  ecalpisareaderS_E_EVEN->AddVariable("LayerEneFracNorm14",   &pisanormvar[ivar++]);
	  ecalpisareaderS_E_EVEN->AddVariable("LayerEneFracNorm15",   &pisanormvar[ivar++]);
	  ecalpisareaderS_E_EVEN->AddVariable("LayerEneFracNorm16",   &pisanormvar[ivar++]);
	  ecalpisareaderS_E_EVEN->AddVariable("LayerEneFracNorm17",   &pisanormvar[ivar++]);
	  ecalpisareaderS_E_EVEN->AddVariable("LayerS1S31",      &pisanormvar[ivar++]);
	  ecalpisareaderS_E_EVEN->AddVariable("LayerS1S32",      &pisanormvar[ivar++]);
	  ecalpisareaderS_E_EVEN->AddVariable("LayerS1S33",      &pisanormvar[ivar++]);
	  ecalpisareaderS_E_EVEN->AddVariable("LayerS1S34",      &pisanormvar[ivar++]);
	  ecalpisareaderS_E_EVEN->AddVariable("LayerS1S35",      &pisanormvar[ivar++]);
	  ecalpisareaderS_E_EVEN->AddVariable("LayerS1S36",      &pisanormvar[ivar++]);
	  ecalpisareaderS_E_EVEN->AddVariable("LayerS1S37",      &pisanormvar[ivar++]);
	  ecalpisareaderS_E_EVEN->AddVariable("LayerSigmaNorm8",      &pisanormvar[ivar++]);
	  ecalpisareaderS_E_EVEN->AddVariable("LayerSigmaNorm9",      &pisanormvar[ivar++]);
	  ecalpisareaderS_E_EVEN->AddVariable("LayerSigmaNorm10",     &pisanormvar[ivar++]);
	  ecalpisareaderS_E_EVEN->AddVariable("LayerSigmaNorm11",     &pisanormvar[ivar++]);
	  ecalpisareaderS_E_EVEN->AddVariable("LayerSigmaNorm12",     &pisanormvar[ivar++]);
	  ecalpisareaderS_E_EVEN->AddVariable("LayerSigmaNorm13",     &pisanormvar[ivar++]);
	  ecalpisareaderS_E_EVEN->AddVariable("LayerSigmaNorm14",     &pisanormvar[ivar++]);
	  ecalpisareaderS_E_EVEN->AddVariable("LayerSigmaNorm15",     &pisanormvar[ivar++]);
	  ecalpisareaderS_E_EVEN->AddVariable("LayerSigmaNorm16",     &pisanormvar[ivar++]);
	  ecalpisareaderS_E_EVEN->AddVariable("LayerSigmaNorm17",     &pisanormvar[ivar++]);
	  ecalpisareaderS_E_EVEN->AddVariable("LayerS3FracNorm1",     &pisanormvar[ivar++]);
	  ecalpisareaderS_E_EVEN->AddVariable("LayerS3FracNorm2",     &pisanormvar[ivar++]);
	  ecalpisareaderS_E_EVEN->AddVariable("LayerS3FracNorm3",     &pisanormvar[ivar++]);
	  ecalpisareaderS_E_EVEN->AddVariable("LayerS3FracNorm4",     &pisanormvar[ivar++]);
	  ecalpisareaderS_E_EVEN->AddVariable("LayerS3FracNorm5",     &pisanormvar[ivar++]);
	  ecalpisareaderS_E_EVEN->AddVariable("LayerS3FracNorm6",     &pisanormvar[ivar++]);
	  ecalpisareaderS_E_EVEN->AddVariable("LayerS3FracNorm7",     &pisanormvar[ivar++]);
	  ecalpisareaderS_E_EVEN->AddVariable("LayerS3FracNorm8",     &pisanormvar[ivar++]);
	  ecalpisareaderS_E_EVEN->AddVariable("LayerS3FracNorm9",     &pisanormvar[ivar++]);
	  ecalpisareaderS_E_EVEN->AddVariable("LayerS3FracNorm10",    &pisanormvar[ivar++]);
	  ecalpisareaderS_E_EVEN->AddVariable("LayerS3FracNorm11",    &pisanormvar[ivar++]);
	  ecalpisareaderS_E_EVEN->AddVariable("LayerS3FracNorm12",    &pisanormvar[ivar++]);
	  ecalpisareaderS_E_EVEN->AddVariable("LayerS3FracNorm13",    &pisanormvar[ivar++]);
	  ecalpisareaderS_E_EVEN->AddVariable("LayerS3FracNorm14",    &pisanormvar[ivar++]);
	  ecalpisareaderS_E_EVEN->AddVariable("LayerS3FracNorm15",    &pisanormvar[ivar++]);
	  ecalpisareaderS_E_EVEN->AddVariable("LayerS3FracNorm16",    &pisanormvar[ivar++]);
	  ecalpisareaderS_E_EVEN->AddVariable("LayerS3FracNorm17",    &pisanormvar[ivar++]);
	  //
	  ecalpisareaderS_E_ODD->BookMVA("BDTS_LAYERS_ODD", Form("%s/ECAL_PISA_BDTS_%d_v%dfinal_E_ODD.weights.xml", WeightsDir_v5,ECALBDT_TMVAVER,iBDTVERSION));
	  ecalpisareaderS_E_EVEN->BookMVA("BDTS_LAYERS_EVEN", Form("%s/ECAL_PISA_BDTS_%d_v%dfinal_E_EVEN.weights.xml", WeightsDir_v5,ECALBDT_TMVAVER,iBDTVERSION));
	  if ( iBDTVERSION == 5 ) 
	    {
	      ecalpisareaderS_E_v5_ODD = ecalpisareaderS_E_ODD;
	      ecalpisareaderS_E_v5_EVEN = ecalpisareaderS_E_EVEN;
	    }
	  else if ( iBDTVERSION == 6 )
	    {
	      ecalpisareaderS_E_v6_ODD = ecalpisareaderS_E_ODD;
	      ecalpisareaderS_E_v6_EVEN = ecalpisareaderS_E_EVEN;
	    }
	  else if ( iBDTVERSION == 7 )
	    {
	      ecalpisareaderS_E_v7_ODD = ecalpisareaderS_E_ODD;
	      ecalpisareaderS_E_v7_EVEN = ecalpisareaderS_E_EVEN;
	    }
	  else
	    {
	      cout<<" BDT reader NOT initialized; error in version number. iBDTVERSION = "<<iBDTVERSION<<endl;
	    }
	  //
	}

   }


   if ( iBDTVERSION == 3 )
     {
       if( apply_precuts )
	 {
	   //*******************************
	   //*****Reject without BDT********
	   //*******************************

	   //Check the energy deposit in the first 2 superlayers
	   if (F2SLEneDep < TMath::Max(0.1, -0.02 + 0.17*log(energyd) + 8.7e-4*pow((float)log(energyd), (float)4.00))) return -0.9991;

	   //Check S1/S3 per layer
	   int first_lay = -1; //first layer in which we want to estimate S1/S3
	   int last_lay  = -1; //last  layer in which we want to estimate S1/S3

	   if (energyd < 4 )
	     {
	       first_lay = 3;
	       last_lay = 6;
	     }
	   else if (energyd < 12)
	     {
	       first_lay = 2;
	       last_lay = 7;
	     }
	   else
	     {
	       first_lay = 2;
	       last_lay = 11;
	     }

	   //request 2 consecutive layers with MIP like energy deposit
	   for (unsigned int ilayer = first_lay; ilayer < last_lay; ++ilayer)
	     {
	       if (LayerS1S3[ilayer] > 0.995 && LayerS1S3[ilayer+1] > 0.995) return -0.9993;
	     }
	 }

       //********************************
       //*****Normalize the Variables****
       //********************************
       // fix: normalization may be NaN below 0.5 GeV or above 1000 GeV;
       energyd = energyd < 0.5 ? 0.5 : (energyd > 1000. ? 1000. : energyd);
       float mean, sigma;
       float x = log(energyd);

       if (BDT_DEBUG) std::cout << Form(" ??? x=%f\n", x) << flush;

       ivar = 0;

       mean = ((((8.00617+(-4.1675*x))+(3.14574*pow(x,2)))+(-0.886598*pow(x,3)))+(0.116057*pow(x,4)))+(-0.00576047*pow(x,5));
       sigma = ((((1.33031+(-1.12867*x))+(0.827807*pow(x,2)))+(-0.2544*pow(x,3)))+(0.0347401*pow(x,4)))+(-0.0017621*pow(x,5));
       pisanormvar[ivar++] = (ShowerMean - mean)/sigma;

       mean = ((((3.29054+(0.294302*x))+(-0.106969*pow(x,2)))+(0.0283536*pow(x,3)))+(-0.00451744*pow(x,4)))+(0.000268761*pow(x,5));
       sigma = ((((0.603386+(-0.471847*x))+(0.277909*pow(x,2)))+(-0.0809714*pow(x,3)))+(0.0108916*pow(x,4)))+(-0.000544912*pow(x,5));
       pisanormvar[ivar++] = (ShowerSigma - mean)/sigma;

       mean = ((((0.0529547+(-0.0758641*x))+(0.0535359*pow(x,2)))+(-0.0154464*pow(x,3)))+(0.00212464*pow(x,4)))+(-0.00010951*pow(x,5));
       sigma = ((((0.0353068+(-0.042352*x))+(0.0279684*pow(x,2)))+(-0.00768403*pow(x,3)))+(0.000983816*pow(x,4)))+(-4.82713e-05*pow(x,5));
       pisanormvar[ivar++] = (L2LFrac - mean)/sigma;

       mean = ((((0.965114+(0.00203957*x))+(0.00303998*pow(x,2)))+(-0.00198015*pow(x,3)))+(0.000365963*pow(x,4)))+(-2.16383e-05*pow(x,5));
       sigma = ((((0.0117814+(0.00603827*x))+(-0.00483091*pow(x,2)))+(0.00120864*pow(x,3)))+(-0.00012701*pow(x,4)))+(4.7641e-06*pow(x,5));
       pisanormvar[ivar++] = (R3cmFrac - mean)/sigma;

       mean = ((((0.938053+(-0.0881351*x))+(0.0519403*pow(x,2)))+(-0.0143827*pow(x,3)))+(0.00188787*pow(x,4)))+(-9.3875e-05*pow(x,5));
       sigma = ((((0.0351859+(0.00491194*x))+(-0.00670658*pow(x,2)))+(0.00162247*pow(x,3)))+(-0.000145594*pow(x,4)))+(3.93369e-06*pow(x,5));
       pisanormvar[ivar++] = (S3totx - mean)/sigma;

       mean = ((((0.939638+(-0.248437*x))+(0.167374*pow(x,2)))+(-0.0461894*pow(x,3)))+(0.00577807*pow(x,4)))+(-0.000271317*pow(x,5));
       sigma = ((((-0.0514172+(0.270373*x))+(-0.172495*pow(x,2)))+(0.0445621*pow(x,3)))+(-0.00520866*pow(x,4)))+(0.000229177*pow(x,5));
       pisanormvar[ivar++] = (S3toty - mean)/sigma;

       mean = ((((50.0474+(11.7252*x))+(17.1394*pow(x,2)))+(-5.85539*pow(x,3)))+(1.04602*pow(x,4)))+(-0.0635492*pow(x,5));
       sigma = ((((8.64203+(-1.0843*x))+(1.68262*pow(x,2)))+(-0.623193*pow(x,3)))+(0.12814*pow(x,4)))+(-0.00829679*pow(x,5));
       pisanormvar[ivar++] = (NEcalHits - mean)/sigma;

       mean = ((((3.03348+(2.31497*x))+(-1.63238*pow(x,2)))+(0.503692*pow(x,3)))+(-0.0692921*pow(x,4)))+(0.00349709*pow(x,5));
       sigma = ((((1.09504+(0.136964*x))+(-0.286982*pow(x,2)))+(0.0906317*pow(x,3)))+(-0.0110907*pow(x,4)))+(0.000477313*pow(x,5));
       pisanormvar[ivar++] = (ShowerFootprintX - mean)/sigma;

       mean = ((((3.46992+(4.02032*x))+(-2.83443*pow(x,2)))+(0.825746*pow(x,3)))+(-0.107017*pow(x,4)))+(0.0051325*pow(x,5));
       sigma = ((((1.03576+(1.40527*x))+(-1.14092*pow(x,2)))+(0.317023*pow(x,3)))+(-0.0380902*pow(x,4)))+(0.00169085*pow(x,5));
       pisanormvar[ivar++] = (ShowerFootprintY - mean)/sigma;

       mean = ((((0.0280052+(-0.00925156*x))+(-0.000652646*pow(x,2)))+(0.000704125*pow(x,3)))+(-0.000110536*pow(x,4)))+(5.57125e-06*pow(x,5));
       sigma = ((((0.0180499+(-0.0075947*x))+(0.000797338*pow(x,2)))+(0.000119519*pow(x,3)))+(-3.02906e-05*pow(x,4)))+(1.7065e-06*pow(x,5));
       pisanormvar[ivar++] = (LayerEneFrac[0] - mean)/sigma;

       mean = ((((0.0517181+(0.00471548*x))+(-0.014246*pow(x,2)))+(0.0044595*pow(x,3)))+(-0.000558943*pow(x,4)))+(2.56099e-05*pow(x,5));
       sigma = ((((0.0156159+(0.0130422*x))+(-0.0112817*pow(x,2)))+(0.00304504*pow(x,3)))+(-0.000358963*pow(x,4)))+(1.58651e-05*pow(x,5));
       pisanormvar[ivar++] = (LayerEneFrac[1] - mean)/sigma;

       mean = ((((0.082507+(0.0263505*x))+(-0.0334409*pow(x,2)))+(0.00971363*pow(x,3)))+(-0.00120384*pow(x,4)))+(5.57467e-05*pow(x,5));
       sigma = ((((0.017612+(0.0248122*x))+(-0.0175606*pow(x,2)))+(0.00443319*pow(x,3)))+(-0.000509795*pow(x,4)))+(2.24742e-05*pow(x,5));
       pisanormvar[ivar++] = (LayerEneFrac[2] - mean)/sigma;

       mean = ((((0.072016+(0.0808915*x))+(-0.0624339*pow(x,2)))+(0.0166968*pow(x,3)))+(-0.0020309*pow(x,4)))+(9.42977e-05*pow(x,5));
       sigma = ((((0.0110229+(0.0352031*x))+(-0.0205781*pow(x,2)))+(0.00481713*pow(x,3)))+(-0.000533901*pow(x,4)))+(2.31054e-05*pow(x,5));
       pisanormvar[ivar++] = (LayerEneFrac[3] - mean)/sigma;

       mean = ((((0.0607379+(0.11278*x))+(-0.0759775*pow(x,2)))+(0.0199047*pow(x,3)))+(-0.00245621*pow(x,4)))+(0.000116962*pow(x,5));
       sigma = ((((0.0146652+(0.0227097*x))+(-0.0118331*pow(x,2)))+(0.00258137*pow(x,3)))+(-0.000282278*pow(x,4)))+(1.24497e-05*pow(x,5));
       pisanormvar[ivar++] = (LayerEneFrac[4] - mean)/sigma;

       mean = ((((0.0252142+(0.15299*x))+(-0.0894013*pow(x,2)))+(0.0227349*pow(x,3)))+(-0.00280797*pow(x,4)))+(0.000134945*pow(x,5));
       sigma = ((((0.0375805+(-0.0183599*x))+(0.0112424*pow(x,2)))+(-0.003171*pow(x,3)))+(0.000395071*pow(x,4)))+(-1.8314e-05*pow(x,5));
       pisanormvar[ivar++] = (LayerEneFrac[5] - mean)/sigma;

       mean = ((((0.0141538+(0.140389*x))+(-0.0775382*pow(x,2)))+(0.0200091*pow(x,3)))+(-0.00255522*pow(x,4)))+(0.000126566*pow(x,5));
       sigma = ((((0.0320343+(-0.00759271*x))+(0.00345219*pow(x,2)))+(-0.000832433*pow(x,3)))+(9.70949e-05*pow(x,4)))+(-4.6765e-06*pow(x,5));
       pisanormvar[ivar++] = (LayerEneFrac[6] - mean)/sigma;

       mean = ((((0.0318491+(0.0867088*x))+(-0.0456798*pow(x,2)))+(0.012293*pow(x,3)))+(-0.00167421*pow(x,4)))+(8.73654e-05*pow(x,5));
       sigma = ((((0.0249615+(0.00276973*x))+(-0.00326062*pow(x,2)))+(0.000844262*pow(x,3)))+(-7.62921e-05*pow(x,4)))+(1.4791e-06*pow(x,5));
       pisanormvar[ivar++] = (LayerEneFrac[7] - mean)/sigma;

       mean = ((((0.0161808+(0.0836758*x))+(-0.0407576*pow(x,2)))+(0.0110358*pow(x,3)))+(-0.00150136*pow(x,4)))+(7.73109e-05*pow(x,5));
       sigma = ((((0.0360674+(-0.0212573*x))+(0.0122676*pow(x,2)))+(-0.00390633*pow(x,3)))+(0.000592575*pow(x,4)))+(-3.3332e-05*pow(x,5));
       pisanormvar[ivar++] = (LayerEneFrac[8] - mean)/sigma;

       mean = ((((0.0244549+(0.0488281*x))+(-0.0214148*pow(x,2)))+(0.00622347*pow(x,3)))+(-0.000900322*pow(x,4)))+(4.77262e-05*pow(x,5));
       sigma = ((((0.0283512+(-0.0156621*x))+(0.0120695*pow(x,2)))+(-0.0044445*pow(x,3)))+(0.000702149*pow(x,4)))+(-3.93844e-05*pow(x,5));
       pisanormvar[ivar++] = (LayerEneFrac[9] - mean)/sigma;

       mean = ((((0.0476718+(-0.0104388*x))+(0.0118922*pow(x,2)))+(-0.00250586*pow(x,3)))+(0.000217343*pow(x,4)))+(-7.34415e-06*pow(x,5));
       sigma = ((((0.0269753+(-0.0147689*x))+(0.0106824*pow(x,2)))+(-0.00368057*pow(x,3)))+(0.000544268*pow(x,4)))+(-2.88223e-05*pow(x,5));
       pisanormvar[ivar++] = (LayerEneFrac[10] - mean)/sigma;

       mean = ((((0.0356985+(-0.0117932*x))+(0.0146905*pow(x,2)))+(-0.0037365*pow(x,3)))+(0.000435898*pow(x,4)))+(-2.01398e-05*pow(x,5));
       sigma = ((((0.004334+(0.0169608*x))+(-0.00710033*pow(x,2)))+(0.00127877*pow(x,3)))+(-0.000117297*pow(x,4)))+(4.53786e-06*pow(x,5));
       pisanormvar[ivar++] = (LayerEneFrac[11] - mean)/sigma;

       mean = ((((0.0412289+(-0.0348184*x))+(0.028702*pow(x,2)))+(-0.00795863*pow(x,3)))+(0.00104425*pow(x,4)))+(-5.26424e-05*pow(x,5));
       sigma = ((((0.01175+(0.00357072*x))+(-0.000299332*pow(x,2)))+(-0.000143466*pow(x,3)))+(2.01647e-05*pow(x,4)))+(-6.62616e-07*pow(x,5));
       pisanormvar[ivar++] = (LayerEneFrac[12] - mean)/sigma;

       mean = ((((0.0428603+(-0.0463803*x))+(0.034574*pow(x,2)))+(-0.00961033*pow(x,3)))+(0.0012701*pow(x,4)))+(-6.39456e-05*pow(x,5));
       sigma = ((((0.0215785+(-0.0138992*x))+(0.00930931*pow(x,2)))+(-0.00255221*pow(x,3)))+(0.000312052*pow(x,4)))+(-1.44322e-05*pow(x,5));
       pisanormvar[ivar++] = (LayerEneFrac[13] - mean)/sigma;

       mean = ((((0.0331326+(-0.0395662*x))+(0.0297418*pow(x,2)))+(-0.0084616*pow(x,3)))+(0.0011505*pow(x,4)))+(-5.91796e-05*pow(x,5));
       sigma = ((((0.0154186+(-0.0101873*x))+(0.00810784*pow(x,2)))+(-0.00236648*pow(x,3)))+(0.000306476*pow(x,4)))+(-1.49202e-05*pow(x,5));
       pisanormvar[ivar++] = (LayerEneFrac[14] - mean)/sigma;

       mean = ((((0.0417723+(-0.0574677*x))+(0.0394755*pow(x,2)))+(-0.0111518*pow(x,3)))+(0.00150212*pow(x,4)))+(-7.64829e-05*pow(x,5));
       sigma = ((((0.0274048+(-0.0298269*x))+(0.0187149*pow(x,2)))+(-0.00503019*pow(x,3)))+(0.000627959*pow(x,4)))+(-3.00033e-05*pow(x,5));
       pisanormvar[ivar++] = (LayerEneFrac[15] - mean)/sigma;

       mean = ((((0.0418007+(-0.0581073*x))+(0.0381565*pow(x,2)))+(-0.0106667*pow(x,3)))+(0.00142576*pow(x,4)))+(-7.19586e-05*pow(x,5));
       sigma = ((((0.0277066+(-0.031663*x))+(0.0200473*pow(x,2)))+(-0.00549493*pow(x,3)))+(0.000702769*pow(x,4)))+(-3.43082e-05*pow(x,5));
       pisanormvar[ivar++] = (LayerEneFrac[16] - mean)/sigma;

       mean = ((((0.0414082+(-0.0566391*x))+(0.0354689*pow(x,2)))+(-0.00973715*pow(x,3)))+(0.0012855*pow(x,4)))+(-6.42874e-05*pow(x,5));
       sigma = ((((0.0237586+(-0.0228116*x))+(0.0135739*pow(x,2)))+(-0.00353872*pow(x,3)))+(0.000439854*pow(x,4)))+(-2.12188e-05*pow(x,5));
       pisanormvar[ivar++] = (LayerEneFrac[17] - mean)/sigma;

       mean = ((((1.22821+(-0.251616*x))+(0.160067*pow(x,2)))+(-0.0592895*pow(x,3)))+(0.0103401*pow(x,4)))+(-0.00055603*pow(x,5));
       sigma = ((((0.939711+(-0.395909*x))+(0.310208*pow(x,2)))+(-0.0973802*pow(x,3)))+(0.0138956*pow(x,4)))+(-0.000742154*pow(x,5));
       pisanormvar[ivar++] = (LayerSigma[1] - mean)/sigma;

       mean = ((((0.617959+(-0.0205301*x))+(0.0470678*pow(x,2)))+(-0.0241603*pow(x,3)))+(0.00501246*pow(x,4)))+(-0.000284843*pow(x,5));
       sigma = ((((0.0997926+(0.448131*x))+(-0.230718*pow(x,2)))+(0.0519568*pow(x,3)))+(-0.00528493*pow(x,4)))+(0.000205337*pow(x,5));
       pisanormvar[ivar++] = (LayerSigma[2] - mean)/sigma;

       mean = ((((0.55324+(0.067058*x))+(0.00619556*pow(x,2)))+(-0.0130305*pow(x,3)))+(0.00346271*pow(x,4)))+(-0.000223909*pow(x,5));
       sigma = ((((0.0724869+(0.312343*x))+(-0.153128*pow(x,2)))+(0.0326618*pow(x,3)))+(-0.00307122*pow(x,4)))+(0.00010513*pow(x,5));
       pisanormvar[ivar++] = (LayerSigma[3] - mean)/sigma;

       mean = ((((1.19272+(-0.145809*x))+(-0.0245348*pow(x,2)))+(0.0130697*pow(x,3)))+(-0.00100788*pow(x,4)))+(9.50474e-06*pow(x,5));
       sigma = ((((0.636325+(-0.0415961*x))+(-0.0831726*pow(x,2)))+(0.0301749*pow(x,3)))+(-0.00391018*pow(x,4)))+(0.000180541*pow(x,5));
       pisanormvar[ivar++] = (LayerSigma[4] - mean)/sigma;

       mean = ((((1.26947+(-0.15897*x))+(-0.0297403*pow(x,2)))+(0.0182927*pow(x,3)))+(-0.00221084*pow(x,4)))+(8.54899e-05*pow(x,5));
       sigma = ((((0.638073+(-0.0483009*x))+(-0.0994808*pow(x,2)))+(0.0377219*pow(x,3)))+(-0.00510204*pow(x,4)))+(0.000243147*pow(x,5));
       pisanormvar[ivar++] = (LayerSigma[5] - mean)/sigma;

       mean = ((((1.04867+(-0.17135*x))+(0.0811997*pow(x,2)))+(-0.0190004*pow(x,3)))+(0.00242135*pow(x,4)))+(-0.000120006*pow(x,5));
       sigma = ((((0.628708+(-0.249955*x))+(0.0793438*pow(x,2)))+(-0.0145469*pow(x,3)))+(0.00142641*pow(x,4)))+(-5.69095e-05*pow(x,5));
       pisanormvar[ivar++] = (LayerSigma[6] - mean)/sigma;

       mean = ((((1.38132+(-0.425016*x))+(0.171989*pow(x,2)))+(-0.0331198*pow(x,3)))+(0.00315927*pow(x,4)))+(-0.000116634*pow(x,5));
       sigma = ((((0.88293+(-0.5042*x))+(0.169749*pow(x,2)))+(-0.0308108*pow(x,3)))+(0.00287951*pow(x,4)))+(-0.000108697*pow(x,5));
       pisanormvar[ivar++] = (LayerSigma[7] - mean)/sigma;

       mean = ((((1.46465+(0.100394*x))+(-0.22424*pow(x,2)))+(0.0801731*pow(x,3)))+(-0.0113209*pow(x,4)))+(0.000574014*pow(x,5));
       sigma = ((((0.993911+(-0.354605*x))+(0.00783056*pow(x,2)))+(0.0199715*pow(x,3)))+(-0.00385046*pow(x,4)))+(0.000218194*pow(x,5));
       pisanormvar[ivar++] = (LayerSigma[8] - mean)/sigma;

       mean = ((((1.27007+(0.561239*x))+(-0.476984*pow(x,2)))+(0.145572*pow(x,3)))+(-0.0194546*pow(x,4)))+(0.000962266*pow(x,5));
       sigma = ((((0.861278+(-0.0891706*x))+(-0.137942*pow(x,2)))+(0.0554071*pow(x,3)))+(-0.00789675*pow(x,4)))+(0.000395316*pow(x,5));
       pisanormvar[ivar++] = (LayerSigma[9] - mean)/sigma;

       mean = ((((1.02514+(0.889447*x))+(-0.628584*pow(x,2)))+(0.184673*pow(x,3)))+(-0.0246292*pow(x,4)))+(0.00122772*pow(x,5));
       sigma = ((((0.681501+(0.148884*x))+(-0.242059*pow(x,2)))+(0.0766961*pow(x,3)))+(-0.00997977*pow(x,4)))+(0.000473377*pow(x,5));
       pisanormvar[ivar++] = (LayerSigma[10] - mean)/sigma;

       mean = ((((0.556668+(1.81577*x))+(-1.13725*pow(x,2)))+(0.312593*pow(x,3)))+(-0.0397691*pow(x,4)))+(0.00190942*pow(x,5));
       sigma = ((((0.24536+(0.898643*x))+(-0.633047*pow(x,2)))+(0.168436*pow(x,3)))+(-0.0200664*pow(x,4)))+(0.000896716*pow(x,5));
       pisanormvar[ivar++] = (LayerSigma[11] - mean)/sigma;

       mean = ((((-0.0673142+(3.3028*x))+(-2.00941*pow(x,2)))+(0.536891*pow(x,3)))+(-0.0665166*pow(x,4)))+(0.00311967*pow(x,5));
       sigma = ((((0.0710544+(1.54797*x))+(-1.0268*pow(x,2)))+(0.268102*pow(x,3)))+(-0.0315711*pow(x,4)))+(0.00139721*pow(x,5));
       pisanormvar[ivar++] = (LayerSigma[12] - mean)/sigma;

       mean = ((((-0.35587+(3.91784*x))+(-2.34062*pow(x,2)))+(0.617827*pow(x,3)))+(-0.0757898*pow(x,4)))+(0.00352304*pow(x,5));
       sigma = ((((0.59+(1.05398*x))+(-0.806572*pow(x,2)))+(0.21685*pow(x,3)))+(-0.025632*pow(x,4)))+(0.00112796*pow(x,5));
       pisanormvar[ivar++] = (LayerSigma[13] - mean)/sigma;

       mean = ((((-2.21671+(6.34382*x))+(-3.47907*pow(x,2)))+(0.877046*pow(x,3)))+(-0.104508*pow(x,4)))+(0.00476299*pow(x,5));
       sigma = ((((-0.0388837+(1.72366*x))+(-1.04301*pow(x,2)))+(0.253324*pow(x,3)))+(-0.0279738*pow(x,4)))+(0.00116763*pow(x,5));
       pisanormvar[ivar++] = (LayerSigma[14] - mean)/sigma;

       mean = ((((-2.44412+(6.73703*x))+(-3.65601*pow(x,2)))+(0.914471*pow(x,3)))+(-0.108349*pow(x,4)))+(0.00491747*pow(x,5));
       sigma = ((((0.0467636+(1.69772*x))+(-1.00689*pow(x,2)))+(0.235455*pow(x,3)))+(-0.0248288*pow(x,4)))+(0.000982412*pow(x,5));
       pisanormvar[ivar++] = (LayerSigma[15] - mean)/sigma;

       mean = ((((-3.7108+(8.71355*x))+(-4.63301*pow(x,2)))+(1.13588*pow(x,3)))+(-0.13208*pow(x,4)))+(0.00589485*pow(x,5));
       sigma = ((((0.782099+(1.02023*x))+(-0.659562*pow(x,2)))+(0.143041*pow(x,3)))+(-0.0129985*pow(x,4)))+(0.000407055*pow(x,5));
       pisanormvar[ivar++] = (LayerSigma[16] - mean)/sigma;

       mean = ((((-3.87155+(8.478*x))+(-4.30158*pow(x,2)))+(1.01809*pow(x,3)))+(-0.115185*pow(x,4)))+(0.00503251*pow(x,5));
       sigma = ((((0.305054+(1.6927*x))+(-0.930333*pow(x,2)))+(0.192447*pow(x,3)))+(-0.017254*pow(x,4)))+(0.000548592*pow(x,5));
       pisanormvar[ivar++] = (LayerSigma[17] - mean)/sigma;

       mean = ((((0.805005+(-0.114091*x))+(0.110727*pow(x,2)))+(-0.0314454*pow(x,3)))+(0.00372487*pow(x,4)))+(-0.000171451*pow(x,5));
       sigma = ((((-0.135127+(0.44909*x))+(-0.248526*pow(x,2)))+(0.0602819*pow(x,3)))+(-0.00676711*pow(x,4)))+(0.000289649*pow(x,5));
       pisanormvar[ivar++] = (LayerS3Frac[1] - mean)/sigma;

       mean = ((((0.964283+(-0.0408338*x))+(0.0237554*pow(x,2)))+(-0.00506671*pow(x,3)))+(0.000387042*pow(x,4)))+(-1.01548e-05*pow(x,5));
       sigma = ((((-0.0187137+(0.106987*x))+(-0.0541627*pow(x,2)))+(0.0122079*pow(x,3)))+(-0.00128415*pow(x,4)))+(5.24546e-05*pow(x,5));
       pisanormvar[ivar++] = (LayerS3Frac[2] - mean)/sigma;

       mean = ((((0.895671+(0.0658757*x))+(-0.0304411*pow(x,2)))+(0.00750985*pow(x,3)))+(-0.000977063*pow(x,4)))+(4.8605e-05*pow(x,5));
       sigma = ((((0.0544806+(-0.00789298*x))+(0.00258956*pow(x,2)))+(-0.00114331*pow(x,3)))+(0.000213614*pow(x,4)))+(-1.26157e-05*pow(x,5));
       pisanormvar[ivar++] = (LayerS3Frac[3] - mean)/sigma;

       mean = ((((0.881474+(-0.0874261*x))+(0.0787225*pow(x,2)))+(-0.022068*pow(x,3)))+(0.00260926*pow(x,4)))+(-0.000113371*pow(x,5));
       sigma = ((((-0.088412+(0.367479*x))+(-0.213404*pow(x,2)))+(0.0515467*pow(x,3)))+(-0.00571672*pow(x,4)))+(0.000241322*pow(x,5));
       pisanormvar[ivar++] = (LayerS3Frac[4] - mean)/sigma;

       mean = ((((0.82898+(-0.0341718*x))+(0.0578207*pow(x,2)))+(-0.0181431*pow(x,3)))+(0.00227775*pow(x,4)))+(-0.000103396*pow(x,5));
       sigma = ((((-0.0151576+(0.284206*x))+(-0.183687*pow(x,2)))+(0.0464887*pow(x,3)))+(-0.00529621*pow(x,4)))+(0.000227284*pow(x,5));
       pisanormvar[ivar++] = (LayerS3Frac[5] - mean)/sigma;

       mean = ((((0.866638+(0.0339053*x))+(-0.00782089*pow(x,2)))+(0.00101355*pow(x,3)))+(-7.00301e-05*pow(x,4)))+(1.81956e-06*pow(x,5));
       sigma = ((((0.0862146+(0.00506827*x))+(-0.0153758*pow(x,2)))+(0.00465927*pow(x,3)))+(-0.000563078*pow(x,4)))+(2.48163e-05*pow(x,5));
       pisanormvar[ivar++] = (LayerS3Frac[6] - mean)/sigma;

       mean = ((((0.852653+(0.0101658*x))+(0.0102941*pow(x,2)))+(-0.00422588*pow(x,3)))+(0.000622576*pow(x,4)))+(-3.26004e-05*pow(x,5));
       sigma = ((((0.0846352+(0.0309626*x))+(-0.0347061*pow(x,2)))+(0.0101897*pow(x,3)))+(-0.0012682*pow(x,4)))+(5.83102e-05*pow(x,5));
       pisanormvar[ivar++] = (LayerS3Frac[7] - mean)/sigma;

       mean = ((((0.845344+(-0.168908*x))+(0.139558*pow(x,2)))+(-0.0398658*pow(x,3)))+(0.00501233*pow(x,4)))+(-0.000235004*pow(x,5));
       sigma = ((((0.0780233+(0.163054*x))+(-0.13121*pow(x,2)))+(0.036521*pow(x,3)))+(-0.00444677*pow(x,4)))+(0.000201374*pow(x,5));
       pisanormvar[ivar++] = (LayerS3Frac[8] - mean)/sigma;

       mean = ((((0.867438+(-0.241374*x))+(0.178746*pow(x,2)))+(-0.0495876*pow(x,3)))+(0.00617755*pow(x,4)))+(-0.000289262*pow(x,5));
       sigma = ((((0.0767509+(0.175949*x))+(-0.13918*pow(x,2)))+(0.0386108*pow(x,3)))+(-0.00470487*pow(x,4)))+(0.000213618*pow(x,5));
       pisanormvar[ivar++] = (LayerS3Frac[9] - mean)/sigma;

       mean = ((((0.893667+(-0.233505*x))+(0.159314*pow(x,2)))+(-0.0434683*pow(x,3)))+(0.00545821*pow(x,4)))+(-0.000260126*pow(x,5));
       sigma = ((((0.147665+(0.0183921*x))+(-0.0418325*pow(x,2)))+(0.0129434*pow(x,3)))+(-0.00161212*pow(x,4)))+(7.30716e-05*pow(x,5));
       pisanormvar[ivar++] = (LayerS3Frac[10] - mean)/sigma;

       mean = ((((0.854272+(-0.2429*x))+(0.170759*pow(x,2)))+(-0.047009*pow(x,3)))+(0.00593322*pow(x,4)))+(-0.000283616*pow(x,5));
       sigma = ((((0.0991305+(0.110779*x))+(-0.0893286*pow(x,2)))+(0.0238283*pow(x,3)))+(-0.00278127*pow(x,4)))+(0.000121071*pow(x,5));
       pisanormvar[ivar++] = (LayerS3Frac[11] - mean)/sigma;

       mean = ((((0.968956+(-0.554806*x))+(0.35558*pow(x,2)))+(-0.0942065*pow(x,3)))+(0.0115146*pow(x,4)))+(-0.000534475*pow(x,5));
       sigma = ((((0.051045+(0.236579*x))+(-0.161262*pow(x,2)))+(0.0411877*pow(x,3)))+(-0.00471004*pow(x,4)))+(0.000202455*pow(x,5));
       pisanormvar[ivar++] = (LayerS3Frac[12] - mean)/sigma;

       mean = ((((1.04861+(-0.68503*x))+(0.413581*pow(x,2)))+(-0.106544*pow(x,3)))+(0.0128047*pow(x,4)))+(-0.000587559*pow(x,5));
       sigma = ((((-0.0826563+(0.418929*x))+(-0.24489*pow(x,2)))+(0.0593716*pow(x,3)))+(-0.00662409*pow(x,4)))+(0.000281122*pow(x,5));
       pisanormvar[ivar++] = (LayerS3Frac[13] - mean)/sigma;

       mean = ((((0.682343+(-0.161807*x))+(0.139274*pow(x,2)))+(-0.0399169*pow(x,3)))+(0.00516174*pow(x,4)))+(-0.000251559*pow(x,5));
       sigma = ((((-0.165984+(0.460383*x))+(-0.222619*pow(x,2)))+(0.0450875*pow(x,3)))+(-0.00404602*pow(x,4)))+(0.00012773*pow(x,5));
       pisanormvar[ivar++] = (LayerS3Frac[14] - mean)/sigma;

       mean = ((((1.02247+(-0.679886*x))+(0.409338*pow(x,2)))+(-0.106314*pow(x,3)))+(0.0129861*pow(x,4)))+(-0.000607491*pow(x,5));
       sigma = ((((-0.137331+(0.473358*x))+(-0.252896*pow(x,2)))+(0.0570351*pow(x,3)))+(-0.00595925*pow(x,4)))+(0.000237701*pow(x,5));
       pisanormvar[ivar++] = (LayerS3Frac[15] - mean)/sigma;

       mean = ((((0.867764+(-0.472237*x))+(0.294031*pow(x,2)))+(-0.0767665*pow(x,3)))+(0.00945774*pow(x,4)))+(-0.000447638*pow(x,5));
       sigma = ((((-0.322784+(0.669384*x))+(-0.315941*pow(x,2)))+(0.0650404*pow(x,3)))+(-0.00621963*pow(x,4)))+(0.000225316*pow(x,5));
       pisanormvar[ivar++] = (LayerS3Frac[16] - mean)/sigma;

       mean = ((((0.755481+(-0.273326*x))+(0.176814*pow(x,2)))+(-0.0468955*pow(x,3)))+(0.00598162*pow(x,4)))+(-0.000295412*pow(x,5));
       sigma = ((((-0.272627+(0.557623*x))+(-0.23831*pow(x,2)))+(0.0425152*pow(x,3)))+(-0.00330163*pow(x,4)))+(8.523e-05*pow(x,5));
       pisanormvar[ivar++] = (LayerS3Frac[17] - mean)/sigma;

       pisanormvar[ivar++] = classification_energy;
     }
   else if ( iBDTVERSION==4 )
     {
       if( apply_precuts )
	 {
	   //*******************************
	   //*****Reject without BDT********
	   //*******************************

	   //Check the energy deposit in the first 2 superlayers
	   if (F2SLEneDep < TMath::Max(0.1, -0.02 + 0.17*log(energyd) + 8.7e-4*pow((float)log(energyd), (float)4.00))) return -0.9991;

	   //Check S1/S3 per layer
	   int first_lay = -1; //first layer in which we want to estimate S1/S3
	   int last_lay  = -1; //last  layer in which we want to estimate S1/S3

	   if (energyd < 4 )
	     {
	       first_lay = 3;
	       last_lay = 6;
	     }
	   else if (energyd < 12)
	     {
	       first_lay = 2;
	       last_lay = 7;
	     }
	   else
	     {
	       first_lay = 2;
	       last_lay = 11;
	     }

	   //request 2 consecutive layers with MIP like energy deposit
	   for (unsigned int ilayer = first_lay; ilayer < last_lay; ++ilayer)
	     {
	       if (LayerS1S3[ilayer] > 0.995 && LayerS1S3[ilayer+1] > 0.995) return -0.9993;
	     }
	 }

       //********************************
       //*****Normalize the Variables****
       //********************************
       // fix: normalization may be NaN below 2 GeV or above 1000 GeV;
       energyd = energyd < 2. ? 2. : (energyd > 1000. ? 1000. : energyd);
       float mean, sigma;
       float x = log(energyd);

       if (BDT_DEBUG) std::cout << Form(" ??? x=%f\n", x) << flush;

       ivar = 0;

       mean = 5.81753*(((((1+(-0.0652423*x))+(0.123152*pow(x,2)))+(-0.0309516*(pow(x,3))))+(0.00355938*(pow(x,4))))+(-0.000154605*(pow(x,5))));
       sigma = 0.654693*(((((1+(0.349885*x))+(-0.258381*pow(x,2)))+(0.0762609*(pow(x,3))))+(-0.0101505*(pow(x,4))))+(0.000480682*(pow(x,5))));
       pisanormvar[ivar++] = (ShowerMean-mean)/sigma;

       mean = 3.19961*(((((1+(0.122833*x))+(-0.0515297*pow(x,2)))+(0.0142255*(pow(x,3))))+(-0.00216798*(pow(x,4))))+(0.000126187*(pow(x,5))));
       sigma = 0.550582*(((((1+(-0.436487*x))+(0.0995998*pow(x,2)))+(-0.00967967*(pow(x,3))))+(-1.31632e-05*(pow(x,4))))+(4.37799e-05*(pow(x,5))));
       pisanormvar[ivar++] = (ShowerSigma-mean)/sigma;

       mean = -0.00563546*(((((1+(-4.90888*x))+(3.06143*pow(x,2)))+(-1.19568*(pow(x,3))))+(0.200186*(pow(x,4))))+(-0.0124911*(pow(x,5))));
       sigma = 0.0202379*(((((1+(-0.700595*x))+(0.315499*pow(x,2)))+(-0.0637316*(pow(x,3))))+(0.00687652*(pow(x,4))))+(-0.000297002*(pow(x,5))));
       pisanormvar[ivar++] = (L2LFrac-mean)/sigma;

       mean = 0.976096*(((((1+(-0.00594019*x))+(0.00626969*pow(x,2)))+(-0.00277484*(pow(x,3))))+(0.000469374*(pow(x,4))))+(-2.69748e-05*(pow(x,5))));
       sigma = 0.0202693*(((((1+(-0.450749*x))+(0.156631*pow(x,2)))+(-0.0379591*(pow(x,3))))+(0.00471605*(pow(x,4))))+(-0.000217455*(pow(x,5))));
       pisanormvar[ivar++] = (R3cmFrac-mean)/sigma;

       mean = 0.947438*(((((1+(-0.0958179*x))+(0.0542999*pow(x,2)))+(-0.0149508*(pow(x,3))))+(0.00197748*(pow(x,4))))+(-9.96043e-05*(pow(x,5))));
       sigma = 0.0514221*(((((1+(-0.191206*x))+(-0.081866*pow(x,2)))+(0.0396561*(pow(x,3))))+(-0.00606394*(pow(x,4))))+(0.000326768*(pow(x,5))));
       pisanormvar[ivar++] = (S3totx-mean)/sigma;

       mean = 0.894616*(((((1+(-0.0830502*x))+(0.0602628*pow(x,2)))+(-0.0178786*(pow(x,3))))+(0.00239022*(pow(x,4))))+(-0.000118785*(pow(x,5))));
       sigma = 0.074716*(((((1+(-0.194879*x))+(-0.118629*pow(x,2)))+(0.0562894*(pow(x,3))))+(-0.00869822*(pow(x,4))))+(0.000470664*(pow(x,5))));
       pisanormvar[ivar++] = (S3toty-mean)/sigma;

       mean = 17.7832*(((((1+(4.11748*x))+(-1.32613*pow(x,2)))+(0.378096*(pow(x,3))))+(-0.0420122*(pow(x,4))))+(0.00192212*(pow(x,5))));
       sigma = -7.45314*(((((1+(-4.362*x))+(3.04245*pow(x,2)))+(-0.998655*(pow(x,3))))+(0.144479*(pow(x,4))))+(-0.00759756*(pow(x,5))));
       pisanormvar[ivar++] = (NEcalHits-mean)/sigma;

       mean = 2.08418*(((((1+(1.53685*x))+(-0.960584*pow(x,2)))+(0.28049*(pow(x,3))))+(-0.037519*(pow(x,4))))+(0.00186672*(pow(x,5))));
       sigma = 1.08555*(((((1+(0.301398*x))+(-0.486968*pow(x,2)))+(0.167722*(pow(x,3))))+(-0.0236229*(pow(x,4))))+(0.00120627*(pow(x,5))));
       pisanormvar[ivar++] = (ShowerFootprintX-mean)/sigma;

       mean = 3.06557*(((((1+(1.38162*x))+(-0.972884*pow(x,2)))+(0.288618*(pow(x,3))))+(-0.0381605*(pow(x,4))))+(0.00186877*(pow(x,5))));
       sigma = 1.97309*(((((1+(0.0319007*x))+(-0.296926*pow(x,2)))+(0.108391*(pow(x,3))))+(-0.0151688*(pow(x,4))))+(0.000761558*(pow(x,5))));
       pisanormvar[ivar++] = (ShowerFootprintY-mean)/sigma;

       mean = 0.0154335*(((((1+(-0.0374406*x))+(-0.1801*pow(x,2)))+(0.0575964*(pow(x,3))))+(-0.00701209*(pow(x,4))))+(0.000309315*(pow(x,5))));
       sigma = -0.0116612*(((((1+(-2.62644*x))+(1.4815*pow(x,2)))+(-0.352021*(pow(x,3))))+(0.0384267*(pow(x,4))))+(-0.00159106*(pow(x,5))));
       pisanormvar[ivar++] = (LayerEneFrac[0]-mean)/sigma;

       mean = 0.0633498*(((((1+(-0.114734*x))+(-0.154378*pow(x,2)))+(0.0562252*(pow(x,3))))+(-0.00731868*(pow(x,4))))+(0.000337497*(pow(x,5))));
       sigma = 0.0316538*(((((1+(-0.137904*x))+(-0.15952*pow(x,2)))+(0.0611123*(pow(x,3))))+(-0.00813668*(pow(x,4))))+(0.000378855*(pow(x,5))));
       pisanormvar[ivar++] = (LayerEneFrac[1]-mean)/sigma;

       mean = 0.0981715*(((((1+(0.086794*x))+(-0.25229*pow(x,2)))+(0.076108*(pow(x,3))))+(-0.00930538*(pow(x,4))))+(0.000418156*(pow(x,5))));
       sigma = 0.0584895*(((((1+(-0.518405*x))+(0.147205*pow(x,2)))+(-0.0279128*(pow(x,3))))+(0.00302415*(pow(x,4))))+(-0.000133801*(pow(x,5))));
       pisanormvar[ivar++] = (LayerEneFrac[2]-mean)/sigma;

       mean = 0.0913995*(((((1+(0.606592*x))+(-0.534856*pow(x,2)))+(0.142581*(pow(x,3))))+(-0.0169281*(pow(x,4))))+(0.000761466*(pow(x,5))));
       sigma = 0.0521775*(((((1+(-0.370914*x))+(0.102182*pow(x,2)))+(-0.0221373*(pow(x,3))))+(0.00253141*(pow(x,4))))+(-0.000108831*(pow(x,5))));
       pisanormvar[ivar++] = (LayerEneFrac[3]-mean)/sigma;

       mean = 0.0900602*(((((1+(0.623416*x))+(-0.425984*pow(x,2)))+(0.097747*(pow(x,3))))+(-0.010668*(pow(x,4))))+(0.000463787*(pow(x,5))));
       sigma = 0.0505887*(((((1+(-0.486493*x))+(0.195421*pow(x,2)))+(-0.0430779*(pow(x,3))))+(0.00438646*(pow(x,4))))+(-0.000166038*(pow(x,5))));
       pisanormvar[ivar++] = (LayerEneFrac[4]-mean)/sigma;

       mean = 0.0820018*(((((1+(0.588403*x))+(-0.295374*pow(x,2)))+(0.0567273*(pow(x,3))))+(-0.00600898*(pow(x,4))))+(0.000280536*(pow(x,5))));
       sigma = 0.0542134*(((((1+(-0.524149*x))+(0.136043*pow(x,2)))+(-0.00183907*(pow(x,3))))+(-0.00348656*(pow(x,4))))+(0.000309278*(pow(x,5))));
       pisanormvar[ivar++] = (LayerEneFrac[5]-mean)/sigma;

       mean = 0.0647088*(((((1+(0.755927*x))+(-0.322877*pow(x,2)))+(0.0691826*(pow(x,3))))+(-0.00892412*(pow(x,4))))+(0.000476596*(pow(x,5))));
       sigma = 0.0460868*(((((1+(-0.426239*x))+(0.0999163*pow(x,2)))+(-0.0125167*(pow(x,3))))+(0.00141731*(pow(x,4))))+(-0.000105953*(pow(x,5))));
       pisanormvar[ivar++] = (LayerEneFrac[6]-mean)/sigma;

       mean = 0.0591098*(((((1+(0.490957*x))+(-0.117617*pow(x,2)))+(0.0186932*(pow(x,3))))+(-0.00285173*(pow(x,4))))+(0.000169852*(pow(x,5))));
       sigma = 0.0441187*(((((1+(-0.424288*x))+(0.120773*pow(x,2)))+(-0.0303416*(pow(x,3))))+(0.00522312*(pow(x,4))))+(-0.000345832*(pow(x,5))));
       pisanormvar[ivar++] = (LayerEneFrac[7]-mean)/sigma;

       mean = 0.0544055*(((((1+(0.185537*x))+(0.115868*pow(x,2)))+(-0.0463891*(pow(x,3))))+(0.00617888*(pow(x,4))))+(-0.000325102*(pow(x,5))));
       sigma = 0.0358471*(((((1+(-0.404967*x))+(0.161259*pow(x,2)))+(-0.0523526*(pow(x,3))))+(0.00851722*(pow(x,4))))+(-0.000492585*(pow(x,5))));
       pisanormvar[ivar++] = (LayerEneFrac[8]-mean)/sigma;

       mean = 0.0586046*(((((1+(-0.35631*x))+(0.418475*pow(x,2)))+(-0.123614*(pow(x,3))))+(0.0161746*(pow(x,4))))+(-0.000828849*(pow(x,5))));
       sigma = 0.0195272*(((((1+(0.15923*x))+(-0.127709*pow(x,2)))+(0.0134291*(pow(x,3))))+(0.000637799*(pow(x,4))))+(-9.40079e-05*(pow(x,5))));
       pisanormvar[ivar++] = (LayerEneFrac[9]-mean)/sigma;

       mean = 0.0384656*(((((1+(-0.106815*x))+(0.243958*pow(x,2)))+(-0.0433174*(pow(x,3))))+(0.00189987*(pow(x,4))))+(4.54321e-05*(pow(x,5))));
       sigma = 0.0193138*(((((1+(0.109858*x))+(-0.222064*pow(x,2)))+(0.0902414*(pow(x,3))))+(-0.0161111*(pow(x,4))))+(0.00103071*(pow(x,5))));
       pisanormvar[ivar++] = (LayerEneFrac[10]-mean)/sigma;

       mean = 0.0288085*(((((1+(-0.203208*x))+(0.392827*pow(x,2)))+(-0.0992274*(pow(x,3))))+(0.0124898*(pow(x,4))))+(-0.000668279*(pow(x,5))));
       sigma = 0.0276025*(((((1+(-0.717605*x))+(0.363821*pow(x,2)))+(-0.0791709*(pow(x,3))))+(0.0067021*(pow(x,4))))+(-0.000146176*(pow(x,5))));
       pisanormvar[ivar++] = (LayerEneFrac[11]-mean)/sigma;

       mean = 0.0113727*(((((1+(1.25241*x))+(-0.32621*pow(x,2)))+(0.160578*(pow(x,3))))+(-0.0269675*(pow(x,4))))+(0.00150497*(pow(x,5))));
       sigma = -0.00706408*(((((1+(-4.56671*x))+(2.99638*pow(x,2)))+(-0.934686*(pow(x,3))))+(0.136241*(pow(x,4))))+(-0.00736457*(pow(x,5))));
       pisanormvar[ivar++] = (LayerEneFrac[12]-mean)/sigma;

       mean = -0.0014262*(((((1+(-18.5479*x))+(8.50689*pow(x,2)))+(-2.91282*(pow(x,3))))+(0.428928*(pow(x,4))))+(-0.0233242*(pow(x,5))));
       sigma = 0.000632082*(((((1+(29.7718*x))+(-20.4101*pow(x,2)))+(6.333*(pow(x,3))))+(-0.886025*(pow(x,4))))+(0.0453158*(pow(x,5))));
       pisanormvar[ivar++] = (LayerEneFrac[13]-mean)/sigma;

       mean = 0.00206941*(((((1+(6.37561*x))+(-2.65265*pow(x,2)))+(1.07375*(pow(x,3))))+(-0.171301*(pow(x,4))))+(0.0102091*(pow(x,5))));
       sigma = -0.0135454*(((((1+(-2.7593*x))+(1.80845*pow(x,2)))+(-0.554322*(pow(x,3))))+(0.0783147*(pow(x,4))))+(-0.0041224*(pow(x,5))));
       pisanormvar[ivar++] = (LayerEneFrac[14]-mean)/sigma;

       mean = -0.00391276*(((((1+(-4.63399*x))+(2.29847*pow(x,2)))+(-0.803834*(pow(x,3))))+(0.122204*(pow(x,4))))+(-0.00712181*(pow(x,5))));
       sigma = -0.0100079*(((((1+(-2.74935*x))+(1.60623*pow(x,2)))+(-0.42156*(pow(x,3))))+(0.0491247*(pow(x,4))))+(-0.002086*(pow(x,5))));
       pisanormvar[ivar++] = (LayerEneFrac[15]-mean)/sigma;

       mean = 0.0118102*(((((1+(-0.976311*x))+(0.686435*pow(x,2)))+(-0.133816*(pow(x,3))))+(0.0104236*(pow(x,4))))+(-6.07892e-05*(pow(x,5))));
       sigma = -0.0132099*(((((1+(-2.47328*x))+(1.61924*pow(x,2)))+(-0.501154*(pow(x,3))))+(0.0719867*(pow(x,4))))+(-0.0038697*(pow(x,5))));
       pisanormvar[ivar++] = (LayerEneFrac[16]-mean)/sigma;

       mean = 0.018997*(((((1+(-1.46883*x))+(0.958373*pow(x,2)))+(-0.244195*(pow(x,3))))+(0.0286415*(pow(x,4))))+(-0.00117624*(pow(x,5))));
       sigma = -0.00102647*(((((1+(-4.50726*x))+(0.00253443*pow(x,2)))+(0.603813*(pow(x,3))))+(-0.144358*(pow(x,4))))+(0.00928059*(pow(x,5))));
       pisanormvar[ivar++] = (LayerEneFrac[17]-mean)/sigma;

       mean = -0.26146*(((((1+(-7.40014*x))+(4.61732*pow(x,2)))+(-1.2305*(pow(x,3))))+(0.149395*(pow(x,4))))+(-0.00720869*(pow(x,5))));
       sigma = 0.813385*(((((1+(-0.822472*x))+(0.481724*pow(x,2)))+(-0.149226*(pow(x,3))))+(0.0220648*(pow(x,4))))+(-0.00119692*(pow(x,5))));
       pisanormvar[ivar++] = (LayerSigma[1]-mean)/sigma;

       mean = 0.532719*(((((1+(-0.00948279*x))+(0.00541066*pow(x,2)))+(-0.00137006*(pow(x,3))))+(0.000895784*(pow(x,4))))+(2.04005e-05*(pow(x,5))));
       sigma = 0.218894*(((((1+(-0.247113*x))+(0.122509*pow(x,2)))+(-0.0385345*(pow(x,3))))+(0.00715622*(pow(x,4))))+(-0.00046392*(pow(x,5))));
       pisanormvar[ivar++] = (LayerSigma[2]-mean)/sigma;

       mean = 0.484337*(((((1+(0.325377*x))+(-0.190782*pow(x,2)))+(0.0535426*(pow(x,3))))+(-0.00601287*(pow(x,4))))+(0.000299395*(pow(x,5))));
       sigma = 0.194305*(((((1+(-0.265873*x))+(0.172771*pow(x,2)))+(-0.0625035*(pow(x,3))))+(0.0110567*(pow(x,4))))+(-0.000691251*(pow(x,5))));
       pisanormvar[ivar++] = (LayerSigma[3]-mean)/sigma;

       mean = -0.064318*(((((1+(-24.591*x))+(15.6678*pow(x,2)))+(-4.39365*(pow(x,3))))+(0.563465*(pow(x,4))))+(-0.0274617*(pow(x,5))));
       sigma = -0.192809*(((((1+(-4.98416*x))+(3.66017*pow(x,2)))+(-1.14303*(pow(x,3))))+(0.162384*(pow(x,4))))+(-0.0086003*(pow(x,5))));
       pisanormvar[ivar++] = (LayerSigma[4]-mean)/sigma;

       mean = 0.144866*(((((1+(9.10087*x))+(-5.8124*pow(x,2)))+(1.62854*(pow(x,3))))+(-0.207829*(pow(x,4))))+(0.0100023*(pow(x,5))));
       sigma = -0.00234759*(((((1+(-241.86*x))+(178.387*pow(x,2)))+(-53.4977*(pow(x,3))))+(7.2096*(pow(x,4))))+(-0.360926*(pow(x,5))));
       pisanormvar[ivar++] = (LayerSigma[5]-mean)/sigma;

       mean = 0.806021*(((((1+(0.0127191*x))+(-0.00560951*pow(x,2)))+(0.00510244*(pow(x,3))))+(-0.00102046*(pow(x,4))))+(7.29341e-05*(pow(x,5))));
       sigma = 0.310532*(((((1+(-0.300253*x))+(0.0809249*pow(x,2)))+(-0.0161393*(pow(x,3))))+(0.00180108*(pow(x,4))))+(-7.66134e-05*(pow(x,5))));
       pisanormvar[ivar++] = (LayerSigma[6]-mean)/sigma;

       mean = 0.829417*(((((1+(0.150745*x))+(-0.0951881*pow(x,2)))+(0.0323211*(pow(x,3))))+(-0.00497555*(pow(x,4))))+(0.00028552*(pow(x,5))));
       sigma = 0.390787*(((((1+(-0.313306*x))+(0.0514155*pow(x,2)))+(-0.00264536*(pow(x,3))))+(-0.000266814*(pow(x,4))))+(2.81029e-05*(pow(x,5))));
       pisanormvar[ivar++] = (LayerSigma[7]-mean)/sigma;

       mean = 0.557175*(((((1+(2.12671*x))+(-1.4178*pow(x,2)))+(0.409941*(pow(x,3))))+(-0.0540956*(pow(x,4))))+(0.00267714*(pow(x,5))));
       sigma = 0.00897424*(((((1+(80.9882*x))+(-57.6913*pow(x,2)))+(16.3878*(pow(x,3))))+(-2.09909*(pow(x,4))))+(0.100461*(pow(x,5))));
       pisanormvar[ivar++] = (LayerSigma[8]-mean)/sigma;

       mean = 0.825791*(((((1+(1.22133*x))+(-0.880486*pow(x,2)))+(0.267216*(pow(x,3))))+(-0.0366376*(pow(x,4))))+(0.00186991*(pow(x,5))));
       sigma = 0.465602*(((((1+(0.508751*x))+(-0.654088*pow(x,2)))+(0.221749*(pow(x,3))))+(-0.0314511*(pow(x,4))))+(0.00161545*(pow(x,5))));
       pisanormvar[ivar++] = (LayerSigma[9]-mean)/sigma;

       mean = 0.910081*(((((1+(0.739722*x))+(-0.509627*pow(x,2)))+(0.159914*(pow(x,3))))+(-0.0229559*(pow(x,4))))+(0.001221*(pow(x,5))));
       sigma = 0.205266*(((((1+(2.68981*x))+(-2.12361*pow(x,2)))+(0.623894*(pow(x,3))))+(-0.0819358*(pow(x,4))))+(0.0040157*(pow(x,5))));
       pisanormvar[ivar++] = (LayerSigma[10]-mean)/sigma;

       mean = 0.0475479*(((((1+(46.9903*x))+(-28.6912*pow(x,2)))+(8.10759*(pow(x,3))))+(-1.06855*(pow(x,4))))+(0.0529542*(pow(x,5))));
       sigma = -0.0118896*(((((1+(-94.7724*x))+(68.6104*pow(x,2)))+(-19.8302*(pow(x,3))))+(2.58454*(pow(x,4))))+(-0.125683*(pow(x,5))));
       pisanormvar[ivar++] = (LayerSigma[11]-mean)/sigma;

       mean = -1.15196*(((((1+(-4.18659*x))+(2.54371*pow(x,2)))+(-0.698499*(pow(x,3))))+(0.0890199*(pow(x,4))))+(-0.00427228*(pow(x,5))));
       sigma = -0.422523*(((((1+(-5.21731*x))+(3.55303*pow(x,2)))+(-0.9936*(pow(x,3))))+(0.125859*(pow(x,4))))+(-0.00596396*(pow(x,5))));
       pisanormvar[ivar++] = (LayerSigma[12]-mean)/sigma;

       mean = -2.29679*(((((1+(-2.7974*x))+(1.5947*pow(x,2)))+(-0.416448*(pow(x,3))))+(0.0507933*(pow(x,4))))+(-0.00234097*(pow(x,5))));
       sigma = -1.22776*(((((1+(-2.77693*x))+(1.71971*pow(x,2)))+(-0.457114*(pow(x,3))))+(0.0557562*(pow(x,4))))+(-0.00255766*(pow(x,5))));
       pisanormvar[ivar++] = (LayerSigma[13]-mean)/sigma;

       mean = -3.65486*(((((1+(-2.17485*x))+(1.16497*pow(x,2)))+(-0.291875*(pow(x,3))))+(0.0345623*(pow(x,4))))+(-0.00155801*(pow(x,5))));
       sigma = -1.55552*(((((1+(-2.25803*x))+(1.25426*pow(x,2)))+(-0.303822*(pow(x,3))))+(0.0341468*(pow(x,4))))+(-0.00145752*(pow(x,5))));
       pisanormvar[ivar++] = (LayerSigma[14]-mean)/sigma;

       mean = -5.07802*(((((1+(-1.84923*x))+(0.927057*pow(x,2)))+(-0.219636*(pow(x,3))))+(0.0247829*(pow(x,4))))+(-0.00107054*(pow(x,5))));
       sigma = -2.64475*(((((1+(-1.62379*x))+(0.757685*pow(x,2)))+(-0.156366*(pow(x,3))))+(0.0149065*(pow(x,4))))+(-0.000533412*(pow(x,5))));
       pisanormvar[ivar++] = (LayerSigma[15]-mean)/sigma;

       mean = -8.09602*(((((1+(-1.62571*x))+(0.788618*pow(x,2)))+(-0.181389*(pow(x,3))))+(0.0199756*(pow(x,4))))+(-0.000846765*(pow(x,5))));
       sigma = -3.55731*(((((1+(-1.67015*x))+(0.82708*pow(x,2)))+(-0.184353*(pow(x,3))))+(0.0193368*(pow(x,4))))+(-0.000777463*(pow(x,5))));
       pisanormvar[ivar++] = (LayerSigma[16]-mean)/sigma;

       mean = -3.12835*(((((1+(-1.7683*x))+(0.662891*pow(x,2)))+(-0.110456*(pow(x,3))))+(0.00795358*(pow(x,4))))+(-0.0001761*(pow(x,5))));
       sigma = -2.70441*(((((1+(-1.70159*x))+(0.783491*pow(x,2)))+(-0.157919*(pow(x,3))))+(0.0146316*(pow(x,4))))+(-0.000505912*(pow(x,5))));
       pisanormvar[ivar++] = (LayerSigma[17]-mean)/sigma;

       mean = 2.71022*(((((1+(-0.130137*x))+(0.129101*pow(x,2)))+(-0.0405934*(pow(x,3))))+(0.00526541*(pow(x,4))))+(-0.000254535*(pow(x,5))));
       sigma = 0.433849*(((((1+(1.74973*x))+(-1.29381*pow(x,2)))+(0.370721*(pow(x,3))))+(-0.0480519*(pow(x,4))))+(0.00236235*(pow(x,5))));
       pisanormvar[ivar++] = (LayerS3Frac[1]-mean)/sigma;

       mean = 0.830443*(((((1+(0.306295*x))+(-0.149546*pow(x,2)))+(0.0335532*(pow(x,3))))+(-0.00362516*(pow(x,4))))+(0.000148019*(pow(x,5))));
       sigma = -0.0215234*(((((1+(-17.32*x))+(10.8897*pow(x,2)))+(-2.92155*(pow(x,3))))+(0.366721*(pow(x,4))))+(-0.0177822*(pow(x,5))));
       pisanormvar[ivar++] = (LayerS3Frac[2]-mean)/sigma;

       mean = 0.878041*(((((1+(0.168915*x))+(-0.0904153*pow(x,2)))+(0.0220529*(pow(x,3))))+(-0.00257015*(pow(x,4))))+(0.000113793*(pow(x,5))));
       sigma = -0.0332275*(((((1+(-4.50798*x))+(2.89585*pow(x,2)))+(-0.803476*(pow(x,3))))+(0.103883*(pow(x,4))))+(-0.00512917*(pow(x,5))));
       pisanormvar[ivar++] = (LayerS3Frac[3]-mean)/sigma;

       mean = 1.08295*(((((1+(-0.117056*x))+(0.0453468*pow(x,2)))+(-0.00841535*(pow(x,3))))+(0.000742509*(pow(x,4))))+(-2.56053e-05*(pow(x,5))));
       sigma = 0.492885*(((((1+(-0.864558*x))+(0.31567*pow(x,2)))+(-0.0581046*(pow(x,3))))+(0.00531972*(pow(x,4))))+(-0.000192369*(pow(x,5))));
       pisanormvar[ivar++] = (LayerS3Frac[4]-mean)/sigma;

       mean = 0.966137*(((((1+(-0.0447478*x))+(0.024797*pow(x,2)))+(-0.00580008*(pow(x,3))))+(0.000631017*(pow(x,4))))+(-2.67751e-05*(pow(x,5))));
       sigma = 0.324553*(((((1+(-0.739022*x))+(0.220331*pow(x,2)))+(-0.0297578*(pow(x,3))))+(0.00153657*(pow(x,4))))+(-4.23326e-06*(pow(x,5))));
       pisanormvar[ivar++] = (LayerS3Frac[5]-mean)/sigma;

       mean = 0.918183*(((((1+(0.0245774*x))+(-0.0165454*pow(x,2)))+(0.00513393*(pow(x,3))))+(-0.000687314*(pow(x,4))))+(3.29047e-05*(pow(x,5))));
       sigma = 0.144763*(((((1+(-0.652088*x))+(0.223288*pow(x,2)))+(-0.0435651*(pow(x,3))))+(0.00449212*(pow(x,4))))+(-0.000187523*(pow(x,5))));
       pisanormvar[ivar++] = (LayerS3Frac[6]-mean)/sigma;

       mean = 0.907456*(((((1+(-0.00101406*x))+(0.00160197*pow(x,2)))+(-5.47284e-05*(pow(x,3))))+(4.57173e-06*(pow(x,4))))+(-2.01113e-06*(pow(x,5))));
       sigma = 0.137874*(((((1+(-0.50364*x))+(0.11603*pow(x,2)))+(-0.0126287*(pow(x,3))))+(0.000406567*(pow(x,4))))+(1.59863e-05*(pow(x,5))));
       pisanormvar[ivar++] = (LayerS3Frac[7]-mean)/sigma;

       mean = 0.914388*(((((1+(-0.157968*x))+(0.113621*pow(x,2)))+(-0.0320707*(pow(x,3))))+(0.00411686*(pow(x,4))))+(-0.000198991*(pow(x,5))));
       sigma = 0.29723*(((((1+(-0.714645*x))+(0.237853*pow(x,2)))+(-0.0420266*(pow(x,3))))+(0.00377121*(pow(x,4))))+(-0.000133952*(pow(x,5))));
       pisanormvar[ivar++] = (LayerS3Frac[8]-mean)/sigma;

       mean = 0.912766*(((((1+(-0.211328*x))+(0.147518*pow(x,2)))+(-0.0414031*(pow(x,3))))+(0.00532721*(pow(x,4))))+(-0.000258884*(pow(x,5))));
       sigma = 0.267796*(((((1+(-0.496191*x))+(0.0753246*pow(x,2)))+(0.0053784*(pow(x,3))))+(-0.00245852*(pow(x,4))))+(0.000172211*(pow(x,5))));
       pisanormvar[ivar++] = (LayerS3Frac[9]-mean)/sigma;

       mean = 0.882403*(((((1+(-0.155903*x))+(0.108684*pow(x,2)))+(-0.0309816*(pow(x,3))))+(0.00412699*(pow(x,4))))+(-0.00020889*(pow(x,5))));
       sigma = 0.251275*(((((1+(-0.532008*x))+(0.1279*pow(x,2)))+(-0.0150123*(pow(x,3))))+(0.000638418*(pow(x,4))))+(7.92237e-06*(pow(x,5))));
       pisanormvar[ivar++] = (LayerS3Frac[10]-mean)/sigma;

       mean = 0.91556*(((((1+(-0.273105*x))+(0.178677*pow(x,2)))+(-0.0495976*(pow(x,3))))+(0.00643063*(pow(x,4))))+(-0.000316353*(pow(x,5))));
       sigma = 0.367702*(((((1+(-0.592629*x))+(0.151355*pow(x,2)))+(-0.0180565*(pow(x,3))))+(0.000785277*(pow(x,4))))+(4.72664e-06*(pow(x,5))));
       pisanormvar[ivar++] = (LayerS3Frac[11]-mean)/sigma;

       mean = 0.89048*(((((1+(-0.417808*x))+(0.285339*pow(x,2)))+(-0.0797402*(pow(x,3))))+(0.0102105*(pow(x,4))))+(-0.00049191*(pow(x,5))));
       sigma = 0.464536*(((((1+(-0.56921*x))+(0.141747*pow(x,2)))+(-0.0176432*(pow(x,3))))+(0.000979462*(pow(x,4))))+(-1.3265e-05*(pow(x,5))));
       pisanormvar[ivar++] = (LayerS3Frac[12]-mean)/sigma;

       mean = 0.914006*(((((1+(-0.505655*x))+(0.329083*pow(x,2)))+(-0.0891581*(pow(x,3))))+(0.0111151*(pow(x,4))))+(-0.000522036*(pow(x,5))));
       sigma = 0.561738*(((((1+(-0.521007*x))+(0.0973216*pow(x,2)))+(-0.0042303*(pow(x,3))))+(-0.000662862*(pow(x,4))))+(5.63393e-05*(pow(x,5))));
       pisanormvar[ivar++] = (LayerS3Frac[13]-mean)/sigma;

       mean = 1.13152*(((((1+(-0.645211*x))+(0.370763*pow(x,2)))+(-0.0952597*(pow(x,3))))+(0.0115274*(pow(x,4))))+(-0.000531245*(pow(x,5))));
       sigma = 0.449899*(((((1+(-0.263936*x))+(-0.0807557*pow(x,2)))+(0.0442253*(pow(x,3))))+(-0.00659785*(pow(x,4))))+(0.000328546*(pow(x,5))));
       pisanormvar[ivar++] = (LayerS3Frac[14]-mean)/sigma;

       mean = 1.22631*(((((1+(-0.728916*x))+(0.404153*pow(x,2)))+(-0.101055*(pow(x,3))))+(0.0119479*(pow(x,4))))+(-0.000539546*(pow(x,5))));
       sigma = 0.354251*(((((1+(-0.00640648*x))+(-0.227689*pow(x,2)))+(0.0758959*(pow(x,3))))+(-0.00957225*(pow(x,4))))+(0.000429167*(pow(x,5))));
       pisanormvar[ivar++] = (LayerS3Frac[15]-mean)/sigma;

       mean = 1.17096*(((((1+(-0.753461*x))+(0.413966*pow(x,2)))+(-0.101175*(pow(x,3))))+(0.0116518*(pow(x,4))))+(-0.000512023*(pow(x,5))));
       sigma = -0.400448*(((((1+(-2.29336*x))+(1.24944*pow(x,2)))+(-0.2953*(pow(x,3))))+(0.0323842*(pow(x,4))))+(-0.00134959*(pow(x,5))));
       pisanormvar[ivar++] = (LayerS3Frac[16]-mean)/sigma;

       mean = 0.972798*(((((1+(-0.549714*x))+(0.27782*pow(x,2)))+(-0.0619123*(pow(x,3))))+(0.00661875*(pow(x,4))))+(-0.000274428*(pow(x,5))));
       sigma = -0.354826*(((((1+(-1.9982*x))+(0.923192*pow(x,2)))+(-0.181456*(pow(x,3))))+(0.0161776*(pow(x,4))))+(-0.000531338*(pow(x,5))));
       pisanormvar[ivar++] = (LayerS3Frac[17]-mean)/sigma;

       pisanormvar[ivar++] = classification_energy;

     }
   else if ( iBDTVERSION>=5 )
     {
       //********************************
       //*****Normalize the Variables****
       //********************************
       // NOTE: starting from version 5, variable 2 has changed from ShowerSigma to F2SLEneDep
       //       and from LayerSigmaNorm1-7 to LayerS1S31-7 .
       //       The new variables ARE NOT normalized.
       // fix: normalization may be NaN below 2 GeV or above 1000 GeV;
       energyd = energyd < 1. ? 1. : (energyd > 1000. ? 1000. : energyd);
       float mean, sigma;
       float x = log(energyd);

       if (MDT_DEBUG) cout << Form("RUN %d EVENT %d --- EnergyD=%f",pev->Run(),pev->Event(),energyd) <<endl;
       if (BDT_DEBUG) std::cout << Form(" ??? x=%f\n", x) << flush;

       ivar = 0;

       mean = 5.81753*(((((1+(-0.0652423*x))+(0.123152*pow(x,2)))+(-0.0309516*(pow(x,3))))+(0.00355938*(pow(x,4))))+(-0.000154605*(pow(x,5))));
       sigma = 0.654693*(((((1+(0.349885*x))+(-0.258381*pow(x,2)))+(0.0762609*(pow(x,3))))+(-0.0101505*(pow(x,4))))+(0.000480682*(pow(x,5))));
       pisanormvar[ivar++] = (ShowerMean-mean)/sigma;
       if (MDT_DEBUG) cout << Form("piBDT[%d] = %s = %f",ivar, "ShowerMean", ShowerMean) << endl;

       pisanormvar[ivar++] = F2SLEneDep;
       if (MDT_DEBUG) cout << Form("piBDT[%d] = %s = %f",ivar, "F2SLEneDep", F2SLEneDep) << endl<<flush;

       mean = -0.00563546*(((((1+(-4.90888*x))+(3.06143*pow(x,2)))+(-1.19568*(pow(x,3))))+(0.200186*(pow(x,4))))+(-0.0124911*(pow(x,5))));
       sigma = 0.0202379*(((((1+(-0.700595*x))+(0.315499*pow(x,2)))+(-0.0637316*(pow(x,3))))+(0.00687652*(pow(x,4))))+(-0.000297002*(pow(x,5))));
       pisanormvar[ivar++] = (L2LFrac-mean)/sigma;
     if (MDT_DEBUG) cout << Form("piBDT[%d] = %s = %f",ivar, "L2LFrac", L2LFrac) << endl<<flush;

       mean = 0.976096*(((((1+(-0.00594019*x))+(0.00626969*pow(x,2)))+(-0.00277484*(pow(x,3))))+(0.000469374*(pow(x,4))))+(-2.69748e-05*(pow(x,5))));
       sigma = 0.0202693*(((((1+(-0.450749*x))+(0.156631*pow(x,2)))+(-0.0379591*(pow(x,3))))+(0.00471605*(pow(x,4))))+(-0.000217455*(pow(x,5))));
       pisanormvar[ivar++] = (R3cmFrac-mean)/sigma;
       if (MDT_DEBUG) cout << Form("piBDT[%d] = %s = %f",ivar, "R3cmFrac", R3cmFrac) << endl<<flush;

       mean = 0.947438*(((((1+(-0.0958179*x))+(0.0542999*pow(x,2)))+(-0.0149508*(pow(x,3))))+(0.00197748*(pow(x,4))))+(-9.96043e-05*(pow(x,5))));
       sigma = 0.0514221*(((((1+(-0.191206*x))+(-0.081866*pow(x,2)))+(0.0396561*(pow(x,3))))+(-0.00606394*(pow(x,4))))+(0.000326768*(pow(x,5))));
       pisanormvar[ivar++] = (S3totx-mean)/sigma;
       if (MDT_DEBUG) cout << Form("piBDT[%d] = %s = %f",ivar, "S3totx", S3totx) << endl<<flush;

       mean = 0.894616*(((((1+(-0.0830502*x))+(0.0602628*pow(x,2)))+(-0.0178786*(pow(x,3))))+(0.00239022*(pow(x,4))))+(-0.000118785*(pow(x,5))));
       sigma = 0.074716*(((((1+(-0.194879*x))+(-0.118629*pow(x,2)))+(0.0562894*(pow(x,3))))+(-0.00869822*(pow(x,4))))+(0.000470664*(pow(x,5))));
       pisanormvar[ivar++] = (S3toty-mean)/sigma;
       if (MDT_DEBUG) cout << Form("piBDT[%d] = %s = %f",ivar, "S3toty", S3toty) << endl<<flush;

       mean = 17.7832*(((((1+(4.11748*x))+(-1.32613*pow(x,2)))+(0.378096*(pow(x,3))))+(-0.0420122*(pow(x,4))))+(0.00192212*(pow(x,5))));
       sigma = -7.45314*(((((1+(-4.362*x))+(3.04245*pow(x,2)))+(-0.998655*(pow(x,3))))+(0.144479*(pow(x,4))))+(-0.00759756*(pow(x,5))));
       pisanormvar[ivar++] = (NEcalHits-mean)/sigma;
       if (MDT_DEBUG) cout << Form("piBDT[%d] = %s = %f",ivar, "NEcalHits", NEcalHits) << endl<<flush;

       mean = 2.08418*(((((1+(1.53685*x))+(-0.960584*pow(x,2)))+(0.28049*(pow(x,3))))+(-0.037519*(pow(x,4))))+(0.00186672*(pow(x,5))));
       sigma = 1.08555*(((((1+(0.301398*x))+(-0.486968*pow(x,2)))+(0.167722*(pow(x,3))))+(-0.0236229*(pow(x,4))))+(0.00120627*(pow(x,5))));
       pisanormvar[ivar++] = (ShowerFootprintX-mean)/sigma;
        if (MDT_DEBUG) cout << Form("piBDT[%d] = %s = %f",ivar, "ShowerFootprintX", ShowerFootprintX) << endl<<flush;

       mean = 3.06557*(((((1+(1.38162*x))+(-0.972884*pow(x,2)))+(0.288618*(pow(x,3))))+(-0.0381605*(pow(x,4))))+(0.00186877*(pow(x,5))));
       sigma = 1.97309*(((((1+(0.0319007*x))+(-0.296926*pow(x,2)))+(0.108391*(pow(x,3))))+(-0.0151688*(pow(x,4))))+(0.000761558*(pow(x,5))));
       pisanormvar[ivar++] = (ShowerFootprintY-mean)/sigma;
        if (MDT_DEBUG) cout << Form("piBDT[%d] = %s = %f",ivar, "ShowerFootprintY", ShowerFootprintY) << endl<<flush;

       mean = 0.0154335*(((((1+(-0.0374406*x))+(-0.1801*pow(x,2)))+(0.0575964*(pow(x,3))))+(-0.00701209*(pow(x,4))))+(0.000309315*(pow(x,5))));
       sigma = -0.0116612*(((((1+(-2.62644*x))+(1.4815*pow(x,2)))+(-0.352021*(pow(x,3))))+(0.0384267*(pow(x,4))))+(-0.00159106*(pow(x,5))));
       pisanormvar[ivar++] = (LayerEneFrac[0]-mean)/sigma;

       mean = 0.0633498*(((((1+(-0.114734*x))+(-0.154378*pow(x,2)))+(0.0562252*(pow(x,3))))+(-0.00731868*(pow(x,4))))+(0.000337497*(pow(x,5))));
       sigma = 0.0316538*(((((1+(-0.137904*x))+(-0.15952*pow(x,2)))+(0.0611123*(pow(x,3))))+(-0.00813668*(pow(x,4))))+(0.000378855*(pow(x,5))));
       pisanormvar[ivar++] = (LayerEneFrac[1]-mean)/sigma;

       mean = 0.0981715*(((((1+(0.086794*x))+(-0.25229*pow(x,2)))+(0.076108*(pow(x,3))))+(-0.00930538*(pow(x,4))))+(0.000418156*(pow(x,5))));
       sigma = 0.0584895*(((((1+(-0.518405*x))+(0.147205*pow(x,2)))+(-0.0279128*(pow(x,3))))+(0.00302415*(pow(x,4))))+(-0.000133801*(pow(x,5))));
       pisanormvar[ivar++] = (LayerEneFrac[2]-mean)/sigma;

       mean = 0.0913995*(((((1+(0.606592*x))+(-0.534856*pow(x,2)))+(0.142581*(pow(x,3))))+(-0.0169281*(pow(x,4))))+(0.000761466*(pow(x,5))));
       sigma = 0.0521775*(((((1+(-0.370914*x))+(0.102182*pow(x,2)))+(-0.0221373*(pow(x,3))))+(0.00253141*(pow(x,4))))+(-0.000108831*(pow(x,5))));
       pisanormvar[ivar++] = (LayerEneFrac[3]-mean)/sigma;

       mean = 0.0900602*(((((1+(0.623416*x))+(-0.425984*pow(x,2)))+(0.097747*(pow(x,3))))+(-0.010668*(pow(x,4))))+(0.000463787*(pow(x,5))));
       sigma = 0.0505887*(((((1+(-0.486493*x))+(0.195421*pow(x,2)))+(-0.0430779*(pow(x,3))))+(0.00438646*(pow(x,4))))+(-0.000166038*(pow(x,5))));
       pisanormvar[ivar++] = (LayerEneFrac[4]-mean)/sigma;

       mean = 0.0820018*(((((1+(0.588403*x))+(-0.295374*pow(x,2)))+(0.0567273*(pow(x,3))))+(-0.00600898*(pow(x,4))))+(0.000280536*(pow(x,5))));
       sigma = 0.0542134*(((((1+(-0.524149*x))+(0.136043*pow(x,2)))+(-0.00183907*(pow(x,3))))+(-0.00348656*(pow(x,4))))+(0.000309278*(pow(x,5))));
       pisanormvar[ivar++] = (LayerEneFrac[5]-mean)/sigma;

       mean = 0.0647088*(((((1+(0.755927*x))+(-0.322877*pow(x,2)))+(0.0691826*(pow(x,3))))+(-0.00892412*(pow(x,4))))+(0.000476596*(pow(x,5))));
       sigma = 0.0460868*(((((1+(-0.426239*x))+(0.0999163*pow(x,2)))+(-0.0125167*(pow(x,3))))+(0.00141731*(pow(x,4))))+(-0.000105953*(pow(x,5))));
       pisanormvar[ivar++] = (LayerEneFrac[6]-mean)/sigma;

       mean = 0.0591098*(((((1+(0.490957*x))+(-0.117617*pow(x,2)))+(0.0186932*(pow(x,3))))+(-0.00285173*(pow(x,4))))+(0.000169852*(pow(x,5))));
       sigma = 0.0441187*(((((1+(-0.424288*x))+(0.120773*pow(x,2)))+(-0.0303416*(pow(x,3))))+(0.00522312*(pow(x,4))))+(-0.000345832*(pow(x,5))));
       pisanormvar[ivar++] = (LayerEneFrac[7]-mean)/sigma;

       mean = 0.0544055*(((((1+(0.185537*x))+(0.115868*pow(x,2)))+(-0.0463891*(pow(x,3))))+(0.00617888*(pow(x,4))))+(-0.000325102*(pow(x,5))));
       sigma = 0.0358471*(((((1+(-0.404967*x))+(0.161259*pow(x,2)))+(-0.0523526*(pow(x,3))))+(0.00851722*(pow(x,4))))+(-0.000492585*(pow(x,5))));
       pisanormvar[ivar++] = (LayerEneFrac[8]-mean)/sigma;
       if (MDT_DEBUG) cout << Form("piBDT[%d] = %s = %f",ivar, "LayerEneFrac[8]", LayerEneFrac[8]) << endl<<flush;

       mean = 0.0586046*(((((1+(-0.35631*x))+(0.418475*pow(x,2)))+(-0.123614*(pow(x,3))))+(0.0161746*(pow(x,4))))+(-0.000828849*(pow(x,5))));
       sigma = 0.0195272*(((((1+(0.15923*x))+(-0.127709*pow(x,2)))+(0.0134291*(pow(x,3))))+(0.000637799*(pow(x,4))))+(-9.40079e-05*(pow(x,5))));
       pisanormvar[ivar++] = (LayerEneFrac[9]-mean)/sigma;

       mean = 0.0384656*(((((1+(-0.106815*x))+(0.243958*pow(x,2)))+(-0.0433174*(pow(x,3))))+(0.00189987*(pow(x,4))))+(4.54321e-05*(pow(x,5))));
       sigma = 0.0193138*(((((1+(0.109858*x))+(-0.222064*pow(x,2)))+(0.0902414*(pow(x,3))))+(-0.0161111*(pow(x,4))))+(0.00103071*(pow(x,5))));
       pisanormvar[ivar++] = (LayerEneFrac[10]-mean)/sigma;
       if (MDT_DEBUG) cout << Form("piBDT[%d] = %s = %f",ivar, "LayerEneFrac[10]", LayerEneFrac[10]) << endl<<flush;

       mean = 0.0288085*(((((1+(-0.203208*x))+(0.392827*pow(x,2)))+(-0.0992274*(pow(x,3))))+(0.0124898*(pow(x,4))))+(-0.000668279*(pow(x,5))));
       sigma = 0.0276025*(((((1+(-0.717605*x))+(0.363821*pow(x,2)))+(-0.0791709*(pow(x,3))))+(0.0067021*(pow(x,4))))+(-0.000146176*(pow(x,5))));
       pisanormvar[ivar++] = (LayerEneFrac[11]-mean)/sigma;

       mean = 0.0113727*(((((1+(1.25241*x))+(-0.32621*pow(x,2)))+(0.160578*(pow(x,3))))+(-0.0269675*(pow(x,4))))+(0.00150497*(pow(x,5))));
       sigma = -0.00706408*(((((1+(-4.56671*x))+(2.99638*pow(x,2)))+(-0.934686*(pow(x,3))))+(0.136241*(pow(x,4))))+(-0.00736457*(pow(x,5))));
       pisanormvar[ivar++] = (LayerEneFrac[12]-mean)/sigma;

       mean = -0.0014262*(((((1+(-18.5479*x))+(8.50689*pow(x,2)))+(-2.91282*(pow(x,3))))+(0.428928*(pow(x,4))))+(-0.0233242*(pow(x,5))));
       sigma = 0.000632082*(((((1+(29.7718*x))+(-20.4101*pow(x,2)))+(6.333*(pow(x,3))))+(-0.886025*(pow(x,4))))+(0.0453158*(pow(x,5))));
       pisanormvar[ivar++] = (LayerEneFrac[13]-mean)/sigma;

       mean = 0.00206941*(((((1+(6.37561*x))+(-2.65265*pow(x,2)))+(1.07375*(pow(x,3))))+(-0.171301*(pow(x,4))))+(0.0102091*(pow(x,5))));
       sigma = -0.0135454*(((((1+(-2.7593*x))+(1.80845*pow(x,2)))+(-0.554322*(pow(x,3))))+(0.0783147*(pow(x,4))))+(-0.0041224*(pow(x,5))));
       pisanormvar[ivar++] = (LayerEneFrac[14]-mean)/sigma;

       mean = -0.00391276*(((((1+(-4.63399*x))+(2.29847*pow(x,2)))+(-0.803834*(pow(x,3))))+(0.122204*(pow(x,4))))+(-0.00712181*(pow(x,5))));
       sigma = -0.0100079*(((((1+(-2.74935*x))+(1.60623*pow(x,2)))+(-0.42156*(pow(x,3))))+(0.0491247*(pow(x,4))))+(-0.002086*(pow(x,5))));
       pisanormvar[ivar++] = (LayerEneFrac[15]-mean)/sigma;

       mean = 0.0118102*(((((1+(-0.976311*x))+(0.686435*pow(x,2)))+(-0.133816*(pow(x,3))))+(0.0104236*(pow(x,4))))+(-6.07892e-05*(pow(x,5))));
       sigma = -0.0132099*(((((1+(-2.47328*x))+(1.61924*pow(x,2)))+(-0.501154*(pow(x,3))))+(0.0719867*(pow(x,4))))+(-0.0038697*(pow(x,5))));
       pisanormvar[ivar++] = (LayerEneFrac[16]-mean)/sigma;

       mean = 0.018997*(((((1+(-1.46883*x))+(0.958373*pow(x,2)))+(-0.244195*(pow(x,3))))+(0.0286415*(pow(x,4))))+(-0.00117624*(pow(x,5))));
       sigma = -0.00102647*(((((1+(-4.50726*x))+(0.00253443*pow(x,2)))+(0.603813*(pow(x,3))))+(-0.144358*(pow(x,4))))+(0.00928059*(pow(x,5))));
       pisanormvar[ivar++] = (LayerEneFrac[17]-mean)/sigma;

       pisanormvar[ivar++] = LayerS1S3[1];
       if (MDT_DEBUG) cout << Form("piBDT[%d] = %s = %f",ivar, "LayerS1S3[1]", LayerS1S3[1]) << endl<<flush;

       pisanormvar[ivar++] = LayerS1S3[2];
      if (MDT_DEBUG) cout << Form("piBDT[%d] = %s = %f",ivar, "LayerS1S3[2]", LayerS1S3[2]) << endl<<flush;

       pisanormvar[ivar++] = LayerS1S3[3];

       pisanormvar[ivar++] = LayerS1S3[4];

       pisanormvar[ivar++] = LayerS1S3[5];

       pisanormvar[ivar++] = LayerS1S3[6];

       pisanormvar[ivar++] = LayerS1S3[7];

       mean = 0.557175*(((((1+(2.12671*x))+(-1.4178*pow(x,2)))+(0.409941*(pow(x,3))))+(-0.0540956*(pow(x,4))))+(0.00267714*(pow(x,5))));
       sigma = 0.00897424*(((((1+(80.9882*x))+(-57.6913*pow(x,2)))+(16.3878*(pow(x,3))))+(-2.09909*(pow(x,4))))+(0.100461*(pow(x,5))));
       pisanormvar[ivar++] = (LayerSigma[8]-mean)/sigma;
      if (MDT_DEBUG) cout << Form("piBDT[%d] = %s = %f",ivar, "LayerSigma[8]", LayerSigma[8]) << endl<<flush;

       mean = 0.825791*(((((1+(1.22133*x))+(-0.880486*pow(x,2)))+(0.267216*(pow(x,3))))+(-0.0366376*(pow(x,4))))+(0.00186991*(pow(x,5))));
       sigma = 0.465602*(((((1+(0.508751*x))+(-0.654088*pow(x,2)))+(0.221749*(pow(x,3))))+(-0.0314511*(pow(x,4))))+(0.00161545*(pow(x,5))));
       pisanormvar[ivar++] = (LayerSigma[9]-mean)/sigma;
      if (MDT_DEBUG) cout << Form("piBDT[%d] = %s = %f",ivar, "LayerSigma[9]", LayerSigma[9]) << endl<<flush;

       mean = 0.910081*(((((1+(0.739722*x))+(-0.509627*pow(x,2)))+(0.159914*(pow(x,3))))+(-0.0229559*(pow(x,4))))+(0.001221*(pow(x,5))));
       sigma = 0.205266*(((((1+(2.68981*x))+(-2.12361*pow(x,2)))+(0.623894*(pow(x,3))))+(-0.0819358*(pow(x,4))))+(0.0040157*(pow(x,5))));
       pisanormvar[ivar++] = (LayerSigma[10]-mean)/sigma;

       mean = 0.0475479*(((((1+(46.9903*x))+(-28.6912*pow(x,2)))+(8.10759*(pow(x,3))))+(-1.06855*(pow(x,4))))+(0.0529542*(pow(x,5))));
       sigma = -0.0118896*(((((1+(-94.7724*x))+(68.6104*pow(x,2)))+(-19.8302*(pow(x,3))))+(2.58454*(pow(x,4))))+(-0.125683*(pow(x,5))));
       pisanormvar[ivar++] = (LayerSigma[11]-mean)/sigma;

       mean = -1.15196*(((((1+(-4.18659*x))+(2.54371*pow(x,2)))+(-0.698499*(pow(x,3))))+(0.0890199*(pow(x,4))))+(-0.00427228*(pow(x,5))));
       sigma = -0.422523*(((((1+(-5.21731*x))+(3.55303*pow(x,2)))+(-0.9936*(pow(x,3))))+(0.125859*(pow(x,4))))+(-0.00596396*(pow(x,5))));
       pisanormvar[ivar++] = (LayerSigma[12]-mean)/sigma;

       mean = -2.29679*(((((1+(-2.7974*x))+(1.5947*pow(x,2)))+(-0.416448*(pow(x,3))))+(0.0507933*(pow(x,4))))+(-0.00234097*(pow(x,5))));
       sigma = -1.22776*(((((1+(-2.77693*x))+(1.71971*pow(x,2)))+(-0.457114*(pow(x,3))))+(0.0557562*(pow(x,4))))+(-0.00255766*(pow(x,5))));
       pisanormvar[ivar++] = (LayerSigma[13]-mean)/sigma;

       mean = -3.65486*(((((1+(-2.17485*x))+(1.16497*pow(x,2)))+(-0.291875*(pow(x,3))))+(0.0345623*(pow(x,4))))+(-0.00155801*(pow(x,5))));
       sigma = -1.55552*(((((1+(-2.25803*x))+(1.25426*pow(x,2)))+(-0.303822*(pow(x,3))))+(0.0341468*(pow(x,4))))+(-0.00145752*(pow(x,5))));
       pisanormvar[ivar++] = (LayerSigma[14]-mean)/sigma;

       mean = -5.07802*(((((1+(-1.84923*x))+(0.927057*pow(x,2)))+(-0.219636*(pow(x,3))))+(0.0247829*(pow(x,4))))+(-0.00107054*(pow(x,5))));
       sigma = -2.64475*(((((1+(-1.62379*x))+(0.757685*pow(x,2)))+(-0.156366*(pow(x,3))))+(0.0149065*(pow(x,4))))+(-0.000533412*(pow(x,5))));
       pisanormvar[ivar++] = (LayerSigma[15]-mean)/sigma;

       mean = -8.09602*(((((1+(-1.62571*x))+(0.788618*pow(x,2)))+(-0.181389*(pow(x,3))))+(0.0199756*(pow(x,4))))+(-0.000846765*(pow(x,5))));
       sigma = -3.55731*(((((1+(-1.67015*x))+(0.82708*pow(x,2)))+(-0.184353*(pow(x,3))))+(0.0193368*(pow(x,4))))+(-0.000777463*(pow(x,5))));
       pisanormvar[ivar++] = (LayerSigma[16]-mean)/sigma;

       mean = -3.12835*(((((1+(-1.7683*x))+(0.662891*pow(x,2)))+(-0.110456*(pow(x,3))))+(0.00795358*(pow(x,4))))+(-0.0001761*(pow(x,5))));
       sigma = -2.70441*(((((1+(-1.70159*x))+(0.783491*pow(x,2)))+(-0.157919*(pow(x,3))))+(0.0146316*(pow(x,4))))+(-0.000505912*(pow(x,5))));
       pisanormvar[ivar++] = (LayerSigma[17]-mean)/sigma;

       mean = 2.71022*(((((1+(-0.130137*x))+(0.129101*pow(x,2)))+(-0.0405934*(pow(x,3))))+(0.00526541*(pow(x,4))))+(-0.000254535*(pow(x,5))));
       sigma = 0.433849*(((((1+(1.74973*x))+(-1.29381*pow(x,2)))+(0.370721*(pow(x,3))))+(-0.0480519*(pow(x,4))))+(0.00236235*(pow(x,5))));
       pisanormvar[ivar++] = (LayerS3Frac[1]-mean)/sigma;

       mean = 0.830443*(((((1+(0.306295*x))+(-0.149546*pow(x,2)))+(0.0335532*(pow(x,3))))+(-0.00362516*(pow(x,4))))+(0.000148019*(pow(x,5))));
       sigma = -0.0215234*(((((1+(-17.32*x))+(10.8897*pow(x,2)))+(-2.92155*(pow(x,3))))+(0.366721*(pow(x,4))))+(-0.0177822*(pow(x,5))));
       pisanormvar[ivar++] = (LayerS3Frac[2]-mean)/sigma;

       mean = 0.878041*(((((1+(0.168915*x))+(-0.0904153*pow(x,2)))+(0.0220529*(pow(x,3))))+(-0.00257015*(pow(x,4))))+(0.000113793*(pow(x,5))));
       sigma = -0.0332275*(((((1+(-4.50798*x))+(2.89585*pow(x,2)))+(-0.803476*(pow(x,3))))+(0.103883*(pow(x,4))))+(-0.00512917*(pow(x,5))));
       pisanormvar[ivar++] = (LayerS3Frac[3]-mean)/sigma;

       mean = 1.08295*(((((1+(-0.117056*x))+(0.0453468*pow(x,2)))+(-0.00841535*(pow(x,3))))+(0.000742509*(pow(x,4))))+(-2.56053e-05*(pow(x,5))));
       sigma = 0.492885*(((((1+(-0.864558*x))+(0.31567*pow(x,2)))+(-0.0581046*(pow(x,3))))+(0.00531972*(pow(x,4))))+(-0.000192369*(pow(x,5))));
       pisanormvar[ivar++] = (LayerS3Frac[4]-mean)/sigma;

       mean = 0.966137*(((((1+(-0.0447478*x))+(0.024797*pow(x,2)))+(-0.00580008*(pow(x,3))))+(0.000631017*(pow(x,4))))+(-2.67751e-05*(pow(x,5))));
       sigma = 0.324553*(((((1+(-0.739022*x))+(0.220331*pow(x,2)))+(-0.0297578*(pow(x,3))))+(0.00153657*(pow(x,4))))+(-4.23326e-06*(pow(x,5))));
       pisanormvar[ivar++] = (LayerS3Frac[5]-mean)/sigma;

       mean = 0.918183*(((((1+(0.0245774*x))+(-0.0165454*pow(x,2)))+(0.00513393*(pow(x,3))))+(-0.000687314*(pow(x,4))))+(3.29047e-05*(pow(x,5))));
       sigma = 0.144763*(((((1+(-0.652088*x))+(0.223288*pow(x,2)))+(-0.0435651*(pow(x,3))))+(0.00449212*(pow(x,4))))+(-0.000187523*(pow(x,5))));
       pisanormvar[ivar++] = (LayerS3Frac[6]-mean)/sigma;

       mean = 0.907456*(((((1+(-0.00101406*x))+(0.00160197*pow(x,2)))+(-5.47284e-05*(pow(x,3))))+(4.57173e-06*(pow(x,4))))+(-2.01113e-06*(pow(x,5))));
       sigma = 0.137874*(((((1+(-0.50364*x))+(0.11603*pow(x,2)))+(-0.0126287*(pow(x,3))))+(0.000406567*(pow(x,4))))+(1.59863e-05*(pow(x,5))));
       pisanormvar[ivar++] = (LayerS3Frac[7]-mean)/sigma;

       mean = 0.914388*(((((1+(-0.157968*x))+(0.113621*pow(x,2)))+(-0.0320707*(pow(x,3))))+(0.00411686*(pow(x,4))))+(-0.000198991*(pow(x,5))));
       sigma = 0.29723*(((((1+(-0.714645*x))+(0.237853*pow(x,2)))+(-0.0420266*(pow(x,3))))+(0.00377121*(pow(x,4))))+(-0.000133952*(pow(x,5))));
       pisanormvar[ivar++] = (LayerS3Frac[8]-mean)/sigma;

       mean = 0.912766*(((((1+(-0.211328*x))+(0.147518*pow(x,2)))+(-0.0414031*(pow(x,3))))+(0.00532721*(pow(x,4))))+(-0.000258884*(pow(x,5))));
       sigma = 0.267796*(((((1+(-0.496191*x))+(0.0753246*pow(x,2)))+(0.0053784*(pow(x,3))))+(-0.00245852*(pow(x,4))))+(0.000172211*(pow(x,5))));
       pisanormvar[ivar++] = (LayerS3Frac[9]-mean)/sigma;

       mean = 0.882403*(((((1+(-0.155903*x))+(0.108684*pow(x,2)))+(-0.0309816*(pow(x,3))))+(0.00412699*(pow(x,4))))+(-0.00020889*(pow(x,5))));
       sigma = 0.251275*(((((1+(-0.532008*x))+(0.1279*pow(x,2)))+(-0.0150123*(pow(x,3))))+(0.000638418*(pow(x,4))))+(7.92237e-06*(pow(x,5))));
       pisanormvar[ivar++] = (LayerS3Frac[10]-mean)/sigma;

       mean = 0.91556*(((((1+(-0.273105*x))+(0.178677*pow(x,2)))+(-0.0495976*(pow(x,3))))+(0.00643063*(pow(x,4))))+(-0.000316353*(pow(x,5))));
       sigma = 0.367702*(((((1+(-0.592629*x))+(0.151355*pow(x,2)))+(-0.0180565*(pow(x,3))))+(0.000785277*(pow(x,4))))+(4.72664e-06*(pow(x,5))));
       pisanormvar[ivar++] = (LayerS3Frac[11]-mean)/sigma;

       mean = 0.89048*(((((1+(-0.417808*x))+(0.285339*pow(x,2)))+(-0.0797402*(pow(x,3))))+(0.0102105*(pow(x,4))))+(-0.00049191*(pow(x,5))));
       sigma = 0.464536*(((((1+(-0.56921*x))+(0.141747*pow(x,2)))+(-0.0176432*(pow(x,3))))+(0.000979462*(pow(x,4))))+(-1.3265e-05*(pow(x,5))));
    pisanormvar[ivar++] = (LayerS3Frac[12]-mean)/sigma;

       mean = 0.914006*(((((1+(-0.505655*x))+(0.329083*pow(x,2)))+(-0.0891581*(pow(x,3))))+(0.0111151*(pow(x,4))))+(-0.000522036*(pow(x,5))));
       sigma = 0.561738*(((((1+(-0.521007*x))+(0.0973216*pow(x,2)))+(-0.0042303*(pow(x,3))))+(-0.000662862*(pow(x,4))))+(5.63393e-05*(pow(x,5))));
       pisanormvar[ivar++] = (LayerS3Frac[13]-mean)/sigma;

       mean = 1.13152*(((((1+(-0.645211*x))+(0.370763*pow(x,2)))+(-0.0952597*(pow(x,3))))+(0.0115274*(pow(x,4))))+(-0.000531245*(pow(x,5))));
       sigma = 0.449899*(((((1+(-0.263936*x))+(-0.0807557*pow(x,2)))+(0.0442253*(pow(x,3))))+(-0.00659785*(pow(x,4))))+(0.000328546*(pow(x,5))));
       pisanormvar[ivar++] = (LayerS3Frac[14]-mean)/sigma;

       mean = 1.22631*(((((1+(-0.728916*x))+(0.404153*pow(x,2)))+(-0.101055*(pow(x,3))))+(0.0119479*(pow(x,4))))+(-0.000539546*(pow(x,5))));
       sigma = 0.354251*(((((1+(-0.00640648*x))+(-0.227689*pow(x,2)))+(0.0758959*(pow(x,3))))+(-0.00957225*(pow(x,4))))+(0.000429167*(pow(x,5))));
       pisanormvar[ivar++] = (LayerS3Frac[15]-mean)/sigma;

       mean = 1.17096*(((((1+(-0.753461*x))+(0.413966*pow(x,2)))+(-0.101175*(pow(x,3))))+(0.0116518*(pow(x,4))))+(-0.000512023*(pow(x,5))));
       sigma = -0.400448*(((((1+(-2.29336*x))+(1.24944*pow(x,2)))+(-0.2953*(pow(x,3))))+(0.0323842*(pow(x,4))))+(-0.00134959*(pow(x,5))));
       pisanormvar[ivar++] = (LayerS3Frac[16]-mean)/sigma;

       mean = 0.972798*(((((1+(-0.549714*x))+(0.27782*pow(x,2)))+(-0.0619123*(pow(x,3))))+(0.00661875*(pow(x,4))))+(-0.000274428*(pow(x,5))));
       sigma = -0.354826*(((((1+(-1.9982*x))+(0.923192*pow(x,2)))+(-0.181456*(pow(x,3))))+(0.0161776*(pow(x,4))))+(-0.000531338*(pow(x,5))));
       pisanormvar[ivar++] = (LayerS3Frac[17]-mean)/sigma;

       pisanormvar[ivar++] = classification_energy;

     }


   for (ivar = 0; ivar < nPISABDTVARs; ++ivar)
     {
      if (pisanormvar[ivar] != pisanormvar[ivar])
      {
         if (pisainputvar[ivar] != pisainputvar[ivar])
         { //the input variable is a NaN.
            std::cerr << Form(" !!! GetEcalBDT() -> pisainputvar[%.2d] (%s) NaN. Input variable is a NaN. Returning -9999\n", ivar, pisavarname[ivar]) << endl << flush;
            return -9999.;
         }
         else
         { //only the output variable is a Nan. There should be a problem in the normalization
            std::cerr << Form(" !!! GetEcalBDT() -> pisanormvar[%.2d] (%sNorm) NaN. Normalized variable is a NaN (Before normalization = %f). Returning -9999\n", ivar, pisavarname[ivar], pisainputvar[ivar]) << endl << flush;
            return -9999.;
         }
      }
   }

   if (BDT_DEBUG)
   {
      for (ivar = 0; ivar < nPISABDTVARs + 1; ++ivar)
         std::cout << Form(" ??? ivar:%02d \t %-20s \t input:%f \t norm:%f\n", ivar, pisavarname[ivar], pisainputvar[ivar], pisanormvar[ivar]);
      std::cout << flush;
   }



   //********************************
   //*****  Calculate BDT    ********
   //********************************
   float bdt;

   if ( iBDTVERSION < 5 )
     {
       bdt = ecalpisareader->EvaluateMVA("BDTG_LAYERS");
     }
   else if ( iBDTVERSION >= 5 && TMVAClassifier == 0 && EnergyFlag==0 )
     {
       if ( pev->Event()%2 == 1 ) 
	 {
	   bdt = ecalpisareader_ODD->EvaluateMVA("BDTG_LAYERS_ODD");
	 }
       else
	 {
	   bdt = ecalpisareader_EVEN->EvaluateMVA("BDTG_LAYERS_EVEN");
	 }
     }
   else if ( iBDTVERSION >= 5 && TMVAClassifier == 1  && EnergyFlag==0 )
     {
       if ( pev->Event()%2 == 1 ) 
	 {
	   bdt = ecalpisareaderS_ODD->EvaluateMVA("BDTS_LAYERS_ODD");
	 }
       else
	 {
	   bdt = ecalpisareaderS_EVEN->EvaluateMVA("BDTS_LAYERS_EVEN");
	 }
     }
   else if ( iBDTVERSION >= 5 && TMVAClassifier == 0 && EnergyFlag==1 )
     {
       if ( pev->Event()%2 == 1 ) 
	 {
	   bdt = ecalpisareader_E_ODD->EvaluateMVA("BDTG_LAYERS_ODD");
	 }
       else
	 {
	   bdt = ecalpisareader_E_EVEN->EvaluateMVA("BDTG_LAYERS_EVEN");
	 }
     }
   else if ( iBDTVERSION >= 5 && TMVAClassifier == 1  && EnergyFlag==1 )
     {
       if ( pev->Event()%2 == 1 ) 
	 {
	   bdt = ecalpisareaderS_E_ODD->EvaluateMVA("BDTS_LAYERS_ODD");
	 }
       else
	 {
	   bdt = ecalpisareaderS_E_EVEN->EvaluateMVA("BDTS_LAYERS_EVEN");
	 }
     }


   else bdt=-999.;

   if (BDT_HISTOS)
     {
       for (int i=0; i<nPISABDTVARs; i++)
	 {
	   hECALBDT[i]->Fill(pisanormvar[i]);
	 }
     }
   //
   return bdt;
}
