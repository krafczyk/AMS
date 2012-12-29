#include "root.h"
#include "TMVA/Tools.h"
#include <EcalChi2CY.h>

TMVA::Reader *ecalBDTCHI2reader_v1 = NULL;
TMVA::Reader *ecalBDTCHI2reader_v2 = NULL;
const unsigned int nPIBCHI2VARs = 44;
float piBCHI2normvar[nPIBCHI2VARs + 1];
float piBCHI2inputvar[nPIBCHI2VARs + 1];
char *piBCHI2vaname[nPIBCHI2VARs + 1] = {
   "ShowerMean", "ShowerSigma",
   "L2LFrac", "R3cmFrac", "S3totx", "S3toty",
   "NEcalHits",
   "ShowerFootprintX", "ShowerFootprintY",
   "LayerEneFrac0", "LayerEneFrac1", "LayerEneFrac2", "LayerEneFrac3", "LayerEneFrac4", "LayerEneFrac5",
   "LayerEneFrac6", "LayerEneFrac7", "LayerEneFrac8", "LayerEneFrac9", "LayerEneFrac10","LayerEneFrac11",
   "LayerEneFrac12","LayerEneFrac13","LayerEneFrac14","LayerEneFrac15","LayerEneFrac16","LayerEneFrac17",
   "LayerChi21",    "LayerChi22",    "LayerChi23",    "LayerChi24",    "LayerChi25",    "LayerChi26",
   "LayerChi27",    "LayerChi28",    "LayerChi29",    "LayerChi210",   "LayerChi211",   "LayerChi212",
   "LayerChi213",   "LayerChi214",   "LayerChi215",   "LayerChi216",   "LayerChi217",
   "EnergyD"
};
EcalAxis *ecalaxis=0;

bool BCHI2_DEBUG = false;

