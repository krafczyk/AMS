//  $Id: AMSMaker.cxx,v 1.2 2001/01/22 17:32:37 choutko Exp $

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMS virtual base class for Makers                                    //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "Debugger.h"
#include <TTree.h>
#include <TList.h>
#include <TClonesArray.h>
#include <TBrowser.h>

#include "AMSMaker.h"
#include "AMSRoot.h"

ClassImp(AMSMaker)

//_____________________________________________________________________________
AMSMaker::AMSMaker()
{
   // should never be explicitly used because m_Histograms will stay 0
   //

   m_Save       = 0;
   m_Fruits     = 0;
   m_BranchName = "";
   m_Histograms = 0;
   DrawFruits   = kTRUE;
}

//_____________________________________________________________________________
AMSMaker::AMSMaker(const char *name, const char *title)
       :TNamed(name,title)
{
   m_Save       = 0;
   m_BranchName = "";
   m_Histograms = new TList();
   DrawFruits   = kTRUE;
   // m_Fruits should be constructed by derived classes
}

//_____________________________________________________________________________
AMSMaker::~AMSMaker()
{
   //dummy
}

//______________________________________________________________________________
void AMSMaker::Browse(TBrowser *b)
{
//  Insert Maker objects in the list of objects to be browsed.

  char name[64];
  if( b == 0  || m_Fruits == 0) return;
  TObject *obj;

// If m_Fruits is a ClonesArray, insert all the objects in the list
// of browsable objects
  if (m_Fruits->InheritsFrom("TClonesArray")) {
     TClonesArray *clones = (TClonesArray*)m_Fruits;
     Int_t nobjects = clones->GetEntries();
     for (Int_t i=0;i<nobjects;i++) {
        obj = clones->At(i);
        sprintf(name,"%s_%d",obj->GetName(),i);
        if (strstr(name,"AMS")) b->Add(obj, &name[4]);
        else                    b->Add(obj, &name[0]);
     }
// m_Fruits points to an object in general. Insert this object in the browser
  } else {
      b->Add( m_Fruits, m_Fruits->GetName());
  }
}

//_____________________________________________________________________________
void AMSMaker::Draw(Option_t *option)
{
//    Insert products of this maker in graphics pad list

  if (! DrawFruits) return;

  TObject *obj;

// If m_Fruits is a ClonesArray, insert all the objects in the list
// of objects to be painted
  if (m_Fruits->InheritsFrom("TClonesArray")) {
     TClonesArray *clones = (TClonesArray*)m_Fruits;
     Int_t nobjects = clones->GetEntries();
     debugger.Print("AMSMaker::Draw():  nobjects = %d\n", nobjects);
     for (Int_t i=0;i<nobjects;i++) {
        obj = clones->At(i);
	if (obj && i==0){ debugger.Print("obj class = %s\n", obj->ClassName());}
        // kludge to skip tracks
	// TString name(obj->ClassName());
        // if (name != TString("AMSTrack") && obj) obj->AppendPad();
        if (obj && Enabled(obj)) obj->AppendPad();
     }
// m_Fruits points to an object in general. Insert this object in the pad
  } else {
     m_Fruits->AppendPad();
  }
}

//_____________________________________________________________________________
void AMSMaker::Clear(Option_t *option)
{
  if (m_Fruits) m_Fruits->Delete(option);
}


//_____________________________________________________________________________
void AMSMaker::Save(Int_t save)
{
  m_Save = save; 
}


//_____________________________________________________________________________
void AMSMaker::PrintInfo()
{
   printf("*********************************\n");
   printf("*                               *\n");
   printf("*     %23s   *\n",GetName());
   printf("*                               *\n");
   printf("*********************************\n");

   Dump();
}

//_____________________________________________________________________________
void AMSMaker::MakeBranch()
{
//   Adds the list of physics objects to the AMS tree as a new branch

   if (m_Save == 0) return;

   TTree *tree = gAMSRoot->Tree();
   if (tree == 0  || m_Fruits == 0  || m_BranchName.Length() == 0) return;

//  Make a branch tree if a branch name has been set
   Int_t buffersize = 4000;
   if (m_Fruits->InheritsFrom("TClonesArray")) {
      debugger.Print("AMSMaker::MakeBranch() tree->Branch(%s, %lx, %d)\n",
		m_BranchName.Data(), m_Fruits, buffersize);
#if defined(ROOT10308)
      tree->Branch(m_BranchName.Data(), &m_Fruits, buffersize);
#else
      tree->Branch(m_BranchName.Data(), (TClonesArray*)m_Fruits, buffersize);
#endif
   } else {
      tree->Branch(m_BranchName.Data(),m_Fruits->ClassName(), &m_Fruits, buffersize);
   }
}

//______________________________________________________________________________
void AMSMaker::Streamer(TBuffer &R__b)
{
   // Stream an object of class AMSMaker.

   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion();
      TNamed::Streamer(R__b);
      R__b >> m_Save;
      R__b >> m_Fruits;
      m_BranchName.Streamer(R__b);
      R__b >> m_Histograms;
      //this is an addition to the standard rootcint version of Streamer
      //branch address for this maker is set automatically
      TTree *tree = gAMSRoot->Tree();
      if (tree == 0  || m_Fruits == 0  || m_BranchName.Length() == 0) return;
      if (m_Fruits->InheritsFrom("TClonesArray")) {
         tree->SetBranchAddress(m_BranchName.Data(),  m_Fruits);
      } else {
         tree->SetBranchAddress(m_BranchName.Data(), &m_Fruits);
      }
   } else {
      R__b.WriteVersion(AMSMaker::IsA());
      TNamed::Streamer(R__b);
      R__b << m_Save;
      R__b << m_Fruits;
      m_BranchName.Streamer(R__b);
      R__b << m_Histograms;
   }
}
