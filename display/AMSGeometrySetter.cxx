
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSGeometrySetter                                                    //
//                                                                      //
// Utility class to set various options of  AMS geometry                //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <TNode.h>
#include <TDialogCanvas.h>
#include "Debugger.h"

#include "TSwitch.h"
#include "AMSGeometrySetter.h"


ClassImp(AMSGeometrySetter)



//_____________________________________________________________________________
AMSGeometrySetter::AMSGeometrySetter()
{
   // Default constructor of AMSGeometrySetter
   //

   m_Geometry = 0;
}

AMSGeoToggle AMSGeometrySetter::m_Toggle[] = {
  { "AMSG1",              kDrawSonsOnly, NULL },
  { "MAGNET",             kDrawNone,     NULL },
  { "TOF",                kDrawSonsOnly, NULL },
  { "TOF_HONEYCOMB",      kDrawNone,     NULL },
  { "TOF_PMT",            kDrawNone,     NULL },
  { "TOF_COUNTER",        kDrawNone,     NULL },
  { "TOF_SCINTILLATOR",   kDrawAll,      NULL },
  { "ANTI",               kDrawNone,     NULL },
  { "ANTI_SUPP_TUBE",     kDrawNone,     NULL },
  { "ANTI_WRAPPER",       kDrawNone,     NULL },
  { "ANTI_SEGMENT",       kDrawNone,     NULL },
  { "ANTI_BUMP",          kDrawNone,     NULL },
  { "TRACKER",            kDrawSonsOnly, NULL },
  { "STK11",              kDrawSonsOnly, NULL },
  { "STK_L1_LADDER",      kDrawAll,      NULL },
  { "STK_L1_FOAM",        kDrawNone,     NULL },
  { "STK_L1_HONEYCOMB",   kDrawNone,     NULL },
  { "STK_L1_ELECTRONICS", kDrawNone,     NULL },
  { "STK22",              kDrawSonsOnly, NULL },
  { "STK_L2_LADDER",      kDrawAll,      NULL },
  { "STK_L2_FOAM",        kDrawNone,     NULL },
  { "STK_L2_HONEYCOMB",   kDrawNone,     NULL },
  { "STK_L2_ELECTRONICS", kDrawNone,     NULL },
  { "STK33",              kDrawSonsOnly, NULL },
  { "STK_L3_LADDER",      kDrawAll,      NULL },
  { "STK_L3_FOAM",        kDrawNone,     NULL },
  { "STK_L3_HONEYCOMB",   kDrawNone,     NULL },
  { "STK_L3_ELECTRONICS", kDrawNone,     NULL },
  { "STK44",              kDrawSonsOnly, NULL },
  { "STK_L4_LADDER",      kDrawAll,      NULL },
  { "STK_L4_FOAM",        kDrawNone,     NULL },
  { "STK_L4_HONEYCOMB",   kDrawNone,     NULL },
  { "STK_L4_ELECTRONICS", kDrawNone,     NULL },
  { "STK55",              kDrawSonsOnly, NULL },
  { "STK_L5_LADDER",      kDrawAll,      NULL },
  { "STK_L5_FOAM",        kDrawNone,     NULL },
  { "STK_L5_HONEYCOMB",   kDrawNone,     NULL },
  { "STK_L5_ELECTRONICS", kDrawNone,     NULL },
  { "STK66",              kDrawSonsOnly, NULL },
  { "STK_L6_LADDER",      kDrawAll,      NULL },
  { "STK_L6_FOAM",        kDrawNone,     NULL },
  { "STK_L6_HONEYCOMB",   kDrawNone,     NULL },
  { "STK_L6_ELECTRONICS", kDrawNone,     NULL },
  { "CTC",                kDrawSonsOnly, NULL },
  { "CTC_SUPPORT",        kDrawNone,     NULL },
  { "CTC_AMP",            kDrawNone,     NULL },
  { "CTC_UPPER",          kDrawSonsOnly, NULL },
  { "CTC_UPPER_WALL",     kDrawNone,     NULL },
  { "CTC_UPPER_CELL",     kDrawAll,      NULL },
  { "CTC_UPPER_GAP",      kDrawNone,     NULL },
  { "CTC_UPPER_PTF",      kDrawNone,     NULL },
  { "CTC_UPPER_AEROGEL",  kDrawNone,     NULL },
  { "CTC_UPPER_PMT",      kDrawNone,     NULL },
  { "CTC_LOWER",          kDrawSonsOnly, NULL },
  { "CTC_LOWER_WALL",     kDrawNone,     NULL },
  { "CTC_LOWER_CELL",     kDrawAll,      NULL },
  { "CTC_LOWER_GAP",      kDrawNone,     NULL },
  { "CTC_LOWER_PTF",      kDrawNone,     NULL },
  { "CTC_LOWER_AEROGEL",  kDrawNone,     NULL },
  { "CTC_LOWER_PMT",      kDrawNone,     NULL },
  { "CTC_EXTRA",          kDrawSonsOnly, NULL },
  { "CTC_EXTRA_WALL",     kDrawNone,     NULL },
  { "CTC_EXTRA_CELL",     kDrawAll,      NULL },
  { "CTC_EXTRA_GAP",      kDrawNone,     NULL },
  { "CTC_EXTRA_PTF",      kDrawNone,     NULL },
  { "CTC_EXTRA_AEROGEL",  kDrawNone,     NULL },
  { "CTC_EXTRA_PMT",      kDrawNone,     NULL },
//{ "SHUTTLE",            kDrawAll,     NULL },
//{ "SHUTTLE_SHU",        kDrawAll,     NULL },
//{ "SHUTTLE_PSH",        kDrawAll,     NULL },
//{ "SHUTTLE_IRON",       kDrawAll,     NULL },
  { "SHUTTLE",            kDrawNone,     NULL },
  { "SHUTTLE_SHU",        kDrawNone,     NULL },
  { "SHUTTLE_PSH",        kDrawNone,     NULL },
  { "SHUTTLE_IRON",       kDrawNone,     NULL },
  { 0,                    kDrawNone,     NULL }
};

