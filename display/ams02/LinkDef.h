//  $Id: LinkDef.h,v 1.6 2001/08/10 16:18:50 choutko Exp $
#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ global gAMSR_Root;
#pragma link C++ global debugger;
#pragma link C++ global gAMSR_Display;

#pragma link C++ class AMSR_Root-;
#pragma link C++ class AMSR_Maker-;
#pragma link C++ class AMSR_3DCluster;
#pragma link C++ class AMSR_ToFCluster;
#pragma link C++ class AMSR_ToFClusterReader;
#pragma link C++ class AMSR_TrdCluster;
#pragma link C++ class AMSR_TrdClusterReader;
#pragma link C++ class AMSR_AntiCluster;
#pragma link C++ class AMSR_AntiClusterReader;
#pragma link C++ class AMSR_TrMCCluster;
#pragma link C++ class AMSR_TrMCClusterReader;
#pragma link C++ class AMSR_Track;
#pragma link C++ class AMSR_TrackReader;
#pragma link C++ class AMSR_EcalShower;
#pragma link C++ class AMSR_EcalShowerReader;
#pragma link C++ class AMSR_SiHit;
#pragma link C++ class AMSR_SiHitReader;
//#pragma link C++ class AMSR_AntiCluster;
//#pragma link C++ class AMSR_AntiClusterReader;
#pragma link C++ class AMSR_Particle;
#pragma link C++ class AMSR_MCParticle;
#pragma link C++ class AMSR_ParticleReader;
#pragma link C++ class AMSR_MCParticleReader;
#pragma link C++ class AMSR_Ntuple;
#pragma link C++ class AMSR_HistBrowser;
#pragma link C++ class AMSR_VirtualDisplay;
#pragma link C++ class AMSR_Display;
#pragma link C++ class AMSR_GeometrySetter;
//#pragma link C++ class AMSR_KeyNode;
#pragma link C++ class AMSR_Canvas;
#pragma link C++ class AMSR_Axis;
#pragma link C++ class TRadioButton;
#pragma link C++ class TSwitch;
#pragma link C++ class TGRunEventDialog;

#pragma link C++ class Debugger;

#endif
