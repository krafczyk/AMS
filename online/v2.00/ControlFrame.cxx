#include "ControlFrame.h"
#include "AMSDisplay.h"
#include "AMSCanvas.h"
#include "AMSTOFHist.h"
ClassImp(AMSControlFrame)

Bool_t AMSControlFrame::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
  char buf[255];
  TGHotString *ptg1=0;
  TGHotString *ptg2=0;
  // Process messages sent to this dialog.

  switch (GET_MSG(msg)) {
  case kC_COMMAND:
    
    switch (GET_SUBMSG(msg)) {
      
    case kCM_CHECKBUTTON:
    case kCM_RADIOBUTTON:
    case kCM_BUTTON:
      if(parm1/100 == 2){
	gAMSDisplay->Dispatch(parm1%10);
	return kTRUE;
      }
      if(parm1/100 == 3){
	(gAMSDisplay->getSubDet(parm1%10))->IsActive()=_pcycle[parm1%10]->GetState();
	return kTRUE;
      }
      if(parm1/100 == 5){
	switch(parm1%10){
	case 2:
	  _plogx[0]->SetState(kButtonUp);
	  gAMSDisplay->IsLogX()=kTRUE;
	  break;
	case 1:
	  _plogx[1]->SetState(kButtonUp);
	  gAMSDisplay->IsLogX()=kFALSE;
	  break;
	case 4:
	  _plogy[0]->SetState(kButtonUp);
	  gAMSDisplay->IsLogY()=kTRUE;
	  break;
	case 3:
	  _plogy[1]->SetState(kButtonUp);
	  gAMSDisplay->IsLogY()=kFALSE;
	  break;
	case 6:
	  _plogz[0]->SetState(kButtonUp);
	  gAMSDisplay->IsLogZ()=kTRUE;
	  break;
	case 5:
	  _plogz[1]->SetState(kButtonUp);
	  gAMSDisplay->IsLogZ()=kFALSE;
	  break;
	  
	}
	gAMSDisplay->RDispatch();
	return kTRUE;
      }
      switch(parm1){
      case 401:
	gAMSDisplay->StartStop(buf);
	if(strlen(buf)){
	  ptg1 =new TGHotString(buf);
	  _pcontrol[0]->SetText(ptg1);
	}
	break;
      case 402:
	gAMSDisplay->Filled(buf);
	if(strlen(buf)){
	  ptg2 =new TGHotString(buf);
	  _pcontrol[1]->SetText(ptg2);
	}
	break;
      case 403:
	gAMSDisplay->Reset();
	break;
      }
      break;
    case kCM_MENU:
      if(parm1/100 >= 7){//TOF channel selection
	AMSTOFHist *TOFch;
	int ilay=0,ibar=0,isid=0;
	ilay=(parm1-701)/28+1;
	ibar=((parm1-701)%28)/2+1;
	isid=(parm1-701)%2;//0->P, 1->N
	TOFch->ShowChan(ilay,ibar,isid);
//   ChDialog *fTOFch=new ChDialog(fClient->GetRoot(), this, 400, 200);
	return kTRUE;
      }      
      if(parm1/100 ==1){
	int sdet=(parm1/10)%10;
	int set=parm1%10;
	AMSCanvas::SubDet(sdet,set);
	return kTRUE;
      } 
      switch   (parm1){
      case 4:
	gApplication->Terminate(0);
	break;
      case 1:
	AMSCanvas::SaveParticleCB();
	break;              
      case 2:
	AMSCanvas::SaveParticleGIF();
	break;              
      case 3:
	AMSCanvas::PrintCB();
	break;              
      }
    }
  }
  
  return kTRUE;
}


void AMSControlFrame::CloseWindow()
{
   // Close dialog in response to window manager close.

   delete this;
}

