
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSR_GeometrySetter                                                    //
//                                                                      //
// Utility class to set various options of  AMS geometry                //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <TNode.h>
#include <TDialogCanvas.h>
#include "Debugger.h"
#include <iostream.h>
#include "TSwitch.h"
#include "AMSR_GeometrySetter.h"


ClassImp(AMSR_GeometrySetter)



//_____________________________________________________________________________
AMSR_GeometrySetter::AMSR_GeometrySetter()
{
   // Default constructor of AMSR_GeometrySetter
   //

   m_Geometry = 0;
}

AMSR_GeoToggle AMSR_GeometrySetter::m_Toggle[] = {
  { "AMSG1",              kDrawSonsOnly, NULL },
  { "MAGNET",             kDrawNone,     NULL },
  { "TOF",                kDrawSonsOnly, NULL },
  { "TOF_HONEYCOMB",      kDrawNone,     NULL },
  { "TOF_PMT",            kDrawNone,     NULL },
  { "TOF_COUNTER",        kDrawNone,     NULL },
  { "TOF_SCINTILLATOR",   kDrawAll,      NULL },
  { "ANTI",               kDrawSonsOnly, NULL },
  { "ANTI_SUPP_TUBE",     kDrawAll,      NULL },
  { "ANTI_WRAPPER",       kDrawNone,     NULL },
  { "ANTI_SEGMENT",       kDrawNone,      NULL },
  { "ANTI_BUMP",          kDrawNone,     NULL },
  { "TRACKER",            kDrawSonsOnly, NULL },
  { "STK11",              kDrawSonsOnly, NULL },
  { "STK_L1_LADDER",      kDrawSonsOnly,     NULL },
  { "L1109100051",        kDrawNone,      NULL },
  { "L11110100051",       kDrawNone,      NULL },
  { "L11211100061",       kDrawNone,      NULL },
  { "L11312100061",       kDrawNone,      NULL },
  { "L11413100071",       kDrawNone,      NULL },
  { "L11514100071",       kDrawNone,      NULL },
  { "L12_1100011",         kDrawNone,      NULL },
  { "L12423100121",       kDrawNone,      NULL },
  { "L12524100121",       kDrawNone,      NULL },
  { "L12625100131",       kDrawNone,      NULL },
  { "L12726100131",       kDrawNone,      NULL },
  { "L12827100141",       kDrawNone,      NULL },
  { "L12928100141",       kDrawNone,      NULL },
  { "L13029100151",       kDrawNone,      NULL },
  { "L13130100151",       kDrawNone,      NULL },
  { "L13_2100011",         kDrawNone,      NULL },
  { "L13231100161",       kDrawNone,      NULL },
  { "L13332100161",       kDrawNone,      NULL },
  { "L13433100171",       kDrawNone,      NULL },
  { "L13534100171",       kDrawNone,      NULL },
  { "L14_3100021",         kDrawNone,      NULL },
  { "L15_4100021",         kDrawNone,      NULL },
  { "L16_5100031",         kDrawNone,      NULL },
  { "L17_6100031",         kDrawNone,      NULL },
  { "L18_7100041",         kDrawNone,      NULL },
  { "L19_8100041",         kDrawNone,      NULL },
  { "STK_L1_FOAM",        kDrawNone,     NULL },
  { "STK_L1_HONEYCOMB",   kDrawNone,     NULL },
  { "STK_L1_ELECTRONICS", kDrawNone,     NULL },
  { "STK22",              kDrawSonsOnly, NULL },
  { "STK_L2_LADDER",      kDrawSonsOnly,  NULL },
  { "L22_1100012",         kDrawNone,      NULL },
  { "L22221100112",       kDrawNone,      NULL },
  { "L22322100112",       kDrawNone,      NULL },
  { "L22423100122",       kDrawNone,      NULL },
  { "L22524100122",       kDrawNone,      NULL },
  { "L22625100132",       kDrawNone,      NULL },
  { "L22726100132",       kDrawNone,      NULL },
  { "L22827100142",       kDrawNone,      NULL },
  { "L22928100142",       kDrawNone,      NULL },
  { "L23_2100012",         kDrawNone,      NULL },
  { "L24_3100022",         kDrawNone,      NULL },
  { "L25_4100022",         kDrawNone,      NULL },
  { "L26_5100032",         kDrawNone,      NULL },
  { "L27_6100032",         kDrawNone,      NULL },
  { "L28_7100042",         kDrawNone,      NULL },
  { "L29_8100042",         kDrawNone,      NULL },
  { "STK_L2_FOAM",        kDrawNone,     NULL },
  { "STK_L2_HONEYCOMB",   kDrawNone,     NULL },
  { "STK_L2_ELECTRONICS", kDrawNone,     NULL },
  { "STK33",              kDrawSonsOnly, NULL },
  { "STK_L3_LADDER",      kDrawSonsOnly, NULL },
  { "L3109100053",        kDrawNone,      NULL },
  { "L31110100053",       kDrawNone,      NULL },
  { "L32019100103",         kDrawNone,      NULL },
  { "L32_1100013",         kDrawNone,      NULL },
  { "L32221100113",       kDrawNone,      NULL },
  { "L32322100113",       kDrawNone,      NULL },
  { "L32423100123",       kDrawNone,      NULL },
  { "L32524100123",       kDrawNone,      NULL },
  { "L32625100133",       kDrawNone,      NULL },
  { "L32726100133",       kDrawNone,      NULL },
  { "L32827100143",       kDrawNone,      NULL },
  { "L32928100143",       kDrawNone,      NULL },
  { "L33_2100013",         kDrawNone,      NULL },
  { "L34_3100023",         kDrawNone,      NULL },
  { "L35_4100023",         kDrawNone,      NULL },
  { "L36_5100033",         kDrawNone,      NULL },
  { "L37_6100033",         kDrawNone,      NULL },
  { "L38_7100043",         kDrawNone,      NULL },
  { "L39_8100043",         kDrawNone,      NULL },

  { "STK_L3_FOAM",        kDrawNone,     NULL },
  { "STK_L3_HONEYCOMB",   kDrawNone,     NULL },
  { "STK_L3_ELECTRONICS", kDrawNone,     NULL },
  { "STK44",              kDrawSonsOnly, NULL },
  { "STK_L4_LADDER",      kDrawSonsOnly, NULL },
  { "L4109100054",        kDrawNone,      NULL },
  { "L42019100104",         kDrawNone,      NULL },
  { "L42_1100014",         kDrawNone,      NULL },
  { "L42221100114",       kDrawNone,      NULL },
  { "L42322100114",       kDrawNone,      NULL },
  { "L42423100124",       kDrawNone,      NULL },
  { "L42524100124",       kDrawNone,      NULL },
  { "L42625100134",       kDrawNone,      NULL },
  { "L42726100134",       kDrawNone,      NULL },
  { "L42827100144",       kDrawNone,      NULL },
  { "L42928100144",       kDrawNone,      NULL },
  { "L43_2100014",         kDrawNone,      NULL },
  { "L44_3100024",         kDrawNone,      NULL },
  { "L45_4100024",         kDrawNone,      NULL },
  { "L46_5100034",         kDrawNone,      NULL },
  { "L47_6100034",         kDrawNone,      NULL },
  { "L48_7100044",         kDrawNone,      NULL },
  { "L49_8100044",         kDrawNone,      NULL },
  { "STK_L4_FOAM",        kDrawNone,     NULL },
  { "STK_L4_HONEYCOMB",   kDrawNone,     NULL },
  { "STK_L4_ELECTRONICS", kDrawNone,     NULL },
  { "STK55",              kDrawSonsOnly, NULL },
  { "STK_L5_LADDER",      kDrawSonsOnly, NULL },
  { "L52_1100015",         kDrawNone,      NULL },
  { "L52221100115",       kDrawNone,      NULL },
  { "L52322100115",       kDrawNone,      NULL },
  { "L52423100125",       kDrawNone,      NULL },
  { "L52524100125",       kDrawNone,      NULL },
  { "L52625100135",       kDrawNone,      NULL },
  { "L52726100135",       kDrawNone,      NULL },
  { "L52827100145",       kDrawNone,      NULL },
  { "L52928100145",       kDrawNone,      NULL },
  { "L53_2100015",         kDrawNone,      NULL },
  { "L54_3100025",         kDrawNone,      NULL },
  { "L55_4100025",         kDrawNone,      NULL },
  { "L56_5100035",         kDrawNone,      NULL },
  { "L57_6100035",         kDrawNone,      NULL },
  { "L58_7100045",         kDrawNone,      NULL },
  { "L59_8100045",         kDrawNone,      NULL },
  { "STK_L5_FOAM",        kDrawNone,     NULL },
  { "STK_L5_HONEYCOMB",   kDrawNone,     NULL },
  { "STK_L5_ELECTRONICS", kDrawNone,     NULL },
  { "STK66",              kDrawSonsOnly, NULL },
  { "STK_L6_LADDER",      kDrawSonsOnly, NULL },
  { "L6109100056",        kDrawNone,      NULL },
  { "L61110100056",       kDrawNone,      NULL },
  { "L61211100066",       kDrawNone,      NULL },
  { "L61312100066",       kDrawNone,      NULL },
  { "L61413100076",       kDrawNone,      NULL },
  { "L61514100076",       kDrawNone,      NULL },
  { "L62_1100016",         kDrawNone,      NULL },
  { "L62221100116",       kDrawNone,      NULL },
  { "L62322100116",       kDrawNone,      NULL },
  { "L62423100126",       kDrawNone,      NULL },
  { "L62524100126",       kDrawNone,      NULL },
  { "L62625100136",       kDrawNone,      NULL },
  { "L62726100136",       kDrawNone,      NULL },
  { "L62827100146",       kDrawNone,      NULL },
  { "L62928100146",       kDrawNone,      NULL },
  { "L63029100156",       kDrawNone,      NULL },
  { "L63130100156",       kDrawNone,      NULL },
  { "L63_2100016",         kDrawNone,      NULL },
  { "L63231100166",       kDrawNone,      NULL },
  { "L63332100166",       kDrawNone,      NULL },
  { "L63433100176",       kDrawNone,      NULL },
  { "L63534100176",       kDrawNone,      NULL },
  { "L64_3100026",         kDrawNone,      NULL },
  { "L65_4100026",         kDrawNone,      NULL },
  { "L66_5100036",         kDrawNone,      NULL },
  { "L67_6100036",         kDrawNone,      NULL },
  { "L68_7100046",         kDrawNone,      NULL },
  { "L69_8100046",         kDrawNone,      NULL },

  { "STK_L6_FOAM",        kDrawNone,     NULL },
  { "STK_L6_HONEYCOMB",   kDrawNone,     NULL },
  { "STK_L6_ELECTRONICS", kDrawNone,     NULL },
  { "CTC",                kDrawSonsOnly, NULL },
  { "CTC_SUPPORT",        kDrawNone,     NULL },
  { "CTC_AMP",            kDrawNone,     NULL },
  { "CTC_UPPER",          kDrawSonsOnly, NULL },
  { "CTC_UPPER_WALL",     kDrawNone,     NULL },
  { "CTC_UPPER_CELL",     kDrawNone,      NULL },
  { "CTC_UPPER_GAP",      kDrawNone,     NULL },
  { "CTC_UPPER_PTF",      kDrawNone,     NULL },
  { "CTC_UPPER_AEROGEL",  kDrawAll,     NULL },
  { "CTC_UPPER_PMT",      kDrawNone,     NULL },
  { "CTC_LOWER",          kDrawSonsOnly, NULL },
  { "CTC_LOWER_WALL",     kDrawNone,     NULL },
  { "CTC_LOWER_CELL",     kDrawNone,      NULL },
  { "CTC_LOWER_GAP",      kDrawNone,     NULL },
  { "CTC_LOWER_PTF",      kDrawNone,     NULL },
  { "CTC_LOWER_AEROGEL",  kDrawAll,     NULL },
  { "CTC_LOWER_PMT",      kDrawNone,     NULL },
  { "CTC_EXTRA",          kDrawSonsOnly, NULL },
  { "CTC_EXTRA_WALL",     kDrawNone,     NULL },
  { "CTC_EXTRA_CELL",     kDrawNone,      NULL },
  { "CTC_EXTRA_GAP",      kDrawNone,     NULL },
  { "CTC_EXTRA_PTF",      kDrawNone,     NULL },
  { "CTC_EXTRA_AEROGEL",  kDrawAll,     NULL },
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
AMSR_GeometrySetter::AMSR_GeometrySetter(TGeometry * geo)
{
   // Constructor of AMSR_GeometrySetter
   //
   m_Geometry = geo;
   debugger.Print("+++ AMSR_GeometrySetter::AMSR_GeometrySetter() geo = %lx\n", geo);
   TNode * node;
   Int_t i = 0;
   while ( m_Toggle[i].name != 0 ) {
     node = m_Geometry->GetNode(m_Toggle[i].name);
//   debugger.Print("got node %lx\n", node);
     if (node) node->SetVisibility(m_Toggle[i].vis);
     else cerr<<"No Node "<<m_Toggle[i].name<<endl;
     i++;
   }
   
}


//_____________________________________________________________________________
void AMSR_GeometrySetter::TurnOn(char * name)
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
void AMSR_GeometrySetter::TurnOnWithSons(char * name)
{
  //
  // turn on drawing of a node, but not its sons
  //
  TNode * node = m_Geometry->GetNode(name);
  if (node) node->SetVisibility(3);
}


//_____________________________________________________________________________
void AMSR_GeometrySetter::TurnOff(char * name)
{
  //
  // turn off drawing of a node but not its sons
  //
  TNode * node = m_Geometry->GetNode(name);
  if (node) node->SetVisibility(2);
}


//_____________________________________________________________________________
void AMSR_GeometrySetter::TurnOffWithSons(char * name)
{
  //
  // turn off drawing of a node and its sons
  //
  TNode * node = m_Geometry->GetNode(name);
  if (node) node->SetVisibility(-1);
}


//_____________________________________________________________________________
void AMSR_GeometrySetter::CheckVisibility(char * name)
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
void AMSR_GeometrySetter::SetGeo()
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
			"gAMSR_Root->Display()->Draw()", 0.0, y-0.1, 1.0, y);
*/

}

/*
//______________________________________________________________________________
void AMSR_GeometrySetter::SetGeo()
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
