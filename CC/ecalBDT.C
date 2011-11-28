#include "root.h"
#ifdef _ECALBDT_
#include "ecalBDT.h"
#endif
#include "TMath.h"

float EcalShowerR::GetEcalBDT()
{
#ifdef _ECALBDT_
   const int nECAL_VIEWs  = 2;
   const int nLAYERs = 18;
   const int nCELLs  = 72;

   float EneDep = 0.; // Total energy deposited [GeV] (sum of every cell of every layer)

   float F2LEneDep = 0.; // Energy deposit [GeV] in the first 2 layer

   float NEcalHits = Nhits;

   float L2LFrac = 0.; // Energy fraction of last 2 layer wrt energy deposit
   float F2LFrac = 0.; // Energy fraction of first 2 layer wrt energy deposit

   float ShowerFootprintX = ShowerFootprint[1];
   float ShowerFootprintY = ShowerFootprint[2];

   float S1S3x  = S13Rpi[0]; // Energy fraction in the cells near the maximum deposit cell on x wrt maximum deposit
   float S1S3y  = S13Rpi[1]; // Energy fraction in the cells near the maximum deposit cell on y wrt maximum deposit
   float S3totx = S3tot[1];  // Energy fraction of the 2 cells near the maximum deposit cell on x wrt to total energy deposit
   float S3toty = S3tot[2];  // Energy fraction of the 2 cells near the maximum deposit cell on x wrt to total energy deposit
   float S5totx = S5tot[1];  // Energy fraction of the 4 cells near the maximum deposit cell on x wrt to total energy deposit
   float S5toty = S5tot[2];  // Energy fraction of the 4 cells near the maximum deposit cell on x wrt to total energy deposit

   float R3cmFrac = Energy3C[0]; // Energy fraction in a circle +- 3 cm around maximum wrt to total energy
   float R5cmFrac = Energy3C[1]; // Energy fraction in a circle +- 5 cm around maximum wrt to total energy

   float MapEneDep[nLAYERs][nCELLs];       // Energy deposit in every cell [MeV]
   float LayerEneDep[nLAYERs];             // Energy deposit [GeV] in each layer (sum of every cell of each layer)
   float LayerClusterEnergy[nLAYERs];      // Corrected energy deposit [GeV] in each layer (sum of clusters' energy for each layer)
   float LayerTruncClusterEnergy[nLAYERs]; // Corrected energy [GeV] for each layer between 3 sigma from the maximum

   float ShowerMean  = 0.; // Longitudinal mean [layer]: (sum_i i*LayerClusterEnergy[i])/(sum_i LayerClusterEnergy[i])
   float ShowerSigma = 0.; // Longitudinal sigma [layer]: TMath::Sqrt((sum_i (i-ShowerMean)^2*LayerClusterEnergy[i])/(sum_i LayerClusterEnergy[i]))
   float ShowerSkewness = 0.; // Longitudinal skewness [layer^3]: (sum_i (i-ShowerMean)^3*LayerClusterEnergy[i])/(sum_i LayerClusterEnergy[i])
   float ShowerKurtosis = 0.; // Longitudinal kurtosis [layer^4]: (sum_i (i-ShowerMean)^4*LayerClusterEnergy[i])/(sum_i LayerClusterEnergy[i])

   float LayerEnergy = 0.; // sum_i LayerClusterEnergy[i]

   float LayerMean[nLAYERs];       // Mean [cell] for each layer: (sum_j j*MapEneDep[i][j])/(sum_ij MapEneDep[i][j])
   float LayerSigma[nLAYERs];      // Sigma [cell] for each layer: TMath::Sqrt((sum_j (j-LayerMean[i])^2*MapEneDep[i][j])/(sum_ij MapEneDep[i][j]))
   float LayerTruncSigma[nLAYERs]; // Sigma [cell] for each layer, between 3 sigma from the maximum

   for (Int_t ilayer = 0; ilayer < nLAYERs; ++ilayer)
   {
      LayerClusterEnergy[ilayer] = 0.;
      for (Int_t icell = 0; icell < nCELLs; ++icell) MapEneDep[ilayer][icell] = 0.;
   }

   Int_t n2DCLUSTERs = NEcal2DCluster();
   for (Int_t i2dcluster = 0; i2dcluster < n2DCLUSTERs; ++i2dcluster)
   {
      Ecal2DClusterR *cluster2d = pEcal2DCluster(i2dcluster);

      Int_t nCLUSTERs = cluster2d->NEcalCluster();
      for (Int_t icluster = 0; icluster < nCLUSTERs; ++icluster)
      {
         EcalClusterR *cluster = cluster2d->pEcalCluster(icluster);

         LayerClusterEnergy[cluster->Plane] += cluster->Edep;

         Int_t nclHITs = cluster->NEcalHit();
         for (Int_t ihit = 0; ihit < nclHITs; ++ihit)
         {
            EcalHitR *hit = cluster->pEcalHit(ihit);
            if (hit->ADC[0] > 4) MapEneDep[hit->Plane][hit->Cell] = hit->Edep;
         }
      }
   }

   for (Int_t ilayer = 0; ilayer < nLAYERs; ++ilayer)
      for (Int_t icell = 0; icell < nCELLs; ++icell) EneDep += MapEneDep[ilayer][icell];

   EneDep /= 1000.;

   if (EneDep <= 0.) return -2.;

   for (Int_t ilayer = 0; ilayer < nLAYERs; ++ilayer)
   {
      LayerEneDep[ilayer] = 0.;
      for (Int_t icell = 0; icell < nCELLs; ++icell)
      {
         MapEneDep[ilayer][icell] /= 1000.;

         LayerEneDep[ilayer] += MapEneDep[ilayer][icell];

         if (ilayer == 16 || ilayer == 17) L2LFrac += MapEneDep[ilayer][icell];
         if (ilayer ==  0 || ilayer ==  1) F2LFrac += MapEneDep[ilayer][icell];
      }

      LayerClusterEnergy[ilayer] /= 1000.;
      LayerEnergy += LayerClusterEnergy[ilayer];
      ShowerMean  += ilayer*LayerClusterEnergy[ilayer];
   }

   if (LayerEnergy <= 0) return -2.;

   F2LEneDep   = F2LFrac;
   L2LFrac     = L2LFrac/EneDep;
   F2LFrac     = F2LFrac/EneDep;
   ShowerMean /= LayerEnergy;

   for (Int_t ilayer = 0; ilayer < nLAYERs; ++ilayer)
   {
      ShowerSigma    += TMath::Power(ilayer-ShowerMean, 2)*LayerClusterEnergy[ilayer];
      ShowerSkewness += TMath::Power(ilayer-ShowerMean, 3)*LayerClusterEnergy[ilayer];
      ShowerKurtosis += TMath::Power(ilayer-ShowerMean, 4)*LayerClusterEnergy[ilayer];
   }
   ShowerSigma    /= LayerEnergy;
   ShowerSigma     = TMath::Sqrt(ShowerSigma);
   ShowerSkewness /= LayerEnergy;
   ShowerKurtosis /= LayerEnergy;

   ReadBDTG* BDTGResponse = new ReadBDTG( );
   std::vector<double> InputVec(17);

   float mean;
   float sigma;
   unsigned int ivar = 0;

   mean = 4.6675+(0.979051*log(EnergyE+1.18325));
   sigma = 0.852135+(-0.0431518*log(EnergyE));
   float ShowerMeanNorm = (ShowerMean - mean)/sigma;

   mean = 3.53852+(0.0296595*log(EnergyE-4.87199));
   sigma = 0.287891+(-0.0394277*log(EnergyE-4.37052));
   float ShowerSigmaNorm = (ShowerSigma - mean)/sigma;

   mean = 50.7688+(-8.72425*log(EnergyE+10.0236));
   sigma = 8.88718+(-0.533865*log(EnergyE-4.9));
   float ShowerSkewnessNorm = (ShowerSkewness - mean)/sigma;

   mean = (523.084+(-239.06/EnergyE))+(-59.4085*atan(0.0467487*EnergyE));
   sigma = 141.4+(-61.9827*atan(0.132307*EnergyE));
   float ShowerKurtosisNorm = (ShowerKurtosis - mean)/sigma;

   mean = 0.00632314+(0.123049*log(EnergyE+0.353085));
   sigma = -0.110641+(0.0866421*log(EnergyE+7.60773));
   float F2LEneDepNorm = (F2LEneDep - mean)/sigma;

   mean = 0.00981856+(0.00470305*sqrt(EnergyE-3.35461));
   sigma = 0.0129705+(0.00166802*sqrt(EnergyE+8.22741));
   float L2LFracNorm = (L2LFrac - mean)/sigma;

   mean = -0.00453365+(0.122221*(pow(EnergyE,(float)-0.513752)));
   sigma = -0.00315033+(0.107401*(pow(EnergyE,(float)-0.543919)));
   float F2LFracNorm = (F2LFrac - mean)/sigma;

   mean = (0.71741+(-0.0251886*atan(EnergyE-3.57118)))+(2.3133e-05*EnergyE);
   sigma = 0.218953+(-0.113094*atan(EnergyE));
   float S1S3xNorm = (S1S3x - mean)/sigma;

   mean = (0.626107+(0.0189872*log(EnergyE)))+(-0.0260725*atan((EnergyE*0.439958)-3.27219));
   sigma = (-0.150083+(0.152807*atan(EnergyE)))+(-0.000646416*EnergyE);
   float S1S3yNorm = (S1S3y - mean)/sigma;

   mean = 0.972864+(-0.00499681*atan((EnergyE*0.0362795)-0.459303));
   sigma = 0.0104377+(-0.00140507*log(EnergyE-3.97738));
   float R3cmFracNorm = (R3cmFrac - mean)/sigma;

   mean = (1.01319+(-0.00461599*log(EnergyE)))+(-0.113443/(EnergyE+4.50707));
   sigma = 0.0043004+(-0.000536274*log(EnergyE-4.9));
   float R5cmFracNorm = (R5cmFrac - mean)/sigma;

   mean = 0.0180086+(-0.0100441*log(EnergyE+9.07422));
   sigma = 0.505472+(-0.311353*atan((0.518772*EnergyE)+3.74849));
   float DifoSumNorm = (DifoSum - mean)/sigma;

   mean = 0.91946+(-0.022189*atan((2.52632*EnergyE)-7.67717));
   sigma = 0.00789299+(0.0720622*(pow(EnergyE,(float)-0.733793)));
   float S3totxNorm = (S3totx - mean)/sigma;

   mean = (-0.0136692*exp(-0.128828*EnergyE))+0.874554;
   sigma = 0.501058+(-0.314568*atan((1.2783*EnergyE)+2.90362));
   float S3totyNorm = (S3toty - mean)/sigma;

   mean = 0.960591+(-0.00355234*log(EnergyE));
   sigma = 0.0217616+(-0.0107094*atan(0.079457*EnergyE));
   float S5totxNorm = (S5totx - mean)/sigma;

   mean = 0.948521+(-0.00193183*log(EnergyE));
   sigma = 0.0227471+(-0.00393239*log(EnergyE-4.78839));
   float S5totyNorm = (S5toty - mean)/sigma;

   mean = (55.4579+(21.667*log(EnergyE)))+(4.34059*(log(EnergyE)*log(EnergyE)));
   sigma = (6.88496+(0.316054*log(EnergyE)))+(0.349772*(log(EnergyE)*log(EnergyE)));
   float NEcalHitsNorm = (NEcalHits - mean)/sigma;

   mean = 3.94369+(0.00837397*(pow(EnergyE,(float)0.888997)));
   sigma = 2.04609+(-1.15052*atan((0.278696*EnergyE)+0.639821));
   float ShowerFootprintXNorm = (ShowerFootprintX - mean)/sigma;

   mean = (5.23776+(-0.210765*log(EnergyE)))+(0.00721687*EnergyE);
   sigma = 1.54466+(-0.845935*atan(EnergyE*0.116966));
   float ShowerFootprintYNorm = (ShowerFootprintY - mean)/sigma;

   // cc bdtg
   InputVec[ivar++] = ShowerMeanNorm;
   InputVec[ivar++] = ShowerSigmaNorm;
   InputVec[ivar++] = ShowerSkewnessNorm;
   InputVec[ivar++] = ShowerKurtosisNorm;
   InputVec[ivar++] = F2LEneDepNorm;
   InputVec[ivar++] = L2LFracNorm;
   InputVec[ivar++] = F2LFracNorm;
   InputVec[ivar++] = R3cmFracNorm;
   InputVec[ivar++] = R5cmFracNorm;
   InputVec[ivar++] = DifoSumNorm;
   InputVec[ivar++] = S3totxNorm;
   InputVec[ivar++] = S3totyNorm;
   InputVec[ivar++] = S5totxNorm;
   InputVec[ivar++] = S5totyNorm;
   InputVec[ivar++] = NEcalHitsNorm;
   InputVec[ivar++] = ShowerFootprintXNorm;
   InputVec[ivar++] = ShowerFootprintYNorm;

   //Get ECALBDTG
   float ECALBDTG = (float) BDTGResponse->GetMvaValue( InputVec );

   delete BDTGResponse;

   return ECALBDTG;
#else
   cout << "*************** WARNING *****************" << endl;
   cout << "FUNCTION GetEcalBDT is dummy in your current executable or  shared library"<< endl;
   cout << "if you want the method to work remove ecalBDT.o and link again your shared library with the variable ECALBDT set to 1" << endl;
   cout << "CAVEAT: the linking of this method only requires about 6 minutes on ams.cern.ch with icc64" << endl;
   return -2.;
#endif
}
