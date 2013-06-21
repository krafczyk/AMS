#include "ACQtProducer-include.C"

// for residuals
#define RoundToShort(x) ( (x) >= 0 ? (  (Short_t)(min( 32767.0,(x)+0.5) ) ) \
                       : (  (Short_t)(max(-32768.0,(x)-0.5) ) ) ) 

/**  Try Tracker Refits (algorithms[i],pattern,refit)
  * \param algorithms one per digit; try from highest digit downward until sucessful
  */
bool ACsoft::ACQtProducer::ProduceTrackerTrackFit(AC::TrackerTrack& trackerTrack, TrTrackR* pTrTrack, const std::vector<int>& algorithms, int pattern, int refit, float beta) {

  Q_ASSERT(fEvent);
  Q_ASSERT(fAMSEvent);
  Q_ASSERT(pTrTrack);

  for (unsigned int i = 0; i < algorithms.size(); ++i) {
    int algorithm = algorithms[i];
    int iTrTrackPar = pTrTrack->iTrTrackPar(algorithm, pattern, refit);
    if (iTrTrackPar > 0) {
      AppendTrackerTrackFit(trackerTrack, pTrTrack, algorithm, pattern, refit, iTrTrackPar);
      return true;
    } else {
      DEBUG_OUT << "Problem building AC::TrackerFit[algo=" << algorithm << " pattern=" << pattern << " refit=" << refit << "]: "
                << "iTrTrackPar returned " << iTrTrackPar << ")" << std::endl;
      // This is not a fatal error and happens for lots of events, not all fits converge. 
    }
  }

  // all algorithm tries failed - but continue processing the event by returning true here.
  return true;
}

bool ACsoft::ACQtProducer::AppendTrackerTrackFit(AC::TrackerTrack& trackerTrack, TrTrackR* pTrTrack, int algorithm, int pattern, int refit, int iTrTrackPar) {

  AC::TrackerTrackFit trackerTrackFit;
  trackerTrackFit.fParameters = AC::TrackerTrackFit::GenerateParametersFromFlags(algorithm, pattern, refit);
  assert(trackerTrackFit.Algorithm() == algorithm);
  assert(trackerTrackFit.Pattern() == pattern);
  assert(trackerTrackFit.Refit() == refit);

  trackerTrackFit.fRigidity = pTrTrack->GetRigidity(iTrTrackPar);
  trackerTrackFit.fInverseRigidityError = pTrTrack->GetErrRinv(iTrTrackPar);
  trackerTrackFit.fChiSquareNormalizedX = pTrTrack->GetNormChisqX(iTrTrackPar);
  trackerTrackFit.fChiSquareNormalizedY = pTrTrack->GetNormChisqY(iTrTrackPar);

  AMSPoint point;
  AMSDir direction;

  pTrTrack->Interpolate(AC::AMSGeometry::ZTrackerLayer1+1.0, point, direction, iTrTrackPar);
  trackerTrackFit.fXLayer1Plus1cm = point.x();
  trackerTrackFit.fYLayer1Plus1cm = point.y();
  
  pTrTrack->Interpolate(AC::AMSGeometry::ZTrackerLayer1, point, direction, iTrTrackPar);
  trackerTrackFit.fThetaLayer1 = direction.gettheta();
  trackerTrackFit.fPhiLayer1 = direction.getphi();
  trackerTrackFit.fXLayer1 = point.x();
  trackerTrackFit.fYLayer1 = point.y();

  pTrTrack->Interpolate(AC::AMSGeometry::ZTRDUpper, point, direction, iTrTrackPar);
  trackerTrackFit.fXTRDUpper = point.x();
  trackerTrackFit.fYTRDUpper = point.y();

  pTrTrack->Interpolate(AC::AMSGeometry::ZTRDCenter, point, direction, iTrTrackPar);
  trackerTrackFit.fXTRDCenter = point.x();
  trackerTrackFit.fYTRDCenter = point.y();

  pTrTrack->Interpolate(AC::AMSGeometry::ZTRDLower, point, direction, iTrTrackPar);
  trackerTrackFit.fXTRDLower = point.x();
  trackerTrackFit.fYTRDLower = point.y();

  pTrTrack->Interpolate(AC::AMSGeometry::ZTOFUpper, point, direction, iTrTrackPar);
  trackerTrackFit.fThetaTOFUpper = direction.gettheta();
  trackerTrackFit.fPhiTOFUpper = direction.getphi();
  trackerTrackFit.fXTOFUpper = point.x();
  trackerTrackFit.fYTOFUpper = point.y();
  
  pTrTrack->Interpolate(AC::AMSGeometry::ZTOFUpper-1.0, point, direction, iTrTrackPar);
  trackerTrackFit.fXTOFUpperMinus1cm = point.x();
  trackerTrackFit.fYTOFUpperMinus1cm = point.y();

  pTrTrack->Interpolate(AC::AMSGeometry::ZTrackerLayer56, point, direction, iTrTrackPar);
  trackerTrackFit.fXLayer56 = point.x();
  trackerTrackFit.fYLayer56 = point.y();

  pTrTrack->Interpolate(AC::AMSGeometry::ZRICH, point, direction, iTrTrackPar);
  trackerTrackFit.fThetaRICH  = direction.gettheta();
  trackerTrackFit.fPhiRICH = direction.getphi();
  trackerTrackFit.fXRICH = point.x();
  trackerTrackFit.fYRICH = point.y();

  pTrTrack->Interpolate(AC::AMSGeometry::ZTrackerLayer9,  point, direction, iTrTrackPar);
  trackerTrackFit.fThetaLayer9 = direction.gettheta();
  trackerTrackFit.fPhiLayer9 = direction.getphi();
  trackerTrackFit.fXLayer9 = point.x();
  trackerTrackFit.fYLayer9 = point.y();
  
  pTrTrack->Interpolate(AC::AMSGeometry::ZTrackerLayer9-1.0,  point, direction, iTrTrackPar);
  trackerTrackFit.fXLayer9Minus1cm = point.x();
  trackerTrackFit.fYLayer9Minus1cm = point.y();

  trackerTrack.fTrackFits.append(trackerTrackFit);
  return true;
}

