#include "GET_NEW_FILE.h"
#include "ProgBar.h"
#include "Data.h"
#include "Tab_Frame.h"
#include "Menu.h"
#include "TApplication.h"
#include <string>
#include <vector>
#include "TCanvas.h"
#include "TGFrame.h"
#include "TGClient.h"
#include "TGTab.h"
#include "TGLayout.h"
#include "TObject.h"

#include "TVirtualPad.h"
#include "TH1F.h"

using namespace std;

extern ProgBar* pbar;

int main(int argc,char*argv[]){
	TApplication app("MyApp",&argc,argv);
	TGMainFrame* hc=new TGMainFrame(gClient->GetRoot());
	pbar=new ProgBar(hc);
	
	TGTab* fTab = new TGTab(hc);
	
	TGCompositeFrame* fCompositeFrame_Summary = fTab->AddTab("Summary");
	TGCompositeFrame* fCompositeFrame_h = fTab->AddTab("History Record");
	TGCompositeFrame* fCompositeFrame_Main = fTab->AddTab("PaticleR");
	vector<string> str(4);
	str[0]="Particle Mass";
	str[1]="Particle Charge";
	str[2]="Particle Momentum";
	str[3]="Particle particle id";
	vector<string> sname(1);
	sname[0]="Current file Summary";
	
	vector<string> hname(1);
	hname[0]="History Infomation: nEvents";
	Data*data=new Data(str,sname);
	Tab_Frame* tab=new Tab_Frame(fCompositeFrame_Main,str);
	Tab_Frame* stab=new Tab_Frame(fCompositeFrame_Summary,sname);
	Tab_Frame* htab=new Tab_Frame(fCompositeFrame_h,hname);

	Menu* fMenuBar=new Menu(hc,data,tab,stab,htab);
	hc->AddFrame(fMenuBar, new TGLayoutHints(kLHintsTop | kLHintsExpandX));
	hc->AddFrame(fTab, new TGLayoutHints(kLHintsLeft |kLHintsExpandX|kLHintsExpandY,2,2,2,2));
	fCompositeFrame_Main->AddFrame(tab,new TGLayoutHints(kLHintsTop|kLHintsExpandX|kLHintsExpandY,2,2,2,2));
	fCompositeFrame_Summary->AddFrame(stab,new TGLayoutHints(kLHintsTop|kLHintsExpandX|kLHintsExpandY,2,2,2,2));
	fCompositeFrame_h->AddFrame(htab,new TGLayoutHints(kLHintsTop|kLHintsExpandX|kLHintsExpandY,2,2,2,2));
	hc->AddFrame(pbar,new TGLayoutHints(kLHintsBottom|kLHintsExpandX,2,2,2,2));
	hc->Connect("CloseWindow()", "TApplication", gApplication, "Terminate()");
	hc->MapSubwindows();
	hc->Resize(hc->GetDefaultSize());
	hc->MapWindow();
	app.Run();
	return 0;
}
