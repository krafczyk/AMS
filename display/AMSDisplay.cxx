
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSDisplay                                                           //
//                                                                      //
// Utility class to display AMSRoot outline, tracks, clusters...        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

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
#include <X3DBuffer.h>

#include "AMSDisplay.h"
#include "AMSRoot.h"
#include "TSwitch.h"
#include "AMSSiHitReader.h"
//#include "AMSParticle.h"
//#include "AMSMCMaker.h"
//#include "AMSClusterMaker.h"
//#include "AMSTriggerMaker.h"
//#include "AMSTrigger.h"
#include "Debugger.h"


ClassImp(AMSDisplay)



/*
//_____________________________________________________________________________
AMSDisplay::AMSDisplay() : AMSVirtualDisplay()
{
   // Default constructor of AMSDisplay
   //

//   m_Particle  = 0;

   m_View = kFrontView;
//   m_DrawAllViews = kFALSE;
//   m_DrawTwoViews = kFALSE;
   m_Geometry     = 0;
}
*/

//_____________________________________________________________________________
AMSDisplay::AMSDisplay(const char *title, TGeometry * geo) 
   : AMSVirtualDisplay()
{
   // Constructor of AMSDisplay
   //

   gAMSRoot->SetDisplay(this);

   //
   // Get a color manager
   //
   m_ColorManager = new AMSColorManager();

   //
   // Migrate some colors to higher end to avoid them being overwritten
   //
   //m_ColorManager->ColorTest(0,255);
   m_ColorManager->Migrate(33, 233);
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

   // Create display canvas
//   m_Canvas = new TCanvas("Canvas", (char*)title,14,47,740,650);
   m_Canvas = new AMSCanvas("Canvas", (Text_t*)title);
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
   m_TitlePad = new TPad("TitlePad", "AMSRoot title", xsep,0.9, 1.0, 1.0);
   m_TitlePad->Draw();
   m_TitlePad->Modified();
   //m_TitlePad->SetFillColor(33);
   m_TitlePad->SetFillColor(33);
   m_TitlePad->SetBorderSize(2);
   m_TitlePad->SetBorderMode(1);
   //
   // Create main display pad
   // ----------------------------
   m_Pad = new TPad("ViewPad", "AMSRoot display",xsep,0.05,1,0.9);
   m_Pad->Modified();
   m_Pad->SetFillColor(0);	//white 
   m_Pad->SetBorderSize(2);
   m_Pad->Draw();
   //
   // Create button pad
   // ----------------------------
   m_Buttons = new TPad("buttons", "newpad",0,0.45,xsep,1.0);
   //m_Buttons->SetFillColor(38);
   m_Buttons->SetFillColor(13);
   m_Buttons->SetBorderSize(2);
   m_Buttons->SetBorderMode(-1);
   m_Buttons->Draw();
   //
   // Create switch pad
   // ----------------------------
   m_TrigPad = new TPad("TrigPad", "trigger pad",0,0.05,xsep,0.45);
   //m_TrigPad->SetFillColor(22);
   m_TrigPad->SetFillColor(13);
   m_TrigPad->SetBorderSize(2);
   m_TrigPad->SetBorderMode(-1);
   m_TrigPad->Draw();
   //
   // Create event info pad
   // ----------------------------
   m_EventInfoPad = new TPad("EventInfoPad", "event info pad", 0, 0, 0.2, 0.05);
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

/*
   m_LogoPad = new TPad("LogoPad", "AMS Logo", 0.0, ysep, xsep, 1.0);
   m_LogoPad->SetFillColor(7);
   m_LogoPad->Draw();
   m_LogoPad->cd();
  //  gGXW->ReadGIF(0, 0, "logo/logo-0.25.gif");
  //  char c = fgetc(stdin);
 */  

   m_Canvas->cd();

   /*
   //
   // Create decoration pad
   //
   TPad * tmpPad[2];
   Float_t y2 = m_Canvas->AbsPixeltoY(m_Canvas->YtoAbsPixel(0.0)+2);
   Float_t x1 = m_Canvas->AbsPixeltoX(m_Canvas->XtoAbsPixel(1.0)-2);
   tmpPad[0] = new TPad("tmp1","tmp1", 0,0,1,0.01);
   tmpPad[0]->SetFillColor(0);
   tmpPad[0]->SetBorderSize(2);
   tmpPad[0]->SetBorderMode(-1);
   tmpPad[0]->Draw();
   tmpPad[1] = new TPad("tmp2","tmp2", x1,0,1,0.05);
   tmpPad[1]->SetFillColor(0);
   tmpPad[1]->SetBorderSize(2);
   tmpPad[1]->SetBorderMode(-1);
   tmpPad[1]->Draw();
   */

   // Create user interface control pad
   DisplayButtons();
   debugger.Print("++++ m_Canvas->cd().\n");


   // Create switch pad
   // m_TrigPad->Range(0,0,dxtr,dytr);
   m_TrigPad->cd();

   TSwitch * sw[7];
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

   maker = (AMSMaker *) gAMSRoot->TrackMaker();
   sw[4] = new TSwitch("Tracks", &(maker->DrawFruits), 
			"gAMSRoot->Display()->Draw()", 0.0, y-height, 1.0, y);
   y -= dy;

   maker = (AMSMaker *) gAMSRoot->ParticleMaker();
   sw[5] = new TSwitch("Particles", &(maker->DrawFruits), 
			"gAMSRoot->Display()->Draw()", 0.0, y-height, 1.0, y);
   y -= dy;

   sw[6] = new TSwitch("Geometry", &m_DrawGeometry, 
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

   m_Buttons->cd();

   Int_t butcolor = 33;
   Float_t dbutton = 0.09;
   Float_t y  = 0.96;
   Float_t dy = 0.014;
   Float_t x0 = 0.05;
   Float_t x1 = 0.95;

   TButton *button;
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

/*
   y -= dbutton +dy;
   char *but7 = "gAMSRoot->Display()->DrawViewGL()";
   button = new TButton("OpenGL",but7,x0,y-dbutton,x1,y);
   button->SetFillColor(38);
   button->Draw();
*/

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
      DrawView(m_Theta,m_Phi);
   }

/*
   // Display the title
//   char c = fgetc(stdin);
   DrawCaption();

   m_Pad->cd();
   DrawView(m_Theta, m_Phi);
   debugger.Print("just finished DrawView();\n");
//   c = fgetc(stdin);

   DrawEventInfo();
   debugger.Print("just finished DrawEventInfo();\n");
*/
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
   DrawView(30, 30);
   DrawCaption();

   // draw top view
   m_Pad->cd(2);
   DrawView(0, -90);
   DrawCaption("Top");

   // draw front view
   m_Pad->cd(3);
   DrawView(90, 0);
   DrawCaption("Front");

   // draw side view
   m_Pad->cd(4);
   DrawView(90, -90);
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
   DrawView(90, 0);
   DrawCaption("Front");

   // draw side view
   m_Pad->cd(2);
   DrawView(90, -90);
   DrawCaption("Side");

   m_Pad->cd(1);
}


