#include "ProgBar.h"
ClassImp(ProgBar);
ProgBar::ProgBar(TGWindow* p):TGTransientFrame(p){
        finfo=new TGLabel(this,"Progress Status\n ");
        AddFrame(finfo,new TGLayoutHints(kLHintsCenterX|kLHintsExpandX));
        fHProg=new TGHProgressBar(this,TGProgressBar::kStandard,100);
        fHProg->SetBarColor("yellow");
        fHProg->ShowPos(true);
        AddFrame(fHProg,new TGLayoutHints(kLHintsCenterX|kLHintsExpandX));
        Connect("CloseWindow()","ProgBar",this,"DeleteWindow()");
}
ProgBar::~ProgBar(){}
void ProgBar::CloseWindows(){ 
	DeleteWindow(); 
}
ProgBar* pbar;
