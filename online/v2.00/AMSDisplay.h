#ifndef AMSOnDisplay_H
#define AMSOnDisplay_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSDisplay                                                           //
//                                                                      //
// Utility class to display AMSRoot outline, tracks, and clusters.      //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <TApplication.h>
#include "AMSCanvas.h"
#include "AMSHist.h"
#include "ControlFrame.h"
class AMSNtuple;
class TCanvas;
class TPad;
class TArc;
class TTUBE;
class TNode;
class TPolyLine3D;
class TList;



class AMSOnDisplay : public TObject {

protected:
  AMSNtuple *      m_ntuple;
  TFile *          m_file;
  Bool_t           m_logx;
  Bool_t           m_logy;
  Bool_t           m_logz;
  AMSControlFrame        *m_ControlFrame;               //Pointer to the display canvas
  AMSCanvas        *m_Canvas;               //Pointer to the display canvas
  TPad             *m_TrigPad;              //Pointer to the trigger pad 
  TPad             *m_ButtonPad;            //Pointer to the buttons pad
  TPad             *m_Pad;                  //Pointer to the online main pad
  TPad             *m_TitlePad;             //Pointer to title pad 
  TPad             *m_RunInfoPad;         //Pointer to event info pad 
  TPad             *m_ObjInfoPad;           //Pointer to object info pad 
  
  TApplication	    *m_theapp;	     //Current App
  static AMSOnDisplay*    _Head; 
  AMSHist *         _subdet[10];      
  Int_t            _msubdet;
  Int_t            _cursubdet;
  Int_t            _cursubdetb;
  char             _grset[20];
  Int_t            _Begin;
  Int_t            _Sample;
public:
   
                    AMSOnDisplay();
                    AMSOnDisplay(const char *title, TFile *file);
  virtual          ~AMSOnDisplay(){_Head=0;}
  virtual void      SetApplication(TApplication* papp){m_theapp=papp;}
  virtual void      DisplayButtons();
  virtual void      Draw(Option_t *option="");
          void      Reset();
          void      DrawTitle(Option_t *option="");
          void      DrawRunInfo(Option_t *option="");
  AMSNtuple *       GetNtuple()const{return m_ntuple;}
  AMSHist *         getCurSubDet(){return _subdet[_cursubdetb];}
  char *            getGrSet(){return _grset;}
  TFile             *GetRootFile()const {return m_file;}
  TPad              *GetPad() {return m_Pad;}
  AMSCanvas         *GetCanvas() { return m_Canvas; }
  TPad              *GetObjInfoPad() { return m_ObjInfoPad; }
  TPad              *GetRunInfoPad() { return m_RunInfoPad; }
  TPad              *GetTitlePad() { return m_TitlePad; }
  void               StartStop(char * buf=0);
  Int_t             getMaxSubDet(){return _msubdet;}
  Bool_t   & IsLogX() {return m_logx;}
  Bool_t   & IsLogY() {return m_logy;} 
  Bool_t   & IsLogZ() {return m_logz;} 
  static AMSOnDisplay * GetAMSDisplay(){return _Head;}
  Int_t & Begin(){ return _Begin;}
  Int_t & Sample(){ return _Sample;}
  void Init();
  void Filled(char *buf=0);
  Int_t Fill();
  Int_t Dispatch(Int_t subdet, Int_t set=-1);
  void DispatchProcesses();
  void AddSubDet( AMSHist & subdet);
  AMSHist * getSubDet(Int_t subdet){return subdet>=0 && subdet<_msubdet?_subdet[subdet]:NULL;}
  static AMSOnDisplay * getAMSDisplay(){return _Head;}
  
  ClassDef(AMSOnDisplay, 0)   //Utility class to display AMS Hist Online

};

EXTERN AMSOnDisplay *gAMSDisplay;



#endif
