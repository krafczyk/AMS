#ifndef stlv_h
#define stlv_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>
#include "/offline/vdev/include/root_RVS.h"
   const Int_t kMaxevroot02 = 1;

class stlv : public TSelector {
   public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   AMSEventR      ev;        //  event      
   
   

   stlv(TTree *tree=0){ };
   ~stlv() {  }

   void    Begin(TTree *tree);
   void    Init(TTree *tree);
   Bool_t  Notify();
   Bool_t  ProcessCut(Int_t entry);
   void    ProcessFill(Int_t entry);
   void    SetOption(const char *option) { fOption = option; }
   void    SetObject(TObject *obj) { fObject = obj; }
   void    SetInputList(TList *input) {fInput = input;}
   TList  *GetOutputList() const { return fOutput; }
   void    Terminate();
};

#endif

#ifdef stlv_cxx
void stlv::Init(TTree *tree)
{
//   Set branch addresses
   if (tree == 0) return;
   fChain    = tree;
   fChain->SetMakeClass(1);

   ev.SetBranchA(fChain);
   

}

Bool_t stlv::Notify()
{
   // Called when loading a new file.
   // Get branch pointers.
   
     ev.GetBranchA(fChain);
    
   return kTRUE;
}

#endif // #ifdef stlv_cxx