bool ACsoft::ACQtProducer::ProduceTrackerTrackFits(AC::TrackerTrack& trackerTrack, TrTrackR* pTrTrack, float beta) {

  Q_ASSERT(fEvent);
  Q_ASSERT(fAMSEvent);
  Q_ASSERT(pTrTrack);

  // Respect override track fits. If set, only produce a single track fit.
  if (fAlgorithm != -1 && fPattern != -1 && fRefit != -1) {
    static std::vector<int>* gTempAlgorithms = 0;
    if (gTempAlgorithms)
      gTempAlgorithms->clear();
    else
      gTempAlgorithms = new std::vector<int>;
    gTempAlgorithms->push_back(fAlgorithm);
    ProduceTrackerTrackFit(trackerTrack, pTrTrack, *gTempAlgorithms, fPattern, fRefit, beta);
    return true;
  }

  static std::vector<int>* gDefaultAlgo = 0;
  if (!gDefaultAlgo) {
    gDefaultAlgo = new std::vector<int>;
    gDefaultAlgo->push_back(0);
  }

  static std::vector<int>* gChikanianAlgo = 0;
  if (!gChikanianAlgo) {
    gChikanianAlgo = new std::vector<int>;
    gChikanianAlgo->push_back(3);
  }

  static std::vector<int>* gOneTwoAlgo = 0;
  if (!gOneTwoAlgo) {
    gOneTwoAlgo = new std::vector<int>;
    gOneTwoAlgo->push_back(1);
    gOneTwoAlgo->push_back(2);
  }

  try {
    // Synchronize any change here with ACQtProducer-Tracker.C / AnalysisParticle / ParticleFactory.
    static int gNoRefit = 0;
    int forceRefitPG = 3;
    int forceRefitMA = forceRefitPG + 10;

    if (!fIsProcessingEventsFromStandardRun) {
      // Tracker External Alignment eventually available - for analysis (pass4 or later)
      ProduceTrackerTrackFit(trackerTrack, pTrTrack, *gDefaultAlgo, 0, gNoRefit, beta);         //   AMS-RootFile Default Fit
      ProduceTrackerTrackFit(trackerTrack, pTrTrack, *gOneTwoAlgo, 3, forceRefitPG, beta);      //   Choutko/Alcaraz algorithm (1/2)  Inner hits (3) Refit with new alignment (3)

      if (pTrTrack->HasExtLayers()) {
        ProduceTrackerTrackFit(trackerTrack, pTrTrack, *gOneTwoAlgo, 0, forceRefitMA, beta);    //   Choutko/Alcaraz algorithm (1/2)  All Hits (0) with MA ext alignment (13)
        ProduceTrackerTrackFit(trackerTrack, pTrTrack, *gOneTwoAlgo, 0, forceRefitPG, beta);    //   Choutko/Alcaraz algorithm (1/2)  All Hits (0) with PG ext alignment (3)

        ProduceTrackerTrackFit(trackerTrack, pTrTrack, *gChikanianAlgo, 0, forceRefitPG, beta); //   Chikanian       algorithm (3)    All Hits (0) with PG ext alignment (3)

        if (pTrTrack->HasExtLayers() == 3) { // has hit in Layer-1 and Layer-9 
          ProduceTrackerTrackFit(trackerTrack, pTrTrack, *gOneTwoAlgo, 4, forceRefitMA, beta);  //   Choutko/Alcaraz algorithm (1/2)  2Inn+2Ext Hits (4) with MA ext alignment (13)
          ProduceTrackerTrackFit(trackerTrack, pTrTrack, *gOneTwoAlgo, 5, forceRefitMA, beta);  //   Choutko/Alcaraz algorithm (1/2)  Upper+L1 Hits (5) with MA ext alignment (13)
          ProduceTrackerTrackFit(trackerTrack, pTrTrack, *gOneTwoAlgo, 6, forceRefitMA, beta);  //   Choutko/Alcaraz algorithm (1/2)  Lower+L9 Hits (6) with MA ext alignment (13)
          ProduceTrackerTrackFit(trackerTrack, pTrTrack, *gOneTwoAlgo, 4, forceRefitPG, beta);  //   Choutko/Alcaraz algorithm (1/2)  2Inn+2Ext Hits (4) with PG ext alignment (3)
          ProduceTrackerTrackFit(trackerTrack, pTrTrack, *gOneTwoAlgo, 5, forceRefitPG, beta);  //   Choutko/Alcaraz algorithm (1/2)  Upper+L1 Hits (5) with PG ext alignment (3)
          ProduceTrackerTrackFit(trackerTrack, pTrTrack, *gOneTwoAlgo, 6, forceRefitPG, beta);  //   Choutko/Alcaraz algorithm (1/2)  Lower+L9 Hits (6) with PG ext alignment (3)
        }
      }
    } else {

      // external layer alignment not available - for calibration (std)
      ProduceTrackerTrackFit(trackerTrack, pTrTrack, *gOneTwoAlgo, 3, forceRefitPG, beta);      //   Choutko/Alcaraz algorithm (1/2)  Inner hits (3) Refit with new alignment (3)

    }
  } catch (...) {
    return false;
  }

  return true;
}

