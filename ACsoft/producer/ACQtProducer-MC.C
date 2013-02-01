#include "ACQtProducer-include.C"

bool ACsoft::ACQtProducer::ProduceMC() {

  Q_ASSERT(fEvent);
  Q_ASSERT(fAMSEvent);

  int nEventGenerators = fAMSEvent->nMCEventg();
  for (int generatorIndex = 0; generatorIndex < nEventGenerators; ++generatorIndex)
  {
    MCEventgR* pMCEventg = fAMSEvent->pMCEventg(generatorIndex);
    if (!pMCEventg)
    {
      WARN_OUT << "Problem building AC::MCEventGenerator[" << generatorIndex << "]: "
               << "AMS ROOT File is broken! pMCEventg() returned null." << std::endl;
      return false;
    }

    AC::MCEventGenerator eventGenerator;
    // take only events which are above the lower edge of the TRD (84.6 cm) and charged particles
    if(pMCEventg->Coo[2] > 84.6 && fabs(pMCEventg->Charge >= 0))
    {
      eventGenerator.fID = pMCEventg->Particle;
      eventGenerator.fMotherParticle = 0; // pMCEventg.parentID; Not implemented yet!
      //eventGenerator.fProcess = AC::MCProcess::Unknown; //Not implemented yet!
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
  }

  int nMCTracks = fAMSEvent->nMCTrack();
  for(int trackIndex = 0; trackIndex < nMCTracks; ++trackIndex)
  {
    MCTrackR* pMCTrack = fAMSEvent->pMCTrack(trackIndex);
    if(!pMCTrack)
    {
      WARN_OUT << "Problem building AC::MCTrack[" << trackIndex << "]: "
               << "AMS ROOT File is broken! pMCTrack() returned null." << std::endl;
      return false;
    }

    AC::MCTrack mcTrack;
    strcpy(mcTrack.fVolumeName, pMCTrack->VolName);
    mcTrack.fX0 = pMCTrack->Pos[0];
    mcTrack.fY0 = pMCTrack->Pos[1];
    mcTrack.fZ0 = pMCTrack->Pos[2];

    fEvent->fMC.fTracks.append(mcTrack);
  }

  int nMCTrdHits = fAMSEvent->nTrdMCCluster();
  for (int hitsIndex = 0 ; hitsIndex < nMCTrdHits ; ++hitsIndex)
  {
    TrdMCClusterR* pTrdMCCluster = fAMSEvent->pTrdMCCluster(hitsIndex);
    if(!pTrdMCCluster)
    {
      WARN_OUT << "Problem building AC::MCTRDHit[" << hitsIndex << "]: "
               << "AMS ROOT File is broken! pTrdMCCluster() returned null." << std::endl;
      return false;
    }

    AC::MCTRDHit mcTrdHit;
    mcTrdHit.fID = pTrdMCCluster->ParticleNo;
    mcTrdHit.fLLT = ((pTrdMCCluster->Layer)<<11) + ((pTrdMCCluster->Ladder)<<6) + pTrdMCCluster->Tube;
    mcTrdHit.fDepositedEnergy = pTrdMCCluster->Edep;
    // mcTrdHit.fEnergyLoss[0] = 2.; // Not implemented yet.
    mcTrdHit.fKineticEnergy = pTrdMCCluster->Ekin;
    mcTrdHit.fStep = pTrdMCCluster->Step;

    fEvent->fMC.fTRDHits.append(mcTrdHit);
  }
  return true;
}