//_____________________________________________________________________________
void AMSDisplay::DrawTitle(Option_t *option)
{
   AMSDisplay * disp = (AMSDisplay *) gAMSRoot->Display();
   TPad * pad = disp->GetTitlePad();

   static TText * text=0;
   static char * atext = "Alpha Magnetic Spectrometer";

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

//_____________________________________________________________________________
void AMSDisplay::DrawEventInfo(Option_t *option)
{
   AMSDisplay * disp = (AMSDisplay *) gAMSRoot->Display();
   TPad * pad = disp->GetEventInfoPad();

   const int kTMAX = 40;
   static TText * text=0;
   static char atext[kTMAX];

   TVirtualPad * gPadSave = gPad;
   pad->cd();

   sprintf(atext,"Run %d/Event %d",gAMSRoot->RunNum(), gAMSRoot->EventNum());
   for (Int_t i=strlen(atext);i<kTMAX-1;i++) atext[i] = ' ';
   atext[kTMAX-1] = 0;

   if (! text) {
	text = new TText(0.04, 0.5, atext);
   }
   else
	text->SetText(0.04, 0.5,atext);

   text->SetTextAlign(12);
   text->SetTextSize(0.60);
   text->Draw();

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
void AMSDisplay::DrawView(Float_t theta, Float_t phi)
{
//    Draw a view of AMS

   static TPaveText * angle=0;

   gPad->SetFillColor(10);	//white for easy printing
   gPad->Clear();

   Int_t iret;
   TView *view = new TView(1);
// view->SetRange(-m_Rin, -m_Rin, -m_Zin, m_Rin, m_Rin, m_Zin);
// temporary kludge
   view->SetRange(-100.0, -100.0, -120.0, 100.0, 100.0, 120.0);

    // add itself to the list
   AppendPad();
   
   TView * tmpView = gPad->GetView();
   debugger.Print("gPad->GetView() = %lx\n", tmpView);
   if (tmpView) debugger.Print("gPad->GetView()->GetAutoRange() = %d\n",
		   tmpView->GetAutoRange());

   // add the geomtry to the pad
   if (m_DrawGeometry) {
     m_Geometry->Draw();
   }

   //Loop on all makers to add their products to the pad
   TIter next(gAMSRoot->Makers());
   AMSMaker *maker;
   while (maker = (AMSMaker*)next()) {
      maker->Draw();
   }

   if ( theta != 9999 && phi != 9999 ) view->SetView(phi, theta, 0, iret);

/*
      if ( ! angle) {
        Float_t xmin = gPad->GetX1();
        Float_t xmax = gPad->GetX2();
        Float_t ymin = gPad->GetY1();
        Float_t ymax = gPad->GetY2();
	angle = new TPaveText(0.25*xmin+0.75*xmax, 0.9*ymin+0.1*ymax,
			      0.01*xmin+0.99*xmax, 0.85*ymin+0.15*ymax);
      }
      else {
        angle->Clear();
      }
      char ptitle[100];
      angle->SetBit(kCanDelete);
      angle->SetFillColor(42);
      sprintf(ptitle,"theta,phi= %5.1f,%5.1f", m_Theta, m_Phi);
      angle->AddText(ptitle);
      angle->Draw();
*/

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
      }
      // - Ping Yeh
   }
}

//_____________________________________________________________________________
void AMSDisplay::GetEvent(Int_t event)
{
//    Read event in memory

   gAMSRoot->GetEvent(event);

   Draw();
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
   if (m_Geometry) delete m_Geometry;
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
     case kTwoView:    DrawFrontAndSideViews();  break;
     default:          SetView(90,0);   break;
   }
}

//_____________________________________________________________________________
void AMSDisplay::SetView(Float_t theta, Float_t phi)
{
//  change viewing angles for current event

   m_Pad->cd();
   m_Phi   = phi;
   m_Theta = theta;
   Int_t iret;

   TView *view = gPad->GetView();
   if (view) view->SetView(m_Phi, m_Theta, 0, iret);
   else      Draw();

   gPad->Modified();
}

//_____________________________________________________________________________
void AMSDisplay::ShowNextEvent(Int_t delta)
{
//  Display (current event_number+delta)
//    delta =  1  shown next event
//    delta = -1 show previous event

  if (delta) {
     Int_t current_event = gAMSRoot->Event();
     Int_t new_event     = current_event + delta;
     if ( new_event >= 0 ) {
       gAMSRoot->Clear();
       gAMSRoot->GetEvent(new_event); 
     }
   }
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
