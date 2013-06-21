#ifndef MC_h
#define MC_h

#include "MCEventGenerator.h"
#include "MCTrack.h"
#include "MCTrackerCluster.h"
#include "MCTRDCluster.h"
#include "MC-Streamer.h"

namespace ACsoft {

namespace AC {

/** Monte-Carlo event data
  */
class MC {
  WTF_MAKE_FAST_ALLOCATED;
public:
  // FIXME: Obtain reasonable inline capacities from MC files (ask Antje).

  /** A vector of MCEventGenerator objects */
  typedef Vector<MCEventGenerator, 1> EventGeneratorsVector;

  /** A vector of MCTrack objects */
  typedef Vector<MCTrack, 1> TracksVector;

  /** A vector of MCTRDCluster objects */
  typedef Vector<MCTRDCluster, 1> TRDClustersVector;

  /** A vector of MCTrackerCluster objects */
  typedef Vector<MCTrackerCluster, 1> TrackerClustersVector;

  AC_MC_Variables

  /** Helper method dumping an MC object to the console
    */
  void Dump() const;

  /** Clear MC to the initial empty state, as it was just constructed.
    */
  void Clear();

  /** Accessors for the MCEventGenerator objects.
    */
  const EventGeneratorsVector& EventGenerators() const { return fEventGenerators; }

  /** Accessors for the MCTRDCluster objects.
    */
  const TRDClustersVector& TRDClusters() const { return fTRDClusters; }

  /** Accessors for the MCTrackerCluster objects.
    */
  const TrackerClustersVector& TrackerClusters() const { return fTrackerClusters; }

  /** Number of MCEventgR in AMS ROOT file.
    */
  Int_t NumberOfEventGenerators() const { return fNumberOfEventGenerators; }

  /** Number of TrMCClusterR in AMS ROOT file.
    */
  Int_t NumberOfTrackerCluster() const { return fNumberOfTrackerCluster; }

  /** Number of TrdMCClusterR in AMS ROOT file.
    */
  Int_t NumberOfTRDCluster() const { return fNumberOfTRDCluster; }

  /** Number of TofMCClusterR in AMS ROOT file.
    */
  Int_t NumberOfTOFCluster() const { return fNumberOfTOFCluster; }

  /** Number of AntiMCClusterR in AMS ROOT file.
    */
  Int_t NumberOfACCCluster() const { return fNumberOfACCCluster; }

  /** Number of RichMCClusterR AMS ROOT file.
    */
  Int_t NumberOfRICHCluster() const { return fNumberOfRICHCluster; }

  /** Number of EcalMCHitR in AMS ROOT file.
    */
  Int_t NumberOfECALHits() const { return fNumberOfECALHits; }

  /** Energy deposition in upper %TOF [MeV]
    */
  Float_t UpperTOFEnergyDeposition() const { return fUpperTOFEnergyDeposition; }

  /** Energy deposition in lower %TOF [MeV]
    */
  Float_t LowerTOFEnergyDeposition() const { return fLowerTOFEnergyDeposition; }

  /** Energy deposition in %Tracker [GeV]
    */
  Float_t TrackerEnergyDeposition() const { return fTrackerEnergyDeposition; }

  /** Energy deposition in %TRD [GeV]
    */
  Float_t TRDEnergyDeposition() const { return fTRDEnergyDeposition; }

  /** Energy deposition in %ACC [GeV]
    */
  Float_t ACCEnergyDeposition() const { return fACCEnergyDeposition; }

  /** Energy deposition in %ECAL [MeV]
    */
  Float_t ECALEnergyDeposition() const { return fECALEnergyDeposition; }

  /** Primary event generator
   */
  const MCEventGenerator* PrimaryEventGenerator() const;

private:
  REGISTER_CLASS(MC)
};

}

}

#endif
