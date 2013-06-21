#include "ACQtProducer-include.C"

static const int gMaximumTRDHits = 100;

bool ACsoft::ACQtProducer::ProduceMC() {

  Q_ASSERT(fEvent);
  Q_ASSERT(fAMSEvent);

  // Store MC cluster/hits summary.
  fEvent->fMC.fNumberOfEventGenerators = fAMSEvent->nMCEventg();
  fEvent->fMC.fNumberOfTrackerCluster = fAMSEvent->nTrMCCluster();
  fEvent->fMC.fNumberOfTRDCluster = fAMSEvent->nTrdMCCluster();
  fEvent->fMC.fNumberOfTOFCluster = fAMSEvent->nTofMCCluster();
  fEvent->fMC.fNumberOfACCCluster = fAMSEvent->nAntiMCCluster();
  fEvent->fMC.fNumberOfRICHCluster = fAMSEvent->nRichMCCluster();
  fEvent->fMC.fNumberOfECALHits = fAMSEvent->nEcalMCHit();

  float primaryParticleMomentum = -1;
  for (int generatorIndex = 0; generatorIndex < fEvent->fMC.fNumberOfEventGenerators; ++generatorIndex) {
    MCEventgR* pMCEventg = fAMSEvent->pMCEventg(generatorIndex);
    if (!pMCEventg) {
      WARN_OUT << "Problem building AC::MCEventGenerator[" << generatorIndex << "]: "
               << "AMS ROOT File is broken! pMCEventg() returned null." << std::endl;
      return false;
    }

    // Store all MCEventGenerators, until we have a clear rule to select.
    if (!generatorIndex)
      primaryParticleMomentum = pMCEventg->Momentum;

    AC::MCEventGenerator eventGenerator;
    eventGenerator.fParticleID = pMCEventg->Particle;
    eventGenerator.fTrackID = pMCEventg->trkID;
    eventGenerator.fMotherParticle = pMCEventg->parentID;
    eventGenerator.fProcess = AC::Unknown; // FIXME: Not implemented yet!
    eventGenerator.fX0 = pMCEventg->Coo[0];
    eventGenerator.fY0 = pMCEventg->Coo[1];
    eventGenerator.fZ0 = pMCEventg->Coo[2];
    eventGenerator.fTheta = acos(pMCEventg->Dir[2]);
    eventGenerator.fPhi = atan2(pMCEventg->Dir[1], pMCEventg->Dir[0]);
    eventGenerator.fMomentum = pMCEventg->Momentum;
    eventGenerator.fMass = pMCEventg->Mass;
    eventGenerator.fCharge = pMCEventg->Charge;
    fEvent->fMC.fEventGenerators.append(eventGenerator);
  }

  // Store MC clusters from selected particles.
  fEvent->fMC.fTrackerEnergyDeposition  = 0;
  for (int clusterIndex = 0; clusterIndex < fEvent->fMC.fNumberOfTrackerCluster; ++clusterIndex) {
    TrMCClusterR* pMCTrCluster = fAMSEvent->pTrMCCluster(clusterIndex);
    if (!pMCTrCluster) {
      WARN_OUT << "Problem building AC::MCTrackerCluster[" << clusterIndex << "]: "
               << "AMS ROOT File is broken! pTrMCCluster() returned null." << std::endl;
      return false;
    }

    fEvent->fMC.fTrackerEnergyDeposition += pMCTrCluster->GetEdep();
    bool processCluster = pMCTrCluster->IsPrimary() || (fabs(pMCTrCluster->GetMomentum() / primaryParticleMomentum) > 0.1 && !pMCTrCluster->IsNoise());
    if (!processCluster)
      continue;

    AC::MCTrackerCluster trackerCluster;
    trackerCluster.fX = pMCTrCluster->GetXgl().x();
    trackerCluster.fY = pMCTrCluster->GetXgl().y();
    trackerCluster.fZ = pMCTrCluster->GetXgl().z();
    trackerCluster.fDepositedEnergy = pMCTrCluster->GetEdep();
    trackerCluster.fMCParticleMomentum = pMCTrCluster->GetMomentum();
    trackerCluster.fID = pMCTrCluster->GetPart();
    fEvent->fMC.fTrackerClusters.append(trackerCluster);
  }

  // Store MC TRD clusters.
  fEvent->fMC.fTRDEnergyDeposition = 0; 
  for (int clusterIndex = 0 ; clusterIndex < fEvent->fMC.fNumberOfTRDCluster; ++clusterIndex) {
    TrdMCClusterR* pTrdMCCluster = fAMSEvent->pTrdMCCluster(clusterIndex);
    if (!pTrdMCCluster) {
      WARN_OUT << "Problem building AC::MCTRDCluster[" << clusterIndex << "]: "
               << "AMS ROOT File is broken! pTrdMCCluster() returned null." << std::endl;
      return false;
    }

    fEvent->fMC.fTRDEnergyDeposition += pTrdMCCluster->Edep;
 
    // Only store the first 100 TRD hits.
    if (clusterIndex > gMaximumTRDHits)
      continue;

    AC::MCTRDCluster trdCluster;
    trdCluster.fID = pTrdMCCluster->ParticleNo;
    trdCluster.fLLT = ((pTrdMCCluster->Layer)<<11) + ((pTrdMCCluster->Ladder)<<6) + pTrdMCCluster->Tube;
    trdCluster.fDepositedEnergy = pTrdMCCluster->Edep;
    // trdCluster.fEnergyLoss[0] = 2.; // FIXME: Not implemented yet.
    trdCluster.fKineticEnergy = pTrdMCCluster->Ekin;
    trdCluster.fStep = pTrdMCCluster->Step;
    fEvent->fMC.fTRDClusters.append(trdCluster);
  }

  // Store MC TOF information.
  fEvent->fMC.fUpperTOFEnergyDeposition = 0;
  fEvent->fMC.fLowerTOFEnergyDeposition = 0;
  for (int clusterIndex = 0; clusterIndex < fEvent->fMC.fNumberOfTOFCluster; ++clusterIndex) {
    TofMCClusterR* pTofMCCluster = fAMSEvent->pTofMCCluster(clusterIndex);
    if (!pTofMCCluster) {
      WARN_OUT << "Problem building AC::MCTOFCluster[" << clusterIndex << "]: "
               << "AMS ROOT File is broken! pTofMCCluster() returned null." << std::endl;
      return false;
    }

    if (pTofMCCluster->Coo[2] > 0)
      fEvent->fMC.fUpperTOFEnergyDeposition += pTofMCCluster->Edep;
    else if (pTofMCCluster->Coo[2] < 0)
      fEvent->fMC.fLowerTOFEnergyDeposition += pTofMCCluster->Edep;
  }

  // Store MC ACC information.
  fEvent->fMC.fACCEnergyDeposition = 0; 
  for (int clusterIndex = 0; clusterIndex < fEvent->fMC.fNumberOfACCCluster; ++clusterIndex) {
    AntiMCClusterR* pAntiMCCluster = fAMSEvent->pAntiMCCluster(clusterIndex);
    if (!pAntiMCCluster) {
      WARN_OUT << "Problem building AC::MCACCCluster[" << clusterIndex << "]: "
               << "AMS ROOT File is broken! pAntiMCCluster() returned null." << std::endl;
      return false;
    }

    fEvent->fMC.fACCEnergyDeposition += pAntiMCCluster->Edep;
  }

  // Store MC ECAL information.
  fEvent->fMC.fECALEnergyDeposition = 0; 
  for (int hitIndex = 0; hitIndex < fEvent->fMC.fNumberOfECALHits; ++hitIndex) {
    EcalMCHitR* pEcalMCHit = fAMSEvent->pEcalMCHit(hitIndex);
    if (!pEcalMCHit) {
      WARN_OUT << "Problem building AC::MCECALHit[" << hitIndex << "]: "
               << "AMS ROOT File is broken! pECALMCHit() returned null." << std::endl;
      return false;
    }

    fEvent->fMC.fECALEnergyDeposition += pEcalMCHit->Edep;
  }

  // FIXME: RICH Edep accessor is not available in gbatch, can't store MC RICH energy deposition.
  return true;
}