static inline bool LookupTOFBetaForTrackerTrackIndex(AMSEventR* AMSEvent, const int trackIndex, float& beta) {

  // Lookup particle which is associated with the given trackIndex.
  int nBetaH = AMSEvent->nBetaH(); 
  for ( int betaIndex = 0; betaIndex < nBetaH; ++betaIndex) {
    BetaHR* pBetaH = AMSEvent->pBetaH(betaIndex);
    if (pBetaH->iTrTrack() == trackIndex) { beta = pBetaH->GetBetaC(); return true; }
  }

  return false;
}

static inline bool LookupParticleIndexForTrackerTrackIndex(AMSEventR* AMSEvent, const int trackIndex, int& iParticle) {

  // Lookup particle which is associated with the given trackIndex.
  int nParticle = AMSEvent->nParticle(); 
  for ( int particleIndex = 0; particleIndex < nParticle; ++particleIndex) {
    ParticleR* pParticle = AMSEvent->pParticle(particleIndex);
    //printf("ACQtProducer-Tracker::LookupParticleIndexForTrackerTrackIndex iTrack:%2d ParticleIndex:%2d pParticle.iTrTrack:%2d\n",trackIndex,particleIndex,pParticle->iTrTrack());
    if (pParticle->iTrTrack() == trackIndex) { iParticle = particleIndex; return true; }
  }

  return false;
}


bool ACsoft::ACQtProducer::ProduceTrackerTrackCCLikelihood(AC::TrackerTrack& trackerTrack, int trackIndex, AMSEventR* fAMSEvent) {

  Q_ASSERT(fAMSEvent);

  // get TrackerLikelihood to minimize charge confusion - needs matching Particle
  int iParticle;
  if ( LookupParticleIndexForTrackerTrackIndex( fAMSEvent, trackIndex, iParticle ) ) {
    trackerTrack.fCCLikelihood = TrkLH::gethead()->GetLikelihoodRatioElectronWG(iParticle);

#ifndef DISABLE_ACQT_CONTROL_PLOTS
    if(    (trackerTrack.fTrdKChargeNumberOfHitsForLikelihoods >= 15) 
        && (trackerTrack.fTrdKChargeNumberOfOffTrackHitsForLikelihoods <  5) ) {
      if( (int)trackerTrack.fTrdKChargeLikelihood.size() >= 2){
        float LHe  = trackerTrack.fTrdKChargeLikelihood[0];
        float LHp  = trackerTrack.fTrdKChargeLikelihood[1];
        if( LHe+LHp > 0.0 ){
          float LRep = -log(LHe/(LHe+LHp));
          if( LRep < 0.5 ) { // lepton
            TrkLHVar var1 = TrkLH::gethead()->GetTrkLHVar(iParticle);
            h2_TrLH_vs_EovR->Fill(var1.energy/var1.rigidity_ch, trackerTrack.CCLikelihood());
          }
        }
      } else 
        WARN_OUT << "Problem building AC::TrackerTrack[" << trackIndex << "]: "
                 << " no TRDK Likelihoods  size:" << trackerTrack.fTrdKChargeLikelihood.size()
                 << " NHfLH:" << trackerTrack.fTrdKChargeNumberOfHitsForLikelihoods
                 << " NHOTfLH:" << trackerTrack.fTrdKChargeNumberOfOffTrackHitsForLikelihoods
                 << std::endl;
    }
#endif
  }

  return true;
}

