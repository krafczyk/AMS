#ifndef AMSDisplay_H
#define AMSDisplay_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSDisplay                                                           //
//                                                                      //
// Utility class to display AMSRoot outline, tracks, and clusters.      //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#include <TGXW.h>
#include <TGClient.h>
#include <TGFrame.h>
#include <TGIcon.h>
#include <TGLabel.h>
#include <TGButton.h>
#include <TGTextEntry.h>
#include <TGMsgBox.h>
#include <TGMenu.h>
#include <TGCanvas.h>
#include <TGComboBox.h>
#include <TGTab.h>
#include <TGSlider.h>
#include <TGFileDialog.h>




#include <TApplication.h>
#ifndef AMSVirtualDisplay_H
#include "AMSVirtualDisplay.h"
#endif
#include "AMSGeometrySetter.h"
#include "AMSCanvas.h"
//#include "AMSColorManager.h"
class TCanvas;
class TPad;
class TArc;
//class ATLFTrigger;
class TTUBE;
class TNode;
class TPolyLine3D;
class TList;
#ifndef ROOT_TGeometry_H
#include <TGeometry.h>
#endif
//class ATLFParticle;



class AMSDisplay : public AMSVirtualDisplay {

private:

   EAMSView          m_View;		     //view
//   Bool_t            m_DrawAllViews;         //Flag True if AllViews selected
//   Bool_t            m_DrawTwoViews;         //Flag True if TwoViews selected
   Bool_t            m_DrawParticles;        //Flag True if particles to be drawn
   Bool_t            m_DrawGeometry;         //Draw geometry?
   Bool_t            m_DrawMoreGeometry;         //Draw geometry?
   TGeometry        *m_Geometry;	     //Pointer to the geometry
   AMSGeometrySetter *m_GeoSetter;	     //Pointer to the geometry setter
// AMSKeyNode       *m_KeyNode;		     //Pointer to selected geom nodes

//   AMSColorManager  *m_ColorManager;         //Pointer to the color manager

   Float_t           m_PTcut;                //PT cut to display objects
   Float_t           m_PTcutEGMUNU;          //PT cut for Electrons, Gammas, MUons, Neutrinos
   Float_t           m_Theta;                //Viewing angle theta
   Float_t           m_Phi;                  //Viewing angle phi
// TCanvas          *m_Canvas;               //Pointer to the display canvas
   AMSCanvas        *m_Canvas;               //Pointer to the display canvas
   TPad             *m_UCPad;                //Pointer to the UC pad 
   TPad             *m_LogoPad;              //Pointer to the logo pad 
   TPad             *m_TrigPad;              //Pointer to the trigger pad 
   TPad             *m_ButtonPad;            //Pointer to the buttons pad
   TPad             *m_Pad;                  //Pointer to the event display main pad
   TPad             *m_AxisPad[4];           //Pointer to the axis pad
   TPad             *m_TitlePad;             //Pointer to title pad 
   TPad             *m_EventInfoPad;         //Pointer to event info pad 
   TPad             *m_ObjInfoPad;           //Pointer to object info pad 
   TPad             *m_PartInfoPad;           //Pointer to object info pad 
//   TPad             *m_AxisPad[4];           //Pointers to axis pad 

   TArc             *m_EM1;                  //Pointer to arc showing ON/OFF trigger EM1
   TArc             *m_PH1;                  //Pointer to arc showing ON/OFF trigger PH1
   TArc             *m_EM2;                  //Pointer to arc showing ON/OFF trigger EM2
   TArc             *m_MU1;                  //Pointer to arc showing ON/OFF trigger MU1
   TArc             *m_MU2;                  //Pointer to arc showing ON/OFF trigger MU2
   TArc             *m_EMU;                  //Pointer to arc showing ON/OFF trigger EMU
   TArc             *m_JT1;                  //Pointer to arc showing ON/OFF trigger JT1
   TArc             *m_JT3;                  //Pointer to arc showing ON/OFF trigger JT3
   TArc             *m_JT4;                  //Pointer to arc showing ON/OFF trigger JT4
   TArc             *m_ALL;                  //Pointer to arc showing ON/OFF trigger ALL
// ATLFParticle     *m_Particle;             //Pointer to Particle graphics manager
   TList	    *m_ToBeDrawn;	     //List of objects to be drawn
   TApplication	    *m_theapp;	     //Current App

      
public:

