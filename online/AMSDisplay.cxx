
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSOnDisplay                                                           //
//                                                                      //
// Utility class to display AMSRoot outline, tracks, clusters...        //
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

#include "AMSDisplay.h"
#include "TSwitch.h"
#include "Debugger.h"
#include "AMSNtuple.h"
AMSOnDisplay * gAMSDisplay;

ClassImp(AMSOnDisplay)

AMSOnDisplay * AMSOnDisplay::_Head=0; 


//_____________________________________________________________________________
AMSOnDisplay::AMSOnDisplay() : TObject()
{
   // Default constructor of AMSOnDisplay
   //
   m_file=0;
   m_logx=kFALSE;
   m_logy=kFALSE;
   m_logz=kFALSE;
   m_theapp=0;
   _msubdet=0;
   _Head=this;   
   _cursubdet=0;
   _cursubdetb=0;
   _grset[0]='\0';
   _Begin=0;
   _Sample=10000;
   gAMSDisplay=this;
}


//_____________________________________________________________________________
AMSOnDisplay::AMSOnDisplay(const char *title, TFile *file):TObject(){
   m_file=file;
   m_theapp=0;
   m_logx=kFALSE;
   m_logy=kFALSE;
   m_logz=kFALSE;
   _grset[0]='\0';
   _Begin=0;
   _Sample=10000;
   _msubdet=0;
   _Head=this;
   _cursubdet=0;
   _cursubdetb=0;
   gAMSDisplay=this;
   m_ntuple = new AMSNtuple(file);
   // Constructor of AMSOnDisplay
   //


   m_Canvas = new AMSCanvas("Canvas", (Text_t*)title,1024,768);
   m_Canvas->SetEditable(kIsNotEditable);

   //
   // Create pads on the canvas
   //
   m_Canvas->cd();


   Float_t xsep = 0.2;
   //
   // Create title pad
   // ----------------------------
   m_TitlePad = new TPad("TitlePad", "AMSRoot Online display", xsep,0.95, 1.0, 1.0);
   m_TitlePad->Draw();
   m_TitlePad->Modified();
   m_TitlePad->SetFillColor(33);
   m_TitlePad->SetBorderSize(2);
   m_TitlePad->SetBorderMode(1);
   //
   // Create main display pad
   // ----------------------------
   m_Pad = new TPad("HistPad", "AMSRoot histos",xsep,0.05,1,0.95);
   m_Pad->Modified();
   m_Pad->SetFillColor(0);	//white 
   m_Pad->SetBorderSize(2);
   m_Pad->Draw();
   //
   // Create button pad
   // ----------------------------
   m_Canvas->cd();
   m_ButtonPad = new TPad("buttons", "button pad",0,0.45,xsep,1.0);
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
   // Create Run info pad
   // ----------------------------
   m_RunInfoPad = new TPad("RunInfoPad", "run info pad", 0, 0.0, 0.2, 0.1);
   m_RunInfoPad->SetFillColor(0);
   m_RunInfoPad->SetBorderSize(1);
   m_RunInfoPad->SetBorderMode(1);
   m_RunInfoPad->Draw();
   //
   // Create object info pad
   // ----------------------------
   m_Canvas->cd();
   m_ObjInfoPad = new TPad("ObjInfoPad", "object info pad", 0.2, 0, 1, 0.05);
   m_ObjInfoPad->SetFillColor(0);
   m_ObjInfoPad->SetBorderSize(1);
   m_ObjInfoPad->SetBorderMode(2);
   m_ObjInfoPad->Draw();

   


}
 void AMSOnDisplay::Init(){  // Create user interface control pad

   m_Canvas->cd();
   DisplayButtons();


   // Create switch pad
   m_TrigPad->cd();

   TSwitch * sw[8];

   Float_t y = 0.96, dy = 0.09, height=0.09;
   sw[0] = new TSwitch("LogX", &(m_logx), 
			"gAMSDisplay->Draw()", 0.0, y-height, 1.0, y);
   y -= dy;

   sw[1] = new TSwitch("LogY", &(m_logy), 
			"gAMSDisplay->Draw()", 0.0, y-height, 1.0, y);
   y -= dy;

   sw[1] = new TSwitch("LogZ", &(m_logz), 
			"gAMSDisplay->Draw()", 0.0, y-height, 1.0, y);
   y -= dy;

   for(int i=0;i<getMaxSubDet();i++){
    char text[80];
    sprintf(text,"Cycle %s",getSubDet(i)->GetName());
    new TSwitch(text,& getSubDet(i)->IsActive(), 
			"gAMSDisplay->Draw()", 0.0, y-height, 1.0, y);
    y -= dy;

   } 
   Draw();
   m_Canvas->cd();
   m_Canvas->Update();

}



//_____________________________________________________________________________
void AMSOnDisplay::DisplayButtons()
{
//    Create the user interface buttons

   m_ButtonPad->cd();

   Int_t butcolor = 33;
   Float_t dbutton = 0.09;
   Float_t y  = 1.0;
   Float_t dy = 0.014;
   Float_t x0 = 0.05;
   Float_t x1 = 0.95;
   TButton *button;
   char but[255];
  for(int i=0;i<getMaxSubDet();i++){
    sprintf(but,"gAMSDisplay->Dispatch(%d)",i);  
    button = new TButton(getSubDet(i)->GetName(),but,x0,y-dbutton,x1,y);
    button->SetFillColor(38);
    button->Draw();
    y -= dbutton +dy;
   }
   button = new TButton("Stop Timer","gAMSDisplay->StartStop()",x0,y-dbutton,x1,y);
   button->SetFillColor(butcolor);
   button->Draw();
    y -= dbutton +dy;
    button = new TButton("Fill","gAMSDisplay->Filled()",x0,y-dbutton,x1,y);
    button->SetFillColor(butcolor);
    button->Draw();


}