bool ACsoft::ACQtProducer::ProduceTrackerTrackCharges(AC::TrackerTrack& trackerTrack, TrTrackR* pTrTrack, float beta) {

  Q_ASSERT(pTrTrack);

  int   pattern = 3;
  float charge  = pTrTrack->GetInnerQ(beta);
  float error   = pTrTrack->GetInnerQ_RMS(beta) / sqrt(max(1.0,(float)pTrTrack->GetInnerQ_NPoints(beta) - 1.0));
  trackerTrack.fChargesNew.append(AC::PatternChargeAndError(pattern,charge, error));

  if (pTrTrack->HasExtLayers() > 0 ) {  // 1:Inner+L1  2:Inner+L9   3:Inner+L1+L9 

    pattern = 10 + pTrTrack->HasExtLayers();
    charge  =      pTrTrack->GetQ(beta);
    error   =      pTrTrack->GetQ_RMS(beta) / sqrt(max(1.0,(float)pTrTrack->GetQ_NPoints(beta) - 1.0));
    trackerTrack.fChargesNew.append(AC::PatternChargeAndError(pattern, charge, error));

  }

  return true;
}


bool ACsoft::ACQtProducer::ProduceTrackerTrackTrdKCharges(AC::TrackerTrack& trackerTrack, int iTrTrack, AMSEventR* fAMSEvent) {

  Q_ASSERT(fAMSEvent);

  TrTrackR* pTrTrack = fAMSEvent->pTrTrack(iTrTrack);
  Q_ASSERT(pTrTrack);

  int iTrackParDefault = pTrTrack->iTrTrackPar(0,0,0); // Get default-fit code

  if( iTrackParDefault>0 && fAMSEvent->NTrdRawHit()>0 ){

    TrdKCluster TRDkcluster = TrdKCluster(fAMSEvent,pTrTrack,iTrackParDefault);  

    if(    TRDkcluster.IsReadAlignmentOK     // 0: no Alignment   1: Static Alignment on Layers  2: Dynamic Alignment for entire TRD 
    && TRDkcluster.IsReadCalibOK     ) { // 0: no Gain Calibration   1: Gain Calibration Succeeded

      TRDkcluster.CalculateTRDCharge();

      trackerTrack.fTrdKCharge        = TRDkcluster.GetTRDCharge();
      trackerTrack.fTrdKChargeError   = TRDkcluster.GetTRDChargeError();
      trackerTrack.fTrdKChargeNumberOfHitsForCharge  = (int)TRDkcluster.GetQTRDHitCollection().size();
      trackerTrack.fTrdKChargeNumberOfHitsForNucleiPDF  = (int)TRDkcluster.GetQTRDHitCollectionNuclei().size();  
      trackerTrack.fTrdKChargeNumberOfHitsForDeltaRayPDF  = (int)TRDkcluster.GetQTRDHitCollectionDeltaRay().size();

      // printf("TrTrack[ ] Q:%8.3f  TrdK NH A/Q/N/D:%3d/%3d/%3d/%3d Charge/Err: %8.3f/%8.3f\n",
      //           trackerTrack.fChargesNew.size() > 0 ? trackerTrack.fChargesNew[0].charge : 0.0,
      //           fAMSEvent->NTrdRawHit(),
      //           trackerTrack.fTrdKChargeNumberOfHitsForCharge,
      //           trackerTrack.fTrdKChargeNumberOfHitsForNucleiPDF,
      //           trackerTrack.fTrdKChargeNumberOfHitsForDeltaRayPDF,
      //         trackerTrack.fTrdKCharge,
      //         trackerTrack.fTrdKChargeError
      //       );



      // TRDK e,p,He Likelihoods:

      double LLR[3]={-1,-1,-1};  //To be filled with 3 LikeLihoodRatios :  e/P, e/He, P/He
      double LLH[3]={-1,-1,-1};  //To be filled with 3 LikeLiHoods :       e, P, He
    
      int   NHitsU    =  0; // To be filled with number of hits taken into account in Likelihood Calculation
      float threshold = 15; // ADC above which will be taken into account in Likelihood Calculation  
                            // 15 ADC is the recommended value for the moment.

      // Calculate e,p,He Likelihoods with rigidity-correction, Fill the LLR, NHitsK, LL  according to Track selection, and return validity 
      int isValid = TRDkcluster.GetLikelihoodRatio_TrTrack(threshold, LLR, NHitsU, 0.0, LLH); 
      int   nhitsO = 0;
      float ampsO  = 0.0; 
      if( isValid && NHitsU>0 ) { // fill TrdKChargeLikelihood Vector:
        trackerTrack.fTrdKChargeNumberOfHitsForLikelihoods   = NHitsU; 
        trackerTrack.fTrdKChargeLikelihood.append(LLH[0]);
        trackerTrack.fTrdKChargeLikelihood.append(LLH[1]);
        trackerTrack.fTrdKChargeLikelihood.append(LLH[2]);

        TRDkcluster.GetOffTrackHit_TrTrack(nhitsO,ampsO); // get number of hits off-track
        trackerTrack.fTrdKChargeNumberOfOffTrackHitsForLikelihoods = nhitsO;


        // Calculate e,p,He Likelihoods also with ECAL-Energy correction:
        int iParticle;
        if ( LookupParticleIndexForTrackerTrackIndex( fAMSEvent, iTrTrack, iParticle ) ) {
          if( fAMSEvent->Particle(iParticle).iEcalShower() >= 0 ) {
            float Eecal = fAMSEvent->Particle(iParticle).pEcalShower()->GetCorrectedEnergy();
            float Rigdt = fAMSEvent->Particle(iParticle).pTrTrack()->GetRigidity(0);
            if(fabs(Rigdt - pTrTrack->GetRigidity(0)) > 1e-5) 
              printf("Event:%8d TrackerTrack[%2d] ERROR Rpart:%8.3f Rtr:%8.3f\n",fAMSEvent->Event(),iTrTrack,Rigdt,pTrTrack->GetRigidity(0));
            if( Eecal > 5.0 ) { // improved beta*gamma correction useful only for electrons above 5 GeV
              int isValid = TRDkcluster.GetLikelihoodRatio_TrTrack(threshold, LLR, NHitsU, Eecal, LLH); 
              if( isValid && NHitsU>0 ) { // fill TrdKChargeLikelihood Vector:
                trackerTrack.fTrdKChargeLikelihood.append(LLH[0]);
                trackerTrack.fTrdKChargeLikelihood.append(LLH[1]);
                trackerTrack.fTrdKChargeLikelihood.append(LLH[2]);
              }
            }
          }
        }

      } else trackerTrack.fTrdKChargeNumberOfHitsForLikelihoods   = 0;



      //printf("TrTrack[ ] TrdK isV:%2d NHU/O:%3d/%3d LLH: %8.5f %8.5f %8.5f\n", isValid,NHitsU,nhitsO,LLH[0],LLH[1],LLH[2] );

    }

  }

  return true;
}