float EcalShowerR::GetEcalBDTCHI2(AMSEventR *pev, unsigned int iBDTCHI2VERSION)
{
   if (BCHI2_DEBUG)
   {
      std::cout << " ??? EcalShowerR::GetEcalBDTCHI2(pev,iBDTCHI2VERSION) \n";
      std::cout << " ??? *************************************************\n";
      std::cout << " ??? ***************Compute the variables*************\n";
      std::cout << " ??? *************************************************\n" << flush;
   }

   EcalAxis::Version=2;
   if (!ecalaxis) ecalaxis = new EcalAxis();
   ecalaxis->process(pev,2);
  
   const unsigned int nLAYERs = 18;
   const unsigned int nCELLs  = 72;

   float MapEneDep[nLAYERs][nCELLs]; // Energy deposit in every cell of ECAL [GeV]
   float ClusterEnergy[nLAYERs];     // Lateral leak corrected energy deposit [GeV]
   float LayerEneDep[nLAYERs];       // Energy deposit [GeV] in each layer (sum of every cell of each layer)

   float LayerEnergy = 0.; // sum_i ClusterEnergy[i]

   float EneDep = 0.;
   float EneDepXY[2]; // 0 = x, 1 = y

   float ClusterEnergyXY[2]; // 0 = x, 1 = y

   float LayerChi2[nLAYERs];
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
      LayerChi2[ilayer]     = 0.;

      LayerMean[ilayer]  = 0.;
      LayerSigma[ilayer] = 0.;

      LayerS1S3[ilayer]   =  1.;
      LayerS3Frac[ilayer] = -1.;

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

      //get Chi2 per layer
      LayerChi2[ilayer] = ecalaxis->ecalchi2->get_chi2(ilayer);

      UChar_t proj = !((ilayer/2) % 2);

      ClusterEnergyXY[proj] += ClusterEnergy[ilayer];
      LayerEnergy           += ClusterEnergy[ilayer];

      ShowerMean += (ilayer + 1)*ClusterEnergy[ilayer];

      unsigned int imaxcell = 0;
      float maxcellene      = 0.;

      for (unsigned int icell = 0; icell < nCELLs; ++icell)
      {
         if (MapEneDep[ilayer][icell] == 0.) continue;

         EneDep += MapEneDep[ilayer][icell];

         LayerEneDep[ilayer] += MapEneDep[ilayer][icell];

         if (MapEneDep[ilayer][icell] >= maxcellene)
         {
            maxcellene = MapEneDep[ilayer][icell];
            imaxcell   = icell;
         }

         LayerMean[ilayer]  += (icell + 1)*MapEneDep[ilayer][icell];
         ShowerMeanXY[proj] += (icell + 1)*MapEneDep[ilayer][icell];
      }

      EneDepXY[proj] += LayerEneDep[ilayer];

      float S1 = MapEneDep[ilayer][imaxcell];
      float S3 = S1;
      if (imaxcell > 0) S3 += MapEneDep[ilayer][imaxcell - 1];
      if (imaxcell < nCELLs - 1) S3 += MapEneDep[ilayer][imaxcell + 1];

      if (!proj) S3totx += S3;
      else S3toty += S3;

      if (S3 > 0.) LayerS1S3[ilayer] = S1/S3;

      if (LayerEneDep[ilayer] > 0.)
      {
         LayerS3Frac[ilayer] = S3/LayerEneDep[ilayer];
         LayerMean[ilayer]   = LayerMean[ilayer]/LayerEneDep[ilayer];
      }
      else
      {
         LayerMean[ilayer] = -1.;
      }
   }

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
         if (MapEneDep[ilayer][icell] == 0.) continue;

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

   S3totx = this->S3tot[1];
   S3toty = this->S3tot[2];
   NEcalHits = this->Nhits;

   float energyd = EnergyD/1000.;

   if (BCHI2_DEBUG) std::cout << " ??? BDT variables computed\n" << flush;

   unsigned int ivar = 0;

   piBCHI2inputvar[ivar++] = ShowerMean;
   piBCHI2inputvar[ivar++] = ShowerSigma;
   piBCHI2inputvar[ivar++] = L2LFrac;
   piBCHI2inputvar[ivar++] = R3cmFrac;
   piBCHI2inputvar[ivar++] = S3totx;
   piBCHI2inputvar[ivar++] = S3toty;
   piBCHI2inputvar[ivar++] = NEcalHits;
   piBCHI2inputvar[ivar++] = ShowerFootprint[1];
   piBCHI2inputvar[ivar++] = ShowerFootprint[2];
   piBCHI2inputvar[ivar++] = LayerEneFrac[0];
   piBCHI2inputvar[ivar++] = LayerEneFrac[1];
   piBCHI2inputvar[ivar++] = LayerEneFrac[2];
   piBCHI2inputvar[ivar++] = LayerEneFrac[3];
   piBCHI2inputvar[ivar++] = LayerEneFrac[4];
   piBCHI2inputvar[ivar++] = LayerEneFrac[5];
   piBCHI2inputvar[ivar++] = LayerEneFrac[6];
   piBCHI2inputvar[ivar++] = LayerEneFrac[7];
   piBCHI2inputvar[ivar++] = LayerEneFrac[8];
   piBCHI2inputvar[ivar++] = LayerEneFrac[9];
   piBCHI2inputvar[ivar++] = LayerEneFrac[10];
   piBCHI2inputvar[ivar++] = LayerEneFrac[11];
   piBCHI2inputvar[ivar++] = LayerEneFrac[12];
   piBCHI2inputvar[ivar++] = LayerEneFrac[13];
   piBCHI2inputvar[ivar++] = LayerEneFrac[14];
   piBCHI2inputvar[ivar++] = LayerEneFrac[15];
   piBCHI2inputvar[ivar++] = LayerEneFrac[16];
   piBCHI2inputvar[ivar++] = LayerEneFrac[17];
   piBCHI2inputvar[ivar++] = LayerChi2[1];
   piBCHI2inputvar[ivar++] = LayerChi2[2];
   piBCHI2inputvar[ivar++] = LayerChi2[3];
   piBCHI2inputvar[ivar++] = LayerChi2[4];
   piBCHI2inputvar[ivar++] = LayerChi2[5];
   piBCHI2inputvar[ivar++] = LayerChi2[6];
   piBCHI2inputvar[ivar++] = LayerChi2[7];
   piBCHI2inputvar[ivar++] = LayerChi2[8];
   piBCHI2inputvar[ivar++] = LayerChi2[9];
   piBCHI2inputvar[ivar++] = LayerChi2[10];
   piBCHI2inputvar[ivar++] = LayerChi2[11];
   piBCHI2inputvar[ivar++] = LayerChi2[12];
   piBCHI2inputvar[ivar++] = LayerChi2[13];
   piBCHI2inputvar[ivar++] = LayerChi2[14];
   piBCHI2inputvar[ivar++] = LayerChi2[15];
   piBCHI2inputvar[ivar++] = LayerChi2[16];
   piBCHI2inputvar[ivar++] = LayerChi2[17];
   piBCHI2inputvar[ivar++] = energyd;


   //********************************
   //*****Init the Reader************
   //********************************

   TMVA::Reader *ecalBDTCHI2reader = NULL;

   if ( iBDTCHI2VERSION == 1 )
   {
     ecalBDTCHI2reader = ecalBDTCHI2reader_v1;
   }
   else if ( iBDTCHI2VERSION == 2 )
   {
     ecalBDTCHI2reader = ecalBDTCHI2reader_v2;
   }
   else
   {
     cout<<" "<<endl;
     cout<<" ====================================================================="<<endl;
     cout<<" [ecalBDTCHI2] ATTENTION    only version 1 and 2 of BDTCHI2 supported"<<endl;
     cout<<" [ecalBDTCHI2] ATTENTION    you have called it with version "<<iBDTCHI2VERSION<<endl;
     cout<<" [ecalBDTCHI2] ATTENTION    BDT will be set to -999 for all entries!!!"<<endl;
     cout<<" ====================================================================="<<endl;
     cout<<" "<<endl;

     return -999;
   }

   if (ecalBDTCHI2reader == NULL)     //if not already Init.....
   {
      std::cout << "##############################################################" << std::endl;
      std::cout << "                   Create GetEcalBDTCHI2 Reader   " << std::endl;
      std::cout << "##############################################################" << std::endl;

      //~ TMVA::Tools::Instance();
      ecalBDTCHI2reader = new TMVA::Reader("Color:!Silent:V:VerbosityLevel=Debug:H");

      ecalBDTCHI2reader->AddSpectator("EnergyD", &piBCHI2normvar[nPIBCHI2VARs]);

      ivar = 0;

      ecalBDTCHI2reader->AddVariable("ShowerMeanNorm",       &piBCHI2normvar[ivar++]);
      ecalBDTCHI2reader->AddVariable("ShowerSigmaNorm",      &piBCHI2normvar[ivar++]);
      ecalBDTCHI2reader->AddVariable("L2LFracNorm",          &piBCHI2normvar[ivar++]);
      ecalBDTCHI2reader->AddVariable("R3cmFracNorm",         &piBCHI2normvar[ivar++]);
      ecalBDTCHI2reader->AddVariable("S3totxNorm",           &piBCHI2normvar[ivar++]);
      ecalBDTCHI2reader->AddVariable("S3totyNorm",           &piBCHI2normvar[ivar++]);
      ecalBDTCHI2reader->AddVariable("NEcalHitsNorm",        &piBCHI2normvar[ivar++]);
      ecalBDTCHI2reader->AddVariable("ShowerFootprintXNorm", &piBCHI2normvar[ivar++]);
      ecalBDTCHI2reader->AddVariable("ShowerFootprintYNorm", &piBCHI2normvar[ivar++]);
      ecalBDTCHI2reader->AddVariable("LayerEneFracNorm0",    &piBCHI2normvar[ivar++]);
      ecalBDTCHI2reader->AddVariable("LayerEneFracNorm1",    &piBCHI2normvar[ivar++]);
      ecalBDTCHI2reader->AddVariable("LayerEneFracNorm2",    &piBCHI2normvar[ivar++]);
      ecalBDTCHI2reader->AddVariable("LayerEneFracNorm3",    &piBCHI2normvar[ivar++]);
      ecalBDTCHI2reader->AddVariable("LayerEneFracNorm4",    &piBCHI2normvar[ivar++]);
      ecalBDTCHI2reader->AddVariable("LayerEneFracNorm5",    &piBCHI2normvar[ivar++]);
      ecalBDTCHI2reader->AddVariable("LayerEneFracNorm6",    &piBCHI2normvar[ivar++]);
      ecalBDTCHI2reader->AddVariable("LayerEneFracNorm7",    &piBCHI2normvar[ivar++]);
      ecalBDTCHI2reader->AddVariable("LayerEneFracNorm8",    &piBCHI2normvar[ivar++]);
      ecalBDTCHI2reader->AddVariable("LayerEneFracNorm9",    &piBCHI2normvar[ivar++]);
      ecalBDTCHI2reader->AddVariable("LayerEneFracNorm10",   &piBCHI2normvar[ivar++]);
      ecalBDTCHI2reader->AddVariable("LayerEneFracNorm11",   &piBCHI2normvar[ivar++]);
      ecalBDTCHI2reader->AddVariable("LayerEneFracNorm12",   &piBCHI2normvar[ivar++]);
      ecalBDTCHI2reader->AddVariable("LayerEneFracNorm13",   &piBCHI2normvar[ivar++]);
      ecalBDTCHI2reader->AddVariable("LayerEneFracNorm14",   &piBCHI2normvar[ivar++]);
      ecalBDTCHI2reader->AddVariable("LayerEneFracNorm15",   &piBCHI2normvar[ivar++]);
      ecalBDTCHI2reader->AddVariable("LayerEneFracNorm16",   &piBCHI2normvar[ivar++]);
      ecalBDTCHI2reader->AddVariable("LayerEneFracNorm17",   &piBCHI2normvar[ivar++]);
      ecalBDTCHI2reader->AddVariable("LayerChi21",      &piBCHI2normvar[ivar++]);
      ecalBDTCHI2reader->AddVariable("LayerChi22",      &piBCHI2normvar[ivar++]);
      ecalBDTCHI2reader->AddVariable("LayerChi23",      &piBCHI2normvar[ivar++]);
      ecalBDTCHI2reader->AddVariable("LayerChi24",      &piBCHI2normvar[ivar++]);
      ecalBDTCHI2reader->AddVariable("LayerChi25",      &piBCHI2normvar[ivar++]);
      ecalBDTCHI2reader->AddVariable("LayerChi26",      &piBCHI2normvar[ivar++]);
      ecalBDTCHI2reader->AddVariable("LayerChi27",      &piBCHI2normvar[ivar++]);
      ecalBDTCHI2reader->AddVariable("LayerChi28",      &piBCHI2normvar[ivar++]);
      ecalBDTCHI2reader->AddVariable("LayerChi29",      &piBCHI2normvar[ivar++]);
      ecalBDTCHI2reader->AddVariable("LayerChi210",     &piBCHI2normvar[ivar++]);
      ecalBDTCHI2reader->AddVariable("LayerChi211",     &piBCHI2normvar[ivar++]);
      ecalBDTCHI2reader->AddVariable("LayerChi212",     &piBCHI2normvar[ivar++]);
      ecalBDTCHI2reader->AddVariable("LayerChi213",     &piBCHI2normvar[ivar++]);
      ecalBDTCHI2reader->AddVariable("LayerChi214",     &piBCHI2normvar[ivar++]);
      ecalBDTCHI2reader->AddVariable("LayerChi215",     &piBCHI2normvar[ivar++]);
      ecalBDTCHI2reader->AddVariable("LayerChi216",     &piBCHI2normvar[ivar++]);
      ecalBDTCHI2reader->AddVariable("LayerChi217",     &piBCHI2normvar[ivar++]);

      char WeightsDir[100];
      sprintf(WeightsDir,"%s/v5.00", getenv("AMSDataDir"));
      //
      if ( iBDTCHI2VERSION == 1 )
	{
	  ecalBDTCHI2reader->BookMVA("BDTG_LAYERS", Form("%s/ECAL_PISA_BDTCHI2_412_v1.weights.xml", WeightsDir));
	  ecalBDTCHI2reader_v1 = ecalBDTCHI2reader;
	}
      else if ( iBDTCHI2VERSION == 2 )
	{
	  ecalBDTCHI2reader->BookMVA("BDTG_LAYERS", Form("%s/ECAL_PISA_BDTCHI2_412_v2.weights.xml", WeightsDir));
	  ecalBDTCHI2reader_v2 = ecalBDTCHI2reader;
	}
   }


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



   if ( iBDTCHI2VERSION == 1 )
     {
       //********************************
       //*****Normalize the Variables****
       //********************************
       // fix: normalization may be NaN below 0.5 GeV or above 1000 GeV;
       energyd = energyd < 0.5 ? 0.5 : (energyd > 1000. ? 1000. : energyd);
       float mean, sigma;
       float x = log(energyd);

       if (BCHI2_DEBUG) std::cout << Form(" ??? x=%f\n", x) << flush;

       ivar = 0;

       mean = 5.81753*(((((1+(-0.0652423*x))+(0.123152*pow(x,2)))+(-0.0309516*(pow(x,3))))+(0.00355938*(pow(x,4))))+(-0.000154605*(pow(x,5))));
       sigma = 0.654693*(((((1+(0.349885*x))+(-0.258381*pow(x,2)))+(0.0762609*(pow(x,3))))+(-0.0101505*(pow(x,4))))+(0.000480682*(pow(x,5))));
       piBCHI2normvar[ivar++] = (ShowerMean-mean)/sigma;

       mean = 3.19961*(((((1+(0.122833*x))+(-0.0515297*pow(x,2)))+(0.0142255*(pow(x,3))))+(-0.00216798*(pow(x,4))))+(0.000126187*(pow(x,5))));
       sigma = 0.550582*(((((1+(-0.436487*x))+(0.0995998*pow(x,2)))+(-0.00967967*(pow(x,3))))+(-1.31632e-05*(pow(x,4))))+(4.37799e-05*(pow(x,5))));
       piBCHI2normvar[ivar++] = (ShowerSigma-mean)/sigma;

       mean = -0.00563546*(((((1+(-4.90888*x))+(3.06143*pow(x,2)))+(-1.19568*(pow(x,3))))+(0.200186*(pow(x,4))))+(-0.0124911*(pow(x,5))));
       sigma = 0.0202379*(((((1+(-0.700595*x))+(0.315499*pow(x,2)))+(-0.0637316*(pow(x,3))))+(0.00687652*(pow(x,4))))+(-0.000297002*(pow(x,5))));
       piBCHI2normvar[ivar++] = (L2LFrac-mean)/sigma;

       mean = 0.976096*(((((1+(-0.00594019*x))+(0.00626969*pow(x,2)))+(-0.00277484*(pow(x,3))))+(0.000469374*(pow(x,4))))+(-2.69748e-05*(pow(x,5))));
       sigma = 0.0202693*(((((1+(-0.450749*x))+(0.156631*pow(x,2)))+(-0.0379591*(pow(x,3))))+(0.00471605*(pow(x,4))))+(-0.000217455*(pow(x,5))));
       piBCHI2normvar[ivar++] = (R3cmFrac-mean)/sigma;

       mean = 0.947438*(((((1+(-0.0958179*x))+(0.0542999*pow(x,2)))+(-0.0149508*(pow(x,3))))+(0.00197748*(pow(x,4))))+(-9.96043e-05*(pow(x,5))));
       sigma = 0.0514221*(((((1+(-0.191206*x))+(-0.081866*pow(x,2)))+(0.0396561*(pow(x,3))))+(-0.00606394*(pow(x,4))))+(0.000326768*(pow(x,5))));
       piBCHI2normvar[ivar++] = (S3totx-mean)/sigma;

       mean = 0.894616*(((((1+(-0.0830502*x))+(0.0602628*pow(x,2)))+(-0.0178786*(pow(x,3))))+(0.00239022*(pow(x,4))))+(-0.000118785*(pow(x,5))));
       sigma = 0.074716*(((((1+(-0.194879*x))+(-0.118629*pow(x,2)))+(0.0562894*(pow(x,3))))+(-0.00869822*(pow(x,4))))+(0.000470664*(pow(x,5))));
       piBCHI2normvar[ivar++] = (S3toty-mean)/sigma;

       mean = 17.7832*(((((1+(4.11748*x))+(-1.32613*pow(x,2)))+(0.378096*(pow(x,3))))+(-0.0420122*(pow(x,4))))+(0.00192212*(pow(x,5))));
       sigma = -7.45314*(((((1+(-4.362*x))+(3.04245*pow(x,2)))+(-0.998655*(pow(x,3))))+(0.144479*(pow(x,4))))+(-0.00759756*(pow(x,5))));
       piBCHI2normvar[ivar++] = (NEcalHits-mean)/sigma;

       mean = 2.08418*(((((1+(1.53685*x))+(-0.960584*pow(x,2)))+(0.28049*(pow(x,3))))+(-0.037519*(pow(x,4))))+(0.00186672*(pow(x,5))));
       sigma = 1.08555*(((((1+(0.301398*x))+(-0.486968*pow(x,2)))+(0.167722*(pow(x,3))))+(-0.0236229*(pow(x,4))))+(0.00120627*(pow(x,5))));
       piBCHI2normvar[ivar++] = (ShowerFootprintX-mean)/sigma;

       mean = 3.06557*(((((1+(1.38162*x))+(-0.972884*pow(x,2)))+(0.288618*(pow(x,3))))+(-0.0381605*(pow(x,4))))+(0.00186877*(pow(x,5))));
       sigma = 1.97309*(((((1+(0.0319007*x))+(-0.296926*pow(x,2)))+(0.108391*(pow(x,3))))+(-0.0151688*(pow(x,4))))+(0.000761558*(pow(x,5))));
       piBCHI2normvar[ivar++] = (ShowerFootprintY-mean)/sigma;

       mean = 0.0154335*(((((1+(-0.0374406*x))+(-0.1801*pow(x,2)))+(0.0575964*(pow(x,3))))+(-0.00701209*(pow(x,4))))+(0.000309315*(pow(x,5))));
       sigma = -0.0116612*(((((1+(-2.62644*x))+(1.4815*pow(x,2)))+(-0.352021*(pow(x,3))))+(0.0384267*(pow(x,4))))+(-0.00159106*(pow(x,5))));
       piBCHI2normvar[ivar++] = (LayerEneFrac[0]-mean)/sigma;

       mean = 0.0633498*(((((1+(-0.114734*x))+(-0.154378*pow(x,2)))+(0.0562252*(pow(x,3))))+(-0.00731868*(pow(x,4))))+(0.000337497*(pow(x,5))));
       sigma = 0.0316538*(((((1+(-0.137904*x))+(-0.15952*pow(x,2)))+(0.0611123*(pow(x,3))))+(-0.00813668*(pow(x,4))))+(0.000378855*(pow(x,5))));
       piBCHI2normvar[ivar++] = (LayerEneFrac[1]-mean)/sigma;

       mean = 0.0981715*(((((1+(0.086794*x))+(-0.25229*pow(x,2)))+(0.076108*(pow(x,3))))+(-0.00930538*(pow(x,4))))+(0.000418156*(pow(x,5))));
       sigma = 0.0584895*(((((1+(-0.518405*x))+(0.147205*pow(x,2)))+(-0.0279128*(pow(x,3))))+(0.00302415*(pow(x,4))))+(-0.000133801*(pow(x,5))));
       piBCHI2normvar[ivar++] = (LayerEneFrac[2]-mean)/sigma;

       mean = 0.0913995*(((((1+(0.606592*x))+(-0.534856*pow(x,2)))+(0.142581*(pow(x,3))))+(-0.0169281*(pow(x,4))))+(0.000761466*(pow(x,5))));
       sigma = 0.0521775*(((((1+(-0.370914*x))+(0.102182*pow(x,2)))+(-0.0221373*(pow(x,3))))+(0.00253141*(pow(x,4))))+(-0.000108831*(pow(x,5))));
       piBCHI2normvar[ivar++] = (LayerEneFrac[3]-mean)/sigma;

       mean = 0.0900602*(((((1+(0.623416*x))+(-0.425984*pow(x,2)))+(0.097747*(pow(x,3))))+(-0.010668*(pow(x,4))))+(0.000463787*(pow(x,5))));
       sigma = 0.0505887*(((((1+(-0.486493*x))+(0.195421*pow(x,2)))+(-0.0430779*(pow(x,3))))+(0.00438646*(pow(x,4))))+(-0.000166038*(pow(x,5))));
       piBCHI2normvar[ivar++] = (LayerEneFrac[4]-mean)/sigma;

       mean = 0.0820018*(((((1+(0.588403*x))+(-0.295374*pow(x,2)))+(0.0567273*(pow(x,3))))+(-0.00600898*(pow(x,4))))+(0.000280536*(pow(x,5))));
       sigma = 0.0542134*(((((1+(-0.524149*x))+(0.136043*pow(x,2)))+(-0.00183907*(pow(x,3))))+(-0.00348656*(pow(x,4))))+(0.000309278*(pow(x,5))));
       piBCHI2normvar[ivar++] = (LayerEneFrac[5]-mean)/sigma;

       mean = 0.0647088*(((((1+(0.755927*x))+(-0.322877*pow(x,2)))+(0.0691826*(pow(x,3))))+(-0.00892412*(pow(x,4))))+(0.000476596*(pow(x,5))));
       sigma = 0.0460868*(((((1+(-0.426239*x))+(0.0999163*pow(x,2)))+(-0.0125167*(pow(x,3))))+(0.00141731*(pow(x,4))))+(-0.000105953*(pow(x,5))));
       piBCHI2normvar[ivar++] = (LayerEneFrac[6]-mean)/sigma;

       mean = 0.0591098*(((((1+(0.490957*x))+(-0.117617*pow(x,2)))+(0.0186932*(pow(x,3))))+(-0.00285173*(pow(x,4))))+(0.000169852*(pow(x,5))));
       sigma = 0.0441187*(((((1+(-0.424288*x))+(0.120773*pow(x,2)))+(-0.0303416*(pow(x,3))))+(0.00522312*(pow(x,4))))+(-0.000345832*(pow(x,5))));
       piBCHI2normvar[ivar++] = (LayerEneFrac[7]-mean)/sigma;

       mean = 0.0544055*(((((1+(0.185537*x))+(0.115868*pow(x,2)))+(-0.0463891*(pow(x,3))))+(0.00617888*(pow(x,4))))+(-0.000325102*(pow(x,5))));
       sigma = 0.0358471*(((((1+(-0.404967*x))+(0.161259*pow(x,2)))+(-0.0523526*(pow(x,3))))+(0.00851722*(pow(x,4))))+(-0.000492585*(pow(x,5))));
       piBCHI2normvar[ivar++] = (LayerEneFrac[8]-mean)/sigma;

       mean = 0.0586046*(((((1+(-0.35631*x))+(0.418475*pow(x,2)))+(-0.123614*(pow(x,3))))+(0.0161746*(pow(x,4))))+(-0.000828849*(pow(x,5))));
       sigma = 0.0195272*(((((1+(0.15923*x))+(-0.127709*pow(x,2)))+(0.0134291*(pow(x,3))))+(0.000637799*(pow(x,4))))+(-9.40079e-05*(pow(x,5))));
       piBCHI2normvar[ivar++] = (LayerEneFrac[9]-mean)/sigma;

       mean = 0.0384656*(((((1+(-0.106815*x))+(0.243958*pow(x,2)))+(-0.0433174*(pow(x,3))))+(0.00189987*(pow(x,4))))+(4.54321e-05*(pow(x,5))));
       sigma = 0.0193138*(((((1+(0.109858*x))+(-0.222064*pow(x,2)))+(0.0902414*(pow(x,3))))+(-0.0161111*(pow(x,4))))+(0.00103071*(pow(x,5))));
       piBCHI2normvar[ivar++] = (LayerEneFrac[10]-mean)/sigma;

       mean = 0.0288085*(((((1+(-0.203208*x))+(0.392827*pow(x,2)))+(-0.0992274*(pow(x,3))))+(0.0124898*(pow(x,4))))+(-0.000668279*(pow(x,5))));
       sigma = 0.0276025*(((((1+(-0.717605*x))+(0.363821*pow(x,2)))+(-0.0791709*(pow(x,3))))+(0.0067021*(pow(x,4))))+(-0.000146176*(pow(x,5))));
       piBCHI2normvar[ivar++] = (LayerEneFrac[11]-mean)/sigma;

       mean = 0.0113727*(((((1+(1.25241*x))+(-0.32621*pow(x,2)))+(0.160578*(pow(x,3))))+(-0.0269675*(pow(x,4))))+(0.00150497*(pow(x,5))));
       sigma = -0.00706408*(((((1+(-4.56671*x))+(2.99638*pow(x,2)))+(-0.934686*(pow(x,3))))+(0.136241*(pow(x,4))))+(-0.00736457*(pow(x,5))));
       piBCHI2normvar[ivar++] = (LayerEneFrac[12]-mean)/sigma;

       mean = -0.0014262*(((((1+(-18.5479*x))+(8.50689*pow(x,2)))+(-2.91282*(pow(x,3))))+(0.428928*(pow(x,4))))+(-0.0233242*(pow(x,5))));
       sigma = 0.000632082*(((((1+(29.7718*x))+(-20.4101*pow(x,2)))+(6.333*(pow(x,3))))+(-0.886025*(pow(x,4))))+(0.0453158*(pow(x,5))));
       piBCHI2normvar[ivar++] = (LayerEneFrac[13]-mean)/sigma;

       mean = 0.00206941*(((((1+(6.37561*x))+(-2.65265*pow(x,2)))+(1.07375*(pow(x,3))))+(-0.171301*(pow(x,4))))+(0.0102091*(pow(x,5))));
       sigma = -0.0135454*(((((1+(-2.7593*x))+(1.80845*pow(x,2)))+(-0.554322*(pow(x,3))))+(0.0783147*(pow(x,4))))+(-0.0041224*(pow(x,5))));
       piBCHI2normvar[ivar++] = (LayerEneFrac[14]-mean)/sigma;

       mean = -0.00391276*(((((1+(-4.63399*x))+(2.29847*pow(x,2)))+(-0.803834*(pow(x,3))))+(0.122204*(pow(x,4))))+(-0.00712181*(pow(x,5))));
       sigma = -0.0100079*(((((1+(-2.74935*x))+(1.60623*pow(x,2)))+(-0.42156*(pow(x,3))))+(0.0491247*(pow(x,4))))+(-0.002086*(pow(x,5))));
       piBCHI2normvar[ivar++] = (LayerEneFrac[15]-mean)/sigma;

       mean = 0.0118102*(((((1+(-0.976311*x))+(0.686435*pow(x,2)))+(-0.133816*(pow(x,3))))+(0.0104236*(pow(x,4))))+(-6.07892e-05*(pow(x,5))));
       sigma = -0.0132099*(((((1+(-2.47328*x))+(1.61924*pow(x,2)))+(-0.501154*(pow(x,3))))+(0.0719867*(pow(x,4))))+(-0.0038697*(pow(x,5))));
       piBCHI2normvar[ivar++] = (LayerEneFrac[16]-mean)/sigma;

       mean = 0.018997*(((((1+(-1.46883*x))+(0.958373*pow(x,2)))+(-0.244195*(pow(x,3))))+(0.0286415*(pow(x,4))))+(-0.00117624*(pow(x,5))));
       sigma = -0.00102647*(((((1+(-4.50726*x))+(0.00253443*pow(x,2)))+(0.603813*(pow(x,3))))+(-0.144358*(pow(x,4))))+(0.00928059*(pow(x,5))));
       piBCHI2normvar[ivar++] = (LayerEneFrac[17]-mean)/sigma;

       mean = 0.;
       sigma = 1.;
       piBCHI2normvar[ivar++] = (LayerChi2[1]-mean)/sigma;

       mean = 0.;
       sigma = 1.;
       piBCHI2normvar[ivar++] = (LayerChi2[2]-mean)/sigma;

       mean = 0.;
       sigma = 1.;
       piBCHI2normvar[ivar++] = (LayerChi2[3]-mean)/sigma;

       mean = 0.;
       sigma = 1.;
       piBCHI2normvar[ivar++] = (LayerChi2[4]-mean)/sigma;

       mean = 0.;
       sigma = 1.;
       piBCHI2normvar[ivar++] = (LayerChi2[5]-mean)/sigma;

       mean = 0.;
       sigma = 1.;
       piBCHI2normvar[ivar++] = (LayerChi2[6]-mean)/sigma;

       mean = 0.;
       sigma = 1.;
       piBCHI2normvar[ivar++] = (LayerChi2[7]-mean)/sigma;

       mean = 0.;
       sigma = 1.;
       piBCHI2normvar[ivar++] = (LayerChi2[8]-mean)/sigma;

       mean = 0.;
       sigma = 1.;
       piBCHI2normvar[ivar++] = (LayerChi2[9]-mean)/sigma;

       mean = 0.;
       sigma = 1.;
       piBCHI2normvar[ivar++] = (LayerChi2[10]-mean)/sigma;

       mean = 0.;
       sigma = 1.;
       piBCHI2normvar[ivar++] = (LayerChi2[11]-mean)/sigma;

       mean = 0.;
       sigma = 1.;
       piBCHI2normvar[ivar++] = (LayerChi2[12]-mean)/sigma;

       mean = 0.;
       sigma = 1.;
       piBCHI2normvar[ivar++] = (LayerChi2[13]-mean)/sigma;

       mean = 0.;
       sigma = 1.;
       piBCHI2normvar[ivar++] = (LayerChi2[14]-mean)/sigma;

       mean = 0.;
       sigma = 1.;
       piBCHI2normvar[ivar++] = (LayerChi2[15]-mean)/sigma;

       mean = 0.;
       sigma = 1.;
       piBCHI2normvar[ivar++] = (LayerChi2[16]-mean)/sigma;

       mean = 0.;
       sigma = 1.;
       piBCHI2normvar[ivar++] = (LayerChi2[17]-mean)/sigma;

       piBCHI2normvar[ivar++] = energyd;

     }
   else if ( iBDTCHI2VERSION == 2 )
     {
       //********************************
       //*****Normalize the Variables****
       //********************************
       // fix: normalization may be NaN below 0.5 GeV or above 1000 GeV;
       energyd = energyd < 0.5 ? 0.5 : (energyd > 1000. ? 1000. : energyd);
       float mean, sigma;
       float x = log(energyd);

       if (BCHI2_DEBUG) std::cout << Form(" ??? x=%f\n", x) << flush;

       ivar = 0;

       mean = 5.81753*(((((1+(-0.0652423*x))+(0.123152*pow(x,2)))+(-0.0309516*(pow(x,3))))+(0.00355938*(pow(x,4))))+(-0.000154605*(pow(x,5))));
       sigma = 0.654693*(((((1+(0.349885*x))+(-0.258381*pow(x,2)))+(0.0762609*(pow(x,3))))+(-0.0101505*(pow(x,4))))+(0.000480682*(pow(x,5))));
       piBCHI2normvar[ivar++] = (ShowerMean-mean)/sigma;

       mean = 3.19961*(((((1+(0.122833*x))+(-0.0515297*pow(x,2)))+(0.0142255*(pow(x,3))))+(-0.00216798*(pow(x,4))))+(0.000126187*(pow(x,5))));
       sigma = 0.550582*(((((1+(-0.436487*x))+(0.0995998*pow(x,2)))+(-0.00967967*(pow(x,3))))+(-1.31632e-05*(pow(x,4))))+(4.37799e-05*(pow(x,5))));
       piBCHI2normvar[ivar++] = (ShowerSigma-mean)/sigma;

       mean = -0.00563546*(((((1+(-4.90888*x))+(3.06143*pow(x,2)))+(-1.19568*(pow(x,3))))+(0.200186*(pow(x,4))))+(-0.0124911*(pow(x,5))));
       sigma = 0.0202379*(((((1+(-0.700595*x))+(0.315499*pow(x,2)))+(-0.0637316*(pow(x,3))))+(0.00687652*(pow(x,4))))+(-0.000297002*(pow(x,5))));
       piBCHI2normvar[ivar++] = (L2LFrac-mean)/sigma;

       mean = 0.976096*(((((1+(-0.00594019*x))+(0.00626969*pow(x,2)))+(-0.00277484*(pow(x,3))))+(0.000469374*(pow(x,4))))+(-2.69748e-05*(pow(x,5))));
       sigma = 0.0202693*(((((1+(-0.450749*x))+(0.156631*pow(x,2)))+(-0.0379591*(pow(x,3))))+(0.00471605*(pow(x,4))))+(-0.000217455*(pow(x,5))));
       piBCHI2normvar[ivar++] = (R3cmFrac-mean)/sigma;

       mean = 0.947438*(((((1+(-0.0958179*x))+(0.0542999*pow(x,2)))+(-0.0149508*(pow(x,3))))+(0.00197748*(pow(x,4))))+(-9.96043e-05*(pow(x,5))));
       sigma = 0.0514221*(((((1+(-0.191206*x))+(-0.081866*pow(x,2)))+(0.0396561*(pow(x,3))))+(-0.00606394*(pow(x,4))))+(0.000326768*(pow(x,5))));
       piBCHI2normvar[ivar++] = (S3totx-mean)/sigma;

       mean = 0.894616*(((((1+(-0.0830502*x))+(0.0602628*pow(x,2)))+(-0.0178786*(pow(x,3))))+(0.00239022*(pow(x,4))))+(-0.000118785*(pow(x,5))));
       sigma = 0.074716*(((((1+(-0.194879*x))+(-0.118629*pow(x,2)))+(0.0562894*(pow(x,3))))+(-0.00869822*(pow(x,4))))+(0.000470664*(pow(x,5))));
       piBCHI2normvar[ivar++] = (S3toty-mean)/sigma;

       mean = 17.7832*(((((1+(4.11748*x))+(-1.32613*pow(x,2)))+(0.378096*(pow(x,3))))+(-0.0420122*(pow(x,4))))+(0.00192212*(pow(x,5))));
       sigma = -7.45314*(((((1+(-4.362*x))+(3.04245*pow(x,2)))+(-0.998655*(pow(x,3))))+(0.144479*(pow(x,4))))+(-0.00759756*(pow(x,5))));
       piBCHI2normvar[ivar++] = (NEcalHits-mean)/sigma;

       mean = 2.08418*(((((1+(1.53685*x))+(-0.960584*pow(x,2)))+(0.28049*(pow(x,3))))+(-0.037519*(pow(x,4))))+(0.00186672*(pow(x,5))));
       sigma = 1.08555*(((((1+(0.301398*x))+(-0.486968*pow(x,2)))+(0.167722*(pow(x,3))))+(-0.0236229*(pow(x,4))))+(0.00120627*(pow(x,5))));
       piBCHI2normvar[ivar++] = (ShowerFootprintX-mean)/sigma;

       mean = 3.06557*(((((1+(1.38162*x))+(-0.972884*pow(x,2)))+(0.288618*(pow(x,3))))+(-0.0381605*(pow(x,4))))+(0.00186877*(pow(x,5))));
       sigma = 1.97309*(((((1+(0.0319007*x))+(-0.296926*pow(x,2)))+(0.108391*(pow(x,3))))+(-0.0151688*(pow(x,4))))+(0.000761558*(pow(x,5))));
       piBCHI2normvar[ivar++] = (ShowerFootprintY-mean)/sigma;

       mean = 0.0154335*(((((1+(-0.0374406*x))+(-0.1801*pow(x,2)))+(0.0575964*(pow(x,3))))+(-0.00701209*(pow(x,4))))+(0.000309315*(pow(x,5))));
       sigma = -0.0116612*(((((1+(-2.62644*x))+(1.4815*pow(x,2)))+(-0.352021*(pow(x,3))))+(0.0384267*(pow(x,4))))+(-0.00159106*(pow(x,5))));
       piBCHI2normvar[ivar++] = (LayerEneFrac[0]-mean)/sigma;

       mean = 0.0633498*(((((1+(-0.114734*x))+(-0.154378*pow(x,2)))+(0.0562252*(pow(x,3))))+(-0.00731868*(pow(x,4))))+(0.000337497*(pow(x,5))));
       sigma = 0.0316538*(((((1+(-0.137904*x))+(-0.15952*pow(x,2)))+(0.0611123*(pow(x,3))))+(-0.00813668*(pow(x,4))))+(0.000378855*(pow(x,5))));
       piBCHI2normvar[ivar++] = (LayerEneFrac[1]-mean)/sigma;

       mean = 0.0981715*(((((1+(0.086794*x))+(-0.25229*pow(x,2)))+(0.076108*(pow(x,3))))+(-0.00930538*(pow(x,4))))+(0.000418156*(pow(x,5))));
       sigma = 0.0584895*(((((1+(-0.518405*x))+(0.147205*pow(x,2)))+(-0.0279128*(pow(x,3))))+(0.00302415*(pow(x,4))))+(-0.000133801*(pow(x,5))));
       piBCHI2normvar[ivar++] = (LayerEneFrac[2]-mean)/sigma;

       mean = 0.0913995*(((((1+(0.606592*x))+(-0.534856*pow(x,2)))+(0.142581*(pow(x,3))))+(-0.0169281*(pow(x,4))))+(0.000761466*(pow(x,5))));
       sigma = 0.0521775*(((((1+(-0.370914*x))+(0.102182*pow(x,2)))+(-0.0221373*(pow(x,3))))+(0.00253141*(pow(x,4))))+(-0.000108831*(pow(x,5))));
       piBCHI2normvar[ivar++] = (LayerEneFrac[3]-mean)/sigma;

       mean = 0.0900602*(((((1+(0.623416*x))+(-0.425984*pow(x,2)))+(0.097747*(pow(x,3))))+(-0.010668*(pow(x,4))))+(0.000463787*(pow(x,5))));
       sigma = 0.0505887*(((((1+(-0.486493*x))+(0.195421*pow(x,2)))+(-0.0430779*(pow(x,3))))+(0.00438646*(pow(x,4))))+(-0.000166038*(pow(x,5))));
       piBCHI2normvar[ivar++] = (LayerEneFrac[4]-mean)/sigma;

       mean = 0.0820018*(((((1+(0.588403*x))+(-0.295374*pow(x,2)))+(0.0567273*(pow(x,3))))+(-0.00600898*(pow(x,4))))+(0.000280536*(pow(x,5))));
       sigma = 0.0542134*(((((1+(-0.524149*x))+(0.136043*pow(x,2)))+(-0.00183907*(pow(x,3))))+(-0.00348656*(pow(x,4))))+(0.000309278*(pow(x,5))));
       piBCHI2normvar[ivar++] = (LayerEneFrac[5]-mean)/sigma;

       mean = 0.0647088*(((((1+(0.755927*x))+(-0.322877*pow(x,2)))+(0.0691826*(pow(x,3))))+(-0.00892412*(pow(x,4))))+(0.000476596*(pow(x,5))));
       sigma = 0.0460868*(((((1+(-0.426239*x))+(0.0999163*pow(x,2)))+(-0.0125167*(pow(x,3))))+(0.00141731*(pow(x,4))))+(-0.000105953*(pow(x,5))));
       piBCHI2normvar[ivar++] = (LayerEneFrac[6]-mean)/sigma;

       mean = 0.0591098*(((((1+(0.490957*x))+(-0.117617*pow(x,2)))+(0.0186932*(pow(x,3))))+(-0.00285173*(pow(x,4))))+(0.000169852*(pow(x,5))));
       sigma = 0.0441187*(((((1+(-0.424288*x))+(0.120773*pow(x,2)))+(-0.0303416*(pow(x,3))))+(0.00522312*(pow(x,4))))+(-0.000345832*(pow(x,5))));
       piBCHI2normvar[ivar++] = (LayerEneFrac[7]-mean)/sigma;

       mean = 0.0544055*(((((1+(0.185537*x))+(0.115868*pow(x,2)))+(-0.0463891*(pow(x,3))))+(0.00617888*(pow(x,4))))+(-0.000325102*(pow(x,5))));
       sigma = 0.0358471*(((((1+(-0.404967*x))+(0.161259*pow(x,2)))+(-0.0523526*(pow(x,3))))+(0.00851722*(pow(x,4))))+(-0.000492585*(pow(x,5))));
       piBCHI2normvar[ivar++] = (LayerEneFrac[8]-mean)/sigma;

       mean = 0.0586046*(((((1+(-0.35631*x))+(0.418475*pow(x,2)))+(-0.123614*(pow(x,3))))+(0.0161746*(pow(x,4))))+(-0.000828849*(pow(x,5))));
       sigma = 0.0195272*(((((1+(0.15923*x))+(-0.127709*pow(x,2)))+(0.0134291*(pow(x,3))))+(0.000637799*(pow(x,4))))+(-9.40079e-05*(pow(x,5))));
       piBCHI2normvar[ivar++] = (LayerEneFrac[9]-mean)/sigma;

       mean = 0.0384656*(((((1+(-0.106815*x))+(0.243958*pow(x,2)))+(-0.0433174*(pow(x,3))))+(0.00189987*(pow(x,4))))+(4.54321e-05*(pow(x,5))));
       sigma = 0.0193138*(((((1+(0.109858*x))+(-0.222064*pow(x,2)))+(0.0902414*(pow(x,3))))+(-0.0161111*(pow(x,4))))+(0.00103071*(pow(x,5))));
       piBCHI2normvar[ivar++] = (LayerEneFrac[10]-mean)/sigma;

       mean = 0.0288085*(((((1+(-0.203208*x))+(0.392827*pow(x,2)))+(-0.0992274*(pow(x,3))))+(0.0124898*(pow(x,4))))+(-0.000668279*(pow(x,5))));
       sigma = 0.0276025*(((((1+(-0.717605*x))+(0.363821*pow(x,2)))+(-0.0791709*(pow(x,3))))+(0.0067021*(pow(x,4))))+(-0.000146176*(pow(x,5))));
       piBCHI2normvar[ivar++] = (LayerEneFrac[11]-mean)/sigma;

       mean = 0.0113727*(((((1+(1.25241*x))+(-0.32621*pow(x,2)))+(0.160578*(pow(x,3))))+(-0.0269675*(pow(x,4))))+(0.00150497*(pow(x,5))));
       sigma = -0.00706408*(((((1+(-4.56671*x))+(2.99638*pow(x,2)))+(-0.934686*(pow(x,3))))+(0.136241*(pow(x,4))))+(-0.00736457*(pow(x,5))));
       piBCHI2normvar[ivar++] = (LayerEneFrac[12]-mean)/sigma;

       mean = -0.0014262*(((((1+(-18.5479*x))+(8.50689*pow(x,2)))+(-2.91282*(pow(x,3))))+(0.428928*(pow(x,4))))+(-0.0233242*(pow(x,5))));
       sigma = 0.000632082*(((((1+(29.7718*x))+(-20.4101*pow(x,2)))+(6.333*(pow(x,3))))+(-0.886025*(pow(x,4))))+(0.0453158*(pow(x,5))));
       piBCHI2normvar[ivar++] = (LayerEneFrac[13]-mean)/sigma;

       mean = 0.00206941*(((((1+(6.37561*x))+(-2.65265*pow(x,2)))+(1.07375*(pow(x,3))))+(-0.171301*(pow(x,4))))+(0.0102091*(pow(x,5))));
       sigma = -0.0135454*(((((1+(-2.7593*x))+(1.80845*pow(x,2)))+(-0.554322*(pow(x,3))))+(0.0783147*(pow(x,4))))+(-0.0041224*(pow(x,5))));
       piBCHI2normvar[ivar++] = (LayerEneFrac[14]-mean)/sigma;

       mean = -0.00391276*(((((1+(-4.63399*x))+(2.29847*pow(x,2)))+(-0.803834*(pow(x,3))))+(0.122204*(pow(x,4))))+(-0.00712181*(pow(x,5))));
       sigma = -0.0100079*(((((1+(-2.74935*x))+(1.60623*pow(x,2)))+(-0.42156*(pow(x,3))))+(0.0491247*(pow(x,4))))+(-0.002086*(pow(x,5))));
       piBCHI2normvar[ivar++] = (LayerEneFrac[15]-mean)/sigma;

       mean = 0.0118102*(((((1+(-0.976311*x))+(0.686435*pow(x,2)))+(-0.133816*(pow(x,3))))+(0.0104236*(pow(x,4))))+(-6.07892e-05*(pow(x,5))));
       sigma = -0.0132099*(((((1+(-2.47328*x))+(1.61924*pow(x,2)))+(-0.501154*(pow(x,3))))+(0.0719867*(pow(x,4))))+(-0.0038697*(pow(x,5))));
       piBCHI2normvar[ivar++] = (LayerEneFrac[16]-mean)/sigma;

       mean = 0.018997*(((((1+(-1.46883*x))+(0.958373*pow(x,2)))+(-0.244195*(pow(x,3))))+(0.0286415*(pow(x,4))))+(-0.00117624*(pow(x,5))));
       sigma = -0.00102647*(((((1+(-4.50726*x))+(0.00253443*pow(x,2)))+(0.603813*(pow(x,3))))+(-0.144358*(pow(x,4))))+(0.00928059*(pow(x,5))));
       piBCHI2normvar[ivar++] = (LayerEneFrac[17]-mean)/sigma;

       mean = 0.;
       sigma = 1.;
       piBCHI2normvar[ivar++] = (LayerChi2[1]-mean)/sigma;

       mean = 0.;
       sigma = 1.;
       piBCHI2normvar[ivar++] = (LayerChi2[2]-mean)/sigma;

       mean = 0.;
       sigma = 1.;
       piBCHI2normvar[ivar++] = (LayerChi2[3]-mean)/sigma;

       mean = 0.;
       sigma = 1.;
       piBCHI2normvar[ivar++] = (LayerChi2[4]-mean)/sigma;

       mean = 0.;
       sigma = 1.;
       piBCHI2normvar[ivar++] = (LayerChi2[5]-mean)/sigma;

       mean = 0.;
       sigma = 1.;
       piBCHI2normvar[ivar++] = (LayerChi2[6]-mean)/sigma;

       mean = 0.;
       sigma = 1.;
       piBCHI2normvar[ivar++] = (LayerChi2[7]-mean)/sigma;

       mean = 0.;
       sigma = 1.;
       piBCHI2normvar[ivar++] = (LayerChi2[8]-mean)/sigma;

       mean = 0.;
       sigma = 1.;
       piBCHI2normvar[ivar++] = (LayerChi2[9]-mean)/sigma;

       mean = 0.;
       sigma = 1.;
       piBCHI2normvar[ivar++] = (LayerChi2[10]-mean)/sigma;

       mean = 0.;
       sigma = 1.;
       piBCHI2normvar[ivar++] = (LayerChi2[11]-mean)/sigma;

       mean = 0.;
       sigma = 1.;
       piBCHI2normvar[ivar++] = (LayerChi2[12]-mean)/sigma;

       mean = 0.;
       sigma = 1.;
       piBCHI2normvar[ivar++] = (LayerChi2[13]-mean)/sigma;

       mean = 0.;
       sigma = 1.;
       piBCHI2normvar[ivar++] = (LayerChi2[14]-mean)/sigma;

       mean = 0.;
       sigma = 1.;
       piBCHI2normvar[ivar++] = (LayerChi2[15]-mean)/sigma;

       mean = 0.;
       sigma = 1.;
       piBCHI2normvar[ivar++] = (LayerChi2[16]-mean)/sigma;

       mean = 0.;
       sigma = 1.;
       piBCHI2normvar[ivar++] = (LayerChi2[17]-mean)/sigma;

       piBCHI2normvar[ivar++] = energyd;

     }
   else
     {
       //if here then ecalBDTCHI2 called with wrong option; set BDT=-999
       return -999;
     }

   for (ivar = 0; ivar < nPIBCHI2VARs; ++ivar)
     {
      if (piBCHI2normvar[ivar] != piBCHI2normvar[ivar])
      {
         if (piBCHI2inputvar[ivar] != piBCHI2inputvar[ivar])
         { //the input variable is a NaN.
            std::cerr << Form(" !!! GetEcalBDTCHI2(pev) -> piBCHI2inputvar[%.2d] (%s) NaN. Input variable is a NaN. Returning -9999\n", ivar, piBCHI2vaname[ivar]) << endl << flush;
            return -9999.;
         }
         else
         { //only the output variable is a Nan. There should be a problem in the normalization
            std::cerr << Form(" !!! GetEcalBDTCHI2(pev) -> piBCHI2normvar[%.2d] (%sNorm) NaN. Normalized variable is a NaN (Before normalization = %f). Returning -9999\n", ivar, piBCHI2vaname[ivar], piBCHI2inputvar[ivar]) << endl << flush;
            return -9999.;
         }
      }
   }

   if (BCHI2_DEBUG)
   {
      for (ivar = 0; ivar < nPIBCHI2VARs + 1; ++ivar)
         std::cout << Form(" ??? ivar:%02d \t %-20s \t input:%f \t norm:%f\n", ivar, piBCHI2vaname[ivar], piBCHI2inputvar[ivar], piBCHI2normvar[ivar]);
      std::cout << flush;
   }

   //********************************
   //*****  Calculate BDT    ********
   //********************************
   float bdt = ecalBDTCHI2reader->EvaluateMVA("BDTG_LAYERS");

   return bdt;
}