//_____________________________________________________________________________
void AMSOnDisplay::Draw(Option_t *option)
{
//    Insert current event in graphics pad list

   m_Canvas->SetEditable(kIsNotEditable);


   DrawTitle();
   DrawRunInfo();
   m_Pad->cd();
   //   int j=0;
   //   if(m_logx)j=1;
   //   cout <<"logx "<<j<<endl;
   if(m_logx)m_Pad->SetLogx(1);
   else  m_Pad->SetLogx(0);
   if(m_logy)m_Pad->SetLogy(1);
   else  m_Pad->SetLogy(0);
   if(m_logz)m_Pad->SetLogz(1);
   else  m_Pad->SetLogz(0);
   m_Canvas->cd();
   

}




//_____________________________________________________________________________
void AMSOnDisplay::DrawTitle(Option_t *option)
{

   static TText * text=0;
   static char atext[255];

   sprintf(atext, "Alpha Magnetic Spectrometer Offline Display  %s.Set_%d",gAMSDisplay->getCurSubDet()->GetName(),gAMSDisplay->getCurSubDet()->getCSet());
   sprintf(_grset,"%d",gAMSDisplay->getCurSubDet()->getCSet());

   TVirtualPad * gPadSave = gPad;
   m_TitlePad->cd();

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
void AMSOnDisplay::DrawRunInfo(Option_t *option)
{

   const int kTMAX = 80;
   static TText * text=0;
   static TText * text1=0;
   static char atext[kTMAX];

   TVirtualPad * gPadSave = gPad;
   m_RunInfoPad->cd();

   sprintf(atext,"Run %d",m_ntuple->GetRun());
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
   sprintf(atext,"%s",m_ntuple->GetTime());
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




void AMSOnDisplay::StartStop(){
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

void AMSOnDisplay::AddSubDet(  AMSHist & subdet){
  if( _msubdet<10){
     _subdet[_msubdet++]=&subdet;
  }
  else cerr <<"AMSOnDisplay::AddSubDet-S-NoMoreRoomForSubdetector "<<(const char*)subdet.GetName()<<endl;
}



Int_t AMSOnDisplay::Dispatch(Int_t subdet, Int_t set){
   TVirtualPad * gPadSave = gPad;
   m_Pad->cd();
   Int_t retcode;
   _cursubdetb=subdet;
   if(subdet>=0 && subdet<_msubdet ){
      int temp=_cursubdet;
      int tempset=_subdet[subdet]->getCSet();
      retcode=_subdet[subdet]->DispatchHist(set);
      _cursubdet=subdet;
      //      cout <<" subdet "<<subdet<<" "<<set<<" "<<tempset<<endl;
      if(set>=0)_subdet[_cursubdet]->getCSet()=set; 
      else _subdet[_cursubdet]->getCSet()=tempset; 
      Draw();
      _subdet[subdet]->getCSet()=retcode;
      _cursubdet=temp;
   }   
   gPadSave->cd();
      m_Canvas->Update();
   return retcode;
}

void AMSOnDisplay::DispatchProcesses(){
  // Check some of the subdet are active
  static Int_t change=1;
  if(!_msubdet){
    cerr <<" AMSOnDisplay::DispatchProcesses-E-NoSubDetectorsFound"<<endl;
    return;
  }
  int active=0;
  for(int i=0;i<_msubdet;i++){
    if(_subdet[i]->IsActive()){
      active++;
      break;
    }
  }
  if(!active){
    cerr <<"no active subproc"<<endl;
    _cursubdet=0;
    _cursubdetb=0;
    _subdet[0]->IsActive()=1;
  }
   if(change==0)_cursubdet=(_cursubdet+1)%_msubdet;
   while(!_subdet[_cursubdet]->IsActive()){
    _cursubdet=(_cursubdet+1)%_msubdet;
   }
   change=Dispatch(_cursubdet);
}

void AMSOnDisplay::Filled(){
  static int filled=0;
  if(filled)return;
   filled=Fill();
  gPad->Clear();
  static int state=0;
   static TText * text=0;
   static char atext2[20]="Fill";
   static char atext1[20]="Filled";

   if (! text) {
	if(filled%2)text = new TText(0.5, 0.5, atext1);
	else text = new TText(0.5, 0.5, atext2);
   }
   else{
	if(filled%2)text->SetText(0.5, 0.5, atext1);
	else text = new TText(0.5, 0.5, atext2);
   }
   text->SetTextAlign(22);
   text->SetTextSize(0.55);
   text->Draw();

}



Int_t AMSOnDisplay::Fill(){
  int retcode=0;
  for(int i=_Begin;i<_Begin+_Sample;i++){
   if(!m_ntuple->ReadOneEvent(i)){
     retcode=1;
     break;
   }
     for(int j=0;j<_msubdet;j++){
      _subdet[j]->Fill(m_ntuple);
     }
  }
    _Begin+=_Sample;
  return retcode;
}


