
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSDisplay                                                           //
//                                                                      //
// Utility class to display AMSRoot outline, tracks, clusters...        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#include <iostream.h>
#include <TROOT.h>
#include <TApplication.h>
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
#include <TButton.h>
#include <TCanvas.h>
#include <TView.h>
#include <TArc.h>
#include <TText.h>
#include <TPaveLabel.h>
#include <TPaveText.h>
#include <TList.h>
#include <TDiamond.h>
#include <TNode.h>
#include <TTUBE.h>
#include <TMath.h>
#include <TPolyLine3D.h>
#include <X3DBuffer.h>
#include <TApplication.h>
#include "AMSDisplay.h"
#include "AMSRoot.h"
#include "TSwitch.h"
#include "AMSSiHitReader.h"
#include "AMSAxis.h"
#include "AMSGeometrySetter.h"
//#include "AMSParticle.h"
//#include "AMSMCMaker.h"
//#include "AMSClusterMaker.h"
//#include "AMSTriggerMaker.h"
//#include "AMSTrigger.h"
#include "Msgbox.h"
#include "Debugger.h"


ClassImp(AMSDisplay)




//_____________________________________________________________________________
AMSDisplay::AMSDisplay() : AMSVirtualDisplay()
{
   // Default constructor of AMSDisplay
   //

//   m_Particle  = 0;

   m_theapp=0;
   m_View = kFrontView;
//   m_DrawAllViews = kFALSE;
//   m_DrawTwoViews = kFALSE;
   m_Geometry     = 0;
}