bool ACsoft::ACQtProducer::ProduceTrackerTrackReconstructedHits(AC::TrackerTrack& trackerTrack, TrTrackR* pTrTrack, float beta) {

  int nReconstructedHits = pTrTrack->NTrRecHit();
  for (int reconstructedHitIndex=0; reconstructedHitIndex < nReconstructedHits; ++reconstructedHitIndex) {
    TrRecHitR* pTrRecHit = pTrTrack->pTrRecHit(reconstructedHitIndex);
    if (!pTrRecHit) {
      WARN_OUT << "Problem building AC::TrackerReconstructedHit[" << reconstructedHitIndex << "]: "
               << "AMS ROOT File is broken! pTrTrack->pTrRecHit() returned null." << std::endl;
      return false;
    }

    AC::TrackerReconstructedHit trackerReconstructedHit;
    trackerReconstructedHit.fLayer = pTrRecHit->GetLayerJ();

    AMSPoint coo;
    AMSDir   dir;
    int multiplicity = pTrRecHit->GetResolvedMultiplicity();
    if (multiplicity == -1) {

      trackerReconstructedHit.fLayer *= -1;
      trackerReconstructedHit.fX = pTrRecHit->GetCoord().x();
      trackerReconstructedHit.fY = pTrRecHit->GetCoord().y();

      pTrTrack->Interpolate( pTrRecHit->GetCoord().z(), coo, dir, 0);
      double residual = (trackerReconstructedHit.fY - coo.y()) * 10000.0; // signed Y-residual in microns
      trackerReconstructedHit.fResidualY = RoundToShort(residual);

      WARN_OUT << "Problem building AC::TrackerReconstructedHit[" << reconstructedHitIndex << "]: "
               << "XY multiplicity unresolved in layer " << trackerReconstructedHit.fLayer << ": "
               << "x=" << trackerReconstructedHit.fX << " y=" << trackerReconstructedHit.fY << std::endl;

    } else {

      trackerReconstructedHit.fX = pTrRecHit->GetGlobalCoordinateA(multiplicity).x();
      trackerReconstructedHit.fY = pTrRecHit->GetGlobalCoordinateA(multiplicity).y();

      pTrTrack->Interpolate( pTrRecHit->GetGlobalCoordinateA(multiplicity).z(), coo, dir, 0);
      double residual = (trackerReconstructedHit.fY - coo.y())*10000.0; // signed Y-residual rounded to microns
      trackerReconstructedHit.fResidualY = RoundToShort(residual);

      // printf("TrHit[%1d] XYZ: %8.3f %8.3f %8.3f  FIT-COO: %8.3f %8.3f %8.3f\n", reconstructedHitIndex,
      //           10.0*pTrRecHit->GetGlobalCoordinateA(multiplicity).x(),
      //           10.0*pTrRecHit->GetGlobalCoordinateA(multiplicity).y(),
      //           10.0*pTrRecHit->GetGlobalCoordinateA(multiplicity).z(),
      //         10.0*coo.x(),10.0*coo.y(),10.0*coo.z()
      //       ); 
    }

    if ( pTrRecHit->OnlyY() ) trackerReconstructedHit.fQLayerJ = -fabs(pTrTrack->GetLayerJQ(pTrRecHit->GetLayerJ(), beta));
    else                      trackerReconstructedHit.fQLayerJ =  fabs(pTrTrack->GetLayerJQ(pTrRecHit->GetLayerJ(), beta));

    trackerTrack.fReconstructedHits.append(trackerReconstructedHit);

#ifndef DISABLE_ACQT_CONTROL_PLOTS
    h1_TrRecHitResidJLay[pTrRecHit->GetLayerJ()-1]->Fill((float)trackerReconstructedHit.fResidualY);
#endif

  }

  return true;
}

