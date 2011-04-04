#include "Tab_Frame.h"
Tab_Frame::Tab_Frame(const TGWindow* p ,vector<string>& pads):TGVerticalFrame(p),fpads_name_list(pads){
	_nPads=pads.size();
	Int_t i=0;
	for(i=0;i<_nPads;i++)
		fpads[pads[i]]=i+1;		
	fEcanvas = new TRootEmbeddedCanvas(Form("Ecanvas_%s",pads[0].c_str()),this,500,500);
	//Display nPads pads in the canvas
	//Calculate best dividence of the canvas
	Int_t w_div=ceil(sqrt(_nPads/12.0)*4);
	Int_t h_div=ceil(sqrt(_nPads/12.0)*3);
	if(_nPads!=1)
		fEcanvas->GetCanvas()->Divide(w_div,h_div);
	else
		fEcanvas->GetCanvas()->Divide(1,1);
	for(i=0;i<_nPads;i++)
		fEcanvas->GetCanvas()->GetPad(i+1)->AddExec("zoom","myzoom()");
	AddFrame(fEcanvas,new TGLayoutHints(kLHintsTop|kLHintsExpandX|kLHintsExpandY,2,2,2,2));

}
Tab_Frame::~Tab_Frame(){
	if(!fEcanvas)
		delete fEcanvas;
}