//_____________________________________________________________________________
AMSDisplay::AMSDisplay(const char *title, TGeometry * geo) 
   : AMSVirtualDisplay()
{
   m_theapp=0;
   // Constructor of AMSDisplay
   //

   gAMSRoot->SetDisplay(this);

   //
   // Get a color manager
   //
   //m_ColorManager = new AMSColorManager();

   //
   // Migrate some colors to higher end to avoid them being overwritten
   //
   //m_ColorManager->ColorTest(0,255);
   //m_ColorManager->Migrate(33, 233);
   //m_ColorManager->ColorTest(0,255);

   // Initialize display default parameters
   SetPTcut();
   SetPTcutEGMUNU();
   SetGeometry(geo);

   // Set front view by default
   m_Theta = 90;
   m_Phi   = -90;
//   m_DrawAllViews  = kFALSE;
//   m_DrawTwoViews  = kFALSE;
//   m_DrawClusters  = kTRUE;
   m_View = kFrontView;
   m_DrawParticles = kTRUE;
   m_DrawGeometry  = kTRUE;
   m_DrawMoreGeometry  = kFALSE;

   // Create display canvas
//   m_Canvas = new TCanvas("Canvas", (char*)title,14,47,740,650);
   m_Canvas = new AMSCanvas("Canvas", (Text_t*)title,1024,768);
   m_Canvas->SetEditable(kIsNotEditable);

   //
   // Create pads on the canvas
   //
   m_Canvas->cd();

   //
   // Logo
   //
   //int const logoSizeX = 127, logoSizeY = 123;
   int const logoSizeX = 130, logoSizeY = 130;
   Int_t logoX0 = m_Canvas->XtoAbsPixel(m_Canvas->GetX1());
   Int_t logoY0 = m_Canvas->YtoAbsPixel(m_Canvas->GetY2()) + logoSizeY;
		// notice the "+" sign.  In AbsPixel large y goes downward
   debugger.Print("logo x,y = %d,%d\n", logoX0,logoY0);
   // Float_t xsep = m_Canvas->AbsPixeltoX(logoSizeX + 4);
   Float_t ysep = m_Canvas->AbsPixeltoY(logoY0 + 4);

   Float_t xsep = 0.2;
   m_Canvas->cd();
   //
   // Create title pad
   // ----------------------------
   m_TitlePad = new TPad("TitlePad", "AMSRoot title", xsep,0.95, 1.0, 1.0);
   m_TitlePad->Draw();
   m_TitlePad->Modified();
   //m_TitlePad->SetFillColor(33);
   m_TitlePad->SetFillColor(33);
   m_TitlePad->SetBorderSize(2);
   m_TitlePad->SetBorderMode(1);
   //
   // Create main display pad
   // ----------------------------
   m_Pad = new TPad("ViewPad", "AMSRoot display",xsep,0.05,1,0.95);
   m_Pad->Modified();
   m_Pad->SetFillColor(0);	//white 
   m_Pad->SetBorderSize(2);
   m_Pad->Draw();
   //
   // Create axis pad
   // ----------------------------
   /*
   for (Int_t i=0; i<4; i++) {
      debugger.Print("::: axis pad # %d\n", i);
      m_AxisPad[i] = new TPad("axis","axis", 0, 0, 0.15, 0.15);
      m_AxisPad[i]->SetFillColor(5);
      m_AxisPad[i]->SetBorderMode(1);
      m_AxisPad[i]->SetBorderSize(1);
   }
   */
   //
   // Create button pad
   // ----------------------------
   m_Canvas->cd();
   m_ButtonPad = new TPad("buttons", "newpad",0,0.4,xsep,1.0);
   //m_ButtonPad->SetFillColor(38);
   m_ButtonPad->SetFillColor(13);
   m_ButtonPad->SetBorderSize(2);
   m_ButtonPad->SetBorderMode(-1);
   m_ButtonPad->Draw();
   //
   // Create switch pad
   // ----------------------------
   m_TrigPad = new TPad("TrigPad", "trigger pad",0,0.1,xsep,0.4);
   //m_TrigPad->SetFillColor(22);
   m_TrigPad->SetFillColor(13);
   m_TrigPad->SetBorderSize(2);
   m_TrigPad->SetBorderMode(-1);
   m_TrigPad->Draw();
   //
   // Create event info pad
   // ----------------------------
   m_EventInfoPad = new TPad("EventInfoPad", "event info pad", 0, 0.0, 0.2, 0.1);
   m_EventInfoPad->SetFillColor(0);
   m_EventInfoPad->SetBorderSize(1);
   m_EventInfoPad->SetBorderMode(1);
   m_EventInfoPad->Draw();
   debugger.Print("EventInfoPad at %d,%d,%d,%d\n", 
         m_EventInfoPad->XtoAbsPixel(0.0), m_EventInfoPad->YtoAbsPixel(0.0), 
         m_EventInfoPad->XtoAbsPixel(1.0), m_EventInfoPad->YtoAbsPixel(1.0) );
   //
   // Create object info pad
   // ----------------------------
   m_Canvas->cd();
   m_ObjInfoPad = new TPad("ObjInfoPad", "object info pad", 0.2, 0, 1, 0.05);
   m_ObjInfoPad->SetFillColor(0);
   m_ObjInfoPad->SetBorderSize(1);
   m_ObjInfoPad->SetBorderMode(2);
   m_ObjInfoPad->Draw();
   debugger.Print("objInfoPad at %d,%d,%d,%d\n", 
         m_ObjInfoPad->XtoAbsPixel(0.0), m_ObjInfoPad->YtoAbsPixel(0.0), 
         m_ObjInfoPad->XtoAbsPixel(1.0), m_ObjInfoPad->YtoAbsPixel(1.0) );

   
   // Create part info pad
   // ----------------------------
   //   m_Canvas->cd();
   //   m_PartInfoPad = new TPad("PartInfoPad", "object info pad", 0.2, 0.05, 1, 0.09);
   //   m_PartInfoPad->SetFillColor(0);
   //   m_PartInfoPad->SetBorderSize(1);
   //   m_PartInfoPad->SetBorderMode(2);
   //   m_PartInfoPad->Draw();
   //   debugger.Print("objInfoPad at %d,%d,%d,%d\n", 
   //         m_PartInfoPad->XtoAbsPixel(0.0), m_ObjInfoPad->YtoAbsPixel(0.0), 
   //         m_PartInfoPad->XtoAbsPixel(1.0), m_ObjInfoPad->YtoAbsPixel(1.0) );



/*
   // Create User Communicator Pad

   //  This overlaps with object info pad!!!!!!!!!!!!!!!!!!!!!!!!!!

<<<<<<< AMSDisplay.cxx
   m_Canvas->cd();
   m_UCPad = new TPad("UserComPad", "User Communication Pad", 0.2, 0, 1, 0.05);
   m_UCPad->SetFillColor(0);
   m_UCPad->SetBorderSize(1);
   m_UCPad->SetBorderMode(2);
   m_UCPad->Draw();
*/

/*
   m_LogoPad = new TPad("LogoPad", "AMS Logo", 0.0, ysep, xsep, 1.0);
   m_LogoPad->SetFillColor(7);
   m_LogoPad->Draw();
   m_LogoPad->cd();
  //  gGXW->ReadGIF(0, 0, "logo/logo-0.25.gif");
  //  char c = fgetc(stdin);
 */  

   m_Canvas->cd();

   // Create user interface control pad
   DisplayButtons();
   debugger.Print("++++ m_Canvas->cd().\n");


   // Create switch pad
   // m_TrigPad->Range(0,0,dxtr,dytr);
   m_TrigPad->cd();

   TSwitch * sw[8];
   AMSMaker * maker;

   Float_t y = 1.0, dy = 0.10, height=0.10;
   maker = (AMSMaker *) gAMSRoot->SiHitMaker();
   sw[0] = new TSwitch("Tracker Hits", &(maker->DrawFruits), 
			"gAMSRoot->Display()->Draw()", 0.0, y-height, 1.0, y);
   y -= dy;

   AMSSiHitReader * tmp = gAMSRoot->SiHitMaker();
   sw[1] = new TSwitch("Used Hits Only", &(tmp->DrawUsedHitsOnly), 
			"gAMSRoot->Display()->Draw()", 0.0, y-height, 1.0, y);
   y -= dy;

   maker = (AMSMaker *) gAMSRoot->ToFClusterMaker();
   sw[2] = new TSwitch("ToF Clusters", &(maker->DrawFruits), 
			"gAMSRoot->Display()->Draw()", 0.0, y-height, 1.0, y);
   y -= dy;

   maker = (AMSMaker *) gAMSRoot->CTCClusterMaker();
   sw[3] = new TSwitch("ATC Clusters", &(maker->DrawFruits), 
			"gAMSRoot->Display()->Draw()", 0.0, y-height, 1.0, y);
   y -= dy;

   maker = (AMSMaker *) gAMSRoot->AntiClusterMaker();
   sw[4] = new TSwitch("Anti Clusters", &(maker->DrawFruits), 
			"gAMSRoot->Display()->Draw()", 0.0, y-height, 1.0, y);
   y -= dy;

   maker = (AMSMaker *) gAMSRoot->TrackMaker();
   sw[5] = new TSwitch("Tracks", &(maker->DrawFruits), 
			"gAMSRoot->Display()->Draw()", 0.0, y-height, 1.0, y);
   y -= dy;

   maker = (AMSMaker *) gAMSRoot->ParticleMaker();
   sw[6] = new TSwitch("Particles", &(maker->DrawFruits), 
			"gAMSRoot->Display()->Draw()", 0.0, y-height, 1.0, y);
   y -= dy;
    
   sw[7] = new TSwitch("Geometry", &m_DrawGeometry, 
			"gAMSRoot->Display()->Draw()", 0.0, y-height, 1.0, y);
   y -= dy;
    
   sw[7] = new TSwitch("More Geometry", &m_DrawMoreGeometry, 
			"gAMSRoot->Display()->Draw()", 0.0, y-height, 1.0, y);


  /*
   TText *t = new TText();
   t->SetTextFont(61);
   t->SetTextSize(0.2);
   t->SetTextAlign(22);
   t->DrawText(0.5*dxtr, 0.93*dytr,"Switches");
   t->SetTextSize(0.14);
   t->SetTextAlign(22);
   t->DrawText(xt,yt,      "Si Cluster");
   t->DrawText(xt,yt-dyt,  "ToF Cluster");
   t->DrawText(xt,yt-2*dyt,"Aerogel");
   t->DrawText(xt,yt-3*dyt,"Tracks");
   t->DrawText(xt,yt-4*dyt,"Particles");
   t->DrawText(xt,yt-5*dyt,"---");
   t->DrawText(xt,yt-6*dyt,"---");
   t->DrawText(xt,yt-7*dyt,"---");
   t->DrawText(xt,yt-8*dyt,"---");
   t->DrawText(xt,yt-9*dyt,"---");
   AppendPad(); // append display object as last object to force selection
  */
   // Create list to support list of fruits

   // Create particle manager
//   m_Particle = new ATLFParticle("particle_manager");

   debugger.Print("++++ m_Canvas->cd() again.\n");
   m_Canvas->cd();
   debugger.Print("++++ m_Canvas->Update().\n");
   m_Canvas->Update();
   debugger.Print("++++ after m_Canvas->Update().\n");

}


