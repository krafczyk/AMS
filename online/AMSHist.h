#ifndef __AMSHIST__
#define __AMSHIST__
#include <TTree.h>
#include <TNamed.h>
#include <TH2.h>
#include <TH1.h>
#include <TFile.h>
#include <TPad.h>
#include <TPaveLabel.h>
class AMSNtuple;
class AMSHist : public TNamed {
protected:
 Int_t _cSet;
 Int_t _mSet;
 Bool_t _active; 
 TH1 ** _fetched2;  
 TH1 ** _filled2;
 Int_t _m2fetched;
 Int_t _m2filled;
 virtual void _Fetch()=0;
 virtual void _Fill()=0;

public:
 Int_t & getCSet() {return _cSet;}
 
 virtual void Fill(AMSNtuple * ntuple);
 AMSHist(Text_t * name, Text_t * title, Int_t maxset=0, Int_t active=1):
 TNamed(name,title),_cSet(0),_mSet(maxset),_active(active),_m2fetched(0),_m2filled(0),_fetched2(0),_filled2(0){}
 Int_t DispatchHist(Int_t cset=-1);
 virtual void ShowSet(Int_t cset)=0;
 virtual ~AMSHist();
 Bool_t & IsActive(){return _active;}
 ClassDef(AMSHist, 1)   //Abstract class to browse hist
     };
#endif