bool ACsoft::ACQtProducer::ProduceTrackerTrackClusterDistances(AC::TrackerTrack& trackerTrack, TrTrackR* pTrTrack) {

  trackerTrack.fClusterDistances.clear();

  // get distance to closest Tracker-Y-Cluster for all layers:
  float yCoordinateInLayer[9]; // TrTrack Y-coordinates for jLayer 1..9 preset
  int clusterIndexInLayer[9];
  for (int j = 0; j < 9; ++j) {
    clusterIndexInLayer[j] = -1;
    yCoordinateInLayer[j] = 9999.999; // preset to value outside Tracker
    trackerTrack.fClusterDistances.append(std::numeric_limits<unsigned short>::max() - 1);
  }

  // collect Y-coordinates from associated TrRecHits:
  int nReconstructedHits = pTrTrack->NTrRecHit();
  for (int reconstructedHitIndex=0; reconstructedHitIndex < nReconstructedHits; ++reconstructedHitIndex) {
    TrRecHitR* pTrRecHit = pTrTrack->pTrRecHit(reconstructedHitIndex);
    if (!pTrRecHit) {
      WARN_OUT << "Problem finding AC::TrackerReconstructedHit[" << reconstructedHitIndex << "]: "
               << "AMS ROOT File is broken! pTrTrack->pTrRecHit() returned null." << std::endl;
      return false;
    }

    int clusterIndex = pTrRecHit->iTrCluster('y');
    if (clusterIndex < 0) 
      WARN_OUT << "Problem: ClusterDistance pTrRecHit(" << reconstructedHitIndex << ")->iTrCluster('y') <0" << std::endl;
    else {
       TrClusterR* pCluster = fAMSEvent->pTrCluster(clusterIndex);
       clusterIndexInLayer[pTrRecHit->GetLayerJ() - 1] = clusterIndex;
       yCoordinateInLayer[pTrRecHit->GetLayerJ() - 1] = pCluster->GetGCoord(pCluster->GetMultiplicity());
     }
  }

  // get Y-coordinates for layers without RecHit from track interpolation:
  for (int j = 0; j < 9; ++j) {
    if (clusterIndexInLayer[j] < 0)
      yCoordinateInLayer[j] = pTrTrack->InterpolateLayerJ(j + 1).y();
  }

  // loop over all Y-TrClusters to get minimum Y-distance to TrRecHit/TrTrack for each layer:
  for (int j = 0; j < (int) fAMSEvent->NTrCluster(); ++j) {
    TrClusterR* pCluster = fAMSEvent->pTrCluster(j);
    if (pCluster->GetSide() != 1)
      continue; // Select Y side only

    int layer = pCluster->GetLayerJ();
    if (clusterIndexInLayer[layer - 1] == j)
      continue;

    float dy = pCluster->GetGCoord(pCluster->GetMultiplicity()) - yCoordinateInLayer[layer - 1];
    float test = min((float)(std::numeric_limits<unsigned short>::max() - 1), (float)fabs(10000.0*dy));
    trackerTrack.fClusterDistances[layer - 1] = min(trackerTrack.fClusterDistances[layer - 1], static_cast<unsigned short>(test));
  }

  return true;
}

