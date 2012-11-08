#ifndef EVENTWRITER_HH
#define EVENTWRITER_HH

#include <string>
#include <bitset>
#include <vector>

#include <Rtypes.h>
#ifndef __CINT__
#include "TrdCandidateMatching.hh"
#include "TrdQt.hh"
#endif

class TFile;
class TTree;

namespace Utilities {
class ConfigHandler;
}

namespace Analysis {

/** Store selected event variables in a ROOT Tree.
  *
  * \todo add help example
  * \todo add howto for adding a variable
  *
  */
class EventWriter
{
public:

  EventWriter( Utilities::ConfigHandler* cfghandler,
               std::string resultDirectory = "",
               std::string resultFileSuffix = "" );

  void Clear();

  void Initialize();

  void Process( const Analysis::Particle& particle );

  void Finish();

private:

  std::string fResultDirectory;
  std::string fOutputFilePrefix;
  std::string fResultFileSuffix;

  TFile* fFile;
  TTree* fTree;

  Int_t     Run, Event, RunTag;
  Int_t     BadRunTag; ///< 0 if good run! bit pattern decodes which subdetector failed (see Analysis::BadRunManager)
  UChar_t   EventRndm;
  Double_t  EventTime;
  Float_t   TriggerRate, TriggerLifeTime, LongitudeDeg, LatitudeDeg, MagLatDeg, MagLongDeg;
  Float_t   GeomagneticCutOff;

  Char_t    nAccHits, nTofHits, nEcalShr, nTrkTrks;

  Int_t     TrkHits;
  Float_t   Rig, eRinv, Chi2TrkX, Chi2TrkY, Rigs[12];
  Float_t   TrkPhi[2], TrkTht[2], TrkXcors[8], TrkYcors[8], TrkXhits[2], TrkYhits[2], TrkSigLay1;

  Float_t   TofC, TofCe, TrkC, TrkCe, RichC, RichCe;
  Float_t   PartBeta, TofBeta, RichBeta;
  Float_t   EcalEnergy, EcalBDT, EcalSum;
  Float_t   TofMeanE, TofMaxE;

  Short_t   ParticleId;
  Float_t   LR_Elec_Prot, LR_Heli_Elec, LR_Heli_Prot;
  Char_t    TrdInAcceptance;
  Short_t   TrdActiveLayers, TrdCandidateLayers, TrdActiveStraws, TrdUnassignedHits;
  Float_t   TrdActivePathLen, TrdCandidatePathLen;
  Float_t   XePressure;
  Short_t   nTrdRawHits;
  Float_t   TrdAngleXZ, TrdAngleYZ;

  std::vector<Float_t> LR_Elec_Prot_MC;
  std::vector<Float_t> LR_Heli_Elec_MC;


#ifndef __CINT__
  TrdCandidateMatching fTrdCandidateMatching;
  TrdQt* fTrdQt;
#endif
};
}

#endif // EVENTWRITER_HH
