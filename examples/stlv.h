#ifndef stlv_h
#define stlv_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>
#include "/offline/vdev/include/root_RVS.h"
   const Int_t kMaxevroot02 = 1;

///  This is an example of user class to process AMS Root Files 
/*!
 This class is derived from the ROOT class TSelector. \n
 The following members functions are called by the TTree::Process() functions: \n
    Begin():       called everytime a loop on the tree starts, \n
                   a convenient place to create your histograms. \n
    Notify():      this function is called at the first entry of a new Tree 
                   in a chain. \n
    ProcessCut():  called at the beginning of each entry to return a flag,
                   true if the entry must be analyzed. \n
    ProcessFill(): called in the entry loop for all entries accepted
                   by Select. \n
    Terminate():   called at the end of a loop on the tree,
                   a convenient place to draw/fit your histograms. \n

   To use this file, try the following session on your Tree T \n

 Root > T->Process("stlv.C")  \n
 Root > T->Process("stlv.C","some options") \n
  Root > T->Process("stlv.C+") \n
 Root > T->Process("stlv.C++") \n

*/
class stlv : public TSelector {
   public :
   TTree          *fChain;   ///<pointer to the analyzed TTree or TChain
   AMSEventR      ev;        ///< AMSEventR instance      
   
   

   stlv(TTree *tree=0){ };
   ~stlv() {  }

   /// User Function called before starting the event loop.
   /// Initialize the tree branches.
   /// Book Histos
   void    Begin(TTree *tree);
   /// Selection function.
   /// Entry is the entry number in the current tree.
   /// Read only the necessary branches to select entries.
   /// Return kFALSE as soon as a bad entry is detected.
   /// To read complete event, call fChain->GetTree()->GetEntry(entry).
   /// Should Not be modified by User
   Bool_t  ProcessCut(Int_t entry);
   /// User Function called for selected entries only.
   /// Entry is the entry number in the current tree.
   /// Read branches not processed in ProcessCut() and fill histograms.
   /// To read complete event, call fChain->GetTree()->GetEntry(entry).
   void    ProcessFill(Int_t entry);

   void    Init(TTree *tree);
   Bool_t  Notify();
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