//_____________________________________________________________________________
AMSGeometrySetter::AMSGeometrySetter(TGeometry * geo)
{
   // Constructor of AMSGeometrySetter
   //
   m_Geometry = geo;
   debugger.Print("+++ AMSGeometrySetter::AMSGeometrySetter() geo = %lx\n", geo);
   TNode * node;
   Int_t i = 0;
   while ( m_Toggle[i].name != 0 ) {
     node = m_Geometry->GetNode(m_Toggle[i].name);
//   debugger.Print("got node %lx\n", node);
     if (node) node->SetVisibility(m_Toggle[i].vis);
     i++;
   }
   
}


//_____________________________________________________________________________
void AMSGeometrySetter::TurnOn(char * name)
{
  //
  // turn on drawing of a node, but not its sons
  //
  TNode * node = m_Geometry->GetNode(name);
  debugger.Print("got node %lx\n", node);
  if (node) node->SetVisibility(-2);
  if (node) debugger.Print("name = %s\n", node->GetName());
}


//_____________________________________________________________________________
void AMSGeometrySetter::TurnOnWithSons(char * name)
{
  //
  // turn on drawing of a node, but not its sons
  //
  TNode * node = m_Geometry->GetNode(name);
  if (node) node->SetVisibility(3);
}


//_____________________________________________________________________________
void AMSGeometrySetter::TurnOff(char * name)
{
  //
  // turn off drawing of a node but not its sons
  //
  TNode * node = m_Geometry->GetNode(name);
  if (node) node->SetVisibility(2);
}


//_____________________________________________________________________________
void AMSGeometrySetter::TurnOffWithSons(char * name)
{
  //
  // turn off drawing of a node and its sons
  //
  TNode * node = m_Geometry->GetNode(name);
  if (node) node->SetVisibility(-1);
}


//_____________________________________________________________________________
void AMSGeometrySetter::CheckVisibility(char * name)
{
  TNode * node = m_Geometry->GetNode(name);
  Int_t vis;
  if (node) printf("%s vis %d\n", node->GetName(), node->GetVisibility());
}


static TDialogCanvas * dialog;
/*
static const Int_t nToggle = 6;
struct myToggle {  
   Bool_t drawIt; 
   TSwitch * sw; 
   char * name; 
};
myToggle toggle[nToggle];
*/
static TButton * okButton;


//______________________________________________________________________________
void AMSGeometrySetter::SetGeo()
{
  TVirtualPad * gPadSave = gPad;

  dialog = new TDialogCanvas("setgeo", "Geometry Component On/Off", 740,650);
  if (dialog == 0) return;

  dialog->BuildStandardButtons();

  gPadSave->cd();

/*
  dialog->cd();


  toggle[0].name = "TOF";
  toggle[1].name = "TRACKER";
  toggle[2].name = "CTC";
  toggle[3].name = "ANTI";
  toggle[4].name = "MAGNET";
  toggle[5].name = "SHUTTLE";

  char onMethod[80], offMethod[80];
  for (Int_t i = 0; i < nToggle; i++) {
    sprintf(onMethod, 
      "display->GetGeometrySetter()->TurnOnWithSons(\"%s\")",
	toggle[i].name);
    sprintf(offMethod, 
      "display->GetGeometrySetter()->TurnOffWithSons(\"%s\")",
	toggle[i].name);
    toggle[i].sw= new TSwitch(toggle[i].name, &toggle[i].drawIt,
		onMethod, 0.0, 0.9-i*0.1, 1.0, 1.0-i*0.1, offMethod);
    toggle[i].sw->Draw();
  }

  okButton = new TButton("OK", "display->OkGeo()", 0.3, 0.1, 0.7, 0.2);
  okButton->Draw();
*/
/*
  sw[0] = new TSwitch("ToF", 
   sw[6] = new TSwitch("Geometry", &m_DrawGeometry, 
			"gAMSRoot->Display()->Draw()", 0.0, y-0.1, 1.0, y);
*/

}

/*
//______________________________________________________________________________
void AMSGeometrySetter::SetGeo()
{
  gPadSave = gPad;
  dialog = new TCanvas("setgeo", "Geometry Component On/Off", 14,47,740,650);
  if (dialog == 0) return;

  dialog->cd();


  toggle[0].name = "TOF";
  toggle[1].name = "TRACKER";
  toggle[2].name = "CTC";
  toggle[3].name = "ANTI";
  toggle[4].name = "MAGNET";
  toggle[5].name = "SHUTTLE";

  char onMethod[80], offMethod[80];
  for (Int_t i = 0; i < nToggle; i++) {
    sprintf(onMethod, 
      "display->GetGeometrySetter()->TurnOnWithSons(\"%s\")",
	toggle[i].name);
    sprintf(offMethod, 
      "display->GetGeometrySetter()->TurnOffWithSons(\"%s\")",
	toggle[i].name);
    toggle[i].sw= new TSwitch(toggle[i].name, &toggle[i].drawIt,
		onMethod, 0.0, 0.9-i*0.1, 1.0, 1.0-i*0.1, offMethod);
    toggle[i].sw->Draw();
  }

  okButton = new TButton("OK", "display->OkGeo()", 0.3, 0.1, 0.7, 0.2);
  okButton->Draw();


}
*/