//_____________________________________________________________________________
AMSDisplay::~AMSDisplay()
{
//   delete m_Particle;
}

//_____________________________________________________________________________
void AMSDisplay::Clear(Option_t *option)
{
//    Delete graphics temporary objects

}

//_____________________________________________________________________________
void AMSDisplay::DisplayButtons()
{
//    Create the user interface buttons

   m_ButtonPad->cd();

   Int_t butcolor = 33;
   Float_t dbutton = 0.075;
   Float_t y  = 0.99;
   Float_t dy = 0.007;
   Float_t x0 = 0.05;
   Float_t x1 = 0.95;
   TButton *button;
   char *but0 = "gAMSRoot->Display()->Select()";
   button = new TButton("Select",but0,x0,y-dbutton,x1,y);
   button->SetFillColor(38);
   button->Draw();

   y -= dbutton +dy;
   char *but1 = "gAMSRoot->Display()->ShowNextEvent(1)";
   button = new TButton("Next",but1,x0,y-dbutton,x1,y);
   button->SetFillColor(38);
   button->Draw();

   y -= dbutton +dy;
   char *but2 = "gAMSRoot->Display()->ShowNextEvent(-1)";
   button = new TButton("Previous",but2,x0,y-dbutton,x1,y);
   button->SetFillColor(38);
   button->Draw();

// enum EAMSView { kFrontView=0, kSideView=1, kTopView=2, 
//                 kAllView=3, kTwoView=4 };
   y -= dbutton +dy;
   //char *but3 = "gAMSRoot->Display()->SetView(0,-90)";
   char *but3 = "gAMSRoot->Display()->SetView(2)";
   button = new TButton("Top View",but3,x0,y-dbutton,x1,y);
   button->SetFillColor(butcolor);
   button->Draw();

   y -= dbutton +dy;
   //char *but4 = "gAMSRoot->Display()->SetView(90,-90)";
   char *but4 = "gAMSRoot->Display()->SetView(1)";
   button = new TButton("Side View",but4,x0,y-dbutton,x1,y);
   button->SetFillColor(butcolor);
   button->Draw();

   y -= dbutton +dy;
   //char *but5 = "gAMSRoot->Display()->SetView(90,0)";
   char *but5 = "gAMSRoot->Display()->SetView(0)";
   button = new TButton("Front View",but5,x0,y-dbutton,x1,y);
   button->SetFillColor(butcolor);
   button->Draw();

   y -= dbutton +dy;
   //char *but6 = "gAMSRoot->Display()->DrawAllViews()";
   char *but6 = "gAMSRoot->Display()->SetView(3)";
   button = new TButton("All Views",but6,x0,y-dbutton,x1,y);
   button->SetFillColor(butcolor);
   button->Draw();

   y -= dbutton +dy;
   char *but7 = "gAMSRoot->Display()->SetView(4)";
   button = new TButton("Two Views",but7,x0,y-dbutton,x1,y);
   button->SetFillColor(butcolor);
   button->Draw();

   y -= dbutton +dy;
   char *but8 = "gAMSRoot->Display()->StartStop()";
   button = new TButton("Stop Timer",but8,x0,y-dbutton,x1,y);
   button->SetFillColor(butcolor);
   button->Draw();


   y -= dbutton +dy;
   char *but9 = "gAMSRoot->Display()->Print()";
   button = new TButton("Print",but9,x0,y-dbutton,x1,y);
   button->SetFillColor(butcolor);
   button->Draw();

   y -= dbutton +dy;
   char *but10 = "gAMSRoot->Display()->SavePS()";
   button = new TButton("Save as .ps",but10,x0,y-dbutton,x1,y);
   button->SetFillColor(butcolor);
   button->Draw();

   y -= dbutton +dy;
   char *but11 = "gAMSRoot->Display()->SaveGIF()";
   button = new TButton("Save as .gif",but11,x0,y-dbutton,x1,y);
   button->SetFillColor(butcolor);
   button->Draw();
/*
   y -= dbutton +dy;
   char *but8 = "gAMSRoot->Display()->DrawViewX3D()";
   button = new TButton("X3D",but8,x0,y-dbutton,x1,y);
   button->SetFillColor(38);
   button->Draw();
*/

/*
   // display logo
   TDiamond *diamond = new TDiamond(0.05,0.015,0.95,0.22);
   diamond->SetFillColor(50);
   diamond->SetTextAlign(22);
   diamond->SetTextColor(5);
   diamond->SetTextSize(0.11);
   diamond->Draw();
   // diamond->AddText(".. ");
   // diamond->AddText("ROOT");
   // diamond->AddText("AMSRoot");
   diamond->AddText(" AMS ");
   // diamond->AddText("... ");
   diamond->AddText(" ");
*/
}