bool ACsoft::ACQtProducer::ProduceVertex(AC::Vertex& vertex, const VertexR* vtx) {

  // check if AMS vertex pointer is valid
  if (!vtx)
    return false;

  // check if both track vectors have the same length
  if (fEvent->fTracker.fTracks.size() != fAMSEvent->nTrTrack())
    return false;

  // loop over both tracks
  for (int iTrack = 0; iTrack < 2; ++iTrack) {

    // get track index and check it for validity
    const int index = vtx->iTrTrack(iTrack);
    if (index < 0 || index >= fAMSEvent->nTrTrack())
      return false;

    // get track and check it for validity
    TrTrackR* pTrTrack = fAMSEvent->pTrTrack(index);
    if (!pTrTrack)
      return false;

    // try to get the kVertex fit and append it to the ACQt TrackFit vector if it's there
    const int fid = TrTrackR::kVertex;
    if (pTrTrack->ParExists(fid)) {
      AC::TrackerTrack& trackerTrack = fEvent->fTracker.fTracks.at(index);
      AppendTrackerTrackFit(trackerTrack, pTrTrack, 7, 0, 0, fid);
    }

  }

  // copy information from AMS vertex
  vertex.fX = vtx->Vertex[0];
  vertex.fY = vtx->Vertex[1];
  vertex.fZ = vtx->Vertex[2];
  vertex.fTheta = vtx->Theta;
  vertex.fPhi = vtx->Phi;
  vertex.fMomentum = vtx->Momentum;
  vertex.fFirstTrackIndex = vtx->iTrTrack(0);
  vertex.fSecondTrackIndex = vtx->iTrTrack(1);

  // calculate galactic coordinates
  int result;
  double glong, glat;
  double theta = TMath::Pi() - vertex.fTheta;
  double phi = vertex.fPhi + TMath::Pi();
  if (phi >= 2*TMath::Pi())
    phi -= 2*TMath::Pi();
  if (fAMSEvent->GetGalCoo(result, glong, glat, theta, phi, 1, 1, 3) >= 0) {
    vertex.fGalacticLongitude = glong;
    vertex.fGalacticLatitude = glat;
  }

  return true;
}