AMSControlFrame::~AMSControlFrame(){

  for(int i=0;i<nsubdets;i++)delete fSubDetMenu[i];
  for(i=0;i<nsubdets;i++)delete fSubDetCascadeMenu[i];
  delete fDetMenu;
  delete fSaveAsMenu;
  for(i=0;i<3;i++)delete _pcontrol[i];
  for(i=0;i<nbuttons;i++)delete _pbutton[i];    
  for(i=0;i<nbuttons;i++)delete _pcycle[i];    
  for(i=0;i<2;i++)delete _plogx[i];    
  for(i=0;i<2;i++)delete _plogy[i];    
  for(i=0;i<2;i++)delete _plogz[i];    
  delete fL1;
  delete fL2;
  delete fL3;
  delete fL4;
  delete _pbutfr;
  delete _pcontrolfr;
  delete _plogfr;
  delete _pcyclefr;
  delete _pcombofr;
  
  delete fMenuBar;
  delete fMenuBarLayout;
  delete fMenuBarItemLayout;
  delete fMenuBarHelpLayout;
  

}



AMSControlFrame::AMSControlFrame(const TGWindow *p, const TGWindow *main,
				 UInt_t w, UInt_t h, UInt_t options) :
  TGTransientFrame(p, main, w, h, options){
    int i, ax, ay;
    
    ChangeOptions((GetOptions() & ~kHorizontalFrame) | kVerticalFrame);
    
    //create menus

#define TOFmenu // TEMP!

#ifdef TOFmenu
   // TOF channel submenus
   fTOFSelectMenu=new TGPopupMenu(fClient->GetRoot());
   fTOFSelectMenu->Associate(this);
   for(int kk=0;kk<4;kk++){
     char sCounter[8];
     fTOFPlaneMenu[kk]=new TGPopupMenu(fClient->GetRoot()); 
     fTOFPlaneMenu[kk]->Associate(this);
     for(int jj=0;jj<14;jj++){
       fTOFCounterMenu[kk][jj]=new TGPopupMenu(fClient->GetRoot()); 
       fTOFCounterMenu[kk][jj]->Associate(this);
       fTOFSideMenu[kk][jj][0]=new TGPopupMenu(fClient->GetRoot()); 
       fTOFSideMenu[kk][jj][0]->Associate(this);
       fTOFSideMenu[kk][jj][1]=new TGPopupMenu(fClient->GetRoot()); 
       fTOFSideMenu[kk][jj][1]->Associate(this);
       fTOFCounterMenu[kk][jj]->AddEntry("Side P",701+28*kk+(2*jj));
       fTOFCounterMenu[kk][jj]->AddEntry("Side N",701+28*kk+(2*jj+1));
       sprintf(sCounter,"Counter %2d",jj+1);
       fTOFPlaneMenu[kk]->AddPopup(sCounter,fTOFCounterMenu[kk][jj]);
     }
   }
   fTOFSelectMenu->AddPopup("Plane &1",fTOFPlaneMenu[0]);
   fTOFSelectMenu->AddPopup("Plane &2",fTOFPlaneMenu[1]);
   fTOFSelectMenu->AddPopup("Plane &3",fTOFPlaneMenu[2]);
   fTOFSelectMenu->AddPopup("Plane &4",fTOFPlaneMenu[3]);
#endif


    char sSubDetMenu[nsubdets][]={ 
      "&AntiCounters",
      "T&racker",
      "Level&1",
      "Level&3",
      "&TOF",
      "&CTC",
      "A&xAMS",
      "General"};
    for(i=0;i<nsubdets;i++) fSubDetMenu[i]=0;
    for( i=0;i<nsubdets;i++)fSubDetCascadeMenu[i]=0; 
    fSaveAsMenu=new TGPopupMenu(fClient->GetRoot());
    fSaveAsMenu->AddEntry("Save As SubDetector.Set.&ps",1);
    fSaveAsMenu->AddEntry("Save As SubDetector.Set.&gif",2);
    fSaveAsMenu->AddSeparator();
    fSaveAsMenu->AddEntry("P&rint",3);
    fSaveAsMenu->AddSeparator();
    fSaveAsMenu->AddEntry("&Quit",4);
    fDetMenu=new TGPopupMenu(fClient->GetRoot());
    for(i=0;i<nsubdets;i++)
      fSubDetCascadeMenu[i] = new TGPopupMenu(fClient->GetRoot());
    for(i=0;i<nsubdets;i++)fSubDetMenu[i] = new TGPopupMenu(fClient->GetRoot());
    fSubDetCascadeMenu[0]->AddEntry("1-4",101);
    fSubDetCascadeMenu[0]->AddEntry("5-8",102);
    fSubDetCascadeMenu[0]->AddEntry("9-12",103);
    fSubDetCascadeMenu[0]->AddEntry("13-16",104);  
    
    fSubDetMenu[0]->AddEntry("Occupancies/Amplitudes Distibution",100);
    fSubDetMenu[0]->AddPopup( "UpVsDown",fSubDetCascadeMenu[0]);
    
    fSubDetMenu[1]->AddEntry("PreClusters Distributions Set 1",110);
    fSubDetMenu[1]->AddEntry("PreClusters Distributions Set 2",111);
    fSubDetMenu[1]->AddEntry("PreClusters S/N",112);
    fSubDetMenu[1]->AddEntry("Clusters Distributions",113);
    fSubDetMenu[1]->AddSeparator();
    fSubDetMenu[1]->AddEntry("Calibration: Sigmas",114);
    fSubDetMenu[1]->AddEntry("Calibration: ClNumPerLadder",115);
    fSubDetMenu[1]->AddEntry("Calibration: CmnNoise",116);
    
    fSubDetMenu[2]->AddEntry("Occupancies Distibutions",120);
    fSubDetMenu[2]->AddEntry("LVL1 vs TOF",121);
    fSubDetMenu[2]->AddEntry("TOF Trigger Pattern",122);
    fSubDetMenu[2]->AddEntry("LVL1 vs Anti",123);
    fSubDetMenu[3]->AddEntry("LVL3 Distributions",130);
    fSubDetMenu[3]->AddEntry("LVL3/AxAMS Comparision",131);
    
    fSubDetMenu[4]->AddEntry("JL1/TOF Side Occupancies",140);
    fSubDetMenu[4]->AddEntry("JL1/TOF Counter Occupancies",141);
    fSubDetMenu[4]->AddEntry("Occupancies Distributions",142);
    fSubDetMenu[4]->AddEntry("Tdiffs/(data size)/config",143);
    fSubDetMenu[4]->AddEntry("NTof/Eloss Distributions ",144);
    fSubDetMenu[4]->AddEntry("Mean Eloss per counter (MeV)",145);
    fSubDetMenu[4]->AddEntry("Anode Charge Spectra",146);
    fSubDetMenu[4]->AddEntry("Dynode Charge Spectra",147);
    fSubDetMenu[4]->AddEntry("Time Stretcher Ratios",148);
#ifdef TOFmenu
   fSubDetMenu[4]->AddSeparator();
   fSubDetMenu[4]->AddPopup("Select one TOF channel",fTOFSelectMenu);
#endif
#ifdef TOFdialog
   fSubDetMenu[4]->AddSeparator();
   fSubDetMenu[4]->AddEntry("Select one TOF channel",700);
#endif

    fSubDetMenu[5]->AddEntry("Layer 1 Distributions",150);
    fSubDetMenu[5]->AddEntry("Layer 2 Distributions",151);
 
    fSubDetMenu[6]->AddEntry("Set 0",160);
    fSubDetMenu[6]->AddEntry("Set 1",161);
    
    fSubDetMenu[7]->AddEntry("Length",170);
    fSubDetMenu[7]->AddEntry("TOF Length",171);
    fSubDetMenu[7]->AddEntry("TRK (red) Length",172);
    fSubDetMenu[7]->AddEntry("TRK (raw,mix) Length",173);
    
    fMenuBarLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 0, 0, 1, 1);
    fMenuBarItemLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0);
    fMenuBarHelpLayout = new TGLayoutHints(kLHintsTop | kLHintsRight);
    

    fDetMenu->Associate(this);
    fSaveAsMenu->Associate(this);
    for(i=0;i<nsubdets;i++)fSubDetMenu[i]->Associate(this);
    for(i=0;i<nsubdets;i++)fSubDetCascadeMenu[i]->Associate(this);
    
    for(i=0;i<nsubdets;i++)fDetMenu->AddPopup(sSubDetMenu[i], fSubDetMenu[i]);
    

   // Create menubar
    fMenuBar = new TGMenuBar(this, 1, 1, kHorizontalFrame);
    
    fMenuBar->AddPopup("&File", fSaveAsMenu, fMenuBarItemLayout);
    fMenuBar->AddPopup("&Control", fDetMenu, fMenuBarItemLayout);
    AddFrame(fMenuBar, fMenuBarLayout);
    const int tggcolor=3;
    const int tgbcolor=7;
    const int tgccolor=2;

   // Create Group frames
   //  TGCompositeFrame * f2 = new TGCompositeFrame(this, 80, 40, kHorizontalFrame| kSunkenFrame);
    SetWindowAttributes_t wattr;
    _pbutfr= new TGGroupFrame(this, new TGString("Subdetectors"),kVerticalFrame );
    wattr.fMask = kWABackPixel | kWAEventMask;
    wattr.fBackgroundPixel = tggcolor;
    wattr.fEventMask = kExposureMask;
    gGXW->ChangeWindowAttributes(_pbutfr->GetId(), &wattr);
    

    _pcontrolfr= new TGGroupFrame(this, new TGString("Controls"));
    wattr.fMask = kWABackPixel | kWAEventMask;
    wattr.fBackgroundPixel = tggcolor;
    wattr.fEventMask = kExposureMask;
    gGXW->ChangeWindowAttributes(_pcontrolfr->GetId(), &wattr);
    
    _pcyclefr= new TGGroupFrame(this, new TGString("Cycle"));
    wattr.fMask = kWABackPixel | kWAEventMask;
    wattr.fBackgroundPixel = tggcolor;
    wattr.fEventMask = kExposureMask;
    gGXW->ChangeWindowAttributes(_pcyclefr->GetId(), &wattr);
    _plogfr= new TGGroupFrame(this, new TGString("Scale"));
    wattr.fMask = kWABackPixel | kWAEventMask;
    wattr.fBackgroundPixel = tggcolor;
    wattr.fEventMask = kExposureMask;
    gGXW->ChangeWindowAttributes(_plogfr->GetId(), &wattr);
    
    fL1 = new TGLayoutHints(kLHintsLeft | kLHintsExpandX |kLHintsExpandY, 0, 0, 2, 2);
    fL2 = new TGLayoutHints(kLHintsLeft | kLHintsExpandX |kLHintsExpandY, 1, 1, 1, 1);
    fL3 = new TGLayoutHints(kLHintsLeft | kLHintsExpandX |kLHintsExpandY, 1, 1, 1, 1);
    fL4 = new TGLayoutHints(kLHintsLeft | kLHintsExpandX |kLHintsExpandY, 1, 1, 1, 1);
    

    for(i=0;i<nbuttons;i++){
      _pbutton[i]=new TGTextButton(_pbutfr, sSubDetMenu[i], 200+i);
      wattr.fMask = kWABackPixel | kWAEventMask;
      wattr.fBackgroundPixel = tgbcolor;
      wattr.fEventMask = kExposureMask;
      gGXW->ChangeWindowAttributes(_pbutton[i]->GetId(), &wattr);
      
    }
    for(i=0;i<nbuttons;i++){
      _pcycle[i]=new TGCheckButton(_pcyclefr, sSubDetMenu[i], 300+i);
      wattr.fMask = kWABackPixel | kWAEventMask;
      wattr.fBackgroundPixel = tggcolor;
      wattr.fEventMask = kExposureMask;
      gGXW->ChangeWindowAttributes(_pcycle[i]->GetId(), &wattr);
    }
    _plogx[0]=new TGRadioButton(_plogfr,"LinX",501);
    _plogx[1]=new TGRadioButton(_plogfr,"LogX",502);
    _plogy[0]=new TGRadioButton(_plogfr,"LinY",503);
    _plogy[1]=new TGRadioButton(_plogfr,"LogY",504);
    _plogz[0]=new TGRadioButton(_plogfr,"LinZ",505);
    _plogz[1]=new TGRadioButton(_plogfr,"LogZ",506);
    for(i=0;i<2;i++){
      wattr.fMask = kWABackPixel | kWAEventMask;
      wattr.fBackgroundPixel = tggcolor;
      wattr.fEventMask = kExposureMask;
      gGXW->ChangeWindowAttributes(_plogx[i]->GetId(), &wattr);
      gGXW->ChangeWindowAttributes(_plogy[i]->GetId(), &wattr);
      gGXW->ChangeWindowAttributes(_plogz[i]->GetId(), &wattr);
    }
    for(i=0;i<nbuttons;i++)_pbutton[i]->Associate(this);
    for(i=0;i<nbuttons;i++)_pcycle[i]->Associate(this);
    for(i=0;i<2;i++)_plogx[i]->Associate(this);   
    for(i=0;i<2;i++)_plogy[i]->Associate(this);   
    for(i=0;i<2;i++)_plogz[i]->Associate(this);   
    for(i=0;i<nsubdets;i++){
      if((gAMSDisplay->getSubDet(i))->IsActive())_pcycle[i]->SetState(kButtonDown);
    }
    _pcontrol[0]=new TGTextButton(_pcontrolfr,"Stop Timer",401);
    _pcontrol[1]=new TGTextButton(_pcontrolfr,"Fill",402);
    _pcontrol[2]=new TGTextButton(_pcontrolfr,"Reset",403);
    for(i=0;i<3;i++){
      wattr.fMask = kWABackPixel | kWAEventMask;
      wattr.fBackgroundPixel = tgccolor;
      wattr.fEventMask = kExposureMask;
      gGXW->ChangeWindowAttributes(_pcontrol[i]->GetId(), &wattr);
    }
    for(i=0;i<3;i++)_pcontrol[i]->Associate(this);
    for(i=0;i<3;i++)_pcontrolfr->AddFrame(_pcontrol[i],fL1);
    for(i=0;i<nbuttons;i++)_pbutfr->AddFrame(_pbutton[i],fL1);
    for(i=0;i<nbuttons;i++)_pcyclefr->AddFrame(_pcycle[i],fL1);
    for(i=0;i<2;i++)_plogfr->AddFrame(_plogx[i],fL1);
    for(i=0;i<2;i++)_plogfr->AddFrame(_plogy[i],fL1);
    for(i=0;i<2;i++)_plogfr->AddFrame(_plogz[i],fL1);
    _plogx[0]->SetState(kButtonDown);   
    _plogy[0]->SetState(kButtonDown);   
    _plogz[0]->SetState(kButtonDown);   
    
    
    AddFrame(_pcontrolfr,fL2);
    AddFrame(_plogfr,fL2);
    AddFrame(_pbutfr,fL3);
    AddFrame(_pcyclefr,fL4);
    MapSubwindows();
    Resize(110,768);
   //    _pcontrolfr->Resize(_pcontrolfr->GetWidth(),_pcontrolfr->GetHeight()/2);
   //_plogfr->Resize(GetDefaultSize());

   // position relative to the parent's window
    Window_t wdum;
    gGXW->TranslateCoordinates(main->GetId(), GetParent()->GetId(),
			       (((TGFrame *) main)->GetWidth()+2 ),0,
			       //(((TGFrame *) main)->GetHeight() ) >> 1,
			       //(((TGFrame *) main)->GetWidth() + fWidth) >> 1,
			       //(((TGFrame *) main)->GetHeight() + fHeight) >> 1,
			       ax, ay, wdum);
    if(ax +fWidth> 1280){
      gGXW->TranslateCoordinates(main->GetId(), GetParent()->GetId(),
				 -fWidth-12,0,
				 ax, ay, wdum);
    }
    Move(ax,ay-26);
    
    SetWindowName("Control");
    MapWindow();
}