//______________________________________________________________________________
Int_t AMSDisplay::DistancetoPrimitive(Int_t px, Int_t py)
{
// Compute distance from point px,py to objects in event

   if (gPad == m_TrigPad) {gPad->SetCursor(kCross); return 0;}

   const Int_t big = 9999;
   const Int_t inview = 0;
   Int_t dist = big;
   Float_t xmin = gPad->GetX1();
   Float_t xmax = gPad->GetX2();
   Float_t dx   = 0.05*(xmax - xmin);
   Float_t x    = gPad->AbsPixeltoX(px);
   if (x < xmin+dx || x > xmax-dx) return dist;

    // scan list of particles
//   dist = m_Particle->DistancetoPrimitive(px, py);
//   if (dist <= 0) return 0;


   return inview;
}

//_____________________________________________________________________________
void AMSDisplay::Draw(Option_t *option)
{
//    Insert current event in graphics pad list

   m_Canvas->SetEditable(kIsNotEditable);

   DrawTitle();
   AddParticleInfo();
   DrawEventInfo();
   m_Pad->cd();
   if ( m_View == kAllView ) {
      DrawAllViews();
      return;
   }
   else if ( m_View == kTwoView) {
      DrawFrontAndSideViews();
      return;
   }
   else {
      debugger.Print("AMSDisplay::Draw() m_Theta = %f, m_Phi = %f", m_Theta, m_Phi);
      DrawView(m_Theta,m_Phi);
   }

   m_Canvas->cd();
   
   static TLine * deco[2]={0,0};

   if ( deco[0] == 0 ) {
     deco[0] = new TLine(0.0,0.0,1.0,0.0);
     deco[0]->SetLineWidth(2);
   }
   if ( deco[1] == 0 ) {
     deco[1] = new TLine(1.0,0.0,1.0,1.0);
     deco[1]->SetLineWidth(2);
   }
   deco[0]->Draw();
   deco[1]->Draw();

}


