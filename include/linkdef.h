//  $Id: linkdef.h,v 1.47 2009/06/11 13:51:31 choumilo Exp $
#ifdef __CINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#ifdef _PGTRACK_
#pragma link C++ class TkObject+;
#pragma link C++ class TkPlane+;
#pragma link C++ class TkLadder+;
#pragma link C++ class TkCoo+;
#pragma link C++ class TkDBc+;
#pragma link C++ class TkSens+;
#pragma link C++ class TrCalDB+;
#pragma link C++ class TrLadCal+;
#pragma link C++ class TrParDB+;
#pragma link C++ class TrLadPar+;
#pragma link C++ class PATT;
#pragma link C++ class TrRecon;
#pragma link C++ class TrAlignFit;
#pragma link C++ class VCon;
#ifdef __ROOTSHAREDLIBRARY__
#pragma link C++ class VCon_root;
#endif
#pragma link C++ class AMSTrMCCluster+;
#pragma link C++ class AMSTrRawCluster+;
#pragma link C++ class AMSTrCluster+;
#pragma link C++ class AMSTrRecHit+;
#pragma link C++ class AMSTrTrack+;
#pragma link C++ class AMSTrTrackPar+;

#else
#pragma link C++ class TrMCClusterR+;
#pragma link C++ class TrRawClusterR+;
#pragma link C++ class TrClusterR+;
#pragma link C++ class TrRecHitR+;
#pragma link C++ class TrTrackR+;
#endif

#pragma link C++ class AMSPoint+;
#pragma link C++ class AMSDir+;
#pragma link C++ class AMSRotMat+;
#pragma link C++ class HeaderR+;
#pragma link C++ class EcalHitR+;
#pragma link C++ class EcalClusterR+;
#pragma link C++ class Ecal2DClusterR+;
#pragma link C++ class EcalShowerR+;
#pragma link C++ class TofRawClusterR+;
#pragma link C++ class TofRawSideR+;
#pragma link C++ class TofClusterR+;
#pragma link C++ class AntiRawSideR+;
#pragma link C++ class AntiClusterR+;

#pragma link C++ class RichHitR+;
#pragma link C++ class RichRingR+;
#pragma link C++ class RichRingBR+;
#pragma link C++ class TrdRawHitR+;
#pragma link C++ class TrdClusterR+;
#pragma link C++ class TrdSegmentR+;
#pragma link C++ class TrdTrackR+;
#pragma link C++ class TrdHSegmentR+;
#pragma link C++ class TrdHTrackR+;
#pragma link C++ class Level1R+;
#pragma link C++ class Level3R+;
#pragma link C++ class BetaR+;
#pragma link C++ class ChargeR+;
#pragma link C++ class VertexR+;
#pragma link C++ class ParticleR+;
#pragma link C++ class AntiMCClusterR+;
#pragma link C++ class TofMCClusterR+;
#pragma link C++ class EcalMCHitR+;
#pragma link C++ class TrdMCClusterR+;
#pragma link C++ class RichMCClusterR+;
#pragma link C++ class MCTrackR+;
#pragma link C++ class MCEventgR+;
#pragma link C++ class DaqEventR+;
#pragma link C++ class AMSEventR+;
#pragma link C++ class AMSEventR::Service+;

#pragma link C++ class AMSChain+;
#pragma link C++ class AMSEventList+;

#endif
