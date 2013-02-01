#include "ACQtProducer-include.C"
 
bool ACsoft::ACQtProducer::ProduceECAL() {

  Q_ASSERT(fEvent);
  Q_ASSERT(fAMSEvent);

  int nShowers = max(fMaxECALShowers + 1, min(5, fAMSEvent->nEcalShower()));
  for (int showerIndex = 0; showerIndex < nShowers; ++showerIndex) {
    EcalShowerR* pEcalShower = fAMSEvent->pEcalShower(showerIndex);
    if (!pEcalShower) {
      WARN_OUT << "Problem building AC::ECALShower[" << showerIndex << "]: "
               << "AMS ROOT File is broken! pEcalShower() returned null." << std::endl;
      return false;
    }

    AC::ECALShower ecalShower;
    ecalShower.fStatus = pEcalShower->Status;

    if (pEcalShower->Nhits > 0xffff)
      WARN_OUT << "Problem building AC::ECALShower[" << showerIndex << "]: "
               << "Nhits: " << pEcalShower->Nhits << " way too large." << std::endl;

    ecalShower.fNumberOfHits = min(0xffff,pEcalShower->Nhits); 
    ecalShower.fDepositedEnergy = pEcalShower->EnergyD;
    ecalShower.fEnergyAt3CMRatio = pEcalShower->Energy3C[0];
    ecalShower.fReconstructedEnergy = pEcalShower->EnergyE;
    ecalShower.fReconstructedEnergyError = pEcalShower->ErEnergyE;
    ecalShower.fRelativeRearLeak = pEcalShower->RearLeak;
    ecalShower.fShowerMaximum = pEcalShower->ParProfile[1];
    ecalShower.fChiSquareProfile = pEcalShower->Chi2Profile;

    AMSDir direction;
    if ((pEcalShower->Entry[2] - pEcalShower->Exit[2]) < 0.0)
      direction.setd(pEcalShower->EMDir[0], pEcalShower->EMDir[1], pEcalShower->EMDir[2]);
    else
      direction.setd(-pEcalShower->EMDir[0], -pEcalShower->EMDir[1], -pEcalShower->EMDir[2]);

    ecalShower.fTheta = direction.gettheta();
    ecalShower.fPhi = direction.getphi();
    ecalShower.fX = pEcalShower->CofG[0];
    ecalShower.fY = pEcalShower->CofG[1];
    ecalShower.fZ = pEcalShower->CofG[2];
    ecalShower.fEntryX = pEcalShower->Entry[0];
    ecalShower.fEntryY = pEcalShower->Entry[1];
    ecalShower.fEntryZ = pEcalShower->Entry[2];
    ecalShower.fExitX = pEcalShower->Exit[0];
    ecalShower.fExitY = pEcalShower->Exit[1];
    ecalShower.fExitZ = pEcalShower->Exit[2];

    // GetEcalBDT() does NOT check if the data structures are valid. Do null pointer checks here before calling GetEcalBDT() to make sure it won't crash on broken files.
    bool canComputeBDT = true;
    int n2DCLUSTERs = pEcalShower->NEcal2DCluster();
    for (int i2dcluster = 0; i2dcluster < n2DCLUSTERs; ++i2dcluster) {
      Ecal2DClusterR* cluster2d = pEcalShower->pEcal2DCluster(i2dcluster);
      if (!cluster2d) {
        canComputeBDT = false;
        break;
      }

      int nCLUSTERs = cluster2d->NEcalCluster();
      for (int icluster = 0; icluster < nCLUSTERs; ++icluster) {
        EcalClusterR* cluster = cluster2d->pEcalCluster(icluster);
        if (!cluster) {
          canComputeBDT = false;
          break;
        }

        int nclHITs = cluster->NEcalHit();
        for (int ihit = 0; ihit < nclHITs; ++ihit) {
          EcalHitR* hit = cluster->pEcalHit(ihit);
          if (!hit) {
            canComputeBDT = false;
            break;
          }
        }

        if (!canComputeBDT)
          break;
      }

      if (!canComputeBDT)
        break;
    }

    if (!canComputeBDT) {
      WARN_OUT << "Problem building AC::ECALShower[" << showerIndex << "]: "
               << "AMS ROOT File is broken! Can't compute BDT." << std::endl;
      return false;
    }

    ++nECAL12;

    timer_ebdt->Start(kFALSE);
    ecalShower.fEstimators.append(pEcalShower->GetEcalBDT());
    timer_ebdt->Stop();

    timer_ese2->Start(kFALSE);
    ecalShower.fEstimators.append(pEcalShower->EcalStandaloneEstimatorV2()); // ok with icc64
    timer_ese2->Stop();

    fEvent->fECAL.fShowers.append(ecalShower);
  }

  return true;
}