/*
//_____________________________________________________________________________
void AMSDisplay::DoubleSize()
{
   TVirtualPad * gPadSave = gPad;
   Float_t xlow, ylow, xup, yup;
   char * name  = m_Canvas->GetName();
   char * title = m_Canvas->GetTitle();
   m_Canvas->GetPadPar(xlow, ylow, xup, yup);
   AMSCanvas * old = m_Canvas;
   m_Canvas = new AMSCanvas(name, title, xup-xlow, 2*(yup-ylow));
   // old->Clear();
   // delete old;
   m_Canvas->cd();

   // m_Pad->cd();
*/
  /*
   m_Pad->GetCanvas()->GetPadPar(xlow,ylow,xup,yup);
   xup = xlow + (xup-xlow) * 2;
   m_Pad->GetCanvas()->SetPad(xlow,ylow,xup,yup);

   m_Pad->GetPadPar(xlow, ylow, xup, yup);
   xup = xlow + (xup-xlow) * 2;
   m_Pad->SetPad(xlow, ylow, xup, yup);
  */
/*
   m_Pad->Draw();

   gPadSave->cd();
}

//_____________________________________________________________________________
void AMSDisplay::HalfSize()
{
   TVirtualPad * gPadSave = gPad;
   m_Pad->cd();

   Float_t xlow, ylow, xup, yup;
   m_Pad->GetCanvas()->GetPadPar(xlow,ylow,xup,yup);
   xup = xlow + (xup-xlow) / 2;
   m_Pad->GetCanvas()->SetPad(xlow,ylow,xup,yup);

   m_Pad->GetPadPar(xlow, ylow, xup, yup);
   xup = xlow + (xup-xlow) / 2;
   m_Pad->SetPad(xlow, ylow, xup, yup);

   gPadSave->cd();
}
*/

//_____________________________________________________________________________
void AMSDisplay::DrawAllViews()
{
//    Draw front,top,side and 30 deg views

//   m_DrawAllViews = kTRUE;
   m_Pad->cd();
   m_Pad->SetFillColor(15);
   m_Pad->Clear();
   m_Pad->Divide(2,2);

   // draw 30 deg view
   m_Pad->cd(1);
   DrawView(30, 30, 0);
   DrawCaption();

   // draw top view
   m_Pad->cd(2);
   DrawView(0, -90, 1);
   DrawCaption("Top");

   // draw front view
   m_Pad->cd(3);
   DrawView(90, 0, 2);
   DrawCaption("Front");

   // draw side view
   m_Pad->cd(4);
   DrawView(90, -90, 3);
   DrawCaption("Side");

   m_Pad->cd(2);
}

//_____________________________________________________________________________
void AMSDisplay::DrawFrontAndSideViews()
{
//    Draw front,top,side and 30 deg views

//   m_DrawAllViews = kFALSE;
   m_Pad->cd();
   m_Pad->SetFillColor(15);
   m_Pad->Clear();
   m_Pad->Divide(2,1);

   // draw front view
   m_Pad->cd(1);
   DrawView(90, 0, 0);
   DrawCaption("Front");

   // draw side view
   m_Pad->cd(2);
   DrawView(90, -90, 1);
   DrawCaption("Side");

   m_Pad->cd(1);
}


//_____________________________________________________________________________
void AMSDisplay::DrawTitle(Option_t *option)
{
   AMSDisplay * disp = (AMSDisplay *) gAMSRoot->Display();
   TPad * pad = disp->GetTitlePad();

   static TText * text=0;
   static char * atext = "Alpha Magnetic Spectrometer Event Display";

   AMSMaker * p = (AMSMaker *) gAMSRoot->ParticleMaker();
   


   TVirtualPad * gPadSave = gPad;
   pad->cd();

   if (! text) {
	text = new TText(0.5, 0.5, atext);
   }
   else
	text->SetText(0.5, 0.5, atext);

   text->SetTextFont(7);
   text->SetTextAlign(22);
   text->SetTextSize(0.65);
   text->Draw();

   gPadSave->cd();
}

