#include "ACQtProducer-include.C"

bool ACsoft::ACQtProducer::ProduceTOFBetas() {

  Q_ASSERT(fEvent);
  Q_ASSERT(fAMSEvent);

  int nBetaHs = fAMSEvent->nBetaH();
  for (int betaIndex = 0; betaIndex < nBetaHs; ++betaIndex) {
    BetaHR* pBetaH = fAMSEvent->pBetaH(betaIndex);
    if (!pBetaH) {
      WARN_OUT << "Problem building AC::TOFBetaH[" << betaIndex << "]: "
               << "AMS ROOT File is broken! pBetaH() returned null." << std::endl;
      return false;
    }

    AC::TOFBeta tofBeta;

    tofBeta.fBeta = pBetaH->GetBeta();
    // GetEBetaV() returns Error of 1/Beta   convert to Error of BetaC here:
    if ( pBetaH->IsGoodBeta() )  tofBeta.fBetaError =  fabs(pBetaH->GetEBetaV()*tofBeta.fBeta*tofBeta.fBeta);   // 4 layers and TrTrack
    else                         tofBeta.fBetaError = -fabs(pBetaH->GetEBetaV()*tofBeta.fBeta*tofBeta.fBeta);
    
    tofBeta.fChargesNew.clear();
    tofBeta.fTOFClusterIndex.clear();
    for (int ilay=0; ilay<4; ilay++){
      if( pBetaH->TestExistHL(ilay) ) {

        tofBeta.fChargesNew.append(pBetaH->GetQL(ilay));

        int ich; for(ich=0; ich<(int)pBetaH->NTofClusterH(); ich++) if( pBetaH->pTofClusterH(ich) == pBetaH->GetClusterHL(ilay) ) break;
        if(ich<(int)pBetaH->NTofClusterH()) tofBeta.fTOFClusterIndex.append(pBetaH->iTofClusterH(ich));  
        else {
	  WARN_OUT << "Problem finding TofCluster for ilay="<<ilay<< " for AC::TOFBetaH[" << betaIndex << "]" << std::endl;
          tofBeta.fTOFClusterIndex.append(-ilay);  
	}
      }
    }

    fEvent->fTOF.fBetas.append(tofBeta);

  }




  return true;
}

bool ACsoft::ACQtProducer::ProduceTOFClusters() {

  Q_ASSERT(fEvent);
  Q_ASSERT(fAMSEvent);

  int nClusterHs = fAMSEvent->nTofClusterH();
  for (int clusterIndex = 0; clusterIndex < nClusterHs; ++clusterIndex) { 
    TofClusterHR* pTofClusterH = fAMSEvent->pTofClusterH(clusterIndex);
    if (!pTofClusterH) {
      WARN_OUT << "Problem building AC::TOFClusterH[" << clusterIndex << "]: "
               << "AMS ROOT File is broken! pTofClusterH() returned null." << std::endl;
      return false;
    }

    if (pTofClusterH->Status & 0x3) {
      WARN_OUT << "Problem building AC::TOFCluster[" << clusterIndex << "]: "
               << "status: " << pTofClusterH->Status << std::endl;

      int nRawSides = pTofClusterH->NTofRawSide();
      for (int rawSideIndex = 0; rawSideIndex < nRawSides; ++rawSideIndex)
        WARN_OUT << "\traw side[" << rawSideIndex << "] status: " << pTofClusterH->pTofRawSide(rawSideIndex)->stat << std::endl;
    }

    AC::TOFCluster TOFCluster;
    TOFCluster.fStatus = pTofClusterH->Status;
    TOFCluster.fLayerBar = AC::TOFCluster::GenerateBitsFromFlags(pTofClusterH->Layer, pTofClusterH->Bar);

    if (TOFCluster.Layer() != pTofClusterH->Layer)
      WARN_OUT << "Problem building AC::TOFCluster[" << clusterIndex << "]: "
               << "Layer does not match, got: " << TOFCluster.Layer() << " expected: " << pTofClusterH->Layer << std::endl;

    if (TOFCluster.Bar() != pTofClusterH->Bar)
      WARN_OUT << "Problem building AC::TOFCluster[" << clusterIndex << "]: "
               << "Bar does not match, got: " << TOFCluster.Bar() << " expected: " << pTofClusterH->Bar << std::endl;

    TOFCluster.fX         = pTofClusterH->Coo[0];
    TOFCluster.fY         = pTofClusterH->Coo[1];
    TOFCluster.fZ         = pTofClusterH->Coo[2];
    TOFCluster.fTime      = pTofClusterH->Time;
    TOFCluster.fTimeError = pTofClusterH->ETime;
    TOFCluster.fEnergy    = pTofClusterH->GetEdep();

    fEvent->fTOF.fClusters.append(TOFCluster);
  }

  return true;
}



bool ACsoft::ACQtProducer::ProduceTOF() {

  Q_ASSERT(fEvent);
  Q_ASSERT(fAMSEvent);

  //TofRecH::ReBuild(1);         // workaround TDV Init() bug in B620 - no Charges for many events  - but kills BetaC 
  //TofRecH::BuildBetaH();       // to get BetaC back
  //TofRecH::BuildTofClusterH(); //
  //printf("\n\n CALL   TofRec::Init()\n\n");
  //TofRecH::Init();
  //printf("\n\n CALLED TofRec::Init()\n\n");

  bool successful = ProduceTOFClusters();
  successful &= ProduceTOFBetas();
  return successful;
}
