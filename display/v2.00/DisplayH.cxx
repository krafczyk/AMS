
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSR_Display                                                           //
//                                                                      //
// Utility class to display AMSR_Root outline, tracks, clusters...        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#include <iostream.h>
#include <TROOT.h>
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

#include "AMSR_Display.h"
#include "AMSR_Root.h"
#include "TSwitch.h"
#include "AMSR_SiHitReader.h"
#include "AMSR_Axis.h"
//#include "AMSR_Particle.h"
//#include "AMSR_MCMaker.h"
//#include "AMSR_ClusterMaker.h"
//#include "AMSR_TriggerMaker.h"
//#include "AMSR_Trigger.h"
#include "Debugger.h"


ClassImp(AMSR_Display)




//_____________________________________________________________________________
AMSR_Display::AMSR_Display() : AMSR_VirtualDisplay()
{
   // Default constructor of AMSR_Display
   //

//   m_Particle  = 0;

   m_theapp=0;
   m_View = kFrontView;
//   m_DrawAllViews = kFALSE;
//   m_DrawTwoViews = kFALSE;
   m_Geometry     = 0;
}


//_____________________________________________________________________________
AMSR_Display::AMSR_Display(const char *title, TGeometry * geo) 
   : AMSR_VirtualDisplay()
{
   m_theapp=0;
   // Constructor of AMSR_Display
   //

   gAMSR_Root->SetDisplay(this);

   //
   // Get a color manager
   //
   //m_ColorManager = new AMSR_ColorManager();

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

   // Create display canvas
//   m_Canvas = new TCanvas("Canvas", (char*)title,14,47,740,650);
   m_Canvas = new AMSR_Canvas("Canvas", (Text_t*)title,1024,768);
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
   m_TitlePad = new TPad("TitlePad", "AMSR_Root title", xsep,0.95, 1.0, 1.0);
   m_TitlePad->Draw();
   m_TitlePad->Modified();
   //m_TitlePad->SetFillColor(33);
   m_TitlePad->SetFillColor(33);
   m_TitlePad->SetBorderSize(2);
   m_TitlePad->SetBorderMode(1);
   //
   // Create main display pad
   // ----------------------------
   m_Pad = new TPad("ViewPad", "AMSR_Root display",xsep,0.05,1,0.95);
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
   m_ButtonPad = new TPad("buttons", "newpad",0,0.45,xsep,1.0);
   //m_ButtonPad->SetFillColor(38);
   m_ButtonPad->SetFillColor(13);
   m_ButtonPad->SetBorderSize(2);
   m_ButtonPad->SetBorderMode(-1);
   m_ButtonPad->Draw();
   //
   // Create switch pad
   // ----------------------------
   m_TrigPad = new TPad("TrigPad", "trigger pad",0,0.1,xsep,0.45);
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

<<<<<<< AMSR_Display.cxx
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
   AMSR_Maker * maker;

   Float_t y = 1.0, dy = 0.10, height=0.10;
   maker = (AMSR_Maker *) gAMSR_Root->SiHitMaker();
   sw[0] = new TSwitch("Tracker Hits", &(maker->DrawFruits), 
			"gAMSR_Root->Display()->Draw()", 0.0, y-height, 1.0, y);
   y -= dy;

   AMSR_SiHitReader * tmp = gAMSR_Root->SiHitMaker();
   sw[1] = new TSwitch("Used Hits Only", &(tmp->DrawUsedHitsOnly), 
			"gAMSR_Root->Display()->Draw()", 0.0, y-height, 1.0, y);
   y -= dy;

   maker = (AMSR_Maker *) gAMSR_Root->ToFClusterMaker();
   sw[2] = new TSwitch("ToF Clusters", &(maker->DrawFruits), 
			"gAMSR_Root->Display()->Draw()", 0.0, y-height, 1.0, y);
   y -= dy;

   maker = (AMSR_Maker *) gAMSR_Root->CTCClusterMaker();
   sw[3] = new TSwitch("ATC Clusters", &(maker->DrawFruits), 
			"gAMSR_Root->Display()->Draw()", 0.0, y-height, 1.0, y);
   y -= dy;

   maker = (AMSR_Maker *) gAMSR_Root->AntiClusterMaker();
   sw[4] = new TSwitch("Anti Clusters", &(maker->DrawFruits), 
			"gAMSR_Root->Display()->Draw()", 0.0, y-height, 1.0, y);
   y -= dy;

   maker = (AMSR_Maker *) gAMSR_Root->TrackMaker();
   sw[5] = new TSwitch("Tracks", &(maker->DrawFruits), 
			"gAMSR_Root->Display()->Draw()", 0.0, y-height, 1.0, y);
   y -= dy;

   maker = (AMSR_Maker *) gAMSR_Root->ParticleMaker();
   sw[6] = new TSwitch("Particles", &(maker->DrawFruits), 
			"gAMSR_Root->Display()->Draw()", 0.0, y-height, 1.0, y);
   y -= dy;
    
   sw[7] = new TSwitch("Geometry", &m_DrawGeometry, 
			"gAMSR_Root->Display()->Draw()", 0.0, y-height, 1.0, y);


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
AMSR_Display::~AMSR_Display()
{
//   delete m_Particle;
}

//_____________________________________________________________________________
void AMSR_Display::Clear(Option_t *option)
{
//    Delete graphics temporary objects

}

//_____________________________________________________________________________
void AMSR_Display::DisplayButtons()
{
//    Create the user interface buttons

   m_ButtonPad->cd();

   Int_t butcolor = 33;
   Float_t dbutton = 0.09;
   Float_t y  = 0.96;
   Float_t dy = 0.014;
   Float_t x0 = 0.05;
   Float_t x1 = 0.95;
   TButton *button;
   char *but0 = "gAMSR_Root->Display()->ShowNextEvent(0)";
   button = new TButton("Select",but0,x0,y-dbutton,x1,y);
   button->SetFillColor(38);
   button->Draw();

   y -= dbutton +dy;
   char *but1 = "gAMSR_Root->Display()->ShowNextEvent(1)";
   button = new TButton("Next",but1,x0,y-dbutton,x1,y);
   button->SetFillColor(38);
   button->Draw();

   y -= dbutton +dy;
   char *but2 = "gAMSR_Root->Display()->ShowNextEvent(-1)";
   button = new TButton("Previous",but2,x0,y-dbutton,x1,y);
   button->SetFillColor(38);
   button->Draw();

// enum EAMSR_View { kFrontView=0, kSideView=1, kTopView=2, 
//                 kAllView=3, kTwoView=4 };
   y -= dbutton +dy;
   //char *but3 = "gAMSR_Root->Display()->SetView(0,-90)";
   char *but3 = "gAMSR_Root->Display()->SetView(2)";
   button = new TButton("Top View",but3,x0,y-dbutton,x1,y);
   button->SetFillColor(butcolor);
   button->Draw();

   y -= dbutton +dy;
   //char *but4 = "gAMSR_Root->Display()->SetView(90,-90)";
   char *but4 = "gAMSR_Root->Display()->SetView(1)";
   button = new TButton("Side View",but4,x0,y-dbutton,x1,y);
   button->SetFillColor(butcolor);
   button->Draw();

   y -= dbutton +dy;
   //char *but5 = "gAMSR_Root->Display()->SetView(90,0)";
   char *but5 = "gAMSR_Root->Display()->SetView(0)";
   button = new TButton("Front View",but5,x0,y-dbutton,x1,y);
   button->SetFillColor(butcolor);
   button->Draw();

   y -= dbutton +dy;
   //char *but6 = "gAMSR_Root->Display()->DrawAllViews()";
   char *but6 = "gAMSR_Root->Display()->SetView(3)";
   button = new TButton("All Views",but6,x0,y-dbutton,x1,y);
   button->SetFillColor(butcolor);
   button->Draw();

   y -= dbutton +dy;
   char *but7 = "gAMSR_Root->Display()->SetView(4)";
   button = new TButton("Two Views",but7,x0,y-dbutton,x1,y);
   button->SetFillColor(butcolor);
   button->Draw();

   y -= dbutton +dy;
   char *but8 = "gAMSR_Root->Display()->SetView(20)";
   button = new TButton("Stop Timer",but8,x0,y-dbutton,x1,y);
   button->SetFillColor(butcolor);
   button->Draw();

/*
   y -= dbutton +dy;
   char *but7 = "gAMSR_Root->Display()->DrawViewGL()";
   button = new TButton("OpenGL",but7,x0,y-dbutton,x1,y);
   button->SetFillColor(38);
   button->Draw();
*/

/*
   y -= dbutton +dy;
   char *but8 = "gAMSR_Root->Display()->DrawViewX3D()";
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
   // diamond->AddText("AMSR_Root");
   diamond->AddText(" AMS ");
   // diamond->AddText("... ");
   diamond->AddText(" ");
*/
}



//______________________________________________________________________________
Int_t AMSR_Display::DistancetoPrimitive(Int_t px, Int_t py)
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
void AMSR_Display::Draw(Option_t *option)
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
      debugger.Print("AMSR_Display::Draw() m_Theta = %f, m_Phi = %f", m_Theta, m_Phi);
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
void AMSR_Display::DoubleSize()
{
   TVirtualPad * gPadSave = gPad;
   Float_t xlow, ylow, xup, yup;
   char * name  = m_Canvas->GetName();
   char * title = m_Canvas->GetTitle();
   m_Canvas->GetPadPar(xlow, ylow, xup, yup);
   AMSR_Canvas * old = m_Canvas;
   m_Canvas = new AMSR_Canvas(name, title, xup-xlow, 2*(yup-ylow));
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
void AMSR_Display::HalfSize()
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
void AMSR_Display::DrawAllViews()
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
void AMSR_Display::DrawFrontAndSideViews()
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
void AMSR_Display::DrawTitle(Option_t *option)
{
   AMSR_Display * disp = (AMSR_Display *) gAMSR_Root->Display();
   TPad * pad = disp->GetTitlePad();

   static TText * text=0;
   static char * atext = "Alpha Magnetic Spectrometer";

   AMSR_Maker * p = (AMSR_Maker *) gAMSR_Root->ParticleMaker();
   


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

void AMSR_Display::AddParticleInfo()
{
   AMSR_Display * disp = (AMSR_Display *) gAMSR_Root->Display();
   TPad * pad = disp->GetObjInfoPad();

   static TText * text=0;
   static char  atext[255] = "Alpha Magnetic Spectrometer";

   


   TVirtualPad * gPadSave = gPad;
   pad->cd();
   pad->Clear();

   AMSR_Maker * p = (AMSR_Maker *) gAMSR_Root->ParticleMaker();
   
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
   text->SetTextSize(0.65);
   text->Draw();

   gPadSave->cd();
}



//_____________________________________________________________________________
void AMSR_Display::DrawEventInfo(Option_t *option)
{
   AMSR_Display * disp = (AMSR_Display *) gAMSR_Root->Display();
   TPad * pad = disp->GetEventInfoPad();

   const int kTMAX = 80;
   static TText * text=0;
   static TText * text1=0;
   static char atext[kTMAX];

   TVirtualPad * gPadSave = gPad;
   pad->cd();

   sprintf(atext,"Run %d/ %d",gAMSR_Root->RunNum(), gAMSR_Root->EventNum());
   for (Int_t i=strlen(atext);i<kTMAX-1;i++) atext[i] = ' ';
   atext[kTMAX-1] = 0;

   if (! text) {
	text = new TText(0.04, 0.75, atext);
   }
   else
	text->SetText(0.04, 0.75,atext);

   //   text->SetTextFont(7);
   text->SetTextAlign(12);
   text->SetTextSize(0.3);
   text->Draw();
   sprintf(atext,"%s",gAMSR_Root->GetTime());
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
void AMSR_Display::DrawCaption(Option_t *option)
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
void AMSR_Display::DrawAxis(Int_t index, Option_t * option)
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
     AMSR_Axis * axis[3];
     axis[0] = new AMSR_Axis(1,0,0, "x");
     axis[0]->SetLineColor(2);
     axis[1] = new AMSR_Axis(0,1,0, "y");
     axis[1]->SetLineColor(3);
     axis[2] = new AMSR_Axis(0,0,1, "z");
     axis[2]->SetLineColor(4);

   axis[0]->Draw(option);
   axis[1]->Draw(option);
   axis[2]->Draw(option);

   debugger.Print("AMSR_Display::DrawAxis()  m_Theta = %f, m_Phi = %f\n", m_Theta, m_Phi);

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
void AMSR_Display::DrawView(Float_t theta, Float_t phi, Int_t index)
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
     m_Geometry->Draw();
   }
   else
     view->SetRange(-100.0, -100.0, -120.0, 100.0, 100.0, 120.0);

   //Loop on all makers to add their products to the pad
   TIter next(gAMSR_Root->Makers());
   AMSR_Maker *maker;
   while (maker = (AMSR_Maker*)next()) {
      maker->Draw();
   }

   Int_t iret;
   if ( theta != 9999 && phi != 9999 ) view->SetView(phi, theta, 0, iret);

   DrawAxis(index);

}

//_____________________________________________________________________________
void AMSR_Display::DrawViewGL()
{
//    Draw current view using OPENGL

   TPad *pad = (TPad*)gPad->GetPadSave();
   pad->cd();
   TView *view = pad->GetView();
   if (!view) return;
   pad->x3d("OPENGL");
}

//_____________________________________________________________________________
void AMSR_Display::DrawViewX3D()
{
//    Draw current view using X3D

   TPad *pad = (TPad*)gPad->GetPadSave();
   pad->cd();
   TView *view = pad->GetView();
   if (!view) return;
   pad->x3d();
}

//______________________________________________________________________________
void AMSR_Display::ExecuteEvent(Int_t event, Int_t px, Int_t py)
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
Int_t AMSR_Display::GetEvent(Int_t event)
{
//    Read event in memory

   Int_t ret=gAMSR_Root->GetEvent(event);

   Draw();
   return ret;
}

//_____________________________________________________________________________
char *AMSR_Display::GetObjectInfo(Int_t px, Int_t py)
{
   static char info[80];
   sprintf(info, "%d, %d", px, py);
   return info;
}


//_____________________________________________________________________________
void AMSR_Display::Paint(Option_t *option)
{
//    Paint miscellaneous items

}


//_____________________________________________________________________________
void AMSR_Display::PaintParticles(Option_t *option)
{
//   if (m_DrawParticles) m_Particle->Paint(option);
}

//_____________________________________________________________________________
void AMSR_Display::SetGeometry(TGeometry * geo)
{
//  Set AMS in/out outline parameters

   debugger.Print("in AMSR_Display::SetGeometry(): m_Geometry = %lx, geo = %lx", m_Geometry, geo);
   //if (m_Geometry) delete m_Geometry;
   m_Geometry = geo;
   m_GeoSetter = new AMSR_GeometrySetter(geo);
}

//_____________________________________________________________________________
void AMSR_Display::SetPTcut(Float_t ptcut)
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
void AMSR_Display::SetPTcutEGMUNU(Float_t ptcut)
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
void AMSR_Display::StartStop(){
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
    else m_theapp->SetIdleTimer(4,"");
   text->SetTextAlign(22);
   text->SetTextSize(0.55);
   text->Draw();


}

void AMSR_Display::SetView(EAMSR_View newView)
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
void AMSR_Display::SetView(Float_t theta, Float_t phi)
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
void AMSR_Display::ShowNextEvent(Int_t delta)
{
//  Display (current event_number+delta)
//    delta =  1  shown next event
//    delta = -1 show previous event

  if (delta) {
     Int_t current_event = gAMSR_Root->Event();
     Int_t new_event     = current_event + delta;
     if ( new_event >= 0 ) {
       gAMSR_Root->Clear();
       gAMSR_Root->GetEvent(new_event); 
     }
  }
  else {
       gAMSR_Root->Clear();
       gAMSR_Root->SelectEvent(); 
  }
  m_Pad->cd(); 
  Draw();
}

void AMSR_Display::DrawEvent()
{
  m_Pad->cd(); 
  Draw();
}






//______________________________________________________________________________
void AMSR_Display::SizeParticles() const
{
//   if (m_DrawParticles)  m_Particle->SizeParticles();
}


//______________________________________________________________________________
void AMSR_Display::SetGeo()
{
  this->GetGeometrySetter()->SetGeo();
}

/*
void AMSR_Display::OkGeo()
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