                     AMSDisplay();
                     AMSDisplay(const char *title, TGeometry * geo=0, int resx=1024, int resy=768);
   virtual          ~AMSDisplay();
   virtual void      SetApplication(TApplication* papp){m_theapp=papp;}
   virtual Bool_t    AllViews() {return (m_View == kAllView);}
           EAMSView  GetView() {return m_View;}
   virtual void      Clear(Option_t *option="");
   virtual void      DisplayButtons();
//   virtual void      DisplayTrigger(ATLFTrigger *trig);
   virtual Int_t     DistancetoPrimitive(Int_t px, Int_t py);
   virtual void      Draw(Option_t *option="");
   virtual void      DrawAllViews();
	   void      DrawFrontAndSideViews();
// Bool_t            DrawClusters() {return m_DrawClusters;}
   Bool_t            DrawParticles() {return m_DrawParticles;}
           void      Select();
           void      DrawTitle(Option_t *option="");
           void      DrawEventInfo(Option_t *option="");
           void      DrawCaption(Option_t *option="");
           void      DrawAxis(Int_t index=0, Option_t *option="");
//         void      UpdateAxis(Int_t index=0, Option_t *option="");
   virtual void      DrawView(Float_t theta=9999, Float_t phi=9999, Int_t index=0);
   virtual void      DrawViewGL();
   virtual void      DrawViewX3D();
//	   void      DoubleSize();
//	   void      HalfSize();
    void AddParticleInfo();
   virtual void      ExecuteEvent(Int_t event, Int_t px, Int_t py);
   virtual Int_t      GetEvent(Int_t event); //*MENU*
// AMSKeyNode       *GetKeyNode() const { return m_KeyNode; }
   TPad             *Pad() {return m_Pad;}
   AMSCanvas        *GetCanvas() { return m_Canvas; }
   TPad             *GetObjInfoPad() { return m_ObjInfoPad; }
   TPad             *GetPartInfoPad() { return m_PartInfoPad; }
   TPad             *GetEventInfoPad() { return m_EventInfoPad; }
   TPad             *GetTitlePad() { return m_TitlePad; }
//   AMSColorManager  *GetColorManager() { return m_ColorManager; }
   virtual void      Paint(Option_t *option="");
   virtual void      PaintParticles(Option_t *option="");
   Float_t           PTcut() {return m_PTcut;}
   Float_t           PTcutEGMUNU() {return m_PTcutEGMUNU;}
   virtual char     *GetObjectInfo(Int_t px, Int_t py);
   virtual void      SetDrawParticles(Bool_t draw=kTRUE) {m_DrawParticles=draw;} // *MENU*
   virtual void      SetPTcut(Float_t ptcut=0.4); // *MENU*
   virtual void      SetPTcutEGMUNU(Float_t ptcut=5); // *MENU*
   virtual void      SetGeometry(TGeometry * geo); // *--MENU*
   void              StartStop();
   void              Print(){m_Canvas->PrintCB();}
   void              SavePS(){m_Canvas->SaveParticleCB();}
   void              SaveGIF(){m_Canvas->SaveParticleGIF();}
   virtual void      SetView(Float_t theta, Float_t phi);
           void      SetView(EAMSView newView=kFrontView);
   virtual void      ShowNextEvent(Int_t delta=1);
   virtual void      DrawEvent();
   virtual void      SizeParticles() const;
   //
   // Geometry related functions
   //
   AMSGeometrySetter * GetGeometrySetter() { return m_GeoSetter; }
           void      SetGeo();
   //      void      OkGeo();

   ClassDef(AMSDisplay, 0)   //Utility class to display AMSRoot outline, tracks,...
			     // Note: class versionID = 0 to inhibit Streamer
			     // because  R__b >> m_Canvas   doesn't compile.
};

#endif
