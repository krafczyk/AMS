//  $Id: linkdef.h,v 1.129 2012/08/15 12:07:30 choutko Exp $
#ifdef __CINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#ifdef _PGTRACK_
#pragma link C++ class MTF1+;
#pragma link C++ class TkObject+;
#pragma link C++ class TkPlane+;
//#pragma link C++ class TkPlaneExt+;
#pragma link C++ class TkLadder+;
#pragma link C++ class TkCoo+;
#pragma link C++ class TkDBc+;
#pragma link C++ class TkSens+;
#pragma link C++ class TrCalDB+;
#pragma link C++ class TrLadCal+;
#pragma link C++ class TrParDB+;
#pragma link C++ class TrLadPar+;
#pragma link C++ class TrGainDB+;
#pragma link C++ class TrLadGain+;
#pragma link C++ class tkpatt;
#pragma link C++ class TrRecon;
#pragma link C++ class TrReconPar;
#pragma link C++ class TrSim;
#pragma link C++ class TrSimSensor;
#pragma link C++ class TrSimCluster;
#pragma link C++ class TrCharge;
#pragma link C++ class TrPdfDB+;
#pragma link C++ class mean_t;
#pragma link C++ class like_t;
#pragma link C++ class MagField;
#pragma link C++ class TrFit+;
#pragma link C++ class TrProp+;
//#pragma link C++ class TrAlignFit;
#pragma link C++ class TrTrackPar+;
#pragma link C++ class TrTasClusterR+;
#pragma link C++ class TrTasPar+;
#pragma link C++ class TrTasDB+;
#pragma link C++ class TrTrackSelection+;
#pragma link C++ class TrInnerDzDB+;
#pragma link C++ class TrInnerDzDB::DzElem+;
#pragma link C++ class TrExtAlignDB+;
#pragma link C++ class TrExtAlignPar+;
#pragma link C++ class TrExtAlignFit+;
#pragma link C++ class TrHistoManHeader;
#pragma link C++ class TrHistoMan;
#pragma link C++ class TrOnlineMonitor;

#pragma link C++ class TKGEOMFFKEY_DEF+;
#pragma link C++ class TRMCFFKEY_DEF+;
#pragma link C++ class TRCALIB_DEF+;
#pragma link C++ class TRALIG_DEF+;
#pragma link C++ class TRCLFFKEY_DEF+;
#pragma link C++ class TRFITFFKEY_DEF+;
#pragma link C++ class TRCHAFFKEY_DEF+;

#pragma link C++ class HistoMan;
#else
#pragma link C++ class TrTrackFitR+;
#pragma link C++ class TrTrackFitR::TrSCooR+;
#endif
////////////// Ext. alignment
#pragma link C++ class DynAlContinuity;
#pragma link C++ class DynAlFit;
#pragma link C++ class DynAlHistory+;
#pragma link C++ class DynAlEvent+;
#pragma link C++ class DynAlFitParameters+;
#pragma link C++ class DynAlFitContainer+;
#pragma link C++ class DynAlManager;
/////////////////////// Rich Tools
#pragma link C++ class GeomHash;
#pragma link C++ class GeomHashEnsemble;
//////////////////////////////////////
#pragma link C++ class RichPMTCalib;
#pragma link C++ class RichConfigManager;
#pragma link C++ class RichConfigContainer+;
///////////////////////////////////////
#pragma link C++ class TrPdf+;
#pragma link C++ class VCon;
#pragma link C++ class TrElem+;
#ifdef __ROOTSHAREDLIBRARY__
#pragma link C++ class VCon_root;
#pragma link C++ class EcalAxis+;
#endif

#pragma link C++ class TrMCClusterR+;
#pragma link C++ class TrRawClusterR+;
#pragma link C++ class TrClusterR+;
#pragma link C++ class TrRecHitR+;
#pragma link C++ class TrTrackR+;
//////////////////////////////////////
#pragma link C++ class TofRecH++;
//#pragma link C++ class TofTDVTool++;
//#pragma link C++ class TofTAlignPar++;
//////////////////////////////////////

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
#pragma link C++ class AMSEnergyLoss+;
#pragma link C++ class HeaderR+;
#pragma link C++ class AMSSetupR::SlowControlR+;
#pragma link C++ class AMSSetupR::SlowControlR::Element+;
#pragma link C++ class AMSSetupR::Header+;
#pragma link C++ class AMSSetupR::GPS+;
#pragma link C++ class EcalHitR+;
#pragma link C++ class EcalClusterR+;
#pragma link C++ class Ecal2DClusterR+;
#pragma link C++ class EcalShowerR+;
#pragma link C++ class TofRawClusterR+;
#pragma link C++ class TofRawSideR+;
#pragma link C++ class TofClusterR+;
#pragma link C++ class TofClusterHR+;
#pragma link C++ class AntiRawSideR+;
#pragma link C++ class AntiClusterR+;

