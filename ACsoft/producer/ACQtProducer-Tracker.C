#include "ACQtProducer-include.C"

// for residuals
#define RoundToShort(x) ( (x) >= 0 ? (  (Short_t)(min( 32767.0,(x)+0.5) ) ) \
			           : (  (Short_t)(max(-32768.0,(x)-0.5) ) ) ) 


// FIXME: Explain what this function does. Thorsten could fill this in.
static int CalculatePatternForFit(TrTrackR* pTrTrack, int pattern) {

  Q_ASSERT(pTrTrack);
  int adjustedPattern = pattern;

  int nHits = 0;
  if (pattern == 2) { // Lay 3,4,5,6,7,8 - Inner without Lay-2

    if( !pTrTrack->GetHitLJ(2)) return -1; // no hit in Jlayer-2,so Inner without Lay-2 does not make sense 

    adjustedPattern = 0;
    int iPatt = 900;
    for (int j = 3; j<= 8; ++j, iPatt *= 10) {
      if (pTrTrack->GetHitLJ(j)) {
        ++nHits;
        adjustedPattern += iPatt;
      }
    }

    return nHits >= 3 ? adjustedPattern : -1; // flag if not enough hits for fit
  }

  if (pattern == 5) { // Lay 1,4,6,8 - Upper Half
    adjustedPattern = 0;
    if (pTrTrack->GetHitLJ(1)) {
      ++nHits;
      adjustedPattern += 9;
    }

    if (pTrTrack->GetHitLJ(4)) {
      ++nHits;
      adjustedPattern += 9000;
    }

    if (pTrTrack->GetHitLJ(6)) { 
      ++nHits;
      adjustedPattern += 900000;
    }

    if (pTrTrack->GetHitLJ(8)) {
      ++nHits;
      adjustedPattern += 90000000;
    }

    return nHits >= 3 ? adjustedPattern : -1;
  } 

  if (pattern == 6) { // Lay 2,3,5,7,9 - Lower Half
    adjustedPattern = 0;
    if (pTrTrack->GetHitLJ(2)) {
      ++nHits;
      adjustedPattern += 90;
    }

    if (pTrTrack->GetHitLJ(3)) {
      ++nHits;
      adjustedPattern += 900;
    }

    if (pTrTrack->GetHitLJ(5)) {
      ++nHits;
      adjustedPattern += 90000;
    }

    if (pTrTrack->GetHitLJ(7)) {
      ++nHits;
      adjustedPattern += 9000000;
    }

    if (pTrTrack->GetHitLJ(9)) {
      ++nHits;
      adjustedPattern += 900000000;
    }

    return nHits >= 3 ? adjustedPattern : -1;
  }

  return adjustedPattern;
}

// FIXME: Explain what this function does. Thorsten could fill this in.
static int CalculateMassAndChargeForFit(TrTrackR* pTrTrack, int refit, float beta, float& mass, float& charge) {

  Q_ASSERT(pTrTrack);
  int adjustedRefit = refit;

  if (pTrTrack->GetQ(beta) <= 1.5) {
    mass = 0.938272297; // Proton mass
    charge = 1.0;       // Charge 1
  } else {
    mass = 3.727379240; // Helium-mass
    charge = 2.0;       // Charge-2
  }

  switch (refit) {
  case 3:
    adjustedRefit =  2;
    break;
  case 4:
    adjustedRefit =  2;  // PG ext. Alignment - refit if necessary
    break;
  case 5:
    adjustedRefit = 12; // MA ext. Alignment - refit if necessary
    break;
  case 6: 
    adjustedRefit =  2;  // PG ext. Alignment - force refit with (mass,z,beta)
    mass = 0.000510999; // electron-mass
    charge = 1.0;       // Charge-1
    break;
  case 7: 
    adjustedRefit =  2;  // PG ext. Alignment - force refit with (mass,z,beta)
    mass = 3.727379240; // Helium-mass
    charge = 2.0;       // Charge-2
    break;
  }

  return adjustedRefit;
}

/**  Try Tracker Refits (algorithms[i],pattern,refit)
  * \param algorithms one per digit; try from highest digit downward until sucessful
  */
