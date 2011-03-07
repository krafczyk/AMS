//  $Id: linkdef.h,v 1.79 2011/03/07 22:56:08 choutko Exp $
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
#pragma link C++ class tkpatt;
#pragma link C++ class TrRecon;
#pragma link C++ class TrReconPar;
#pragma link C++ class TrSim;
#pragma link C++ class TrSimSensor;
#pragma link C++ class TrSimCluster;
#pragma link C++ class TrCharge;
#pragma link C++ class MagField;
#pragma link C++ class TrFit+;
#pragma link C++ class TrProp+;
#pragma link C++ class TrAlignFit;
#pragma link C++ class TrTrackPar+;
#pragma link C++ class TrTasClusterR+;
#pragma link C++ class TrTasPar+;
#pragma link C++ class TrTasDB+;
#pragma link C++ class TrTrackSelection+;

#pragma link C++ class TrHistoManHeader;
#pragma link C++ class TrHistoMan;
#pragma link C++ class TrOnlineMonitor;

#pragma link C++ class TKGEOMFFKEY_DEF+;
#pragma link C++ class TRMCFFKEY_DEF+;
#pragma link C++ class TRCALIB_DEF+;
#pragma link C++ class TRALIG_DEF+;
#pragma link C++ class TRCLFFKEY_DEF+;
#pragma link C++ class TRFITFFKEY_DEF+;

#pragma link C++ class HistoMan;
#endif
#pragma link C++ class VCon;
#pragma link C++ class TrElem+;
#ifdef __ROOTSHAREDLIBRARY__
#pragma link C++ class VCon_root;
#endif

#pragma link C++ class TrMCClusterR+;
#pragma link C++ class TrRawClusterR+;
#pragma link C++ class TrClusterR+;
#pragma link C++ class TrRecHitR+;
#pragma link C++ class TrTrackR+;

#ifdef __ROOTSHAREDLIBRARY__
#pragma link C++ class AMSChain+;
#pragma link C++ class AMSEventList+;
#ifndef _PGTRACK_
#pragma link C++ class TrHitA+;
#pragma link C++ class TrTrackA+;
#endif
#endif

#pragma link C++ class ScalerMon+;
#pragma link C++ class Lvl1TrigConfig+;
#pragma link C++ class AMSPoint+;
#pragma link C++ class AMSDir+;
#pragma link C++ class AMSRotMat+;
#pragma link C++ class HeaderR+;
#pragma link C++ class AMSSetupR::SlowControlR+;
#pragma link C++ class AMSSetupR::SlowControlR::Node+;
#pragma link C++ class AMSSetupR::SlowControlR::DataType+;
#pragma link C++ class AMSSetupR::SlowControlR::SubType+;
#pragma link C++ class AMSSetupR::Header+;
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
#pragma link C++ class TrdHReconR+;
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
#pragma link C++ class AMSSetupR+;
#pragma link C++ class AMSSetupR::BValues+;
#pragma link C++  class AMSSetupR::TDVR+;

#ifdef __AMSVMC__
#pragma link C++ class amsvmc_MCApplication+;
#pragma link C++ class amsvmc_DetectorConstruction+;
#pragma link C++ class amsvmc_PrimaryGenerator+;
#pragma link C++ class amsvmc_MCStack+;
#pragma link C++ class amsvmc_RootManager+;
#pragma link C++ class amsvmc_MagField+;
#pragma link C++ class amsvmc_RunConfiguration+;
#endif

//#pragma link C++ class SlowControlEl+;
#pragma link C++ class SlowControlDB+;
#pragma link C++ class std::pair<unsigned int,std::vector<float> >+;
#pragma link C++ class std::map<unsigned int,std::vector<float> >+;
//#pragma link C++ class std::pair<int,SlowControlEl>+;
//#pragma link C++ class std::map<int,SlowControlEl>+;

#pragma link C++ class SubType+;
#pragma link C++ class std::pair<int,SubType>+;
#pragma link C++ class std::map<int,SubType>+;
#pragma link C++ class DataType+;
#pragma link C++ class std::pair<int,DataType>+;
#pragma link C++ class std::map<int,DataType>+;
#pragma link C++ class Node+;


#endif