void AMSDisplay::AddParticleInfo()
{
   AMSDisplay * disp = (AMSDisplay *) gAMSRoot->Display();
   TPad * pad = disp->GetObjInfoPad();

   static TText * text=0;
   static char  atext[255] = "Alpha Magnetic Spectrometer";

   


   TVirtualPad * gPadSave = gPad;
   pad->cd();
   pad->Clear();

   AMSMaker * p = (AMSMaker *) gAMSRoot->ParticleMaker();
   
  TObject *fruits = p->Fruits();
  TObject *obj;
  char * info=0;

// If m_Fruits is a ClonesArray, insert all the objects in the list
// of objects to be painted

  if (fruits->InheritsFrom("TClonesArray")) {
     TClonesArray *clones = (TClonesArray*)fruits;
     Int_t nobjects = clones->GetEntries();
     for (Int_t i=0;i<nobjects;i++) {
        obj = clones->At(i);
        if (obj && i==0){ 
           debugger.Print("obj class = %s\n", obj->ClassName());}
        if (obj && p->Enabled(obj)) 
           info = obj->GetObjectInfo(0,0);
     }
// m_Fruits points to an object in general. Insert this object in the pad
  }
  else {
     info = fruits->GetObjectInfo(0,0);
  }

  debugger.Print("get particle info: %s\n", info);
  atext[0]=0;
  sprintf(atext, "Particle: %s", info);



   if (! text) {
	text = new TText(0.5, 0.5, atext);
   }
   else
	text->SetText(0.5, 0.5, atext);

   text->SetTextFont(7);
   text->SetTextAlign(22);
   text->SetTextSize(0.45);
   text->Draw();

   gPadSave->cd();
}



//_____________________________________________________________________________
void AMSDisplay::DrawEventInfo(Option_t *option)
{
   AMSDisplay * disp = (AMSDisplay *) gAMSRoot->Display();
   TPad * pad = disp->GetEventInfoPad();

   const int kTMAX = 80;
   static TText * text=0;
   static TText * text1=0;
   static char atext[kTMAX];

   TVirtualPad * gPadSave = gPad;
   pad->cd();

   sprintf(atext,"Run %d/ %d",gAMSRoot->RunNum(), gAMSRoot->EventNum());
   for (Int_t i=strlen(atext);i<kTMAX-1;i++) atext[i] = ' ';
   atext[kTMAX-1] = 0;

   if (! text) {
	text = new TText(0.04, 0.75, atext);
   }
   else
	text->SetText(0.04, 0.75,atext);

   //   text->SetTextFont(7);
   text->SetTextAlign(12);
   text->SetTextSize(0.28);
   text->Draw();
   sprintf(atext,"%s",gAMSRoot->GetTime());
    for ( i=strlen(atext);i<kTMAX-1;i++) atext[i] = ' ';
      atext[kTMAX-1] = 0;
   
      if (! text1) {
   	text1 = new TText(0.04, 0.25, atext);
      }
      else
   	text1->SetText(0.04, 0.25,atext);
   
      //   text->SetTextFont(7);
      text1->SetTextAlign(12);
      text1->SetTextSize(0.22);
      text1->Draw();

   gPadSave->cd();
}


//_____________________________________________________________________________
void AMSDisplay::DrawCaption(Option_t *option)
{
//    Draw the event title

   Float_t xmin = gPad->GetX1();
   Float_t xmax = gPad->GetX2();
   Float_t ymin = gPad->GetY1();
   Float_t ymax = gPad->GetY2();
   Float_t dx   = xmax-xmin;
   Float_t dy   = ymax-ymin;

   if (strlen(option) == 0) {

   } else {
      TPaveLabel *label = new TPaveLabel(xmin +0.01*dx, ymax-0.15*dy, xmin +0.25*dx, ymax-0.01*dy,option);
      label->SetBit(kCanDelete);
      label->SetFillColor(42);
      label->Draw();
   }
}


//_____________________________________________________________________________
void AMSDisplay::DrawAxis(Int_t index, Option_t * option)
{
   //
   // Draw x-y-z axes in lowerleft corner of current pad
   //
   // This appends the axisPad to the current pad and then updates the axis


   TPad * axisPad = new TPad("axis","axis", 0.0, 0.0, 0.15, 0.15);
					// this will be deleted when
					// the parent pad calls Clear()

   m_AxisPad[index] = axisPad;		// record it with the display
					// for future reference

   axisPad->SetBorderSize(1);
   axisPad->SetBorderMode(1);
   axisPad->Draw();	// append to current pad

   TVirtualPad * gPadSave = gPad;
   axisPad->cd();

   //
   // axes
   //
     AMSAxis * axis[3];
     axis[0] = new AMSAxis(1,0,0, "x");
     axis[0]->SetLineColor(2);
     axis[1] = new AMSAxis(0,1,0, "y");
     axis[1]->SetLineColor(3);
     axis[2] = new AMSAxis(0,0,1, "z");
     axis[2]->SetLineColor(4);

   axis[0]->Draw(option);
   axis[1]->Draw(option);
   axis[2]->Draw(option);

   debugger.Print("AMSDisplay::DrawAxis()  m_Theta = %f, m_Phi = %f\n", m_Theta, m_Phi);

   TView * axisView = new TView(1);
   axisView->SetRange(-1.5,-1.5,-1.5, 1.5,1.5,1.5);
   debugger.Print("SetView()\n");
   Int_t iret;
   //axisView->SetView(m_Theta, m_Phi, 0, iret);
   //axisView->SetView(m_Theta, -m_Phi, 0, iret);		// kludge
   //axisPad->Modified();

   //
   // Force update the drawing
   //
   m_AxisPad[index]->Modified();
   m_AxisPad[index]->Update();

   gPadSave->cd();

}