bool ACsoft::ACQtProducer::ProduceTracker() {

  Q_ASSERT(fEvent);
  Q_ASSERT(fAMSEvent);

  int nTracks = fAMSEvent->nTrTrack();
  for (int trackIndex = 0; trackIndex < nTracks; ++trackIndex) {
    TrTrackR* pTrTrack = fAMSEvent->pTrTrack(trackIndex);
    if (!pTrTrack) {
      WARN_OUT << "Problem building AC::TrackerTrack[" << trackIndex << "]: "
               << "AMS ROOT File is broken! pTrTrack() return null." << std::endl;
      return false;
    }

    AC::TrackerTrack trackerTrack;
    trackerTrack.fStatus = pTrTrack->getstatus(); 
    trackerTrack.fNumberOfHitsX = pTrTrack->GetNhitsX();
    trackerTrack.fNumberOfHitsY = pTrTrack->GetNhitsY();


    // get Charge from new estimator getQ - needs beta - get it from matching Particle
    float beta = 999; // default, for which getQ() ignores beta
    LookupTOFBetaForTrackerTrackIndex(fAMSEvent, trackIndex, beta);
    if (!ProduceTrackerTrackCharges(trackerTrack, pTrTrack, beta)) {
      WARN_OUT << "Problem building AC::TrackerTrack[" << trackIndex << "]: "
               << "Building tracker charges failed." << std::endl;
      return false;
    }

#ifndef AMS_ACQT_INTERFACE
    // TRDK charges for this TrackerTrack: 
    timer_trdk->Start(kFALSE);
    if (!ProduceTrackerTrackTrdKCharges(trackerTrack, trackIndex, fAMSEvent)) {
      WARN_OUT << "Problem building AC::TrackerTrack[" << trackIndex << "]: "
               << "Building TrdK  charges failed." << std::endl;
      return false;
    }
    timer_trdk->Stop();
#endif

    if (!ProduceTrackerTrackFits(trackerTrack, pTrTrack, beta)) {
      WARN_OUT << "Problem building AC::TrackerTrack[" << trackIndex << "] Fits: "
               << "AMS ROOT File is broken! Tracker fitting throwed false" << std::endl;
      return false;
    }

    if (!ProduceTrackerTrackReconstructedHits(trackerTrack, pTrTrack, beta)) {
      WARN_OUT << "Problem building AC::TrackerTrack[" << trackIndex << "] ReconstructedHits: "
               << "AMS ROOT File is broken! Tracker fitting throwed." << std::endl;
      return false;
    }

#ifndef AMS_ACQT_INTERFACE
    if (!ProduceTrackerTrackClusterDistances(trackerTrack, pTrTrack)) {
      WARN_OUT << "Problem building AC::TrackerTrack[" << trackIndex << "] ClusterDistances: "
               << "AMS ROOT File is broken! Tracker fitting throwed." << std::endl;
      return false;
    }

	// calculate TrackerLikelihood (F.Nozzoli) to reduce CC from interactions
    timer_trkl->Start(kFALSE);
    if ( !ProduceTrackerTrackCCLikelihood(trackerTrack, trackIndex, fAMSEvent) ) {
      WARN_OUT << "Problem building AC::TrackerTrack[" << trackIndex << "] CCLikelihood: "
               << "AMS ROOT File is broken! CCLikelihood throwed false" << std::endl;
      return false;
    }
    timer_trkl->Stop();
#endif

    fEvent->fTracker.fTracks.append(trackerTrack);
  }

#ifndef DISABLE_ACQT_CONTROL_PLOTS
  // debug tracker resolution - multiple scattering  choutko vs chikanian
  if( fEvent->fTracker.fTracks.size()==1 ){
    float Qinner = fEvent->fTracker.fTracks[0].fChargesNew[0].charge;
    if( (Qinner>0.9) && (Qinner<1.2) ){
      
      if(    (fEvent->fTracker.fTracks[0].fTrdKChargeNumberOfHitsForLikelihoods >= 15) 
    && (fEvent->fTracker.fTracks[0].fTrdKChargeNumberOfOffTrackHitsForLikelihoods <  5) ) {

        float LHe  = fEvent->fTracker.fTracks[0].fTrdKChargeLikelihood[0];
        float LHp  = fEvent->fTracker.fTracks[0].fTrdKChargeLikelihood[1];
        if( LHe+LHp > 0.0 ){

          float LRep = -log(LHe/(LHe+LHp));
          h1_LRep->Fill(LRep);
          if( LRep > 0.6 ){ // proton
            int iFit_choutko = fEvent->fTracker.fTracks[0].GetFit( 1,0,4);
            int iFit_chikani = fEvent->fTracker.fTracks[0].GetFit( 3,0,4);
            if( (iFit_choutko>=0) && (iFit_chikani>=0) ){
              float R_choutko    = fEvent->fTracker.fTracks[0].fTrackFits[iFit_choutko].fRigidity;
              float Rinv_choutko = fEvent->fTracker.fTracks[0].fTrackFits[iFit_choutko].fInverseRigidityError;
              float R_chikani    = fEvent->fTracker.fTracks[0].fTrackFits[iFit_chikani].fRigidity;
              float Rinv_chikani = fEvent->fTracker.fTracks[0].fTrackFits[iFit_chikani].fInverseRigidityError;

              h2_relerrR_vs_R_choutko->Fill(R_choutko, Rinv_choutko*R_choutko);
              h2_relerrR_vs_R_chikani->Fill(R_chikani, Rinv_chikani*R_chikani);
            }
          } 
        }
      }
    }
  }
#endif

#ifndef AMS_ACQT_INTERFACE
  // produce vertices
  if (fAMSEvent->nVertex() > 0) {
    VertexR* vtx = fAMSEvent->pVertex(0);
    vtx->Recover();
    if ((vtx->getstatus() & AMSDBc::GOOD) > 0 &&
        vtx->NTrTrack() == 2 &&
        vtx->Vertex[2] > 79.9 &&
        vtx->Vertex[2] < 80.1) {
      AC::Vertex vertex;
      if (!ProduceVertex(vertex, vtx)) {
        WARN_OUT << "Problem building AC::Vertex[" << 0 << "]"
                 << "Building vertices failed ." << std::endl;
        return false;
      }
      fEvent->fTracker.fVertices.append(vertex);
    }
  }
#endif

  fEvent->fTracker.fClusters = fAMSEvent->nTrCluster();
  return true;
}