bool ACsoft::ACQtProducer::ProduceTrackerTrackFit(AC::TrackerTrack& trackerTrack, TrTrackR* pTrTrack, int algorithms, int pattern, int refit, float beta) {

  Q_ASSERT(fEvent);
  Q_ASSERT(fAMSEvent);
  Q_ASSERT(pTrTrack);

  float mass = 0, charge = 0;
  int adjustedRefit = CalculateMassAndChargeForFit(pTrTrack, refit, beta, mass, charge);
  int adjustedPattern = CalculatePatternForFit(pTrTrack, pattern);

  if( adjustedPattern < 0 ) return true; // do not try useless fit


  // extract digits into array (order: last to first):
  int algorithm[10];
  int Nalgorithm=0;
  int _algorithms = algorithms;
  if(algorithms==0){
    algorithm[0]=0;
    Nalgorithm = 1;
  } else {
    while( _algorithms > 0 ) { algorithm[Nalgorithm++]=_algorithms%10; _algorithms/=10; }
  }
  for(int ialgo=Nalgorithm-1; ialgo>=0; ialgo--){

    //printf("TrFit algorithms:%3d [%1d] algo:%1d Pattern:%1d/%9d Refit:%1d/%2d M:%8.5f C:%1.0f\n",
    //   algorithms,ialgo,algorithm[ialgo],pattern,adjustedPattern,refit,adjustedRefit,mass,charge);

    int iTrTrackPar = pTrTrack->iTrTrackPar(algorithm[ialgo], adjustedPattern, adjustedRefit, mass, charge);
  
    if (iTrTrackPar > 0) {
      AppendTrackerTrackFit(trackerTrack, pTrTrack, algorithm[ialgo], pattern, refit, iTrTrackPar);
      return true;
    } else {
      WARN_OUT << "Problem building AC::TrackerFit[algo=" << algorithm << " pattern=" << pattern << " refit=" << refit << "]: "
               << "iTrTrackPar returned " << iTrTrackPar << ")" << std::endl;
      // This is not a fatal error and happens for lots of events, not all fits converge. 
    }
  }

  // all algorith tries failed - but continue processing the event by returning true here.
  return true;

}

