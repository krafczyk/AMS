#ifndef AMSSiHitReader_H
#define AMSSiHitReader_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMS Silicion Tracker Hit Reader                                      //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TObject
#include <TObject.h>
#endif
#ifndef ROOT_TTree
#include <TTree.h>
#endif

#include "AMSMaker.h"


class AMSSiHitReader : public AMSMaker {

 protected:

   Int_t          m_Nclusters;

//        histograms
//   TH1F          *m_Multiplicity;


 public:
   Bool_t         DrawUsedHitsOnly;

                  AMSSiHitReader() {;}
                  AMSSiHitReader(const char *name, const char *title);
   virtual       ~AMSSiHitReader() {;}
   virtual void   Init(TTree * tree=0);
   virtual void   Finish();

   virtual void   Make();
   virtual void   PrintInfo();
   virtual Bool_t Enabled(TObject * obj);

//
//      Getters
//

//
//      Setters
//
   void           AddCluster(Int_t status, Int_t plane, Int_t px, Int_t py,
                             Float_t * hit, Float_t * errhit,
                             Float_t ampl, Float_t asym,
                             TObjArray * tracks=0);
   void           RemoveCluster(const Int_t cluster);
   void           Clear(Option_t *option="");

   ClassDef(AMSSiHitReader, 1)   // AMS Time of Flight Cluster Maker
};

#endif



