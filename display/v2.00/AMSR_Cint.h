//  $Id: AMSR_Cint.h,v 1.5 2001/01/22 17:32:38 choutko Exp $
/********************************************************************
* AMSR_Cint.h
********************************************************************/
#ifdef __CINT__
#error AMSR_Cint.h/C is only for compilation. Abort cint.
#endif
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
extern "C" {
#define G__ANSIHEADER
#include "G__ci.h"
extern void G__cpp_setup_tagtableAMSR_Cint();
extern void G__cpp_setup_inheritanceAMSR_Cint();
extern void G__cpp_setup_typetableAMSR_Cint();
extern void G__cpp_setup_memvarAMSR_Cint();
extern void G__cpp_setup_globalAMSR_Cint();
extern void G__cpp_setup_memfuncAMSR_Cint();
extern void G__cpp_setup_funcAMSR_Cint();
extern void G__set_cpp_environmentAMSR_Cint();
}


#include "TROOT.h"
#include "TMemberInspector.h"
#include "TTree.h"
#include "TFile.h"
#include "TH1.h"
#include "TRadioButton.h"
#include "TSwitch.h"
#include "AMSR_Axis.h"
#include "AMSR_3DCluster.h"
#include "AMSR_Maker.h"
#include "AMSR_HistBrowser.h"
#include "AMSR_Root.h"
#include "AMSR_ToFCluster.h"
#include "AMSR_ToFClusterReader.h"
#include "AMSR_AntiCluster.h"
#include "AMSR_AntiClusterReader.h"
#include "AMSR_TrMCCluster.h"
#include "AMSR_TrMCClusterReader.h"
#include "AMSR_Track.h"
#include "AMSR_TrackReader.h"
#include "AMSR_SiHit.h"
#include "AMSR_SiHitReader.h"
#include "AMSR_CTCCluster.h"
#include "AMSR_CTCClusterReader.h"
#include "AMSR_Particle.h"
#include "AMSR_ParticleReader.h"
#include "AMSR_MCParticle.h"
#include "AMSR_MCParticleReader.h"
#include "AMSR_Ntuple.h"
#include "AMSR_VirtualDisplay.h"
#include "Debugger.h"
#include "AMSR_Canvas.h"
#include "AMSR_GeometrySetter.h"
#include "AMSR_Display.h"
#include "TGRunEventDialog.h"

#ifndef G__MEMFUNCBODY
#endif

