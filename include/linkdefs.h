//  $Id: linkdefs.h,v 1.42 2012/10/31 14:57:50 shaino Exp $
#ifdef __CINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class GeoMagField+;
#pragma link C++ class GeoMagTrace+;
#ifdef _PGTRACK_
#pragma link C++ class MTF1+;
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
#pragma link C++ class TrFit;
#pragma link C++ class TrProp;
#pragma link C++ class TrAlignFit;
#pragma link C++ class VCon;
#ifdef __ROOTSHAREDLIBRARY__
#pragma link C++ class VCon_root;
#endif
#pragma link C++ class EcalAxis+;

#pragma link C++ class TrTrackPar+;
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
#pragma link C++ class RichConfigContainer;
///////////////////////////////////////
#pragma link C++ class TrElem+;
#pragma link C++ class TrMCClusterR+;
#pragma link C++ class TrRawClusterR+;
#pragma link C++ class TrClusterR+;
#pragma link C++ class TrRecHitR+;
#pragma link C++ class TrTrackR+;
#pragma link C++ class ChargeSubDR+;
//////////////////////////////////////
#pragma link C++ class TofRecH++;
//#pragma link C++ class TofTDVTool++;
//#pragma link C++ class TofTAlignPar++;
//////////////////////////////////////

#ifdef __ROOTSHAREDLIBRARY__
#pragma link C++ class AMSChain+;
#pragma link C++ class AMSEventList+;
#endif

#pragma link C++ class ScalerMon+;
#pragma link C++ class AMSSetupR::ISSAtt+;
#pragma link C++ class AMSSetupR::AMSSTK+;
#pragma link C++ class  AMSSetupR::GPS+;
#pragma link C++ class AMSSetupR::ISSINTL+;
#pragma link C++ class AMSSetupR::ISSSA+;
#pragma link C++ class AMSSetupR::ISSCTRS+;
#pragma link C++ class AMSSetupR::ISSGTOD+;
#pragma link C++ class AMSSetupR::BadRun+;
#pragma link C++ class AMSSetupR::ISSCTRSR+;
#pragma link C++ class AMSSetupR::GPSWGS84+;
#pragma link C++ class AMSSetupR::GPSWGS84R+;
#pragma link C++ class AMSSetupR::DSPError+;

#pragma link C++ class Lvl1TrigConfig+;
#pragma link C++  class AMSSetupR::TDVR+;
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
#pragma link C++ class TrdHCalibR+;
#pragma link C++ class TrdHChargeR+;
#pragma link C++ class TrdHReconR+;
#pragma link C++ class TrPdf+;
#pragma link C++ class Level1R+;
#pragma link C++ class Level3R+;
#pragma link C++ class BetaR+;
#pragma link C++ class TofBetaPar+;
#pragma link C++ class BetaHR+;
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
#pragma link C++ class AMSSetupR::Header+;

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
#pragma link C++ class AMSSetupR::SlowControlR+;
#pragma link C++ class AMSSetupR::SlowControlR::Element+;
#pragma link C++ class AMSSetupR::Header+;
#pragma link C++ class std::map<std::string,Node>+;
#pragma link C++ class std::pair<std::string,Node>+;
#pragma link C++ class std::map<unsigned short,std::vector<RichRingBSegmentR> >+;

#endif