//_____________________________________________________________________________
void AMSDisplay::DrawView(Float_t theta, Float_t phi, Int_t index)
{
//    Draw a view of AMS

   static TPaveText * angle=0;

   gPad->SetFillColor(10);	//white for easy printing
   gPad->Clear();


    // add itself to the list
   AppendPad();

   TView *view = new TView(1);
   // add the geomtry to the pad
   if (m_DrawGeometry) {
     view->SetRange(-800.0, -800.0, -520.0, 800.0, 800.0, 520.0);
     EVisibility vis;
     if(m_DrawMoreGeometry)vis=kDrawAll;
     else vis=kDrawNone;
     TNode * node=m_Geometry->GetNode("ANTI_SUPP_TUBE");
     if(node)node->SetVisibility(vis);
     node=m_Geometry->GetNode("STK_L1_HONEYCOMB");
     if(node)node->SetVisibility(vis);
     node=m_Geometry->GetNode("STK_L2_HONEYCOMB");
     if(node)node->SetVisibility(vis);
     node=m_Geometry->GetNode("STK_L3_HONEYCOMB");
     if(node)node->SetVisibility(vis);
     node=m_Geometry->GetNode("STK_L4_HONEYCOMB");
     if(node)node->SetVisibility(vis);
     node=m_Geometry->GetNode("STK_L5_HONEYCOMB");
     if(node)node->SetVisibility(vis);
     node=m_Geometry->GetNode("STK_L6_HONEYCOMB");
     if(node)node->SetVisibility(vis);
     m_Geometry->Draw();
   }
   else
     view->SetRange(-100.0, -100.0, -120.0, 100.0, 100.0, 120.0);

   //Loop on all makers to add their products to the pad
   TIter next(gAMSRoot->Makers());
   AMSMaker *maker;
   while (maker = (AMSMaker*)next()) {
      maker->Draw();
   }

   Int_t iret;
   if ( theta != 9999 && phi != 9999 ) view->SetView(phi, theta, 0, iret);

   DrawAxis(index);

}

//_____________________________________________________________________________
void AMSDisplay::DrawViewGL()
{
//    Draw current view using OPENGL

   TPad *pad = (TPad*)gPad->GetPadSave();
   pad->cd();
   TView *view = pad->GetView();
   if (!view) return;
   pad->x3d("OPENGL");
}

//_____________________________________________________________________________
void AMSDisplay::DrawViewX3D()
{
//    Draw current view using X3D

   TPad *pad = (TPad*)gPad->GetPadSave();
   pad->cd();
   TView *view = pad->GetView();
   if (!view) return;
   pad->x3d();
}

//______________________________________________________________________________
void AMSDisplay::ExecuteEvent(Int_t event, Int_t px, Int_t py)
{
//*-*-*-*-*-*-*-*-*-*-*Execute action corresponding to one event*-*-*-*
//*-*                  =========================================

   if (gPad->GetView()) {
      gPad->GetView()->ExecuteRotateView(event, px, py);
      // + Ping Yeh
      if (event == kButton1Up) {
        debugger.Print("(lattitude, longitude) = (%f,%f)\n", 
	  gPad->GetView()->GetLatitude(), gPad->GetView()->GetLongitude());
	m_Theta = gPad->GetView()->GetLatitude();
	m_Phi   = gPad->GetView()->GetLongitude();
	SetView(m_Theta, m_Phi);
      }
      // - Ping Yeh
   }
}

//_____________________________________________________________________________
Int_t AMSDisplay::GetEvent(Int_t event)
{
//    Read event in memory

   Int_t ret=gAMSRoot->GetEvent(event);

   Draw();
   return ret;
}

//_____________________________________________________________________________
char *AMSDisplay::GetObjectInfo(Int_t px, Int_t py)
{
   static char info[80];
   sprintf(info, "%d, %d", px, py);
   return info;
}


//_____________________________________________________________________________
void AMSDisplay::Paint(Option_t *option)
{
//    Paint miscellaneous items

}


//_____________________________________________________________________________
void AMSDisplay::PaintParticles(Option_t *option)
{
//   if (m_DrawParticles) m_Particle->Paint(option);
}

//_____________________________________________________________________________
void AMSDisplay::SetGeometry(TGeometry * geo)
{
//  Set AMS in/out outline parameters

   debugger.Print("in AMSDisplay::SetGeometry(): m_Geometry = %lx, geo = %lx", m_Geometry, geo);
   //if (m_Geometry) delete m_Geometry;
   m_Geometry = geo;
   m_GeoSetter = new AMSGeometrySetter(geo);
}

