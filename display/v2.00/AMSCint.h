/********************************************************************
* AMSCint.h
********************************************************************/
#ifdef __CINT__
#error AMSCint.h/C is only for compilation. Abort cint.
#endif
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
extern "C" {
#define G__ANSIHEADER
#include "G__ci.h"
extern void G__cpp_setup_tagtableAMSCint();
extern void G__cpp_setup_inheritanceAMSCint();
extern void G__cpp_setup_typetableAMSCint();
extern void G__cpp_setup_memvarAMSCint();
extern void G__cpp_setup_globalAMSCint();
extern void G__cpp_setup_memfuncAMSCint();
extern void G__cpp_setup_funcAMSCint();
extern void G__set_cpp_environmentAMSCint();
}


#include "TROOT.h"
#include "TMemberInspector.h"
#include "TTree.h"
#include "TFile.h"
#include "TH1.h"
#include "TRadioButton.h"
#include "TSwitch.h"
#include "AMSAxis.h"
#include "AMS3DCluster.h"
#include "AMSMaker.h"
#include "AMSHistBrowser.h"
#include "AMSRoot.h"
#include "AMSToFCluster.h"
#include "AMSToFClusterReader.h"
#include "AMSAntiCluster.h"
#include "AMSAntiClusterReader.h"
#include "AMSTrMCCluster.h"
#include "AMSTrMCClusterReader.h"
#include "AMSTrack.h"
#include "AMSTrackReader.h"
#include "AMSSiHit.h"
#include "AMSSiHitReader.h"
#include "AMSCTCCluster.h"
#include "AMSCTCClusterReader.h"
#include "AMSParticle.h"
#include "AMSParticleReader.h"
#include "AMSVirtualDisplay.h"
#include "Debugger.h"
#include "AMSCanvas.h"
#include "AMSDisplay.h"
#include "AMSGeometrySetter.h"

#ifndef G__MEMFUNCBODY
#endif

extern G__linked_taginfo G__AMSCintLN_TClass;
extern G__linked_taginfo G__AMSCintLN_TList;
extern G__linked_taginfo G__AMSCintLN_TObjArray;
extern G__linked_taginfo G__AMSCintLN_TObject;
extern G__linked_taginfo G__AMSCintLN_TString;
extern G__linked_taginfo G__AMSCintLN_TNamed;
extern G__linked_taginfo G__AMSCintLN_TVirtualPad;
extern G__linked_taginfo G__AMSCintLN_TGeometry;
extern G__linked_taginfo G__AMSCintLN_TApplication;
extern G__linked_taginfo G__AMSCintLN_TAttLine;
extern G__linked_taginfo G__AMSCintLN_TAttFill;
extern G__linked_taginfo G__AMSCintLN_TTree;
extern G__linked_taginfo G__AMSCintLN_TH1F;
extern G__linked_taginfo G__AMSCintLN_TAttText;
extern G__linked_taginfo G__AMSCintLN_TAttPad;
extern G__linked_taginfo G__AMSCintLN_TBox;
extern G__linked_taginfo G__AMSCintLN_TWbox;
extern G__linked_taginfo G__AMSCintLN_TCanvas;
extern G__linked_taginfo G__AMSCintLN_TPad;
extern G__linked_taginfo G__AMSCintLN_TButton;
extern G__linked_taginfo G__AMSCintLN_TText;
extern G__linked_taginfo G__AMSCintLN_TRadioButton;
extern G__linked_taginfo G__AMSCintLN_TSwitch;
extern G__linked_taginfo G__AMSCintLN__x3d_data_;
extern G__linked_taginfo G__AMSCintLN__x3d_sizeof_;
extern G__linked_taginfo G__AMSCintLN_TPolyLine3D;
extern G__linked_taginfo G__AMSCintLN_AMSAxis;
extern G__linked_taginfo G__AMSCintLN_TMarker3DBox;
extern G__linked_taginfo G__AMSCintLN_AMSDisplay;
extern G__linked_taginfo G__AMSCintLN_AMS3DCluster;
extern G__linked_taginfo G__AMSCintLN_AMSMaker;
extern G__linked_taginfo G__AMSCintLN_AMSHistBrowser;
extern G__linked_taginfo G__AMSCintLN_AMSToFClusterReader;
extern G__linked_taginfo G__AMSCintLN_AMSTrackReader;
extern G__linked_taginfo G__AMSCintLN_AMSSiHitReader;
extern G__linked_taginfo G__AMSCintLN_AMSCTCClusterReader;
extern G__linked_taginfo G__AMSCintLN_AMSAntiClusterReader;
extern G__linked_taginfo G__AMSCintLN_AMSTrMCClusterReader;
extern G__linked_taginfo G__AMSCintLN_AMSParticleReader;
extern G__linked_taginfo G__AMSCintLN_AMSVirtualDisplay;
extern G__linked_taginfo G__AMSCintLN_AMSRoot;
extern G__linked_taginfo G__AMSCintLN_AMSToFCluster;
extern G__linked_taginfo G__AMSCintLN_AMSAntiCluster;
extern G__linked_taginfo G__AMSCintLN_AMSTrMCCluster;
extern G__linked_taginfo G__AMSCintLN_THelix;
extern G__linked_taginfo G__AMSCintLN_AMSTrack;
extern G__linked_taginfo G__AMSCintLN_AMSSiHit;
extern G__linked_taginfo G__AMSCintLN_AMSCTCCluster;
extern G__linked_taginfo G__AMSCintLN_AMSParticle;
extern G__linked_taginfo G__AMSCintLN_EAMSView;
extern G__linked_taginfo G__AMSCintLN_Debugger;
extern G__linked_taginfo G__AMSCintLN_AMSCanvas;
extern G__linked_taginfo G__AMSCintLN_AMSGeoToggle;
extern G__linked_taginfo G__AMSCintLN_AMSGeometrySetter;
extern G__linked_taginfo G__AMSCintLN_TArc;
