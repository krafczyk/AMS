#include "root.h"
#include "TMVA/Tools.h"

TMVA::Reader *ecalpisareader = NULL;
const unsigned int nPISABDTVARs = 61;
float pisanormvar[nPISABDTVARs + 1];
float pisainputvar[nPISABDTVARs + 1];

const bool DEBUG = false;

float EcalShowerR::GetEcalBDT()
{
   //******************************
   //*****Compute the variables****
   //******************************

   const unsigned int nLAYERs = 18;
   const unsigned int nCELLs  = 72;

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

      LayerMean[ilayer]  = 0.;
      LayerSigma[ilayer] = 0.;

      LayerS1S3[ilayer]   = -1.;
      LayerS3Frac[ilayer] = -1.;

      for (unsigned int icell = 0; icell < nCELLs; ++icell)
         MapEneDep[ilayer][icell] = 0.;
   }

   int n2DCLUSTERs = NEcal2DCluster();
   for (int i2dcluster = 0; i2dcluster < n2DCLUSTERs; ++i2dcluster)
   {
      Ecal2DClusterR *cluster2d = pEcal2DCluster(i2dcluster);

      if (!(cluster2d->Status & 32)) continue;

      int nCLUSTERs = cluster2d->NEcalCluster();
      for (int icluster = 0; icluster < nCLUSTERs; ++icluster)
      {
         EcalClusterR *cluster = cluster2d->pEcalCluster(icluster);

         if (!(cluster->Status & 32)) continue;

         ClusterEnergy[cluster->Plane] += cluster->Edep;

         int nclHITs = cluster->NEcalHit();
         for (int ihit = 0; ihit < nclHITs; ++ihit)
         {
            EcalHitR *hit = cluster->pEcalHit(ihit);

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

   unsigned int ivar;



   //********************************
   //*****Init the Reader************
   //********************************

   if (ecalpisareader == NULL)     //if not already Init.....
   {
      std::cout << "##############################################################" << std::endl;
      std::cout << "                   Create GetEcalBDT Reader   " << std::endl;
      std::cout << "##############################################################" << std::endl;

      //~ TMVA::Tools::Instance();
      ecalpisareader = new TMVA::Reader("Color:!Silent:V:VerbosityLevel=Debug:H");

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

      ecalpisareader->BookMVA("BDTG_LAYERS", Form("%s/v5.00/ECAL_PISA_BDT_407.weights.xml", getenv("AMSDataDir")));
   }



   //*******************************
   //*****Reject without BDT********
   //*******************************

   const UInt_t nENERGYBINs = 14;
   Float_t EnergyBin[nENERGYBINs + 1] = { 2.00, 3.12, 4.86, 7.57, 11.81, 18.41, 28.69,
      44.72, 69.71, 108.66, 169.38, 264.03, 411.56, 641.53, 1000.00 };

   // 90% efficiency cut in energy bins defined above
   Float_t cuts[nENERGYBINs] = { 0.550000, 0.710000, 0.812000, 0.866000, 0.896000, 0.898000,
      0.916000, 0.908000, 0.928000, 0.948000, 0.966000, 0.968000, 0.970000, 0.968000 };
 
   Float_t offset;

   if (energyd < EnergyBin[0]) offset = cuts[0];
   for (unsigned int ibin = 0; ibin < nENERGYBINs; ++ibin)
   {
      if (EnergyBin[ibin] <= energyd && energyd < EnergyBin[ibin + 1])
      {
         offset = cuts[ibin];
         break;
      }
   }
   if (energyd >= EnergyBin[nENERGYBINs]) offset = cuts[nENERGYBINs - 1];

   //Check the energy deposit in the first 2 superlayers
   if (F2SLEneDep < -0.02 + 0.17*log(energyd) + 8.7e-4*pow((float)log(energyd), (float)4.00)) return (-0.9991 - offset);

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

   for (unsigned int ilayer = first_lay; ilayer <= last_lay; ++ilayer)
   {
      if (LayerS1S3[ilayer] < 0 || LayerS1S3[ilayer] > 0.995) return (-0.9993 - offset);
   }



   //********************************
   //*****Normalize the Variables****
   //********************************
   // fix: normalization may be NaN below 2 GeV or above 1000 GeV;
   energyd = energyd < 2. ? 2. : (energyd > 1000. ? 1000. : energyd);
   float mean, sigma;
   float x = log(energyd);

   if (DEBUG) std::cerr << Form(" ??? x=%f\n", x);

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

   pisanormvar[ivar++] = energyd;

   if (DEBUG)
   {
      ivar = 0;

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
      pisainputvar[ivar++] = energyd;
   }

   for (ivar = 0; ivar < nPISABDTVARs; ++ivar)
   {
      if (!(pisanormvar[ivar] == pisanormvar[ivar]))
      {
         if (!(pisainputvar[ivar] == pisainputvar[ivar]))
         { //the input variable is a NaN.
            std::cerr << Form("GetEcalBDT() -> pisainputvar[%.2d] NaN. Input variable is a NaN. Unpredictable BDT output expected\n", ivar) << endl;
         }
         else
         { //only the output variable is a Nan. There should be a problem in the normalization
            std::cerr << Form("GetEcalBDT() -> pisanormvar[%.2d] NaN. Normalized variable is a NaN. Unpredictable BDT output expected\n", ivar) << endl;
         }
      }
   }

   if (DEBUG)
   {
      for (ivar = 0; ivar < nPISABDTVARs + 1; ++ivar)
         std::cerr << Form("ivar:%02d \t input:%f \t norm:%f\n", ivar, pisainputvar[ivar], pisanormvar[ivar]);
   }



   //********************************
   //*****  Calculate BDT    ********
   //********************************
   float bdt = ecalpisareader->EvaluateMVA("BDTG_LAYERS") - offset;

   return bdt;
}