bool ACsoft::ACQtProducer::AppendTrackerTrackFit(AC::TrackerTrack& trackerTrack, TrTrackR* pTrTrack, int algorithm, int pattern, int refit, int iTrTrackPar) {

  AC::TrackerTrackFit trackerTrackFit;
  trackerTrackFit.fParameters = ((algorithm & 3) << 6) + ((pattern & 7) << 3) + ((refit & 7));

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
    ProduceTrackerTrackFit(trackerTrack, pTrTrack, fAlgorithm, fPattern, fRefit, beta);
    return true;
  }

  try {
    // FIXME: Is this still needed?
    pTrTrack->iTrTrackPar(2, 3, 2); // dummy call 


    if (fUseTrackerExternalAlignment) {
      // Tracker External Alignment available - for analysis (pass2 or later)

      ProduceTrackerTrackFit(trackerTrack, pTrTrack,  0, 0, 0, beta);     //   AMS-RootFile Default Fit

      ProduceTrackerTrackFit(trackerTrack, pTrTrack, 12, 3, 3, beta);     //   Choutko[Alcaraz] algorithm (P3=1[2])  Inn Hits (P4=3) Refit with new alignment (P5=3)
      ProduceTrackerTrackFit(trackerTrack, pTrTrack, 12, 2, 3, beta);     //   Choutko[Alcaraz] algorithm (P3=1[2])  Inn Hits without Jlayer-2 (P4=2) Refit with new alignment (P5=3)

      if (pTrTrack->HasExtLayers()) {
        ProduceTrackerTrackFit(trackerTrack, pTrTrack, 12, 0, 5, beta);   //   Choutko[Alcaraz] algorithm (P3=1[2])  All Hits (P4=0) with MA ext alignment (P5=5): 
        ProduceTrackerTrackFit(trackerTrack, pTrTrack, 12, 0, 4, beta);   //   Choutko[Alcaraz] algorithm (P3=1[2])  All Hits (P4=0) with PG ext alignment (P5=4): 

        if (pTrTrack->HasExtLayers() == 3) {
          // has hit in Layer-1 and Layer-9 
          ProduceTrackerTrackFit(trackerTrack, pTrTrack, 12, 4, 5, beta); //   Choutko[Alcaraz] algorithm (P3=1[2])  2Inn+2Ext Hits (P4=4) with MA ext alignment (P5=5): 
          ProduceTrackerTrackFit(trackerTrack, pTrTrack, 12, 5, 5, beta); //   Choutko[Alcaraz] algorithm (P3=1[2])  Upper+L1 Hits (P4=5) with MA ext alignment (P5=5): 
          ProduceTrackerTrackFit(trackerTrack, pTrTrack, 12, 6, 5, beta); //   Choutko[Alcaraz] algorithm (P3=1[2])  Lower+L9 Hits (P4=6) with MA ext alignment (P5=5): 
          ProduceTrackerTrackFit(trackerTrack, pTrTrack, 12, 4, 4, beta); //   Choutko[Alcaraz] algorithm (P3=1[2])  2Inn+2Ext Hits (P4=4) with PG ext alignment (P5=4): 
          ProduceTrackerTrackFit(trackerTrack, pTrTrack, 12, 5, 4, beta); //   Choutko[Alcaraz] algorithm (P3=1[2])  Upper+L1 Hits (P4=5) with PG ext alignment (P5=4): 
          ProduceTrackerTrackFit(trackerTrack, pTrTrack, 12, 6, 4, beta); //   Choutko[Alcaraz] algorithm (P3=1[2])  Lower+L9 Hits (P4=6) with PG ext alignment (P5=4): 
	}
      }

      // Fits for electron Mass
      ProduceTrackerTrackFit(trackerTrack, pTrTrack, 12, 0, 6, beta);     //   Choutko[Alcaraz] algorithm (P3=1[2])  All Hits (P4=0) with PG ext alignment and e Mass (P5=6): 

    } else {

      // external layer alignment not available - for calibration (std)
      ProduceTrackerTrackFit(trackerTrack, pTrTrack, 12, 3, 3, beta);     //   Choutko[Alcaraz] algorithm (P3=1[2])  Inn Hits (P4=3) Refit with new alignment (P5=3)

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


bool ACsoft::ACQtProducer::ProduceTrackerTrackCharges(AC::TrackerTrack& trackerTrack, TrTrackR* pTrTrack, float beta) {

  Q_ASSERT(pTrTrack);

  int pattern = 3;
  float charge = pTrTrack->GetInnerQ(beta);
  float error = pTrTrack->GetInnerQ_RMS(beta) / sqrt(max(1.0,(float)pTrTrack->GetInnerQ_NPoints(beta) - 1.0));
  trackerTrack.fChargesNew.append(AC::PatternChargeAndError(pattern,charge, error));

  if (pTrTrack->GetHitLJ(1) > 0) { // has hit in L1
    pattern = 1;
    charge = pTrTrack->GetLayerJQ(1, beta);
    error = (float)pTrTrack->GetInnerQ_NPoints(beta);
    if (charge > 0.0)
      trackerTrack.fChargesNew.append(AC::PatternChargeAndError(pattern, charge, error));
  }

  if (pTrTrack->GetHitLJ(9) > 0) { // has hit in L9
    pattern = 9;
    charge = pTrTrack->GetLayerJQ(9, beta);
    error = (float)pTrTrack->GetInnerQ_NPoints(beta);
    if (charge > 0.0)
      trackerTrack.fChargesNew.append(AC::PatternChargeAndError(pattern, charge, error));
  }

  return true;
}


bool ACsoft::ACQtProducer::ProduceTrackerTrackTrdKCharges(AC::TrackerTrack& trackerTrack, TrTrackR* pTrTrack, AMSEventR* fAMSEvent) {

  Q_ASSERT(pTrTrack);
  Q_ASSERT(fAMSEvent);

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
      // 	      trackerTrack.fChargesNew.size() > 0 ? trackerTrack.fChargesNew[0].charge : 0.0,
      // 	      fAMSEvent->NTrdRawHit(),
      // 	      trackerTrack.fTrdKChargeNumberOfHitsForCharge,
      // 	      trackerTrack.fTrdKChargeNumberOfHitsForNucleiPDF,
      // 	      trackerTrack.fTrdKChargeNumberOfHitsForDeltaRayPDF,
      //         trackerTrack.fTrdKCharge,
      //         trackerTrack.fTrdKChargeError
      //       );


      double LLR[3]={-1,-1,-1};  //To be filled with 3 LikeLihoodRatios :  e/P, e/H, P/H
      double LLH[3]={-1,-1,-1};  //To be filled with 3 LikeLiHoods :       e, P, H
    
      int   NHitsU    =  0; // To be filled with number of hits taken into account in Likelihood Calculation
      float threshold = 15; // ADC above which will be taken into account in Likelihood Calculation  
                            // 15 ADC is the recommended value for the moment.

      // Calculate Likelihood Ratio, Fill the LLR, NHitsK, LL  according to Track selection, and return validity 
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
      }


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
      // 	      10.0*pTrRecHit->GetGlobalCoordinateA(multiplicity).x(),
      // 	      10.0*pTrRecHit->GetGlobalCoordinateA(multiplicity).y(),
      // 	      10.0*pTrRecHit->GetGlobalCoordinateA(multiplicity).z(),
      //         10.0*coo.x(),10.0*coo.y(),10.0*coo.z()
      //       ); 
    }

    trackerReconstructedHit.fSignalX = pTrRecHit->GetSignalCombination(0);
    trackerReconstructedHit.fSignalY = pTrRecHit->GetSignalCombination(1);

    trackerTrack.fReconstructedHits.append(trackerReconstructedHit);
#ifndef AMS_ACQT_SILENCE_COMMON_WARNINGS
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

bool ACsoft::ACQtProducer::ProduceTracker() {

  Q_ASSERT(fEvent);
  Q_ASSERT(fAMSEvent);

  // FIXME: Document what's happening here, and why it's insufficient to just use nTrTrack() here.
  int nTracks = max(fMaxTrackerTracks + 1, min(5, fAMSEvent->nTrTrack()));
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

    // get Charge from new estimator getQ - needs beta - look for matching Particle
    float beta = 999; // default, for which getQ() ignores beta
    LookupTOFBetaForTrackerTrackIndex(fAMSEvent, trackIndex, beta);
    if (!ProduceTrackerTrackCharges(trackerTrack, pTrTrack, beta)) {
      WARN_OUT << "Problem building AC::TrackerTrack[" << trackIndex << "]: "
               << "Building tracker charges failed." << std::endl;
      return false;
    }


    // TRDK charges for this TrackerTrack: 
    timer_trdk->Start(kFALSE);
    if (!ProduceTrackerTrackTrdKCharges(trackerTrack, pTrTrack, fAMSEvent)) {
      WARN_OUT << "Problem building AC::TrackerTrack[" << trackIndex << "]: "
               << "Building TrdK  charges failed." << std::endl;
      return false;
    }
    timer_trdk->Stop();


    if (!ProduceTrackerTrackFits(trackerTrack, pTrTrack, beta)) {
      WARN_OUT << "Problem building AC::TrackerTrack[" << trackIndex << "] Fits: "
               << "AMS ROOT File is broken! Tracker fitting throwed." << std::endl;
      return false;
    }

    if (!ProduceTrackerTrackReconstructedHits(trackerTrack, pTrTrack, beta)) {
      WARN_OUT << "Problem building AC::TrackerTrack[" << trackIndex << "] ReconstructedHits: "
               << "AMS ROOT File is broken! Tracker fitting throwed." << std::endl;
      return false;
    }

    if (!ProduceTrackerTrackClusterDistances(trackerTrack, pTrTrack)) {
      WARN_OUT << "Problem building AC::TrackerTrack[" << trackIndex << "] ClusterDistances: "
               << "AMS ROOT File is broken! Tracker fitting throwed." << std::endl;
      return false;
    }

    fEvent->fTracker.fTracks.append(trackerTrack);
  }

  fEvent->fTracker.fClusters = fAMSEvent->nTrCluster();
  return true;
}