extern G__linked_taginfo G__AMSR_CintLN_TClass;
extern G__linked_taginfo G__AMSR_CintLN_TList;
extern G__linked_taginfo G__AMSR_CintLN_TObjArray;
extern G__linked_taginfo G__AMSR_CintLN_TObject;
extern G__linked_taginfo G__AMSR_CintLN_TString;
extern G__linked_taginfo G__AMSR_CintLN_TNamed;
extern G__linked_taginfo G__AMSR_CintLN_TVirtualPad;
extern G__linked_taginfo G__AMSR_CintLN_TGeometry;
extern G__linked_taginfo G__AMSR_CintLN_TAttLine;
extern G__linked_taginfo G__AMSR_CintLN_TAttFill;
extern G__linked_taginfo G__AMSR_CintLN_TTree;
extern G__linked_taginfo G__AMSR_CintLN_TH1F;
extern G__linked_taginfo G__AMSR_CintLN_TAttText;
extern G__linked_taginfo G__AMSR_CintLN_TAttPad;
extern G__linked_taginfo G__AMSR_CintLN_TBox;
extern G__linked_taginfo G__AMSR_CintLN_TWbox;
extern G__linked_taginfo G__AMSR_CintLN_TCanvas;
extern G__linked_taginfo G__AMSR_CintLN_TPad;
extern G__linked_taginfo G__AMSR_CintLN_TButton;
extern G__linked_taginfo G__AMSR_CintLN_TText;
extern G__linked_taginfo G__AMSR_CintLN_TRadioButton;
extern G__linked_taginfo G__AMSR_CintLN_TSwitch;
extern G__linked_taginfo G__AMSR_CintLN__x3d_data_;
extern G__linked_taginfo G__AMSR_CintLN__x3d_sizeof_;
extern G__linked_taginfo G__AMSR_CintLN_TPolyLine3D;
extern G__linked_taginfo G__AMSR_CintLN_AMSR_Axis;
extern G__linked_taginfo G__AMSR_CintLN_TMarker3DBox;
extern G__linked_taginfo G__AMSR_CintLN_AMSR_Display;
extern G__linked_taginfo G__AMSR_CintLN_AMSR_3DCluster;
extern G__linked_taginfo G__AMSR_CintLN_AMSR_Maker;
extern G__linked_taginfo G__AMSR_CintLN_AMSR_HistBrowser;
extern G__linked_taginfo G__AMSR_CintLN_EDataFileType;
extern G__linked_taginfo G__AMSR_CintLN_AMSR_ToFClusterReader;
extern G__linked_taginfo G__AMSR_CintLN_AMSR_TrackReader;
extern G__linked_taginfo G__AMSR_CintLN_AMSR_SiHitReader;
extern G__linked_taginfo G__AMSR_CintLN_AMSR_CTCClusterReader;
extern G__linked_taginfo G__AMSR_CintLN_AMSR_AntiClusterReader;
extern G__linked_taginfo G__AMSR_CintLN_AMSR_TrMCClusterReader;
extern G__linked_taginfo G__AMSR_CintLN_AMSR_ParticleReader;
extern G__linked_taginfo G__AMSR_CintLN_AMSR_MCParticleReader;
extern G__linked_taginfo G__AMSR_CintLN_AMSR_Ntuple;
extern G__linked_taginfo G__AMSR_CintLN_AMSR_VirtualDisplay;
extern G__linked_taginfo G__AMSR_CintLN_AMSR_Root;
extern G__linked_taginfo G__AMSR_CintLN_AMSR_ToFCluster;
extern G__linked_taginfo G__AMSR_CintLN_AMSR_AntiCluster;
extern G__linked_taginfo G__AMSR_CintLN_AMSR_TrMCCluster;
extern G__linked_taginfo G__AMSR_CintLN_THelix;
extern G__linked_taginfo G__AMSR_CintLN_AMSR_Track;
extern G__linked_taginfo G__AMSR_CintLN_AMSR_SiHit;
extern G__linked_taginfo G__AMSR_CintLN_AMSR_CTCCluster;
extern G__linked_taginfo G__AMSR_CintLN_AMSR_Particle;
extern G__linked_taginfo G__AMSR_CintLN_AMSR_MCParticle;
extern G__linked_taginfo G__AMSR_CintLN_AMSR_Canvas;
extern G__linked_taginfo G__AMSR_CintLN_EAMSR_View;
extern G__linked_taginfo G__AMSR_CintLN_Debugger;
extern G__linked_taginfo G__AMSR_CintLN_TGObject;
extern G__linked_taginfo G__AMSR_CintLN_TGWindow;
extern G__linked_taginfo G__AMSR_CintLN_TGFrame;
extern G__linked_taginfo G__AMSR_CintLN_TGCompositeFrame;
extern G__linked_taginfo G__AMSR_CintLN_TGLayoutHints;
extern G__linked_taginfo G__AMSR_CintLN_TGVerticalFrame;
extern G__linked_taginfo G__AMSR_CintLN_TGHorizontalFrame;
extern G__linked_taginfo G__AMSR_CintLN_TGMainFrame;
extern G__linked_taginfo G__AMSR_CintLN_TGTransientFrame;
extern G__linked_taginfo G__AMSR_CintLN_TRootCanvas;
extern G__linked_taginfo G__AMSR_CintLN_TGLabel;
extern G__linked_taginfo G__AMSR_CintLN_TGTextBuffer;
extern G__linked_taginfo G__AMSR_CintLN_TGTextEntry;
extern G__linked_taginfo G__AMSR_CintLN_TGTextButton;
extern G__linked_taginfo G__AMSR_CintLN_TGFileInfo;
extern G__linked_taginfo G__AMSR_CintLN_AMSR_GeoToggle;
extern G__linked_taginfo G__AMSR_CintLN_AMSR_GeometrySetter;
extern G__linked_taginfo G__AMSR_CintLN_TGTextFrame;
extern G__linked_taginfo G__AMSR_CintLN_TGRunEventDialog;