#pragma link C++ class RichHitR+;
#pragma link C++ class RichRingR+;
#pragma link C++ class RichRingTables+;
#pragma link C++ class RichRingBSegmentR+;
#pragma link C++ class RichRingBR+;
#pragma link C++ class TrdRawHitR+;
#pragma link C++ class TrdClusterR+;
#pragma link C++ class TrdSegmentR+;
#pragma link C++ class TrdTrackR+;
#pragma link C++ class TrdHSegmentR+;
#pragma link C++ class TrdHTrackR+;
#pragma link C++ class TrdHReconR+;
#pragma link C++ class TrdHChargeR+;
#pragma link C++ class TrdHCalibR+;

#ifdef _PGTRACK_
#pragma link C++ class TofTrack+;
#pragma link C++ class TofCandidate+;
#pragma link C++ class TofRecon+;
#pragma link C++ class TofTimeCalibration+;
#pragma link C++ class TofGeometry+;
#endif

#ifdef _PGTRACK_
#pragma link C++ class AC_TrdHits+;
#pragma link C++ class TrdSCalibR+;
#pragma link C++ class TrdSChi2Fit+;
#pragma link C++ class TrdKCalib+;
#pragma link C++ class TrdKPDF+;
#pragma link C++ class TrdKCluster+;
#pragma link C++ class TrdKHit+;
#endif

#pragma link C++ class TrdTFit+;
#pragma link C++ class Tfunc+;
#pragma link C++ class Level1R+;
#pragma link C++ class Level3R+;
#pragma link C++ class TofBetaPar+;
#pragma link C++ class BetaR+;
#pragma link C++ class BetaHR+;
#pragma link C++ class ChargeSubDR+;
#pragma link C++ class ChargeR+;
#pragma link C++ class VertexR+;
#pragma link C++ class ParticleR+;
#pragma link C++ class AntiMCClusterR+;
#pragma link C++ class TofMCClusterR+;
#pragma link C++ class TofMCPmtHitR+;
#pragma link C++ class EcalMCHitR+;
#pragma link C++ class TrdMCClusterR+;
#pragma link C++ class RichMCClusterR+;
#pragma link C++ class MCTrackR+;
#pragma link C++ class MCEventgR+;
#pragma link C++ class DaqEventR+;
#pragma link C++ class AMSEventR+;
#pragma link C++ class AMSEventR::Service+;
#pragma link C++ class AMSSetupR+;
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

#pragma link C++ class AMSSetupR::ISSAtt+;
#pragma link C++ class AMSSetupR::AMSSTK+;
#pragma link C++ class AMSSetupR::ISSSA+;
#pragma link C++ class AMSSetupR::ISSCTRS+;
#pragma link C++ class AMSSetupR::ISSCTRSR+;
#pragma link C++ class AMSSetupR::GPSWGS84+;
#pragma link C++ class AMSSetupR::GPSWGS84R+;
#pragma link C++ class AMSSetupR::ISSGTOD+;
#pragma link C++ class AMSSetupR::BadRun+;
#pragma link C++ class AMSSetupR::DSPError+;
#pragma link C++ class SlowControlDB+;
#pragma link C++ class std::pair<unsigned int,std::vector<float> >+;
#pragma link C++ class std::map<unsigned int,std::vector<float> >+;

#pragma link C++ class SubType+;
#pragma link C++ class std::pair<int,SubType>+;
#pragma link C++ class std::map<int,SubType>+;
#pragma link C++ class DataType+;
#pragma link C++ class std::pair<int,DataType>+;
#pragma link C++ class std::map<int,DataType>+;
#pragma link C++ class Node+;
#pragma link C++ class std::map<std::string,Node>+;
#pragma link C++ class std::pair<std::string,Node>+;
#pragma link C++ class std::map<std::string,unsigned int>+;
#pragma link C++ class std::pair<std::string,unsigned int>+;
#pragma link C++ class std::map<unsigned short,std::vector<RichRingBSegmentR> >+;

#endif
