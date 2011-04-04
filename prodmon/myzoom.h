#include "TCanvas.h"
#include "TVirtualPad.h"
#include "TROOT.h"
#include "TObject.h"

#include "string.h"
void myzoom() {
	TVirtualPad * pad = gPad;
	if(pad->GetEvent()==61){
		TCanvas *big = (TCanvas*)gROOT->GetListOfCanvases()->FindObject("big");
		if (!big)
			big = new TCanvas("big","big",600,600);
		big->cd();
		TIter next(pad->GetListOfPrimitives());
		TObject* obj;
		while(obj=next()){
			if(strcmp(obj->GetName(),"TFrame")!=0&&strcmp(obj->GetName(),"title")!=0){		
				obj->Draw();
				break;
			}
		}
		while(obj=next()){
			if(strcmp(obj->GetName(),"TFrame")!=0&&strcmp(obj->GetName(),"title")!=0)	
				obj->Draw("same");
		}
		big->SetGrid(1,1);
		big->Update();
	}
	
}