//_____________________________________________________________________________
void AMSDisplay::SetPTcut(Float_t ptcut)
{
   m_PTcut = ptcut;

   if (m_View == kAllView) {
      m_Pad->cd(1); gPad->Modified();
      m_Pad->cd(2); gPad->Modified();
      m_Pad->cd(3); gPad->Modified();
      m_Pad->cd(4); gPad->Modified();
      m_Pad->cd();
   }
}

//_____________________________________________________________________________
void AMSDisplay::SetPTcutEGMUNU(Float_t ptcut)
{
   m_PTcutEGMUNU = ptcut;

   if (m_View == kAllView) {
      m_Pad->cd(1); gPad->Modified();
      m_Pad->cd(2); gPad->Modified();
      m_Pad->cd(3); gPad->Modified();
      m_Pad->cd(4); gPad->Modified();
      m_Pad->cd();
   }
}

//_____________________________________________________________________________
void AMSDisplay::StartStop(){
  if(!m_theapp)return;
  gPad->Clear();
  static int state=0;
  state=(state+1)%2;
   static TText * text=0;
   static char atext2[20]="Stop Timer";
   static char atext1[20]="Start Timer";

   if (! text) {
	if(state%2)text = new TText(0.5, 0.5, atext1);
	else text = new TText(0.5, 0.5, atext2);
   }
   else{
	if(state%2)text->SetText(0.5, 0.5, atext1);
	else text = new TText(0.5, 0.5, atext2);
   }
    if(state%2)m_theapp->RemoveIdleTimer();
    else m_theapp->SetIdleTimer(6,"");
   text->SetTextAlign(22);
   text->SetTextSize(0.55);
   text->Draw();


}

void AMSDisplay::SetView(EAMSView newView)
{
 /*
   //
   // first check if we need to resize the pad
   //
   if (newView == kTwoView && m_View != kTwoView) {
      DoubleSize();
   }
   else if (m_View == kTwoView && newView != kTwoView) {
      HalfSize();
   }
 */
   m_View = newView;
   switch (m_View) {
     case kFrontView:  SetView(90,0);   break;
     case kSideView:   SetView(90,-90); break;
     case kTopView:    SetView(0,0);    break;
     case kAllView:    DrawAllViews();  break;
     case 20:          StartStop(); m_View=kTwoView;break;
     case kTwoView:    DrawFrontAndSideViews();  break;
     default:          SetView(90,0);   break;
   }
}

//_____________________________________________________________________________
void AMSDisplay::SetView(Float_t theta, Float_t phi)
{
//  change viewing angles for current event

   TVirtualPad * gPadSave = gPad;

   m_Pad->cd();
   m_Phi   = phi;
   m_Theta = theta;
   Int_t iret;

   TView *view = gPad->GetView();
   if (view) view->SetView(m_Phi, m_Theta, 0, iret);
   else      Draw();
   gPad->Modified();

   m_AxisPad[0]->cd();
   view = gPad->GetView();
   if (view) view->SetView(m_Phi, m_Theta, 0, iret);
   else      Draw();
   gPad->Modified();

   gPadSave->cd();
}

//_____________________________________________________________________________
void AMSDisplay::ShowNextEvent(Int_t delta)
{
//  Display (current event_number+delta)
//    delta =  1  shown next event
//    delta = -1 show previous event

     Int_t current_event = gAMSRoot->Event();
     Int_t new_event     = current_event + delta;
     if ( new_event >= 0 ) {
       gAMSRoot->Clear();
       gAMSRoot->GetEvent(new_event); 
     }
  m_Pad->cd(); 
  Draw();
}

void AMSDisplay::Select(){
   new MsgBox(gClient->GetRoot(), m_Canvas->fTheCanvas, 400, 200);

       gAMSRoot->Clear();
       gAMSRoot->SelectEvent(); 
       m_Pad->cd(); 
       Draw();

}

void AMSDisplay::DrawEvent()
{
  m_Pad->cd(); 
  Draw();
}






//______________________________________________________________________________
void AMSDisplay::SizeParticles() const
{
//   if (m_DrawParticles)  m_Particle->SizeParticles();
}


//______________________________________________________________________________
void AMSDisplay::SetGeo()
{
  this->GetGeometrySetter()->SetGeo();
}

/*
void AMSDisplay::OkGeo()
{
  debugger.Print("delete okButton = %lx\n", okButton);
  if (okButton) delete okButton;
  for (Int_t i = 0; i<nToggle; i++) {
    if (toggle[i].sw) delete toggle[i].sw;
    if (toggle[i].name ) delete toggle[i].name;
  }
  debugger.Print("delete dialog = %lx\n", dialog);
  if (dialog) delete dialog;
  gPadSave->cd(); 
}
*/
