//  $Id: AMSCTCClusterReader.h,v 1.2 2001/01/22 17:32:37 choutko Exp $
#ifndef AMSCTCClusterReader_H
#define AMSCTCClusterReader_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMS Cerenkov Threshold Counter Cluster Reader                        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TObject
#include <TObject.h>
#endif
#ifndef ROOT_TTree
#include <TTree.h>
#endif

#include "AMSMaker.h"


class AMSCTCClusterReader : public AMSMaker {

private:

   Int_t          m_Nclusters;

//        histograms
//   TH1F          *m_Multiplicity;


  public:
                  AMSCTCClusterReader() {;}
                  AMSCTCClusterReader(const char *name, const char *title);
   virtual       ~AMSCTCClusterReader() {;}
   virtual void   Init(TTree * tree=0);
   virtual void   Finish();

   virtual void   Make();
   virtual void   PrintInfo();
// virtual Bool_t Enabled(TObject * obj);

//
//      Getters
//

//
//      Setters
//
   void           AddCluster(Int_t status, Int_t plane, 
                             Float_t rawsignal, Float_t signal, Float_t ersig,
                             Float_t * coo, Float_t * ercoo,
                             Int_t ntracks=0, TObjArray * tracks=0);
   void           RemoveCluster(const Int_t cluster);
   void           Clear(Option_t *option="");

   ClassDef(AMSCTCClusterReader, 1)   // AMS Time of Flight Cluster Maker
};

#endif



